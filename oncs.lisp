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
  "Return a cons tree of the given onc tree."
  (if (onc-p onc)
      (if (lambda-p onc)
          (list 'λ (second (ocar onc)) (from-oncs (ocdr onc)))
          (cons (from-oncs (ocar onc)) (from-oncs (ocdr onc))))
      onc))

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

(defun oreplace (onc item new)
  "Replace ITEM with NEW in ONC."
  (cond ((onc-p onc) (make-onc :car (oreplace (ocar onc) item new)
                               :cdr (oreplace (ocdr onc) item new)))
        ((consp onc) (cons (oreplace (car onc) item new)
                           (oreplace (cdr onc) item new)))
        ((equal item onc) new)
        (t onc)))

(defun lambda-p (onc &aux sexp)
  "Check if ONC is a lambda abstraction.
Works whether ONC is an onc or is a lambda cons."
  (setq sexp (if (onc-p onc) (ocar onc) onc))
  (and (consp sexp)
       (symbolp (car sexp))
       (or (equal (car sexp) :lambda)
           (equal (car sexp) 'λ))))

(defun lambda-var (onc)
  "Return the variable of a lambda abstraction (ONC or cons)."
  (second (if (onc-p onc) (ocar onc) onc)))

(defun free-variables (onc &optional non-free)
  "Return the free variables of an onc."
  (if (onc-p onc)
      (if (lambda-p onc)
          (free-variables (ocdr onc) (cons (lambda-var onc) non-free))
          (union (free-variables (ocar onc) non-free)
                 (free-variables (ocdr onc) non-free)))
      (when (and onc (not (member onc non-free))) (list onc))))

(defun symbol-inc (symbol)
  "Increment SYMBOL, used generating unique symbols."
  (intern (concatenate 'string (symbol-name symbol) "+")
          (if (keywordp symbol) "KEYWORD" *package*)))

(defun uniquify (left right &aux new)
  "Modify LEFT until it's `lambda-var' is unique in context RIGHT."
  (let ((taken (union (free-variables left) (free-variables right)))
        (var (lambda-var left)))
    (loop :while (member var taken) :do
       (setq new (symbol-inc var))
       (setf left (oreplace left var new))
       (setq var new))
    left))

(defun app-abs (a b)
  "Apply A to B."
  (unless (and (consp (ocar a)) (eq :lambda (car (ocar a))))
    (error "attempt to apply expression which is not an abstraction."))
  (app-abs- (lambda-var a) (ocdr a) b))

(defun app-abs- (var a b)
  "[var->b]a"
  (if (equal var a) b
      (progn
        (cond
          ((and (onc-p a) (lambda-p a))
           (unless (equal var (lambda-var a)) ; just skip if shadowed by var
             (setf a (uniquify a b))
             (setf (ocar a) (app-abs- var (ocar a) b))
             (setf (ocdr a) (app-abs- var (ocdr a) b))))
          ((onc-p a)
           (setf (ocar a) (app-abs- var (ocar a) b))
           (setf (ocdr a) (app-abs- var (ocdr a) b))))
        a)))

(defun oeval (onc)
  "Evaluate ONCS"
  (if (onc-p onc)
      (let ((car (oeval (ocar onc)))
            (cdr (oeval (ocdr onc))))
        (if (and (onc-p car) (lambda-p car))
            (if (onc-p cdr)
                (make-onc :car (app-abs car (ocar cdr))
                          :cdr (ocdr cdr))
                (app-abs car cdr))
            (make-onc :car car :cdr cdr)))
      onc))
