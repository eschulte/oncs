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

(defixture complex-expression
  (:setup
   (setf *a* '(λ :a (:x :y (λ :b (:b :a)) :a (λ :a (:b :a))))))
  (:teardown))

(deftest test-to-oncs ()
  (with-fixture simple-onc
    (is (oequal (make-onc :car :a) (to-oncs '(:a))))
    (is (oequal
         (make-onc :car :a :cdr (make-onc :car :a))
         (to-oncs '(:a :a))))
    (is (oequal *a* (to-oncs '(λ :a (:a :a)))))))

(deftest test-from-oncs ()
  (is (equal (from-oncs (make-onc :car :a :cdr (make-onc :car :a)))
             '(:a :a))))

(deftest simple-app-abs ()
  (with-fixture simple-onc
    (is (oequal (app-abs *a* :b)
                (to-oncs '(:b :b))))))

(deftest test-idempotent-onc-conversion ()
  (with-fixture complex-expression
    (is (equal *a* (from-oncs (to-oncs *a*))))))

(deftest variable-hiding ()
  (with-fixture complex-expression
    (is (equal (from-oncs (app-abs (to-oncs *a*) :foo))
               '(:x :y (λ :b (:b :foo)) :foo (λ :a (:b :a)))))))

(with-fixture complex-expression
  (from-oncs (app-abs (to-oncs *a*) :b)))

(test-oncs)
