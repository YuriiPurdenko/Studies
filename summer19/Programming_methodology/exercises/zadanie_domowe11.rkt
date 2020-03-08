#lang racket

;;FUNKCJE Z PREFIKSEM OLD ODNASZA SIE DO POPRZEDNIEJ IMPLEMENTACJI WHILE NA LISTACH 
;;FUNKCJE BEZ PREFIKSA OLD ODNOSZA SIE DO IMPLEMENTACJI WHILE NA MLISCIE MCONSOW

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

(define (oldlookup x xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x) (cadar xs)]
    [else (oldlookup x (cdr xs))]))

(define (lookup x mxs)
  (cond
    [(null? mxs)
     (error x "unknown identifier :(")]
    [(eq? (mcar (mcar mxs)) x) (mcdr (mcar mxs))]
    [else (lookup x (mcdr mxs))]))

;; aktualizacja środowiska dla danej zmiennej (koniecznie już
;; istniejącej w środowisku!)

(define (oldupdate x v xs)
  (cond
    [(null? xs)
     (error x "unknown identifier :(")]
    [(eq? (caar xs) x)
     (cons (list (caar xs) v) (cdr xs))]
    [else
     (cons (car xs) (oldupdate x v (cdr xs)))]))

(define (update x v mxs)
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

(define (env-empty) null)
(define oldenv-lookup oldlookup)
(define (oldenv-add x v env) (cons (list x v) env))
(define oldenv-update oldupdate)
(define oldenv-discard cdr)
(define (oldenv-from-assoc-list xs) xs)

(define env-lookup lookup)
(define (env-add x v env) (mcons (mcons x v) env))
(define env-update update)
(define env-discard mcdr)
(define (env-from-assoc-list xs)
  (if (null? xs)
      xs
      (mcons (mcons (caar xs) (second (car xs))) (env-from-assoc-list (cdr xs)))))

;; ewaluacja wyrażeń ze środowiskiem

(define (oldeval e env)
  (match e
    [(const n) n]
    [(op s l r) ((op-to-proc s) (oldeval l env)
                                (oldeval r env))]
    [(let-expr x e1 e2)
     (let ((v1 (oldeval e1 env)))
       (oldeval e2 (oldenv-add x v1 env)))]
    [(variable x) (oldenv-lookup x env)]
    [(if-expr b t e) (if (oldeval b env)
                         (oldeval t env)
                         (oldeval e env))]))

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

(define (oldinterp p m)
  (match p
    [(skip) m]
    [(comp s1 s2) (oldinterp s2 (oldinterp s1 m))]
    [(assign x e)
     (oldenv-update x (oldeval e m) m)]
    [(while b s)
     (if (oldeval b m)
         (oldinterp p (oldinterp s m))
         m)]
    [(var-block x e s)
     (oldenv-discard
       (oldinterp s (oldenv-add x (oldeval e m) m)))]
    [(if-stm b t e) (if (oldeval b m)
                        (oldinterp t m)
                        (oldinterp e m))]))

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

(define (oldfactorial n)
  (oldenv-lookup 'i (oldinterp fact-in-WHILE
                         (oldenv-from-assoc-list `((i ,n))))))

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
     
(define (oldfind-prime-using-WHILE n)
  (oldenv-lookup 'i (oldinterp find-prime-in-WHILE
                         (oldenv-from-assoc-list `((i ,n) (is-prime ,true))))))
;; porownajmy wydajnosc!

;; ten sam algorytm do wyszukiwania liczby pierwszej nie mniejszej niż n
;; zapisany funkcyjnie jest dosc brzydki, ale odpowiada temu zapisanemu w WHILE


(define (test)
  (begin
    (display "wait...\n")
    (flush-output (current-output-port))
    (test-performance)))

(define (test-performance)
  (define xs (oldprimetimer-iter  0 0 0 100 10000))
  (define xs2 (primetimer-iter  0 0 0 100 10000))
  (display "FOR SMALL \n old WHILE time  (cpu, real, gc): ")
          (display (exact->inexact (/ (first xs) 10000)))  (display ", ")
          (display (exact->inexact (/ (second xs) 10000))) (display ", ")
          (display (exact->inexact (/ (third xs) 10000)))  (display "\n")
          (display "WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first xs2) 10000)))  (display ", ")
          (display (exact->inexact (/ (second xs2) 10000))) (display ", ")
          (display (exact->inexact (/ (third xs2) 10000)))   (display "\n")
  (define xxs (oldprimetimer-iter 0 0 0 10000 1000))
  (define xxs2 (primetimer-iter 0 0 0 10000 1000))
  (display "FOR MEDIUM \n old WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first xxs) 1000)))  (display ", ")
          (display (exact->inexact (/ (second xxs) 1000))) (display ", ")
          (display (exact->inexact (/ (third xxs) 1000)))   (display "\n")
          (display "WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first xxs2) 1000)))  (display ", ")
          (display (exact->inexact (/ (second xxs2) 1000))) (display ", ")
          (display (exact->inexact(/ (third xxs2) 1000)))   (display "\n")
  (define ys (oldprimetimer-iter 0 0 0 1000000 100))
  (define ys2 (primetimer-iter 0 0 0 1000000 100))
  (display "FOR LARGE \n old WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first ys) 100)))  (display ", ")
          (display (exact->inexact (/ (second ys) 100))) (display ", ")
          (display (exact->inexact (/ (third ys) 100)))   (display "\n")
          (display "WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first ys2) 100)))  (display ", ")
          (display (exact->inexact (/ (second ys2) 100))) (display ", ")
          (display (exact->inexact(/ (third ys2) 100)))   (display "\n"))

(define (oldprimetimer-iter a b c x n)
  (if (= n 0)
      (list a b c)
      (let-values
            (((r1 cpu1 real1 gc1) (time-apply oldfind-prime-using-WHILE (list x))))
        (oldprimetimer-iter (+ a cpu1) (+ b real1) (+ c gc1) x (- n 1)))))

(define (primetimer-iter d e f x n)
  (if (= n 0)
      (list d e f)
      (let-values
            (((r2 cpu2 real2 gc2) (time-apply find-prime-using-WHILE (list x))))
        (primetimer-iter (+ d cpu2) (+ e real2) (+ f gc2) x (- n 1)))))
  
 

(define (test2)
  (begin
    (display "wait...\n")
    (flush-output (current-output-port))
    (test-performance2)))

(define (test-performance2)
  
  (define xs (oldfactorialtimer-iter  0 0 0 100 10000))
  (define xs2 (factorialtimer-iter  0 0 0 100 10000))
  (display "FOR SMALL \n old WHILE time  (cpu, real, gc): ")
          (display (exact->inexact (/ (first xs) 10000)))  (display ", ")
          (display (exact->inexact (/ (second xs) 10000))) (display ", ")
          (display (exact->inexact (/ (third xs) 10000)))  (display "\n")
          (display "WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first xs2) 10000)))  (display ", ")
          (display (exact->inexact (/ (second xs2) 10000))) (display ", ")
          (display (exact->inexact (/ (third xs2) 10000)))   (display "\n")
  (define xxs (oldfactorialtimer-iter 0 0 0 10000 1000))
  (define xxs2 (factorialtimer-iter 0 0 0 10000 1000))
  (display "FOR MEDIUM \n old WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first xxs) 1000)))  (display ", ")
          (display (exact->inexact (/ (second xxs) 1000))) (display ", ")
          (display (exact->inexact (/ (third xxs) 1000)))   (display "\n")
          (display "WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first xxs2) 1000)))  (display ", ")
          (display (exact->inexact (/ (second xxs2) 1000))) (display ", ")
          (display (exact->inexact(/ (third xxs2) 1000)))   (display "\n")
  (define ys (oldfactorialtimer-iter 0 0 0 100000 10))
  (define ys2 (factorialtimer-iter 0 0 0 100000 10))
  (display "FOR LARGE \n old WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first ys) 10)))  (display ", ")
          (display (exact->inexact (/ (second ys) 10))) (display ", ")
          (display (exact->inexact (/ (third ys) 10)))   (display "\n")
          (display "WHILE time (cpu, real, gc): ")
          (display (exact->inexact (/ (first ys2) 10)))  (display ", ")
          (display (exact->inexact (/ (second ys2) 10))) (display ", ")
          (display (exact->inexact(/ (third ys2) 10)))   (display "\n"))

(define (oldfactorialtimer-iter a b c x n)
  (if (= n 0)
      (list a b c)
      (let-values
            (((r1 cpu1 real1 gc1) (time-apply oldfactorial (list x))))
        (oldfactorialtimer-iter (+ a cpu1) (+ b real1) (+ c gc1) x (- n 1)))))

(define (factorialtimer-iter d e f x n)
  (if (= n 0)
      (list d e f)
      (let-values
            (((r2 cpu2 real2 gc2) (time-apply factorial (list x))))
        (factorialtimer-iter (+ d cpu2) (+ e real2) (+ f gc2) x (- n 1)))))

#|
Aby można było wiarygodnie ocenić poprawę w szybkości działania programu należy uwzględnić średni czas działania
programu na tych samych zmiennych oraz sprzęcie. Powyżej uruchamiane były testy (test) i (test2) dla małej wartości 100 średniej 10000 oraz dużej 1000000 liczba
uruchuchomień jest malejąca, ponieważ im większy ten tym teoretycznie powinna być większa rozbieżność o ile taka istnieje, a także z powodu ograniczeń sprzętowych.
Po uruchomieniu test, otrzymałem output:

FOR SMALL 
 old WHILE time  (cpu, real, gc): 0.6412, 0.6393, 0.1664
WHILE time (cpu, real, gc): 0.6928, 0.5993, 0.1444
FOR MEDIUM 
 old WHILE time (cpu, real, gc): 51.736, 51.56, 8.116
WHILE time (cpu, real, gc): 45.232, 45.09, 2.596
FOR LARGE 
 old WHILE time (cpu, real, gc): 4405.84, 4394.34, 261.88
WHILE time (cpu, real, gc): 4376.72, 4362.56, 193.48

Można zauważyć, że dla małych wartości działa około 7% procent wolniej zaś dla średnich jest 14% szybszy, lecz dla dużych danych czas już jest bardzo zbliżony.

Podobnie dla drugiego testu (z wyjątkiem dużego testu, który działa 4% wolniej)

FOR SMALL 
 old WHILE time  (cpu, real, gc): 0.2656, 0.267, 0.0044
WHILE time (cpu, real, gc): 0.3396, 0.3389, 0.0068
FOR MEDIUM 
 old WHILE time (cpu, real, gc): 60.16, 59.977, 10.496
WHILE time (cpu, real, gc): 52.6, 52.42, 3.48
FOR LARGE 
 old WHILE time (cpu, real, gc): 10551.6, 10514.7, 2138.0
WHILE time (cpu, real, gc): 11026.0, 10987.5, 2758.0

Można postawić wniosek, że od pewnego rozmiaru danych warto zacząć używać nowego środowiska, a dla małych obliczeń 
korzystać z początkowej wersji.

|#

