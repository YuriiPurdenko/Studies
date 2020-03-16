#lang racket
(define (var? t)
  (symbol? t))

(define (neg? t)
  (and (list? t)
       (= 2 (length t))
       (eq? 'neg (car t))))

(define (conj? t)
  (and (list? t)
       (= 3 (length t))
       (eq?'conj (car t))))

(define (disj? t)
  (and (list? t)
       (= 3 (length t))
       (eq?'disj (car t))))

;;ZAD 1 ----------------------------------------------------------------------------------------------------

(define (prop? f)
  (or (var? f)
      (and (neg? f)
           (prop? (neg-subf f)))
      (and (disj? f)
           (prop? (disj-left f))
           (prop? (disj-right f)))
      (and (conj? f)
           (prop? (conj-left f))
           (prop? (conj-right f)))))

(define (conj f p)
  (list 'conj f p))

(define (disj f p)
  (list 'disj f p))

(define (neg f)
  (list 'neg f ))

(define (conj-left t)
  (second t))

(define (conj-right t)
  (third t))

(define (disj-left t)
  (second t))

(define (disj-right t)
  (third t))

(define (neg-subf t)
  (second t))

;;ZAD 2 ----------------------------------------------------------------------------------------------------
(define (free-vars t)
  (cond ((var? t) (list t))
        ((neg? t) (free-vars (neg-subf t)))
        ((conj? t) (remove-duplicates (append (free-vars (conj-left t)) (free-vars (conj-right t)))))
        ((disj? t) (remove-duplicates (append (free-vars (disj-left t)) (free-vars (disj-right t)))))
        (else false)))

(free-vars (conj (disj 'x 'y) (disj 'x 't)))

;;ZAD 3 ----------------------------------------------------------------------------------------------------

(define (gen-vals  xs)
  (if(null? xs)
     (list  null)
     (let*((vss   (gen-vals (cdr xs)))
           (x     (car xs))
           (vst   (map (lambda(vs) (cons (list x true)   vs)) vss))
           (vsf   (map (lambda(vs) (cons (list x false) vs)) vss)))
       (append  vst  vsf))))

(define (eval-var f vals)
  (let ((val (findf (lambda (x) (= f (car x))) vals)))
             (if (eq? val false)
                 'false
                 (cdr val))))

(define (eval-form f vals)
  (cons [(var? f) (eval-var f vals)]
        [(neg? f) (let ((val (eval-form (neg-subf vals))))
                    (if (eq? val 'false)
                        'false
                        (not val)))]
        [(disj f) (let ((val1 (eval-form (disj-left vals)))
                        (val2 (eval-form (disj-right vals))))
                    (if (or (eq? val1 'false)
                            (eq? val2 'false))
                        'false
                        (or val1 val2)))]))

(define (falsifable-eval? f)
  (define (iter vals)
    (if (null? vals)
        false
        (if (eval-form f (car vals))
            (iter (cdr vals))
            (car vals))))
  (iter (gen-vals (free-vars f))))

;;ZAD 4 ----------------------------------------------------------------------------------------------------

(define (nnf? f)
  (cond
    [(conj? f) (and (nnf? (conj-left f))
                    (nnf? (conj-right f)))]
    [(disj? f) (and (nnf? (conj-left f))
                    (nnf? (conj-right f)))]
    [(neg? f) (var? (neg-subf f))]
    [else (var? f)]))

(define (lit p? v)
  (list (lit p? v)))
#|
(define (lit p? v)
  (if p?
      v
      (neg v)))
|#
;;ZAD 5 ----------------------------------------------------------------------------------------------------

(define (convert-to-nnf f)
  (define (neg-convert f)
    (cond
      [(var? f) (neg f)]
      [(neg? f) (convert-to-nnf (neg-subf f))]
      [(conj? f) (disj (neg-convert (conj-left))
                       (neg-convert (conj-right)))]
      [(disj? f) (conj (neg-convert (disj-left))
                       (neg-convert (disj-right)))]))
  (cond
    [(var? f) f]
    [(neg? f) (neg-convert (neg-subf f))]
    [(conj? f) (conj (convert-to-nnf (conj-left f)))]
    [(disj? f) (disj (convert-to-nnf (disj-left f)))]))


#|
DOWOD

ZAL

DLA KAZDEGO P (PROP? P) CZYLI P JEST FORMULA
(NNF? (CONVERT-TO-NNF P))
(NNF? (NEG-CONVERT P))

ROZW

PRZYPADEK ZE ROWY
(VAR? P)  (CONVERT-TO-NNF P)==P
          (NEG-CONVERT P)==(NEG P) (NEG? (

ZAL ZE
     P=(NEG R) I (PROP? R)
           (CONVERT-TO-NNF P)==(CONVER-TO-NNF (NEG R))==(NEG-CONVERT R)
           (NEG-CONVERT P)= )NEG-CONVERT (NEG R))==)CONVER


|#

;;ZAD 6 ----------------------------------------------------------------------------------------------------

(define (clause? x)
  (and (list? x)
       (andmap lit? x)))

(define (cnf? x)
  (and [list? x]
       [andmap clause? x]))

(define (convert-to-cnf f)
  (cond
    [(conj? f) (append (convert-to-cnf (conj-left f)) (convert-to-cnf (conj-right f)))]
    [(lit? f)  (list (list f))]
    [(disj? f) (let ((l (convert-to-cnf (disj-left f))) (r (convert-to-cnf (disj-right f))))
                   (append-map  (λ (c) (map (λ (c2) (append c c2)) r)) L))]))

;;ZAD 7 ----------------------------------------------------------------------------------------------------

(define (falsifiable-cnf? f)
  (ormap falsifiable-clause f)
  (define (falsifibale c)
    (id (taoutology? c)
        #f
        (map (λ (b) (cons(lit-name l) (lit-neg? l))))))
  (define (tautology? c)
    (if (empty? c)
        #f
        (if (find-negation (cdr c) (car c))
            #t
            (tautology? (cdr c))))))

;;ZAD * ----------------------------------------------------------------------------------------------------
#|
T(p) - zdanie rownowazne p w NNF
N(p) - zdanie rownowazne ¬p w NNF

ZAL INDUKCYJNE
       T(fi)=fi   N(fi)=¬fi - przystawanie

       T(p)=p     N(p)=¬p
       T(¬p)=¬p   N(¬p)=p

       T(¬fi)=N(fi)
       T(fi)=N(¬fi)

       T(fi1 ^ fi2)=T(fi1) ^ T(fi2)   ROZWAZYC WARTOSCIOWANIA I CO SIE DZIEJE)

       RESZTA ANALOGICZNIE

T(p) - formula rownowazne p w NNF

Jesli mamy literal to oczywiste jest ze T(L)=L

        T(fi1 ^ fi2) = T(fi1) ^ T(fi2)

        T(fi1 lub fi2) = 


|#
