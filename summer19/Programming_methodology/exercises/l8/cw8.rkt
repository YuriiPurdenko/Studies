#lang racket
(provide (all-defined-out))

;; definicja wyrażeń z let-wyrażeniami i if-wyrażeniami

(struct variable (x)         #:transparent)
(struct const    (val)       #:transparent)
(struct op       (symb l r)  #:transparent)
(struct let-expr (x e1 e2)   #:transparent)
(struct if-expr  (b t e)     #:transparent)

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
    [_                  false]))

;; definicja instrukcji w języku WHILE

(struct skip      ()       #:transparent) ; skip
(struct comp      (s1 s2)  #:transparent) ; s1; s2
(struct assign    (x e)    #:transparent) ; x := e
(struct while     (b s)    #:transparent) ; while (b) s
(struct if-stm    (b t e)  #:transparent) ; if (b) t else e
(struct var-block (x e s)  #:transparent) ; var x := e in s

(define (stm? e)
  (match e
    [(skip) true]
    [(comp s1 s2)   (and (stm? s1) (stm? s2))]
    [(assign x e)   (and (symbol? x) (expr? e))]
    [(while b s)    (and (expr? b) (stm? s))]
    [(if-stm b t e) (and (expr? b) (stm? t) (stm? e))]
    [_ false]))
  
;; wyszukiwanie wartości dla klucza na liście asocjacyjnej
;; dwuelementowych list

(define (lookup x xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x) (cadar xs)]
    [else (lookup x (cdr xs))]))

;; aktualizacja środowiska dla danej zmiennej (koniecznie już
;; istniejącej w środowisku!)

(define (update x v xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x)
     (cons (list (caar xs) v) (cdr xs))]
    [else
     (cons (car xs) (update x v (cdr xs)))]))

;; kilka operatorów do wykorzystania w interpreterze

(define (op-to-proc x)
  (lookup x `((+ ,+)
              (* ,*)
              (- ,-)
              (/ ,/)
              (%, modulo)
              (> ,>)
              (>= ,>=)
              (< ,<)
              (<= ,<=)
              (= ,=)
              (!= ,(lambda (x y) (not (= x y)))) 
              (&& ,(lambda (x y) (and x y)))
              (|| ,(lambda (x y) (or x y)))
              )))

;; interfejs do obsługi środowisk

(define (env-empty) null)
(define env-lookup lookup)
(define (env-add x v env) (cons (list x v) env))
(define env-update update)
(define env-discard cdr)
(define (env-from-assoc-list xs) xs)

;; ewaluacja wyrażeń ze środowiskiem

(define (eval e env)
  (match e
    [(const n) n]
    [(op s l r) ((op-to-proc s) (eval l env)
                                (eval r env))]
    [(let-expr x e1 e2)
     (let ((v1 (eval e1 env)))
       (eval e2 (env-add x v1 env)))]
    [(variable x) (env-lookup x env)]
    [(if-expr b t e) (if (eval b env)
                         (eval t env)
                         (eval e env))]))

;; interpretacja programów w języku WHILE, gdzie środowisko m to stan
;; pamięci. Interpreter to procedura, która dostaje program i początkowy
;; stan pamięci, a której wynikiem jest końcowy stan pamięci. Pamięć to
;; aktualne środowisko zawierające wartości zmiennych

(define (interp p m)
  (match p
    [(skip) m]
    [(comp s1 s2) (interp s2 (interp s1 m))]
    [(assign x e)
     (env-update x (eval e m) m)]
    [(while b s)
     (if (eval b m)
         (interp p (interp s m))
         m)]
    [(var-block x e s)
     (env-discard
       (interp s (env-add x (eval e m) m)))]
    [(if-stm b t e) (if (eval b m)
                        (interp t m)
                        (interp e m))]))

;;ZAD 1 ----------------------------------------------------------------------------------------------------------

#|
(define (cycle xs)
  (define (cycle-it xs sol)
    (if (null? xs)
        (mcar sol)
        (mcons (car xs) (cycle-it (cdr xs) ( 
|#

;;ZAD 2 ----------------------------------------------------------------------------------------------------------

(define (set-nth! n xs v)
  (cond [(null? xs) "error"]
        [(= n 1) (set-mcar! xs v)]
        [else (set-nth! (- n 1) (mcdr xs) v)]))

;;ZAD 3 ----------------------------------------------------------------------------------------------------------

(define Fibonacci
  (var-block 'x 'i
             (var-block 'a (const 0)
                        (var-block 'b (const 1)
                                   (while (op '> (variable 'x) (const 0))
                                          (var-block 'c (op '+ (variable 'b) (variable 'a))
                                                     (comp (assign 'b (variable 'a))
                                                           (comp (assign 'a (variable 'c))
                                                                 (comp (assign 'x (op '- (variable 'x) (const 1)))
                                                                       (assign 'i (variable 'a)))))))))))
(define (Fibonacci-WHILE n)
  (env-lookup 'i (interp Fibonacci (env-from-assoc-list (cons 'i n)))))

;;ZAD 4 ----------------------------------------------------------------------------------------------------------

;Widac ze jest gorliwa

;;ZAD 5 ----------------------------------------------------------------------------------------------------------

(struct incr (x)) ;x++

#|

stm?:
...
[(incr x) (symbol? x)]


interp:
[(incr x)
(env-update x (eval (op '+ (const 1) (variable 'x)) m) m)]
;;albo        (+ (eval (variable 'x) m) 1) m)]

|#

;;ZAD 6 ----------------------------------------------------------------------------------------------------------

#|
(struct for-loop (x e1 e2 s1 s2))  ; for (x:=e1;e2;s1) s2

stm?:

[(for x e1 e2 s1 s2) (and (symbol? x) (expr? e1) (expr? e1) (stm? s1) (stm? s2))]

interp:

[(for x e1 e2 s1 s2) (env-discard (interp (while e2 (comp sw s1)) (env-add x (eval e1))))]
|#

;;ZAD 7 ----------------------------------------------------------------------------------------------------------

(define (for x e1 e2 s1 s2)
  (var-block x e1
             (while e2 (comp s2 s1))))

;;ZAD 8 ----------------------------------------------------------------------------------------------------------

(define (gen-symbol s i)
  (string- >symbol (string-append s (number- >string i))))

(define (gen-prolog n k c)
  (if (< n k)
      (comp (assign 'done false) c)
      (gen-prolog n (+ k 1) (comp (assign (gen-symbol 'x k)) c))))

(define (gen-for k n)
  (if (< n k)
      (gen-epilog n)
      (for (gen-symbol 'y k) (const 1) (op '&& (op '<= (variable (gen-symbol 'y k))
                                                       (tutaj cos powinno byc))
                                               (op '= (variable 'done) false))
        (inc (gen-symbol 'y k)))
      (gen-if k)))

(define (gen-epilog n (k 1) k (skip))
  (if (> k n)
      (comp (assign 'done true) c)
      (gen-epilog n (+ k 1) (comp (assign (gen-symbol x k)
                                          (variable (gen-symbol y k)))))))
(define (gen-if n (k 1) (c true))
  (if (> k n)
      (if c (gen-for (+ k 1) n)
            (op '&& (op '!= (variable (gen-symbol 'y k))
                            (variable (gen-symbol 'y i)))
                    (op '!=  (abs (variable (gen-symbol 'y k))
                                  (variable (gen-symbol 'y i)))
                        (const (- k (- i 1))))))
      c))

(define (abs x)
  (if-exp (op '< x (const 0))
          (op '- 0 x)
          x))
                                       


                                                     
