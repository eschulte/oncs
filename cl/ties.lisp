;;; ties.lisp --- Ties between Places and Places bound by Ties

;; Copyright (C) 2012  Eric Schulte

;;; Commentary:

;; A lisp virtual machine implemented as cons cells embedded in a
;; spatially extended processing and memory medium.  See the README.

;;; Code:
(require :alexandria)
(defpackage #:ties (:use :common-lisp :alexandria))
(in-package :ties)


;;; Types of structures
(defvar *medium* nil "The actual processing and memory medium.")

;; a local reference in the form a number for each dimension with
;; small absolute value
(defstruct (tie (:conc-name))
  (forward  nil :type list)
  (backward nil :type list))

(defun comb (loc dir)   (map 'list #'+ loc dir))
(defun at (loc)         (apply #'aref *medium* loc))
(defun follow (dir loc) (comb loc (funcall dir (at loc))))
(defun -> (loc)         (follow #'forward loc))
(defun <- (loc)         (follow #'backward loc))

(defun move (loc dir)
  "Move the tie at LOC in DIR updating any refs."
  (flet ((update (neighbor)
           ))
    (when (and (update (forward loc))
               (update (backward loc)))
      (setf (map )))))

;; two overlaid planes, each octagonal
