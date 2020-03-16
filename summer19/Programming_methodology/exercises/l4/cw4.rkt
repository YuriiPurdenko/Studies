#lang racket
(define (concatMap f xs)
  (if (null? xs)
      null
      (append (f (car xs))
              (concatMap f (cdr xs)))))

(define (tagged-list? len sym x)
  (and (list? x)
       (= (length x) len)
       (eq? (first x) sym)))

(define leaf 'leaf)

(define (leaf? x)
  (eq? x 'leaf))

(define (node e l r)
  (list 'node e l r))

(define (node? x)
  (tagged-list? 4 'node x))

(define (node-elem x)
  (second x))

(define (node-left x)
  (third x))

(define (node-right x)
  (fourth x))

(define (insert x t)
  (cond
    [(leaf? t)            (node x leaf leaf)]
    [(= (node-elem t) x)  t]
    [(> (node-elem t) x)  (node (node-elem t)
                                (insert x (node-left t))
                                (node-right t))]
    [(< (node-elem t) x)  (node (node-elem t)
                                (node-left t)
                                (insert x (node-right t)))]))
                                
;;ZAD 1 ------------------------------------------------------------------------------------------------------------------------------

(define (append2 . arg)
  (cond [(null? arg) null]
        [(null? (cdr arg)) (car arg)]
        [(null? (car arg)) (apply append2 [cdr arg])]
        [else (cons [caar arg] [apply append2 (cons (cdar arg) (cdr arg))])]))
  

;;ZAD 2 ------------------------------------------------------------------------------------------------------------------------------

(define (btree? t)
  (or (eq? t 'leaf)
      (and (tagged-list? 4 'node t)
           (btree? (third t))
           (btree? (fourth t)))))


(define (mirror tree)
  (if (leaf? tree)
      'leaf
      (node (node-elem tree) (mirror (node-right tree)) (mirror (node-left tree)))))

;;ZAD 3 ------------------------------------------------------------------------------------------------------------------------------

(define (flatten tree)
  (define (iter T acc)
    (if (leaf? T)
        acc
        (iter (node-left T) (cons (node-elem T) (iter (node-right T) acc)))))
  (iter tree null))
                                  

;;ZAD 4 ------------------------------------------------------------------------------------------------------------------------------

(define (treesort l)        ;;to samo co quicksort
  (define (aux l t)
    (if (null? l)
        t
        (aux (cdr l) (insert (car l) t))))
  (flatten (aux l leaf)))

;;ZAD 5 ------------------------------------------------------------------------------------------------------------------------------

(define (delete t k)
  (cond [(leaf? t) 'leaf]
        [(> (node-elem t) k) (node (node-elem t) (delete (node-left  t) k) (node-right t))]
        [(< (node-elem t) k) (node (node-elem t) (node-left t) (delete (node-right t) k))]
        [else (delete-root t)]))

(define (delete-root t)
  (cond [leaf? (node-left t)] (node-right t)
        [leaf? (node-right t)] (node-left t)
        [else (let ([succ (find-succ t (node-left t))])
                (node (node-left t) (node-elem succ) (delete (node-right t) (node-elem succ))))]))

(define (find-succ t)
  (min-tree (node-right t)))

(define (min-tree t)
  (if (leaf? (node-left t)) t
             (min-tree (node-left t))))
;;lepsza implementacja jest jak sie zwroci uwage na naastepnika
;;przy schodzniu do poddrzewa trzba robic dwie operacja naraz (szukanie i jednoczesne budowanie poddrzewa bez niego)


;;ZAD 6 ------------------------------------------------------------------------------------------------------------------------------


(define(queens  board-size)
  ;; Return  the  representation  of a board  with 0 queens  inserted
  (define(empty-board)
    (list))
  ;; Return  the  representation  of a board  with a new  queen  at
  ;; (row , col) added  to the  partial  representation`rest'
  (define(adjoin-position  row  col  rest)
    (cons row rest))
  ;; Return  true if the  queen  in k-th  column  does  not  attack  any of
  ;; the  others
  (define(safe? k positions)
    (define (iter it pos)
      (cond [(null? pos) true]
            [(= (car pos) (car positions)) false]
            [(let ([diff (- it k)]) 
               (or (= (+ (car positions) diff) (car pos))
                   (= (- (car positions) diff) (car pos)))) false]
            [else (iter (- it 1) (cdr pos))]))
    (iter (- k 1) (cdr positions)))
  ;; Return a list of all  possible  solutions  for k first  columns
  (define(queen-cols k)
    (if(= k 0)
       (list (empty-board))
       (filter
        (lambda(positions) (safe? k positions))
        (concatMap
         (lambda(rest-of-queens)
           (map (lambda(new-row)
                  (adjoin-position  new-row k rest-of-queens))
                (from-to 1 board-size)))
         (queen-cols  (- k 1))))))
  (queen-cols board-size))

(define (from-to x y)
  (if (> x y)
      null
      (cons x (from-to (+ x 1) y))))

;;ZAD 7 ------------------------------------------------------------------------------------------------------------------------------
