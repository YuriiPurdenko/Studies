#lang racket

(require racket/contract)


;;ZAD 1 -------------------------------------------------------------------------

(define/contract (suffixes xs)
  (parametric->/c [a] (-> (listof a) (listof (listof a))))
  (if (null? xs)
      (cons xs null)
      (cons xs (suffixes (cdr xs)))))

(suffixes (list 1 2 3))


;;ZAD 2 -------------------------------------------------------------------------

(define/contract (sublists  xs)
  (parametric->/c [a] (-> (listof a) (listof (listof a))))
  (if (null? xs)
      (list  null)
      (append-map (lambda (ys) (list (cons (car xs) ys) ys))
                  (sublists (cdr xs)))))

(sublists '(1 2))
(sublists '(1 2 3))

;;ZAD 3 -------------------------------------------------------------------------


(define/contract (k x y)
  (parametric->/c [a b] (-> a b a)) ;;a b negatywne  a pozytywne
  x)

(define/contract (s f g x)
  (parametric->/c [a b c] (-> (-> a b c) (-> a b) a c)) ;;a b pozytywne c negatywne a pozytywne b negatywne a negatywne c pozytywne
  (f x (g x)))

(define/contract (compose f g)
  (parametric->/c [a b c] (-> (-> b c) (-> a b) (-> a c))) ;;b pozytywne c negatywne a pozytywne b negatywne a negatywne c pozytywne
  (lambda (x) (f (g (x)))))

(define/contract (fun g)
  (parametric->/c [a] (-> (-> (-> a a) a) a)) ;;a negatywne a pozyytywne a negatywne a pozytywne
  (g identity))

;;ZAD 4 -------------------------------------------------------------------------

(define/contract (ff x)
  (parametric->/c [a b] (-> a b))
  (ff x))


;;ZAD 5 -------------------------------------------------------------------------

(define/contract (foldl-map f a xs)
  (parametric->/c [a acc b] (-> (-> a acc (cons/c b acc)) acc (listof a) (cons/c (listof b) acc)))
  (define (it a xs ys)
    (if (null? xs)
        (cons (reverse  ys) a)
        (let [(p (f (car xs) a))]
          (it (cdr p) (cdr xs) (cons (car p) ys)))))
  (it a xs null))

(foldl-map (lambda (x a) (cons a (+ a x))) 0 '(1 2 3))

;;ZAD 6 -------------------------------------------------------------------------


;; definicja wyrażeń z let-wyrażeniami

(struct const    (val)      #:transparent)
(struct op       (symb l r) #:transparent)
(struct let-expr (x e1 e2)  #:transparent)
(struct variable (x)        #:transparent)

(define (expr? e)
  (match e
    [(variable s)       (symbol? s)]
    [(const n)          (number? n)]
    [(op s l r)         (and (member s '(+ *))
                             (expr? l)
                             (expr? r))]
    [(let-expr x e1 e2) (and (symbol? x)
                             (expr? e1)
                             (expr? e2))]
    [_                  false]))

;; podstawienie wartości (= wyniku ewaluacji wyrażenia) jako stałej w wyrażeniu

(define (subst x v e)
  (match e
    [(op s l r)   (op s (subst x v l)
                        (subst x v r))]
    [(const n)    (const n)]
    [(variable y) (if (eq? x y)
                      (const v)
                      (variable y))]
    [(let-expr y e1 e2)
     (if (eq? x y)
         (let-expr y
                   (subst x v e1)
                   e2)
         (let-expr y
                   (subst x v e1)
                   (subst x v e2)))]))


;; (gorliwa) ewaluacja wyrażenia w modelu podstawieniowym

(define (eval e)
  (match e
    [(const n)    n]
    [(op '+ l r)  (+ (eval l) (eval r))]
    [(op '* l r)  (* (eval l) (eval r))]
    [(let-expr x e1 e2)
     (eval (subst x (eval e1) e2))]
    [(variable n) (error n "cannot reference an identifier before its definition ;)")]))




;;ROZW

(define (member/c l)
  (flat-contract [lambda (e) (member e)]))

(define expr/c
  (flat-rec-contract a
   [struct/c const number?]
   [struct/c variable symbol?]
   [struct/c op {member/c '(+ *)} a a]
   [struct/c let-expr symbol? a a]))


(define subst-contract
  (-> symbol? number? expr/c expr/c))

(define eval-contract
  (-> expr/c number?))

