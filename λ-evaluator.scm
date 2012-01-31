;; evaluation rules
;; - λ substitution
(use-modules (ice-9 match) (ice-9 format))
(define-syntax comment
  (syntax-rules ()
    ((d ...) '())))

(define reduce (lambda* (fn l #:optional (i '()))
                 (if (null? l) i (fn (car l) (reduce fn (cdr l) i)))))

(define (replace x y form)
  "Replace X with Y in FORM."
  (cond
   ((list? form) (map (lambda (f) (replace x y f)) form))
   ((equal? form x) y)
   (#t form)))

(define (eval-top form)
  "Perform one pass of evaluation on form."
  (reduce (lambda (el acc)
            (if (and (list? el) (equal? 'λ (car el)))
                (append (replace (cadr el) (car acc) (cddr el)) (cdr acc))
                (cons el acc)))
          form))

(define (eval-step form)
  "Evaluate a single step in λ calculus."
  (cond
   ;; substitute lambda expression
   ((and (list? form) (list? (car form)) (equal? (caar form) 'λ))
    (let ((x (cadar form))
          (y (cadr  form))
          (target (cddar form))
          (rest (cdr form)))
      (format #t "rp: ~a~%wt: ~a~%in: ~a~%rt: ~a~%" x y target rest)
      (append (replace (cadar form) (cadr form) (cddar form)) rest)))
   ;; map evaluation over the elements of a list
   ((list? form) (map eval-step form))
   ;; return atoms directly
   (#t form)))

(comment
 (eval-step '((λ f (λ x (f (x x))) (λ x (f (x x))))
              (λ f (λ n ((= 0 n) 1 (* n (f (- n 1))))))))

 (eval-step '((λ f (λ x (f (x x))) (λ x (f (x x))))
              (λ f (λ x ((foo x) (f x))))
              5))

 (eval-top '(((λ f (λ x (f (x x))) (λ x (f (x x))))
              (λ f (λ x ((foo x) (f x)))))
             5))

 ;; recursive foo'er
 (λ f (λ x ((foo x) (f x)))))
