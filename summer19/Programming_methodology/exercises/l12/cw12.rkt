#lang racket
(require quickcheck)
(require racket/contract)

(define (average z y)
  (/ (+ z y) 2))

(define (square x)
  (* x x))

(define (dist x y)
  (abs (- x y)))

(define (close-enough? x y) (< abs (- x y) 0.00001))

(define (sqrt x)
(->i ([x (and/c real? positive?)])
      [result (x) (lambda (r) (close-enough? (* r r) x))])
  ;; lokalne definicje
  ;; poprawianie przybliżenia
  (define (improve guess)
    (average guess (/ x guess)))
  ;; sprawdzanie czy przybliżenie jest wystarczająco dobre
  (define (good-enough? g)
    (< (dist x (square g))
       0.0001))
  ;; procedura iterująca poprawianie przybliżenia aż
  ;; osiągniemy wystarczająco dobre przybliżenie
  (define (iter guess)
    (if (good-enough? guess)
        guess
        (iter (improve guess))))
  
  (iter 1.0))

;ZAD 1 ------------------------------------------------------------------------------------------------------------------------------

(define sqrt/c
  (->i ([x (and/c real? positive?)])
       [result (x) (lambda (r) (close-enough? (* r r) x))]))
;;jest powyzej zaimplementowany

;ZAD 2 ------------------------------------------------------------------------------------------------------------------------------

(define filt/c
  (parametric->/c (a) (->i [(p? [-> a any/c]) (l [listof a])]
                         [res (p?) (and/c
                                    [listof p?]
                                    [listof a])])))


;ZAD 3 ------------------------------------------------------------------------------------------------------------------------------

(define-signature  monoid^
  (( contracted
     [elem?    (-> any/c  boolean?)]
     [neutral  elem?]
     [oper     (-> elem? elem? elem?)])))

(define-unit monoid-int@
  (import)
  (export monoid^)
  (define elem? integer?)
  (define neutral 0)
  (define oper +))

(define-unit monoid-list@
  (import)
  (export monoid^)
  (define elem? list?)
  (define neutral null)
  (define (oper a b) (append a b)))

(define-values/invoke-unit/infer monoid-int@)

;(define-values/invoke-unit/infer monoid-list@)

(quickcheck
 (property ((x arbitrary-integer))
           (and (= x (oper x neutral))
                (= x (oper neutral x)))))
#|
(quickcheck
 (property ((x arbitrary-integer)
            (y arbitrary-integer)
            (z arbitrary-integer))
           (= (oper x (oper y z))
              (oper (oper x y) z))))
|#
;losowanie listy integerow
;(x arbitart-list
;   arbitary-int)
           
;ZAD 4 ------------------------------------------------------------------------------------------------------------------------------


;ZAD 5 ------------------------------------------------------------------------------------------------------------------------------

#|
(define-signature set^
  (contracted
   [set? (list?)]
   [contains? (-> set? bumber? boolean?)]
   [empty? (-> set? boolean?)]
   [make (-> number? set?)]
   [union (-> set? set? set?)]
   [intersection (-> set? set? set?)]))

(define-unit set-int@
  (import)
  (export set^)
  (define (contains? s n)
    (member n s))
  (define (empty? s)
    (null? s))
  (define (make n)
    (list n))
  (define (union s t))
  (define (insert s n)
    (cond [(empty? s) (list n)]
          [(= (car s) n) s]
          [else (cons (car s) (insert (cdr s) n))]
          (fold (insert s t))))
  (define (intersection s t)
    (define (f n)
      (contains? s n)
      (filter f t))))
|#

;ZAD 6 ------------------------------------------------------------------------------------------------------------------------------
#|
(quickcheck
 (propert ([x arbitrary-int])
          (not (contains? empty x))))

(quickcheck
 (property ([x arbitrary-int]
            [y arbitrary-int])
           (or (and (= x y) (contains? y (make x)))
               (not (contains? y make x)))))
(quickcheck
 (property ([s (arbitrary-list arbitrary-int)]
            [t (arbitrary-list arbitrary-int)])
           (let ((u (union s t)))
             (or (andmap (lambda (n) (contains? u n))
                         (append s t))
                 (andmap (lambda (n) (or (contains? s n)
                                         (contains s u)))
                         u)))))
|#
