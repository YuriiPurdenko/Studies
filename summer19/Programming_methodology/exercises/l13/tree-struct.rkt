#lang typed/racket

;;; drzewa binarne

(struct (a b) node ([v : a] [l : b] [r : b]) #:transparent)
(struct leaf () #:transparent)

(define-type (Tree a) (U leaf (node a (Tree a))))

(define-predicate tree? (Tree Any))

;;; wyszukiwanie i wstawianie w drzewach przeszukiwań binarnych

(: find-bst (-> Integer (Tree Integer) Boolean))
(define (find-bst v t)
  (match t
    [(leaf) false]
    [(node w l r)
     (cond [(= v w) true]
           [(< v w) (find-bst v l)]
           [else (find-bst v r)])]))

(: insert-bst (-> Integer (Tree Integer) (Tree Integer)))
(define (insert-bst v t)
  (match t
    [(leaf) (node v (leaf) (leaf))]
    [(node w l r)
     (if (< v w)
         (node w (insert-bst v l) r)
         (node w l (insert-bst v r)))]))

