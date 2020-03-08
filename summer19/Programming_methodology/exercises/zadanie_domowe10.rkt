#lang racket

;;KOD Z WYKŁADU WRAZ Z POPRAWIONYM WHILEM

(provide (all-defined-out))
(require rackunit)
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


(define (oldlookup x xs)	;;LOOKUP Z WYKŁADU
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x) (cadar xs)]
    [else (oldlookup x (cdr xs))]))


(define (lookup x mxs)		;;LOOKUP DZIAŁAJĄCY NA MLISCIE MCONSOW
  (cond
    [(null? mxs)
     (error x "unknown identifier :(")]
    [(eq? (mcar (mcar mxs)) x) (mcdr (mcar mxs))]
    [else (lookup x (mcdr mxs))]))

;; aktualizacja środowiska dla danej zmiennej (koniecznie już
;; istniejącej w środowisku!)

(define (update x v mxs)	;;UPDATE DZIALAJACY NA MLISCIE MCONSOW
  (cond
    [(null? mxs)
     (error x "unknown identifier :(")]
    [(eq? (mcar (mcar mxs)) x)
     (begin (set-mcdr! (mcar mxs) v) mxs)]
    [else
     (mcons (mcar mxs) (update x v (mcdr mxs)))]))

;; kilka operatorów do wykorzystania w interpreterze

(define (op-to-proc x)		
  (oldlookup x `((+ ,+)
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

(define (env-empty) null)		;;INTERFEJS DOSTOSOWANY DO MLISTY MCONSOW
(define env-lookup lookup)
(define (env-add x v env) (mcons (mcons x v) env))
(define env-update update)
(define env-discard mcdr)
(define (env-from-assoc-list xs)
  (if (null? xs)
      xs
      (mcons (mcons (caar xs) (second (car xs))) (env-from-assoc-list (cdr xs)))))

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

;; silnia zmiennej i

(define fact-in-WHILE
  (var-block 'x (const 0)                                           ; var x := 0 in
     (comp (assign 'x (const 1))                                    ;   x := 1
     (comp (while (op '> (variable 'i) (const 0))                   ;   while (i > 0)
              (comp (assign 'x (op '* (variable 'x) (variable 'i))) ;     x := x * i
                    (assign 'i (op '- (variable 'i) (const 1)))))   ;     i := i - 1
           (assign 'i (variable 'x))))))                            ;   i := x

(define (factorial n)
  (env-lookup 'i (interp fact-in-WHILE
                         (env-from-assoc-list `((i ,n))))))
(define (fact-iter n i c)
  (if (> i n)
      c
      (fact-iter n (+ i 1) (* i c))))

(define (fact-it n)
  (fact-iter n 1 1))
;; najmniejsza liczba pierwsza nie mniejsza niż i

(define find-prime-in-WHILE
 (var-block 'c (variable 'i)                                         ; var c := i in
 (var-block 'continue (const true)                                   ; var continue := true in
 (comp
  (while (variable 'continue)                                        ; while (continue)
   (var-block 'is-prime (const true)                                 ;   var is-prime := true in
   (var-block 'x (const 2)                                           ;   var x := 2 in
    (comp
     (while (op '&& (variable 'is-prime)                             ;   while (is-prime &&
                    (op '< (variable 'x) (variable 'c)))             ;            x < c)
        (comp (if-stm (op '= (op '% (variable 'c) (variable 'x))     ;     if (c % x =
                             (const 0))                              ;                 0)
                      (assign 'is-prime (const false))               ;       is-prime := false
                      (skip))                                        ;     else skip
              (assign 'x (op '+ (variable 'x) (const 1)))))          ;     x := x + 1 
     (if-stm (variable 'is-prime)                                    ;   if (is-prime)
             (assign 'continue (const false))                        ;     continue := false
             (comp (assign 'continue (const true))                   ;   else continue := true
                   (assign 'c (op '+ (variable 'c) (const 1))))))))) ;        c := c + 1
  (assign 'i (variable 'c))))))                                      ; i := c

(define (find-prime-using-WHILE n)
  (env-lookup 'i (interp find-prime-in-WHILE
                         (env-from-assoc-list `((i ,n) (is-prime ,true))))))          

(define (find-prime-native n)
  (define (is-prime c isp x)
    (if (and isp (< x c))
      (if (= (modulo c x) 0)
          (is-prime c false (+ x 1))
          (is-prime c isp   (+ x 1)))
      isp))
  (if (is-prime n true 2)
      n
      (find-prime-native (+ n 1))))


					;;TESTY SPRAWDZAJACY POPRAWNOSC DZIALANIE WCZESNIEJ ZAIMPLEMENTOWANYCH FUNKCJI 
(define better-while-tests		;;W WHILE W SRODOWISKU NA MLISCIE MCONSOW
  (test-suite
   "Tests of better WHILE implementation"
   (test-case
    "Factorial"
    (and
     (equal? (factorial 5) (fact-it 5))
     (equal? (factorial 10) (fact-it 10))
     (equal? (factorial 50) (fact-it 50))
     (equal? (factorial 100) (fact-it 100))
     (equal? (factorial 500) (fact-it 500))
     (equal? (factorial 1000) (fact-it 1000))
     (equal? (factorial 5000) (fact-it 5000))
     (equal? (factorial 10000) (fact-it 10000))))
   (test-case
    "Prime"
    (and
     (equal? (find-prime-using-WHILE 10) (find-prime-native 10))
     (equal? (find-prime-using-WHILE 50) (find-prime-native 50))
     (equal? (find-prime-using-WHILE 100) (find-prime-native 100))
     (equal? (find-prime-using-WHILE 500) (find-prime-native 500))
     (equal? (find-prime-using-WHILE 1000) (find-prime-native 1000))
     (equal? (find-prime-using-WHILE 5000) (find-prime-native 5000))
     (equal? (find-prime-using-WHILE 10000) (find-prime-native 10000))))))
     

(run-test better-while-tests)

