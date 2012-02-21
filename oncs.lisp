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
(defvar *medium* nil "The actual processing and memory medium.")

;; a local reference in the form a number for each dimension with
;; small absolute value
(defstruct (tie (:conc-name))
  (forward  nil :type list)
  (backward nil :type list))

(defun follow (dir loc)
  (map 'list #'+ loc (funcall dir (apply #'aref *medium* loc))))

(defun -> (loc) (follow #'forward loc))
(defun <- (loc) (follow #'backward loc))
