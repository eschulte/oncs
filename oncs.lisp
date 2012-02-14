;;; oncs.lisp --- Oncs are Not Conses

;; Copyright (C) 2012  Eric Schulte

;;; Commentary:

;; ONCs is a lisp virtual machine implemented as cons cells embedded
;; in a spatially extended medium of combined processing and memory
;; elements.  See the README file in this directory for more
;; information.

;;; Code:
(require :alexandria)
(defpackage #:oncs (:use :common-lisp :alexandria))
(in-package :oncs)


;;; Types of structures
(defvar *word-size* 32)
(defvar *world-size* '(100 100)) ;; <- '(1000 1000) blows the stack

(defstruct (onc-header (:conc-name))
  (exe-p nil   :type boolean)
  (srl-p nil   :type boolean)
  (act-p nil   :type boolean)
  (hld-p nil   :type boolean)
  (dir   :none :type keyword))

(defstruct (onc (:conc-name o-))
  (hdr (make-onc-header) :type onc-header)
  (env (make-array *word-size* :element-type 'boolean :initial-element nil)
       :type (vector boolean))
  (msg (make-array *word-size* :element-type 'boolean :initial-element nil)
       :type (vector boolean))
  (car (make-array *word-size* :element-type 'boolean :initial-element nil)
       :type (vector boolean))
  (cdr (make-array *word-size* :element-type 'boolean :initial-element nil)
       :type (vector boolean))
  (loc nil))

(defun x (lst) (first lst))
(defun y (lst) (second lst))

(defun o-get (onc)
  (case (dir (o-hdr onc))
    (:north (aref *medium* (mod (- 1 (x (o-loc onc))) (x *world-size*))))
    (:south (aref *medium* (mod (+ 1 (x (o-loc onc))) (x *world-size*))))
    (:east  (aref *medium* (mod (+ 1 (y (o-loc onc))) (y *world-size*))))
    (:west  (aref *medium* (mod (- 1 (y (o-loc onc))) (y *world-size*))))
    (:car   (o-car onc))
    (:cdr   (o-cdr onc))
    (:msg   (o-msg onc))
    (:env   (o-env onc))))


;;; objects in the world
(defvar *medium* (make-array *world-size* :element-type 'onc)
  "The actual processing and memory medium in which computation occurs.")
(loop :for x :below (x *world-size*) :do
   (loop :for y :below (y *world-size*) :do
      (setf (aref *medium* x y) (make-onc :loc (list x y)))))

(defvar *environment* (make-hash-table) ;; TODO: implement in the medium
  "Placeholder environment which will eventually be implemented in the medium.")

(defvar *message-queue* nil "Message queue.")


;;; functions defining the physics of this machine
(defun accept-message (onc message)
  "Accept from message stream into onc."
  (if (keywordp message)
      ;; if a keyword, save in header
      (setf (dir (o-hdr onc)) message)
      ;; otherwise it is data so handle appropriately given header-dir
      (let ((location (o-get onc)))
        (if (onc-p location)
            (push (cons message location) *message-queue*)
            (setf location message)))))
