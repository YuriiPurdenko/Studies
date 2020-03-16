#lang racket

(provide (all-defined-out))

;; definicja wyrażeń

(struct variable     (x)        #:transparent)
(struct const        (val)      #:transparent)
(struct op           (symb l r) #:transparent)
(struct let-expr     (x e1 e2)  #:transparent)
(struct letrec-expr  (x e1 e2)  #:transparent)
(struct if-expr      (b t e)    #:transparent)
(struct cons-expr    (l r)      #:transparent)
(struct car-expr     (p)        #:transparent)
(struct cdr-expr     (p)        #:transparent)
(struct pair?-expr   (p)        #:transparent)
(struct null-expr    ()         #:transparent)
(struct null?-expr   (e)        #:transparent)
(struct symbol-expr  (v)        #:transparent)
(struct symbol?-expr (e)        #:transparent)
(struct lambda-expr  (x b)      #:transparent)
(struct app-expr     (f e)      #:transparent)
(struct set!-expr    (x v)      #:transparent)

;; wartości zwracane przez interpreter

(struct val-symbol (s)   #:transparent)
(struct closure (x b e)) ; Racket nie jest transparentny w tym miejscu,
                         ; to my też nie będziemy
(struct blackhole ()) ; lepiej tzrymać się z daleka!

;; wyszukiwanie wartości dla klucza na liście asocjacyjnej
;; dwuelementowych list

(define (lookup x xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x) (cadar xs)]
    [else (lookup x (cdr xs))]))

(define (mlookup x xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (mcar (mcar xs)) x)
     (match (mcar (mcdr (mcar xs)))
       [(blackhole) (error "Stuck in a black hole :(")]
       [x x])]
    [else (mlookup x (mcdr xs))]))

(define (mupdate! x v xs)
  (define (update! ys)
    (cond
      [(null? ys) (error x "unknown identifier :(")]
      [(eq? x (mcar (mcar ys)))
       (set-mcar! (mcdr (mcar ys)) v)]
      [else (update! (mcdr ys))]))
  (begin (update! xs) xs))

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
              (% ,modulo)
              (!= ,(lambda (x y) (not (= x y)))) 
              (&& ,(lambda (x y) (and x y)))
              (|| ,(lambda (x y) (or x y)))
              (eq? ,(lambda (x y) (eq? (val-symbol-s x)
                                       (val-symbol-s y))))
              )))

;; interfejs do obsługi środowisk

(define (env-empty) null)
(define env-lookup mlookup)
(define (env-add x v env)
  (mcons (mcons x (mcons v null)) env))
(define env-update! mupdate!)

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
    [(letrec-expr x e1 e2)
     (let* ((new-env (env-add x (blackhole) env))
            (v1 (eval e1 new-env)))
       (eval e2 (env-update! x v1 new-env)))]
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
    [(lambda-expr x b) (closure x b env)]
    [(app-expr f e)    (let ((vf (eval f env))
                             (ve (eval e env)))
                         (match vf
                           [(closure x b c-env)
                            (eval b (env-add x ve c-env))]
                           [_ (error "application: not a function :(")]))]
    [(set!-expr x e)
     (env-update! x (eval e env) env)]
    ))

(define (run e)
  (eval e (env-empty)))

;; przykład

(define fact-in-expr
  (letrec-expr 'fact (lambda-expr 'n
     (if-expr (op '= (const 0) (variable 'n))
              (const 1)
              (op '* (variable 'n)
                  (app-expr (variable 'fact)
                            (op '- (variable 'n)
                                   (const 1))))))
     (app-expr (variable 'fact)
               (const 5))))



;;ZAD 1 ----------------------------------------------------------------------------------------------------

;AD1

;(let (x 5) (lambda (z) (let (y 5) (+ x y z))))
;(closure x b env)
;env->((x 5))

;(closure 'z (let (y 5) (+ x y z)) ((x 5)))

;;AD2

;(let (x 5) (lambda (x) (let (y 5) (+ x y))))

;env->((x 5))

;(closure 'x (let (y 5) (+ x y))) ((x 5)))

;;AD3

;((lambda (x) (lambda (y) (+ x y))) 10)

  ;1* - dla pierwszej lambdy
      ;(closure 'x (lambda (y) (+ x y)) ())
  ;2* -dla drugiej - (lambda (y) (+ x y))
      ;(closure 'y  (+ x y) (x 10))


;;ZAD 2 ----------------------------------------------------------------------------------------------------


;;REVERSE
(define rev
(letrec-expr 'reverse-app
             (lambda-expr 'tail
                          (lambda-expr 'xs
                                       (if-expr (null?-expr (variable 'xs))
                                                (variable 'tail)
                                                 (app-expr (app-expr (variable 'reverse-app)
                                                                     (cons-expr (car-expr (variable 'xs))
                                                                           (variable 'tail)))
                                                           (cdr-expr (variable 'xs))))))
             (app-expr (variable 'reverse-app) (null-expr))))
;;(eval (app-expr rev 
#|
APPEND - robisz reverse dwa razy i laczysz dzieki temu listy :D

MAP

(lambda-expr 'fu
             (letrec-expr 'map2 (lambda-expr 'xs
                                             (if-expr (null?-expr (variable 'xs))
                                                      (null-expr)
                                                      (cons-expr (app-expr (variable 'fu)
                                                                           (car-expr (variable 'xs)))
                                                                 (app-expr (variable 'map2)
                                                                           (cdr-expr (variable 'xs))))))
                          (variable 'map2)))
|#
                                                            
                                                  


;;ZAD 3 ----------------------------------------------------------------------------------------------------

(define (free-vars vs e)
  (match e
    [(variable x) (if [member x vs]
                      null
                      (list x))]
    [(let-expr x e1 e2) (append (free-vars vs e1)
                                (free-vars (cons x vs) e2))]
    [(letrec-expr x e1 e2) (append (free-vars (cons x vs) e1)
                                   (free-vars (cons x vs) e2))]
    [(lambda-expr x e1) (free-vars (cons x vs) e1)]
    [_ (let ((es (match e
                   [(if-expr e1 e2 e3) (list e1 e2 e3)]
                   [(cons-expr e1 e2) (list e1 e2)]
                   [(pair?-expr e1) (list e1)])))
         (append-map (curry free-vars vs) e))]))

;;w miejscu do wstawiania srodowiska do domkniecia
;;(let ((Fvs (free-vars '() b))) (filter [lambda (xs) (member [first xs] fvs)] env))

;;ZAD 4 ----------------------------------------------------------------------------------------------------

;; Mamy dwa program p1 i p2 najpierw nalezy sprawdzic czy jest to sama struktura

#|
(define (alfaR p1 p2 vs)
  (match p1
    [(variable x) (match p2
                    [(variable y) (let ((find n-lookup x vs)) (if find
                                                                  (eq? find y)
                                                                  (eq? x y)))]
                    [_ false])]
    [(let-expr x a1 a2) (match p2
                          [let-expr y b1 b2] (and (alfaR a1 b1 vs)

                                                  (alfaR a2 b2 (cons list x y) vs)))
                        |#



;;ZAD 5 ----------------------------------------------------------------------------------------------------
;;INDEKSY DE BRUIJNA

(define (transform e [i 0])
  (match e
