;;; test.lisp --- Tests for Oncs

;; Copyright (C) 2012  Eric Schulte

;;; Code:
(in-package :oncs)
(defsuite test-oncs)
(in-suite test-oncs)

(defixture simple-onc
  (:setup
   ;; (lambda a . a a)
   (setf *a* (make-onc
              :car '(:lambda :a)
              :cdr (make-onc
                    :car :a
                    :cdr (make-onc :car :a)))))
  (:teardown))

(deftest test-simple-read ()
  (with-fixture simple-onc
    (is (oequal (make-onc :car :a) (to-onc '(:a))))
    (is (oequal
         (make-onc :car :a :cdr (make-onc :car :a))
         (to-onc '(:a :a))))
    (is (oequal *a* (to-onc '((:lambda :a) :a :a))))))

(deftest simple-app-abs ()
  (with-fixture simple-onc
    (is (oequal (app-abs *a* :b)
                (to-onc '(:b :b))))))

;; (test-oncs)
