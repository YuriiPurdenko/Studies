#lang racket

;; sygnatura: grafy
(define-signature graph^
  ((contracted
    [graph        (-> list? (listof edge?) graph?)]
    [graph?       (-> any/c boolean?)]
    [graph-nodes  (-> graph? list?)]
    [graph-edges  (-> graph? (listof edge?))]
    [edge         (-> any/c any/c edge?)]
    [edge?        (-> any/c boolean?)]
    [edge-start   (-> edge? any/c)]
    [edge-end     (-> edge? any/c)]
    [has-node?    (-> graph? any/c boolean?)]
    [outnodes     (-> graph? any/c list?)]
    [remove-node  (-> graph? any/c graph?)]
    )))

;; prosta implementacja grafów
(define-unit simple-graph@
  (import)
  (export graph^)

  (define (graph? g)
    (and (list? g)
         (eq? (length g) 3)
         (eq? (car g) 'graph)))

  (define (edge? e)
    (and (list? e)
         (eq? (length e) 3)
         (eq? (car e) 'edge)))

  (define (graph-nodes g) (cadr g))

  (define (graph-edges g) (caddr g))

  (define (graph n e) (list 'graph n e))

  (define (edge n1 n2) (list 'edge n1 n2))

  (define (edge-start e) (cadr e))

  (define (edge-end e) (caddr e))

  (define (has-node? g n) (not (not (member n (graph-nodes g)))))
  
  (define (outnodes g n)
    (filter-map
     (lambda (e)
       (and (eq? (edge-start e) n)
            (edge-end e)))
     (graph-edges g)))

  (define (remove-node g n)
    (graph
     (remove n (graph-nodes g))
     (filter
      (lambda (e)
        (not (eq? (edge-start e) n)))
      (graph-edges g)))))

;; sygnatura dla struktury danych
(define-signature bag^
  ((contracted
    [bag?       (-> any/c boolean?)]
    [empty-bag  (and/c bag? bag-empty?)]
    [bag-empty? (-> bag? boolean?)]
    [bag-insert (-> bag? any/c (and/c bag? (not/c bag-empty?)))]
    [bag-peek   (-> (and/c bag? (not/c bag-empty?)) any/c)]
    [bag-remove (-> (and/c bag? (not/c bag-empty?)) bag?)])))

;; struktura danych - stos
(define-unit bag-stack@
  (import)
  (export bag^)

  (define (bag? xs)
    (list? xs))

  (define (bag-empty? xs)
    (null? xs))
  
 (define empty-bag
   null)

  (define (bag-insert xs x)
    (cons x xs))

  (define (bag-peek xs)
    (car xs))
  
  (define (bag-remove xs)
    (cdr xs))
)

;; struktura danych - kolejka FIFO
;; do zaimplementowania przez studentów
(define-unit bag-fifo@
  (import)
  (export bag^)

  ;tworze strukture-liste gdzie pierwszy element to lista wejsciowa a druga to lista wyjsciowawyjsciowa
  (define (in-bag xs)
    (car xs))
  
  (define (out-bag xs)
    (cadr xs))

  (define (bag? xs)
    (and (list? xs)
         (list? (in-bag xs))
         (list? (out-bag xs))))

  (define (bag-empty? xs)
    (and (null? (in-bag xs)) (null? (out-bag xs))))

  (define empty-bag
    (list null null))

  (define (bag-insert xs x)
    (if (null? (out-bag xs))
        (list  null (reverse (cons x (in-bag xs))))
        (list (cons x (in-bag xs)) (out-bag xs))))

  (define (bag-peek xs)  ;; w insercie i outpucie nie dopuszczam aby out bag byl pusty
    (car (out-bag xs)))
        
    
  (define (bag-remove xs)
    (if (null? (cdr (out-bag xs)))
        (list null (reverse (in-bag xs)))
        (list (in-bag xs) (cdr (out-bag xs)))))

)

;; sygnatura dla przeszukiwania grafu
(define-signature graph-search^
  (search))

;; implementacja przeszukiwania grafu
;; uzależniona od implementacji grafu i struktury danych
(define-unit/contract graph-search@
  (import bag^ graph^)
  (export (graph-search^
           [search (-> graph? any/c (listof any/c))]))
  (define (search g n)
    (define (it g b l)
      (cond
        [(bag-empty? b) (reverse l)]
        [(has-node? g (bag-peek b))
         (it (remove-node g (bag-peek b))
             (foldl
              (lambda (n1 b1) (bag-insert b1 n1))
              (bag-remove b)
              (outnodes g (bag-peek b)))
             (cons (bag-peek b) l))]
        [else (it g (bag-remove b) l)]))
    (it g (bag-insert empty-bag n) '()))
  )

;; otwarcie komponentu grafu
(define-values/invoke-unit/infer simple-graph@)

;; graf testowy
(define test-graph
  (graph
   (list 1 2 3 4)
   (list (edge 1 3)
         (edge 1 2)
         (edge 2 4))))

(define test-tree-1
  (graph 
    (list 1 2 3 4 5 )
    (list 
      (edge 3 2)
      (edge 2 3)
      (edge 5 2)
      (edge 3 5)
      (edge 4 3)
      (edge 2 4)
      (edge 1 5)
      (edge 4 1)
)))

(define test-graph-1
  (graph 
    (list 1 2 3 4 5 )
    (list 
      (edge 3 1)
      (edge 1 3)
      (edge 3 5)
      (edge 5 3)
      (edge 3 4)
      (edge 4 3)
      (edge 2 5)
      (edge 5 2)
      (edge 2 3)
      (edge 3 2)
      (edge 2 1)
      (edge 1 2)
      (edge 2 4)
      (edge 4 2)
)))

(define test-tree-2
  (graph 
    (list 1 2 3 4 5 6 7 8 9 10 )
    (list 
      (edge 10 3)
      (edge 3 10)
      (edge 5 10)
      (edge 10 5)
      (edge 2 3)
      (edge 5 2)
      (edge 7 10)
      (edge 5 7)
      (edge 8 2)
      (edge 5 8)
      (edge 6 7)
      (edge 2 6)
      (edge 1 6)
      (edge 7 1)
      (edge 4 7)
      (edge 8 4)
      (edge 9 10)
      (edge 2 9)
)))

(define test-graph-2
  (graph 
    (list 1 2 3 4 5 6 7 8 9 10 )
    (list 
      (edge 2 3)
      (edge 3 2)
      (edge 8 3)
      (edge 3 8)
      (edge 8 2)
      (edge 2 8)
      (edge 9 3)
      (edge 3 9)
      (edge 9 8)
      (edge 8 9)
      (edge 1 8)
      (edge 8 1)
      (edge 10 1)
      (edge 1 10)
      (edge 7 10)
      (edge 10 7)
      (edge 6 7)
      (edge 7 6)
      (edge 6 1)
      (edge 1 6)
      (edge 6 10)
      (edge 10 6)
      (edge 6 8)
      (edge 8 6)
      (edge 6 3)
      (edge 3 6)
      (edge 6 9)
      (edge 9 6)
      (edge 6 2)
      (edge 2 6)
      (edge 4 7)
      (edge 7 4)
)))

(define test-tree-3
  (graph 
    (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 )
    (list 
      (edge 15 4)
      (edge 4 15)
      (edge 12 15)
      (edge 4 12)
      (edge 2 12)
      (edge 15 2)
      (edge 9 12)
      (edge 4 9)
      (edge 13 15)
      (edge 4 13)
      (edge 5 4)
      (edge 9 5)
      (edge 3 13)
      (edge 2 3)
      (edge 11 3)
      (edge 2 11)
      (edge 10 12)
      (edge 5 10)
      (edge 7 2)
      (edge 11 7)
      (edge 14 10)
      (edge 15 14)
      (edge 6 2)
      (edge 4 6)
      (edge 8 7)
      (edge 2 8)
      (edge 1 10)
      (edge 5 1)
)))

(define test-graph-3
  (graph 
    (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 )
    (list 
      (edge 10 7)
      (edge 7 10)
      (edge 10 8)
      (edge 8 10)
      (edge 10 15)
      (edge 15 10)
      (edge 4 15)
      (edge 15 4)
      (edge 4 10)
      (edge 10 4)
      (edge 4 8)
      (edge 8 4)
      (edge 4 7)
      (edge 7 4)
      (edge 14 15)
      (edge 15 14)
      (edge 1 10)
      (edge 10 1)
      (edge 1 7)
      (edge 7 1)
      (edge 1 15)
      (edge 15 1)
      (edge 1 4)
      (edge 4 1)
      (edge 11 7)
      (edge 7 11)
      (edge 11 4)
      (edge 4 11)
      (edge 6 7)
      (edge 7 6)
      (edge 6 11)
      (edge 11 6)
      (edge 6 12)
      (edge 12 6)
      (edge 6 10)
      (edge 10 6)
      (edge 6 4)
      (edge 4 6)
      (edge 3 12)
      (edge 12 3)
      (edge 3 4)
      (edge 4 3)
      (edge 2 11)
      (edge 11 2)
      (edge 2 15)
      (edge 15 2)
      (edge 2 1)
      (edge 1 2)
      (edge 5 14)
      (edge 14 5)
      (edge 5 12)
      (edge 12 5)
      (edge 5 7)
      (edge 7 5)
      (edge 5 1)
      (edge 1 5)
      (edge 5 15)
      (edge 15 5)
      (edge 5 6)
      (edge 6 5)
      (edge 13 12)
      (edge 12 13)
      (edge 13 3)
      (edge 3 13)
      (edge 13 15)
      (edge 15 13)
      (edge 13 9)
      (edge 9 13)
      (edge 13 1)
      (edge 1 13)
      (edge 13 4)
      (edge 4 13)
      (edge 13 8)
      (edge 8 13)
      (edge 13 5)
      (edge 5 13)
      (edge 13 2)
      (edge 2 13)
      (edge 13 6)
      (edge 6 13)
      (edge 13 7)
      (edge 7 13)
)))

(define test-tree-4
  (graph 
    (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 )
    (list 
      (edge 4 15)
      (edge 15 4)
      (edge 23 15)
      (edge 15 23)
      (edge 17 23)
      (edge 15 17)
      (edge 14 23)
      (edge 4 14)
      (edge 7 17)
      (edge 14 7)
      (edge 16 7)
      (edge 4 16)
      (edge 22 16)
      (edge 23 22)
      (edge 12 4)
      (edge 17 12)
      (edge 18 14)
      (edge 15 18)
      (edge 25 12)
      (edge 14 25)
      (edge 3 4)
      (edge 15 3)
      (edge 20 14)
      (edge 4 20)
      (edge 2 23)
      (edge 23 2)
      (edge 13 14)
      (edge 12 13)
      (edge 10 3)
      (edge 12 10)
      (edge 24 16)
      (edge 14 24)
      (edge 5 24)
      (edge 25 5)
      (edge 1 13)
      (edge 18 1)
      (edge 9 7)
      (edge 10 9)
      (edge 19 12)
      (edge 2 19)
      (edge 6 1)
      (edge 17 6)
      (edge 21 18)
      (edge 6 21)
      (edge 11 4)
      (edge 14 11)
      (edge 8 10)
      (edge 25 8)
)))

(define test-graph-4
  (graph 
    (list 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 )
    (list 
      (edge 3 15)
      (edge 15 3)
      (edge 5 3)
      (edge 3 5)
      (edge 14 15)
      (edge 15 14)
      (edge 14 3)
      (edge 3 14)
      (edge 24 14)
      (edge 14 24)
      (edge 4 14)
      (edge 14 4)
      (edge 4 24)
      (edge 24 4)
      (edge 4 15)
      (edge 15 4)
      (edge 22 3)
      (edge 3 22)
      (edge 22 14)
      (edge 14 22)
      (edge 22 5)
      (edge 5 22)
      (edge 22 24)
      (edge 24 22)
      (edge 22 4)
      (edge 4 22)
      (edge 22 15)
      (edge 15 22)
      (edge 6 3)
      (edge 3 6)
      (edge 6 24)
      (edge 24 6)
      (edge 6 4)
      (edge 4 6)
      (edge 6 22)
      (edge 22 6)
      (edge 6 15)
      (edge 15 6)
      (edge 20 15)
      (edge 15 20)
      (edge 20 3)
      (edge 3 20)
      (edge 21 1)
      (edge 1 21)
      (edge 21 20)
      (edge 20 21)
      (edge 21 24)
      (edge 24 21)
      (edge 21 4)
      (edge 4 21)
      (edge 17 6)
      (edge 6 17)
      (edge 17 4)
      (edge 4 17)
      (edge 17 1)
      (edge 1 17)
      (edge 17 5)
      (edge 5 17)
      (edge 17 20)
      (edge 20 17)
      (edge 17 22)
      (edge 22 17)
      (edge 17 21)
      (edge 21 17)
      (edge 17 24)
      (edge 24 17)
      (edge 13 21)
      (edge 21 13)
      (edge 13 4)
      (edge 4 13)
      (edge 13 15)
      (edge 15 13)
      (edge 13 6)
      (edge 6 13)
      (edge 13 1)
      (edge 1 13)
      (edge 13 17)
      (edge 17 13)
      (edge 13 20)
      (edge 20 13)
      (edge 19 5)
      (edge 5 19)
      (edge 19 21)
      (edge 21 19)
      (edge 19 13)
      (edge 13 19)
      (edge 19 4)
      (edge 4 19)
      (edge 19 22)
      (edge 22 19)
      (edge 19 1)
      (edge 1 19)
      (edge 19 24)
      (edge 24 19)
      (edge 19 6)
      (edge 6 19)
      (edge 19 14)
      (edge 14 19)
      (edge 2 14)
      (edge 14 2)
      (edge 12 1)
      (edge 1 12)
      (edge 12 17)
      (edge 17 12)
      (edge 12 3)
      (edge 3 12)
      (edge 12 22)
      (edge 22 12)
      (edge 12 19)
      (edge 19 12)
      (edge 12 2)
      (edge 2 12)
      (edge 12 20)
      (edge 20 12)
      (edge 12 4)
      (edge 4 12)
      (edge 12 24)
      (edge 24 12)
      (edge 12 6)
      (edge 6 12)
      (edge 11 6)
      (edge 6 11)
      (edge 11 12)
      (edge 12 11)
      (edge 11 19)
      (edge 19 11)
      (edge 11 4)
      (edge 4 11)
      (edge 11 14)
      (edge 14 11)
      (edge 11 13)
      (edge 13 11)
      (edge 11 17)
      (edge 17 11)
      (edge 11 24)
      (edge 24 11)
      (edge 10 21)
      (edge 21 10)
      (edge 10 14)
      (edge 14 10)
      (edge 10 5)
      (edge 5 10)
      (edge 10 24)
      (edge 24 10)
      (edge 10 11)
      (edge 11 10)
      (edge 10 20)
      (edge 20 10)
      (edge 10 4)
      (edge 4 10)
      (edge 7 10)
      (edge 10 7)
      (edge 7 12)
      (edge 12 7)
      (edge 7 1)
      (edge 1 7)
      (edge 7 6)
      (edge 6 7)
      (edge 9 12)
      (edge 12 9)
      (edge 9 4)
      (edge 4 9)
      (edge 9 5)
      (edge 5 9)
      (edge 25 24)
      (edge 24 25)
      (edge 25 6)
      (edge 6 25)
      (edge 25 14)
      (edge 14 25)
      (edge 25 12)
      (edge 12 25)
      (edge 25 13)
      (edge 13 25)
      (edge 25 1)
      (edge 1 25)
      (edge 25 5)
      (edge 5 25)
      (edge 25 15)
      (edge 15 25)
      (edge 25 10)
      (edge 10 25)
      (edge 25 3)
      (edge 3 25)
      (edge 25 17)
      (edge 17 25)
      (edge 25 19)
      (edge 19 25)
      (edge 25 11)
      (edge 11 25)
      (edge 25 4)
      (edge 4 25)
      (edge 25 21)
      (edge 21 25)
      (edge 25 7)
      (edge 7 25)
      (edge 25 22)
      (edge 22 25)
      (edge 25 20)
      (edge 20 25)
      (edge 25 9)
      (edge 9 25)
      (edge 8 14)
      (edge 14 8)
      (edge 8 21)
      (edge 21 8)
      (edge 8 24)
      (edge 24 8)
      (edge 8 17)
      (edge 17 8)
      (edge 8 3)
      (edge 3 8)
      (edge 8 11)
      (edge 11 8)
      (edge 8 4)
      (edge 4 8)
      (edge 8 13)
      (edge 13 8)
      (edge 16 20)
      (edge 20 16)
      (edge 16 19)
      (edge 19 16)
      (edge 16 14)
      (edge 14 16)
      (edge 16 4)
      (edge 4 16)
      (edge 16 11)
      (edge 11 16)
      (edge 16 2)
      (edge 2 16)
      (edge 16 22)
      (edge 22 16)
      (edge 16 13)
      (edge 13 16)
      (edge 16 24)
      (edge 24 16)
      (edge 16 15)
      (edge 15 16)
      (edge 16 5)
      (edge 5 16)
      (edge 16 17)
      (edge 17 16)
      (edge 16 6)
      (edge 6 16)
      (edge 16 9)
      (edge 9 16)
      (edge 16 8)
      (edge 8 16)
      (edge 16 7)
      (edge 7 16)
      (edge 16 3)
      (edge 3 16)
      (edge 16 1)
      (edge 1 16)
      (edge 16 25)
      (edge 25 16)
      (edge 16 10)
      (edge 10 16)
      (edge 18 16)
      (edge 16 18)
      (edge 18 9)
      (edge 9 18)
      (edge 18 1)
      (edge 1 18)
      (edge 23 2)
      (edge 2 23)
      (edge 23 18)
      (edge 18 23)
      (edge 23 17)
      (edge 17 23)
      (edge 23 11)
      (edge 11 23)
)))

;; TODO: napisz inne testowe grafy!

;; otwarcie komponentu stosu
(define-values/invoke-unit/infer bag-stack@)
;; opcja 2: otwarcie komponentu kolejki
;;(define-values/invoke-unit/infer bag-fifo@)

;; testy w Quickchecku
(require quickcheck)

;; test przykładowy: jeśli do pustej struktury dodamy element
;; i od razu go usuniemy, wynikowa struktura jest pusta

(quickcheck
 (property ([s arbitrary-symbol])
           (bag-empty? (bag-remove (bag-insert empty-bag s)))))

;;sprawdzam czy pierwszy wrzucony element jest pierwszym zdejmowanym

(quickcheck
 (property ([s arbitrary-real])
           (equal? s (bag-peek (bag-insert empty-bag s)))))

;;funkcje pomocnicze

(define (compon-bag xs)
  (define (iter xs n)
    (if (bag-empty? xs)
        n
        (iter (bag-remove xs) (cons (bag-peek xs) n))))
  (iter xs null))

(define (rec-ins-bag xs)
  (define (iter xs n)
    (if (null? xs)
        n
        (iter (cdr xs) (bag-insert n (car xs)))))
  (iter xs empty-bag))

(define (iter-remove xs n)
  (if (= n 0)
      xs
      (iter-remove (bag-remove xs) (- n 1))))

;;sprawdzam czy wrzucenie x elementow, a pozniej wyrzucenie x sprawi ze bag bedzie pusty

(quickcheck
 (property ([x (arbitrary-list arbitrary-natural)])
            (bag-empty? (iter-remove (rec-ins-bag x) (length x)))))

;;sprawdzam wlasnosci kolejnosci przechowywania danych
;;dla stosu

 (quickcheck
 (property ([x (arbitrary-list arbitrary-integer)])
            (equal? x (compon-bag (rec-ins-bag x)))))

;;dla kolejki
#|
(quickcheck
 (property ([x (arbitrary-list arbitrary-integer)])
            (equal? (reverse x) (compon-bag (rec-ins-bag x)))))
|#
#|
(define (rec-bag xs)
  (define (bag-iter xs out)
    (if null? xs)
  |#
;; TODO: napisz inne własności do sprawdzenia!
;; jeśli jakaś własność dotyczy tylko stosu lub tylko kolejki,
;; wykomentuj ją i opisz to w komentarzu powyżej własności

;; otwarcie komponentu przeszukiwania
(define-values/invoke-unit/infer graph-search@)

;; uruchomienie przeszukiwania na przykładowym grafie

(search test-graph 1)
(search test-tree-1 1)
(search test-graph-1 1)
(search test-graph-1 2)
(search test-tree-1 2)
(search test-graph-1 3)
(search test-tree-1 3)
(search test-tree-2 1)
(search test-graph-2 1)
(search test-graph-2 6)
(search test-tree-2 6)
(search test-graph-2 7)
(search test-tree-2 7)
(search test-tree-3 1)
(search test-graph-3 1)
(search test-tree-3 5)
(search test-graph-3 5)
(search test-tree-3 11)
(search test-graph-3 11)
(search test-tree-4 1)
(search test-graph-4 1)
(search test-tree-4 13)
(search test-graph-4 13)
(search test-tree-4 23)
(search test-graph-4 23)
;; TODO: uruchom przeszukiwanie na swoich przykładowych grafach!
