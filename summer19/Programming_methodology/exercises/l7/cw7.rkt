#lang racket

;; definicja wyrażeń z let-wyrażeniami

(struct const    (val)      #:transparent)
(struct op       (symb l r) #:transparent)
(struct let-expr (x e1 e2)  #:transparent)
(struct variable (x)        #:transparent)


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

;; pochodna funkcji

(define (∂ f)
  (match f
    [(const n)   (const 0)]
    [(variable _)  (const 1)]
    [(op '+ f g) (op '+ (∂ f) (∂ g))]
    [(op '* f g) (op '+ (op '* (∂ f) g)
                        (op '* f (∂ g)))]))

;;ZAD 1 ----------------------------------------------------------------------------------------------------

;;moje
(struct comp (symb a b f x))
(struct min (f x))

(define (expr? e)
  (match e
    [(variable s)       (symbol? s)]
    [(const n)          (number? n)]
    [(op s l r)         (and (member s '(+ * **))
                             (expr? l)
                             (expr? r))]
    [(comp s a b f x)     (and (member s '(sum integ))
                             (expr? a)
                             (expr? b)
                             (expr? f)
                             (symbol? x))]
    [(min f x)            (expr? f)
                          (symbol? x)]
    [(let-expr x e1 e2) (and (symbol? x)
                             (expr? e1)
                             (expr? e2))]
    [_                  false]))

;;rozw martyny


;;ZAD 2 ----------------------------------------------------------------------------------------------------

(struct ifabs (war f t))

(define (ifabs?? x)
  (match x
    [(ifabs war f t) (and (expr? war)
                          (expr? f)
                          (expr? t))]
    [_ #f]))

(struct condabs (war wyn))

(define (condabs?? x)
  (match x
    [(condabs war wyn) (and (list? war)
                            (list? wyn)
                            (> (length war) 0)
                            (= (length war) (length wyn))
                            (andmap expr? war)
                            (andmap expr? wyn))]
    [_ #f]))

(struct lambdaabs (x y))

(define (lambdaabs?? l)
  (match l
    [(lambdaabs x y) (and (list? x)
                          (andmap symbol? x)
                          (expr? y))]
    [_ #f]))

                          
                          
;;ZAD 3 ----------------------------------------------------------------------------------------------------

(define (from-quote e)
  (cond
    [(number? e) (const e)]
    [(null? [cddr e])  (from-quote (second e))]
    [else (op [first e] [from-quote (second e)] [from-quote (cons [first e] [cddr e])])]))

;nie dziala dla dzielenie i mnozenia itp

;;ZAD 4 ----------------------------------------------------------------------------------------------------


;;ZAD 5 ----------------------------------------------------------------------------------------------------

(define (finds x e)
  (match e
    [(const v) false]
    [(variable v) (eq? v x)]
    [(op _ l r) (or (finds x l)
                    (finds x r))]
    [(let-expr s e1 e2)
     (if (eq? s x)
         (finds x e1)
         (or (finds x e1)
             (finds x e2)))]
    [_ false]))

(define (simplify-let e)
  (match e
    [(const v) (const v)]
    [(variable v) (variable v)]
    [(op s l r) (op s (simplify-let l)
                    (simplify-let l))]
    [(let-expr s e1 e2)
     (if (finds s e2)
         (let-expr s (simplify-let e1)
                     (simplify-let e2))
         (simplify-let e2))]))

;;ZAD 6 ----------------------------------------------------------------------------------------------------

(define stakc? list?)
(define stack-push cons)
(define stack-top car)
(define stack-pop cdr)

(define (eval-rpn e)
  (define (iter e s)
    (cond [(null? e) (stack-top s)]
          [(const? (car e)) (iter (cdr e) (stack-push (car e)))]
          [(equal? (car e) '+) (iter (cdr e) (let* ([x (stack-top s)]
                                                   [s (stack-pop s)]
                                                   [y (stack-top s)]
                                                   [s (stack-pop s)])
                                             (stack-push (+ x y) s)))]
          [(equal? (car e) '*)
           (iter (cdr e) (let* ([x (stack-top s)]
                               [s (stack-pop s)]
                               [y (stack-top s)]
                               [s (stack-pop s)])
                       (stack-push (* x y) s)))]))
  (iter e (list)))
                                                    
                                               
           

;;ZAD 7 ----------------------------------------------------------------------------------------------------

(define (make-cycle ml)
  (define (iter temp)
    (if (null? (mcdr temp))
        (set-mcdr! temp ml)
        (iter (mcdr temp))))
  (iter ml))
      
