;; evaluate λ calculus
(use-modules (ice-9 match) (ice-9 format))
(define-syntax comment (syntax-rules () ((d ...) '())))

(define reduce (lambda* (fn l #:optional (i '()))
                 (if (null? l) i (fn (car l) (reduce fn (cdr l) i)))))

(define (replace x y form)
  "Replace X with Y in FORM."
  (cond ;; TODO: variable capture: should only replace *free* variables
   ((list? form) (map (lambda (f) (replace x y f)) form))
   ((equal? form x) y)
   (#t form)))

(define (eval-top form)
  "Perform one pass of evaluation on form."
  (reduce (lambda (el acc)
            (if (list? el)
                (if (and (equal? 'λ (car el)) (not (null? acc)))
                    (append (replace (cadr el) (car acc) (caddr el)) (cdr acc))
                    (if (null? acc)
                        (cons (eval-top el) acc)
                        (append (eval-top el) acc)))
                (cons el acc)))
          form))

(let ((Ω '(λ f ((λ x (f (x x))) (λ x (f (x x))))))     ; Y-combinator
      (fac '(λ f (λ n ((= 0 n) 1 (* n (f (- n 1))))))) ; factorial
      (r-foo '(λ f (λ x ((foo x) (f x)))))) ; recursively foo forever
  (comment
   (eval-top `((,Ω ,fac) 5)))
  (eval-top `((,Ω ,r-foo) 5)))
