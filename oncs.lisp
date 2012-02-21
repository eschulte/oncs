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


;;; objects in the world
(defun make-torus-medium (world-size)
  "Used to initialize the medium."
  (let ((medium (make-array world-size :element-type 'onc)))
    (loop :for x :below (x world-size) :do
       (loop :for y :below (y world-size) :do
          (setf (aref medium x y) (make-onc :loc (list x y)))))
    medium))

(defvar *medium* nil
  "The actual processing and memory medium in which computation occurs.")

(defvar *environment* (make-hash-table) ;; TODO: implement in the medium
  "Placeholder environment which will eventually be implemented in the medium.")

(defvar *message-queue* nil "Message queue.")


;;; functions defining the physics of this machine
(defun o-get (onc)
  (let ((dim (third (type-of *medium*)))
        (loc (o-loc onc)))
    (case (dir (o-hdr onc))
      ;; pointers to directions
      (:north (aref *medium* (x loc) (mod (+ 1 (y loc)) (y dim))))
      (:south (aref *medium* (x loc) (mod (- 1 (y loc)) (y dim))))
      (:east  (aref *medium* (mod (+ 1 (x loc)) (x dim)) (y loc)))
      (:west  (aref *medium* (mod (- 1 (x loc)) (x dim)) (y loc)))
      (:car   (o-car onc))
      (:cdr   (o-cdr onc))
      (:msg   (o-msg onc))
      (:env   (o-env onc))
      ;; directives to action
      (:act   ))))

(defun accept-message (onc message)
  "Accept from message stream into onc."
  (if (keywordp message)
      ;; if a keyword, save in header
      (setf (dir (o-hdr onc)) message)
      ;; otherwise it is data so handle appropriately given header-dir
      (let ((dim (third (type-of *medium*)))
            (loc (o-loc onc)))
        (flet ((push-place (message x y)
                 (push message (aref *message-queue* x y))))
          (case (dir (o-hdr onc))
            (:north (push-place message (x loc) (mod (+ 1 (y loc)) (y dim))))
            (:south (push-place message (x loc) (mod (- 1 (y loc)) (y dim))))
            (:east  (push-place message (mod (+ 1 (x loc)) (x dim)) (y loc)))
            (:west  (push-place message (mod (- 1 (x loc)) (x dim)) (y loc)))
            (:car   (setf (o-car onc) message))
            (:cdr   (setf (o-cdr onc) message))
            (:msg   (setf (o-msg onc) message))
            (:env   (setf (o-env onc) message)))))))
