;;; test.lisp --- Tests for Oncs

;; Copyright (C) 2012  Eric Schulte

;;; Code:
(in-package :oncs)
(defsuite test-oncs)
(in-suite test-oncs)

(defixture onc
  (:setup (setq onc (make-onc
                     :car '(:lambda :a)
                     :cdr (make-onc
                           :car :a
                           :cdr (make-onc :car :a)))))
  (:teardown))

(defvar expr '(λ :a (:x :y (λ :b (:b :a)) :a (λ :a (:b :a))))
  "Variable used for tests.")

(defvar true '(λ :t (λ :f :t))
  "Variable used for tests.")

(defvar false '(λ :t (λ :f :f))
  "Variable used for tests.")

(deftest test-to-oncs ()
  (is (oequal (make-onc :car :a) (to-oncs '(:a))))
  (is (oequal
       (make-onc :car :a :cdr (make-onc :car :a))
       (to-oncs '(:a :a))))
  (with-fixture onc
    (is (oequal onc (to-oncs '(λ :a (:a :a)))))))

(deftest from-a-simple-oncs ()
  (is (equal (from-oncs (make-onc :car :a :cdr (make-onc :car :a)))
             '(:a :a))))

(deftest simple-app-abs ()
  (with-fixture onc
    (is (oequal (app-abs onc :b) (to-oncs '(:b :b))))))

(deftest idempotent-onc-conversion ()
  (is (equal expr (from-oncs (to-oncs expr)))))

(deftest variable-hiding ()
  (is (equal (from-oncs (app-abs (to-oncs expr) :foo))
             '(:x :y (λ :b (:b :foo)) :foo (λ :a (:b :a))))))

(deftest free-variable-calculation ()
  (is (null (set-difference (free-variables (to-oncs expr))
                            '(:B :Y :X)))))

(deftest uniquify-is-not-overly-agressive ()
  (let ((left (to-oncs '(λ :a (:a :b :c :d))))
        (right (to-oncs '(λ :a (:b :c :d)))))
    (is (oequal (uniquify left right) left))))

(deftest oreplace-in-simple-expression ()
  (with-fixture onc
    (is (oequal (oreplace onc :a :b)
                (to-oncs '(λ :b (:b :b)))))))

(deftest uniquify-with-work-to-do ()
  (is (equal (lambda-var (uniquify (to-oncs '(λ :b (:b :d))) (to-oncs expr)))
             :B+)))

(deftest unique-var-for-app-abs ()
  (is (equal (from-oncs (app-abs (to-oncs expr) :b))
             '(:X :Y (Λ :B+ (:B+ :B)) :B (Λ :A (:B :A))))))

(deftest evalutation-with-church-constants-true-and-false ()
  (is (equal (from-oncs (fix (to-oncs (list true :foo :bar))))
             '(:foo)))
  (is (equal (from-oncs (fix (to-oncs (list false :foo :bar))))
             '(:bar))))

(test-oncs)
