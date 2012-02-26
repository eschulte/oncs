;;; oncs.lisp --- Oncs are Not Conses

;; Copyright (C) 2012  Eric Schulte

;;; Commentary:

;; A lisp virtual machine implemented as cons cells embedded in a
;; spatially extended processing and memory medium.  See the README.

;;; Code:
(require :alexandria)
(require :stefil)
(defpackage #:oncs (:use :common-lisp :alexandria :stefil))
(in-package :oncs)

(defstruct (onc (:conc-name o))
  "The basic building blocks of computation."
  (car nil) (cdr nil))

(defun oequal (a b)
  "Test equality between oncs."
  (cond ((null a) (null b))
        ((symbolp a) (equal a b))
        ((consp a) (and (consp b)
                        (oequal (car a) (car b))
                        (oequal (cdr a) (cdr b))))
        ((onc-p a) (and (onc-p b)
                        (oequal (ocar a) (ocar b))
                        (oequal (ocdr a) (ocdr b))))))

(defun lambda-p (sexpr)
  (and (consp sexpr)
       (symbolp (car sexpr))
       (or (equal (car sexpr) :lambda)
           (equal (car sexpr) 'λ))))

(defun lambda-var (onc)
  (second (ocar onc)))

(defun app-abs (a b)
  "Apply A to B."
  (unless (and (consp (ocar a)) (eq :lambda (car (ocar a))))
    (error "attempt to apply expression which is not an abstraction."))
  (app-abs- (lambda-var a) (ocdr a) b))

(defun free-variables (onc)
  "Return the free variables of an onc."
  #| TODO: implement |# )

(defun uniquify (a c)
  "Ensure the variable of abstraction A is unique in context C."
  (let* ((taken (append (free-variables a) (free-variables b)))
         (var (lambda-var a))
         (new var))
    (while (member var taken)
      (setq new (make-symbol (concatenate 'string (symbol-name new) "0")))
      (replace-all var new a))
    a))

(defun app-abs- (var a b)
  "[var->b]a"
  (if (equal var a) b
      (progn
        (cond
          ((and (onc-p a) (lambda-p (ocar a)))
           (unless (equal var (lambda-var a)) ; just skip if shadowed by var
             (setf a (uniquify a b))
             (setf (ocar a) (app-abs- var (ocar a) b))
             (setf (ocdr a) (app-abs- var (ocdr a) b))))
          ((onc-p a)
           (setf (ocar a) (app-abs- var (ocar a) b))
           (setf (ocdr a) (app-abs- var (ocdr a) b))))
        a)))

(defun to-oncs (sexpr)
  "Read an SEXPR into an onc structure."
  (cond
    ((lambda-p sexpr)
     (when (> (length sexpr) 3)
       (error "extra argument to λ:~S" (cdddr sexpr)))
     (make-onc
      :car `(:lambda ,(second sexpr))
      :cdr (to-oncs (third sexpr))))
    ((consp sexpr)
     (make-onc
      :car (to-oncs (car sexpr))
      :cdr (to-oncs (cdr sexpr))))
    (t sexpr)))

(defun from-oncs (onc)
  (if (onc-p onc)
      (if (lambda-p (ocar onc))
          (list 'λ (second (ocar onc)) (from-oncs (ocdr onc)))
          (cons (from-oncs (ocar onc)) (from-oncs (ocdr onc))))
      onc))
