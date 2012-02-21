;;; test.lisp --- Tests for Ties

;; Copyright (C) 2012  Eric Schulte

;;; Code:
(in-package :ties)
(require :stefil)
(use-package :stefil)
(defsuite test-ties)
(in-suite test-ties)

(defixture short-chain
  (:setup
   (setf *medium* (make-array '(4 4) :element-type 'tie))
   (setf (aref *medium* 1 1)
         (make-tie :forward '(1 0)))
   (setf (aref *medium* 2 1)
         (make-tie :backward '(-1 0) :forward '(0 1)))
   (setf (aref *medium* 2 2)
         (make-tie :backward '(0 -1))))
  (:teardown))

(deftest follow-chain ()
  (with-fixture short-chain
    (is (equal '(2 1) (-> '(1 1))))
    (is (equal '(2 2) (-> (-> '(1 1)))))
    (is (equal '(2 1) (<- (-> (-> '(1 1))))))
    (is (equal '(1 1) (<- (<- (-> (-> '(1 1)))))))))
