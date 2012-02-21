;;; test-oncs.lisp --- Tests for Oncs

;; Copyright (C) 2012  Eric Schulte

;;; Code:
(in-package :oncs)
(require :stefil)
(use-package :stefil)
(defsuite oncs-test)
(in-suite oncs-test)

(defixture small-world
  (:setup
   (let ((world-size '(4 4)))
     (setf *medium* (make-torus-medium world-size))
     (setf *message-queue* (make-array world-size :initial-element nil))))
  (:teardown))

(deftest get-directions ()
  (with-fixture small-world
    (setf (dir (o-hdr (aref *medium* 1 1))) :north)
    (is (equal '(1 2) (o-loc (o-get (aref *medium* 1 1)))))
    (setf (dir (o-hdr (aref *medium* 1 1))) :west)
    (is (equal '(0 1) (o-loc (o-get (aref *medium* 1 1)))))))

(deftest get-local ()
  (with-fixture small-world
    (setf (dir (o-hdr (aref *medium* 1 1))) :car)
    (equal (type-of (o-get (aref *medium* 1 1)))
           `(simple-vector ,*word-size*))))

(deftest accept-header ()
  (with-fixture small-world
    (let* ((location '(1 1))
           (onc (apply #'aref *medium* location))
           (payload (make-array *word-size*)))
      ;; put something unique in the payload
      (setf (aref payload 4) 7)
      ;; accept a header message
      (accept-message onc :cdr)
      (is (equal :cdr (dir (o-hdr onc))))
      ;; store the payload into the message
      (accept-message onc payload)
      (is (equal payload (o-cdr onc))))))
