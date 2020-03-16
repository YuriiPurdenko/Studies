#lang racket
(require rackunit)
;; definicja wyrażeń

(struct variable     (x)        #:transparent)
(struct const        (val)      #:transparent)
(struct op           (symb l r) #:transparent)
(struct let-expr     (x e1 e2)  #:transparent)
(struct if-expr      (b t e)    #:transparent)
(struct cons-expr    (l r)      #:transparent)
(struct car-expr     (p)        #:transparent)
(struct cdr-expr     (p)        #:transparent)
(struct pair?-expr   (p)        #:transparent)
(struct null-expr    ()         #:transparent)
(struct null?-expr   (e)        #:transparent)
(struct symbol-expr  (v)        #:transparent)
(struct symbol?-expr (e)        #:transparent)
(struct lambda-expr  (xs b)     #:transparent)
(struct app-expr     (f es)     #:transparent)
(struct apply-expr   (f e)      #:transparent)
(struct closure      (xs f es))

(define (expr? e)
  (match e
    [(variable s)       (symbol? s)]
    [(const n)          (or (number? n)
                            (boolean? n))]
    [(op s l r)         (and (member s '(+ *))
                             (expr? l)
                             (expr? r))]
    [(let-expr x e1 e2) (and (symbol? x)
                             (expr? e1)
                             (expr? e2))]
    [(if-expr b t e)    (andmap expr? (list b t e))]
    [(cons-expr l r)    (andmap expr? (list l r))]
    [(car-expr p)       (expr? p)]
    [(cdr-expr p)       (expr? p)]
    [(pair?-expr p)     (expr? p)]
    [(null-expr)        true]
    [(null?-expr p)     (expr? p)]
    [(symbol-expr v)    (symbol? v)]
    [(symbol?-expr p)   (expr? p)]
    [(lambda-expr xs b) (and (list? xs)
                             (andmap symbol? xs)
                             (expr? b)
                             (not (check-duplicates xs)))]
    [(app-expr f es)    (and (expr? f)
                             (list? es)
                             (andmap expr? es))]
    [(apply-expr f e)   (and (expr? f)
                             (expr? e))]
    [_                  false]))

;; wartości zwracane przez interpreter

(struct val-symbol (s)   #:transparent)

(define (my-value? v)
  (or (number? v)
      (boolean? v)
      (and (pair? v)
           (my-value? (car v))
           (my-value? (cdr v)))
      (and (lambda-expr? v)
           (expr? v))
      ; null-a reprezentujemy symbolem (a nie racketowym
      ; nullem) bez wyraźnej przyczyny
      (and (symbol? v) (eq? v 'null))
      (and ((val-symbol? v) (symbol? (val-symbol-s v))))
      (and (closure? v)
           (andmap symbol? (closure-xs v))
           (expr? (closure-f v))
           (andmap env? (closure-es v)))))

;; wyszukiwanie wartości dla klucza na liście asocjacyjnej
;; dwuelementowych list

(define (lookup x xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x) (cadar xs)]
    [else (lookup x (cdr xs))]))

;; kilka operatorów do wykorzystania w interpreterze

(define (op-to-proc x)
  (lookup x `(
              (+ ,+)
              (* ,*)
              (- ,-)
              (/ ,/)
              (> ,>)
              (>= ,>=)
              (< ,<)
              (<= ,<=)
              (= ,=)
              (eq? ,(lambda (x y) (eq? (val-symbol-s x)
                                       (val-symbol-s y))))
              )))

;; interfejs do obsługi środowisk

(define (env-empty) null)
(define env-lookup lookup)
(define (env-add x v env) (cons (list x v) env))

(define (env? e)
  (and (list? e)
       (andmap (lambda (xs) (and (list? e)
                                 (= (length e) 2)
                                 (symbol? (first e)))))))

;; interpretacja wyrażeń

(define (eval e env)
  (match e
    [(const n) n]
    [(op s l r)
     ((op-to-proc s) (eval l env)
                     (eval r env))]
    [(let-expr x e1 e2)
     (let ((v1 (eval e1 env)))
       (eval e2 (env-add x v1 env)))]
    [(variable x) (env-lookup x env)]
    [(if-expr b t e) (if (eval b env)
                         (eval t env)
                         (eval e env))]
    [(cons-expr l r)
     (let ((vl (eval l env))
           (vr (eval r env)))
       (cons vl vr))]
    [(car-expr p)      (car (eval p env))]
    [(cdr-expr p)      (cdr (eval p env))]
    [(pair?-expr p)    (pair? (eval p env))]
    [(null-expr)       'null]
    [(null?-expr e)    (eq? (eval e env) 'null)]
    [(symbol-expr v)   (val-symbol v)]
    [(lambda-expr xs f) (closure xs f env)]
    [(app-expr f es)    (let ((clos (eval f env))
                             (lis (map (lambda(x) (eval x env)) es)))
                             (match clos
                               [(closure xs f clo-env)
                                (if (= (length es) (length xs))
                                    (eval f (update-env clo-env lis xs))
                                    (error "bad arity"))]
                               [_ (error "error - bad input")]))]
    [(apply-expr f e)   (let ((clos (eval f env))
                             (lis (eval e env)))
                             (match clos
                               [(closure xs f clo-env)
                                (if (= (length-expr lis 0) (length xs))
                                    (eval f (update-expr-env clo-env lis xs))
                                    (error "bad arity"))]
                               [_ (error "error - bad input")]))]
    ))
(define (update-env env es xs)
  (if (null? xs)
      env
      (update-env (env-add (car xs) (car es) env) (cdr es) (cdr xs))))

(define (update-expr-env env es xs)
  (if (equal? 'null es)
      env
      (update-env (env-add (car xs) (car es) env) (cdr es) (cdr xs))))

(define (length-expr a b)
  (if (equal? 'null a)
      b
      (length-expr (cdr a) (+ b 1))))

(define (run e)
  (eval e (env-empty)))

(define test1-app
  (let-expr 'x (lambda-expr '(x y) (op '+ (variable 'x) (variable 'y)))
            (app-expr (variable 'x)(list (const 1)(const 1)))))
(define test2-app
  (let-expr 'x (lambda-expr '(x y) (op '* (variable 'x) (variable 'y)))
            (app-expr (variable 'x)(list (const 2)(const 2)))))
(define test3-app
  (let-expr 'x (lambda-expr '(x y) (op '/ (variable 'x) (variable 'y)))
            (app-expr (variable 'x)(list (const 9)(const 3)))))

(define test1-apply
  (let-expr 'x (lambda-expr '(x y) (op '+ (variable 'x) (variable 'y)))
            (apply-expr (variable 'x)(cons-expr (const 1)(cons-expr (const 1) (null-expr))))))
(define test2-apply
  (let-expr 'x (lambda-expr '(x y) (op '* (variable 'x) (variable 'y)))
            (apply-expr (variable 'x)(cons-expr (const 2)(cons-expr (const 2) (null-expr))))))
(define test3-apply
  (let-expr 'x (lambda-expr '(x y) (op '/ (variable 'x) (variable 'y)))
            (apply-expr (variable 'x)(cons-expr (const 9)(cons-expr (const 3) (null-expr))))))

(define test4-app
  (let-expr 'x (lambda-expr '(x y z) (op '+ (variable 'x) (op '+ (variable 'z) (variable 'y))))
            (app-expr (variable 'x)(list (const 1)(const 1)(const 1)))))
(define test5-app
  (let-expr 'x (lambda-expr '(x y z) (op '* (variable 'x) (op '* (variable 'z) (variable 'y))))
            (app-expr (variable 'x)(list (const 2)(const 2)(const 2)))))
(define test6-app
  (let-expr 'x (lambda-expr '(x y z) (op '/ (variable 'x) (op '/ (variable 'z) (variable 'y))))
            (app-expr (variable 'x)(list (const 9)(const 3)(const 9)))))

(define test4-apply
  (let-expr 'x (lambda-expr '(x y z) (op '+ (variable 'x) (op '+ (variable 'z) (variable 'y))))
            (apply-expr (variable 'x)(cons-expr (const 1)(cons-expr (const 1) (cons-expr (const 1) (null-expr)))))))
(define test5-apply
  (let-expr 'x (lambda-expr '(x y z) (op '* (variable 'x) (op '* (variable 'z) (variable 'y))))
            (apply-expr (variable 'x)(cons-expr (const 2)(cons-expr (const 2) (cons-expr (const 2) (null-expr)))))))
(define test6-apply
  (let-expr 'x (lambda-expr '(x y z) (op '/ (variable 'x) (op '/ (variable 'z) (variable 'y))))
            (apply-expr (variable 'x)(cons-expr (const 9)(cons-expr (const 3) (cons-expr (const 9) (null-expr)))))))

(define test7-app
  (let-expr 'x (lambda-expr '(x y z a) (op '+ (variable 'x) (op '+ (variable 'z) (op '+ (variable 'a) (variable 'y)))))
            (app-expr (variable 'x)(list (const 1)(const 1)(const 1)(const 1)))))
(define test8-app
  (let-expr 'x (lambda-expr '(x y z a) (op '* (variable 'x) (op '* (variable 'z) (op '* (variable 'a) (variable 'y)))))
            (app-expr (variable 'x)(list (const 2)(const 2)(const 2)(const 2)))))
(define test9-app
  (let-expr 'x (lambda-expr '(x y z a) (op '/ (variable 'x) (op '/ (variable 'z) (op '/ (variable 'a) (variable 'y)))))
            (app-expr (variable 'x)(list (const 9)(const 3)(const 9)(const 9)))))

(define test7-apply
  (let-expr 'x (lambda-expr '(x y z a) (op '+ (variable 'x) (op '+ (variable 'z) (op '+ (variable 'a) (variable 'y)))))
            (apply-expr (variable 'x)(cons-expr (const 1)(cons-expr (const 1) (cons-expr (const 1) (cons-expr (const 1) (null-expr))))))))
(define test8-apply
  (let-expr 'x (lambda-expr '(x y z a) (op '* (variable 'x) (op '* (variable 'z) (op '* (variable 'a) (variable 'y)))))
            (apply-expr (variable 'x)(cons-expr (const 2)(cons-expr (const 2) (cons-expr (const 2) (cons-expr (const 2) (null-expr))))))))
(define test9-apply
  (let-expr 'x (lambda-expr '(x y z a) (op '/ (variable 'x) (op '/ (variable 'z) (op '/ (variable 'a) (variable 'y)))))
            (apply-expr (variable 'x)(cons-expr (const 9)(cons-expr (const 3) (cons-expr (const 9) (cons-expr (const 9) (null-expr))))))))

(define own-language-tests		
  (test-suite
   "Tests of own language"
   (test-case
    "2 arguments"
    (and (equal? (eval test1-apply (env-empty)) 2)
         (equal? (eval test1-app (env-empty)) 2)
         (equal? (eval test2-apply (env-empty)) 4)
         (equal? (eval test2-app (env-empty)) 4)
         (equal? (eval test3-apply (env-empty)) 3)
         (equal? (eval test3-app (env-empty)) 3))
    )
   (test-case
    "3 arguments"
    (and (equal? (eval test4-apply (env-empty)) 3)
         (equal? (eval test4-app (env-empty)) 3)
         (equal? (eval test5-apply (env-empty)) 8)
         (equal? (eval test5-app (env-empty)) 8)
         (equal? (eval test6-apply (env-empty)) 3)
         (equal? (eval test6-app (env-empty)) 3))
    )
(test-case
    "4 arguments"
    (and (equal? (eval test7-apply (env-empty)) 4)
         (equal? (eval test7-app (env-empty)) 4)
         (equal? (eval test8-apply (env-empty)) 16)
         (equal? (eval test8-app (env-empty)) 16)
         (equal? (eval test9-apply (env-empty)) 3)
         (equal? (eval test9-app (env-empty)) 3))
    )
))
     

(run-test own-language-tests)