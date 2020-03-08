#lang racket
(require rackunit)
(require rackunit/text-ui)

(struct const (val))
(struct op (sym l r))
(struct variable ())
(struct der (f))

;;Predykat wyrażeń w mojej składni abstakcyjnej

(define (expr? x)
  (match x
    [(const c)   (number? c)]
    [(op s l r)  (and (member s '(+ *))
                      (expr? l)
                      (expr? r))]
    [(variable) true]
    [(der f)     (expr? f)]
    [_           false]))

;;Ewaluator mojej składni wyrażeń z wewnętrznie zdefiniowaną funkcją różniczkującą wyrazenia
(define (eval e x)
  
  (define (deriv f)
    (match f
      [(const c)  (const 0)]
      [(variable) (const 1)]
      [(op s l r) (if (eq? s '+)
                    (op '+ (deriv l) (deriv r))
                    (op '+ (op '* (deriv l) r)
                           (op '* l (deriv r))))]
      [(der ff)   (deriv (deriv ff))]))
      
  (match e
    [(const c)  c]
    [(op '+ l r) (+ (eval l x) (eval r x))]
    [(op '* l r) (* (eval l x) (eval r x))]
    [(variable) x]
    [(der f)    (eval (deriv f) x)]))


;;TESTY

(define abstract-expression-tests
  (test-suite
   "Test of abstract evaluator"
    (test-case
     "Eval of const"
     (equal? 25 (eval (const 25) 1)))
    (test-case
     "Eval of sum"
     (equal? 35 (eval (op '+ (const 10) (const 25)) 1)))
    (test-case
     "Eval of qutoient"
     (equal? 250 (eval (op '* (const 10) (const 25)) 1)))
    (test-case
     "Eval of variable"
     (equal? 10 (eval (variable) 10)))
    (test-case
     "Eval of variable sum and qutoient"
     (equal? 50 (eval (op '* (variable) (op '+ (variable) (variable))) 5)))
    (test-case
     "Eval of variable qutoient and const"
     (equal? 20 (eval (op '* (const 2) (op '+ (variable) (variable))) 5)))
    (test-case
     "Eval of variable sum and const"
     (equal? 35 (eval (op '* (variable) (op '+ (const 2) (variable))) 5)))
    (test-case
     "Eval of const and derivative"
     (equal? 0 (eval (der (const 5)) 10)))
    (test-case
     "Eval of derivative and variable"
     (equal? 1 (eval (der (variable)) 10)))
    (test-case
     "Eval of derivative and squared variable"
     (equal? 20 (eval (der (op '* (variable) (variable))) 10)))
    (test-case
     "Eval of derivative and variable sum"
     (equal? 2 (eval (der (op '+ (variable) (variable))) 10)))
    (test-case
     "Eval of double derivative and squared variable"
     (equal? 2 (eval (der (der (op '* (variable) (variable)))) 10)))
    (test-case
     "Eval of a complex expression" ;;x^3 + ((∂ 5)*(x^2) + (∂(∂ x^4)))
     (equal? 56 (eval (op '+ (op '* (op '* (variable) (variable)) (variable)) (op '+ (op '* (der (const 5)) (op '* (variable) (variable))) (der (der (op '* (op '* (variable) (variable)) (op '* (variable) (variable))))))) 2)))))

(run-test abstract-expression-tests)