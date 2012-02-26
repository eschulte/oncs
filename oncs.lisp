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

;; (defstruct (ptr (:conc-name))
;;   "The relative addresses used to connect oncs."
;;   (x nil) (y nil))

(defun app-abs (a b)
  "Apply A to B."
  (unless (and (consp (ocar a)) (eq :lambda (car (ocar a))))
    (error "attempt to apply expression which is not an abstraction."))
  (app-abs- (cdr (ocar a)) (ocdr a) b))

(defun app-abs- (var a b)
  "[var->b]a"
  (cond
    ((null a) nil)
    ((symbolp a) b)
    ((onc-p a)
     ;; unless a lambda-expression hiding var
     (unless (and (consp (ocar a))
                  (eq :lambda (car (ocar a)))
                  (eq var (cdr (ocar a))))
       (setf (ocar a) (app-abs- var (ocar a) b))
       (setf (ocdr a) (app-abs- var (ocdr a) b)))
     a)))

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

(defun to-onc (sexpr)
  "Read an SEXPR into an onc structure."
  (if (and (consp sexpr) (not (eq :lambda (car sexpr))))
      (make-onc :car (to-onc (car sexpr))
                :cdr (to-onc (cdr sexpr)))
      sexpr))
