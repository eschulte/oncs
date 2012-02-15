;;; test-oncs.lisp --- Tests for Oncs

;; Copyright (C) 2012  Eric Schulte

;;; Code:
(in-package :oncs)
(require :stefil)
(use-package :stefil)
(defsuite oncs-test)
(in-suite oncs-test)

(defixture small-world
  (:setup (setf *medium* (make-torus-medium '(4 4))))
  (:teardown))

(deftest get-north ()
  (with-fixture small-world
    (setf (dir (o-hdr (aref *medium* 1 1))) :north)
    (is (equal '(1 2) (o-loc (o-get (aref *medium* 1 1)))))
    (setf (dir (o-hdr (aref *medium* 1 1))) :west)
    (is (equal '(0 1) (o-loc (o-get (aref *medium* 1 1)))))))
