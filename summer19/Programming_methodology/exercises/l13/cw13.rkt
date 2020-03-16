#lang typed/racket

;;ZAD 1 ----------------------------------------------------------------------------------------------------------------------------------

(: prefixes (All (a) (-> (Listof a) (Listof (Listof a)))))
(define (prefixes xs)
  (: suffixes (All (a) (-> (Listof a) (Listof (Listof a)) (Listof (Listof a)))))
  (define (suffixes xs acc)
    (if (null? xs)
        (cons null acc)
        (suffixes (cdr xs) (cons (reverse xs) acc))))
  (suffixes (reverse xs) null))
        

;;ZAD 2 ----------------------------------------------------------------------------------------------------------------------------------

(struct vector2 ([x : Real] [y : Real]) #:transparent)
(struct vector3 ([x : Real] [y : Real] [z : Real]) #:transparent)

(define-type Vector (U vector2 vector3))

(: vector-length (-> Vector Number))
(define (vector-length a)
  (match a
    [(vector2 x y) (sqrt (+ (* x x) (* y y)))]
    [(vector3 x y z) (sqrt (+ (* x x) (* y y) (* z z)))]))
    
  
;;ZAD 3 ----------------------------------------------------------------------------------------------------------------------------------

(: map (All [a] (-> (-> a a) (Listof a) (Listof a))))
(define (map p xs)
  (if (null? xs)
      null
      (cons (p (car xs)) (map p (cdr xs)))))
;;Tutaj define mapa

;;Problem jest taki, ze bierze najogolniejszy wspolny typ x i (p x) :(

;;ZAD 4 ----------------------------------------------------------------------------------------------------------------------------------

(define-type Leaf 'leaf)


(define-type (Node a b) (List 'node a b))

(define-type (Tree a) (U Leaf (Node a (Listof (Tree a)))))


(: preorder (All (a) (-> (Tree a) (Listof a))))
(define (preorder tree)
  (: dfs (All (a) (-> (Listof (Tree a)) (Listof a) (Listof a))))
  (define (dfs xs ans)
    (cond
      [(equal? xs null) ans]
      [else (dfs (cdr xs) (append ans (preorder (car xs))))]))
  (if (equal? tree 'leaf)
      null
      (cons (cadr tree) (dfs (caddr tree) null))))


;;ZAD 5 ----------------------------------------------------------------------------------------------------------------------------------

;; definicja wyrażeń z let-wyrażeniami

(struct const    ([val : Number])      #:transparent)

(struct (a b) op ([symb : a] [l : b] [r : b]) #:transparent)

(struct variable ()        #:transparent)

(define-type Expr (U const (op (U '+ '*) Expr ) variable))

(define-predicate expr? Expr)


;;ZAD 6 ----------------------------------------------------------------------------------------------------------------------------------




;;ZAD 7 ----------------------------------------------------------------------------------------------------------------------------------



