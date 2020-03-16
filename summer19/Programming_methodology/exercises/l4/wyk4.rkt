#lang racket

(define (tagged-list? len sym x)
  (and (list? x)
       (= (length x) len)
       (eq? (first x) sym)))

;; drzewa binarne etykietowane w wierzchołkach wewnętrznych

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

(define (tree? x)
  (or (leaf? x)
      (and (node? x)
           (tree? (node-left  x))
           (tree? (node-right x)))))

;; operacje na drzewach BST

(define (find x t)
  (cond
    [(leaf? t)            false]
    [(= (node-elem t) x)  true]
    [(> (node-elem t) x)  (find x (node-left t))]
    [(< (node-elem t) x)  (find x (node-right t))]))

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

(define empty leaf)

;; przykładowe własności:
;; (find x (insert x t)) == true
;; (find x t)            == (find x (insert y t)) jeśli (not (= x y))
;; (find x empty)        == false

;; Kodowanie Huffmana

;; elementy z priorytetem

(define (elt p d)
  (cons p d))

(define (elt-prio c)
  (car c))

(define (elt-data c)
  (cdr c))

;; kopce jako listy uporządkowane

(define heap-empty null)

(define (heap-empty? x)
  (null? x))

(define (heap-insert e h)
  (cond
    [(null? h)                (cons e null)]
    [(< (elt-prio e)
        (elt-prio (car h)))   (cons e h)]
    [else                     (cons (car h) (heap-insert e (cdr h)))]))

(define (heap-min h)
  (car h))

(define (heap-pop h)
  (cdr h))

;; prosta implementacja słowników

(define dict-empty null)

(define (dict-insert k v d)
  (cons (cons k v) d))

(define (dict-find k d)
  (cond
    [(null? d)         false]
    [(eq? k (caar d))  (cdar d)]
    [else              (dict-find k (cdr d))]))

;; drzewa kodowe
(define (htleaf s)
  s)

(define (htleaf-sym s)
  s)

(define (htleaf? x)
  (symbol? x))

(define (htnode t1 t2)
  (cons t1 t2))

(define (htnode? x)
  (cons? x))

(define (htnode-left t)
  (car t))

(define (htnode-right t)
  (cdr t))


(define (make-tree plist)
  (define (add-to-heap h plist)
    (if (null? plist) h
        (let ((e   (elt (cdar plist)
                        (htleaf (caar plist))))
              (rst (cdr plist)))
          (add-to-heap (heap-insert e h) rst))))
  (define (aux h)
    (let* ((e1 (heap-min h))
           (p1 (elt-prio e1))
           (t1 (elt-data e1))
           (h1 (heap-pop h)))
      (if (heap-empty? h1) t1
          (let* ((e2 (heap-min h1))
                 (p2 (elt-prio e2))
                 (t2 (elt-data e2))
                 (h2 (heap-pop h1))
                 (e  (elt (+ p1 p2)
                          (htnode t1 t2))))
            (aux (heap-insert e h2))))))
  (aux (add-to-heap heap-empty plist)))

(define (ht->dict t)
  (define (aux t path dict)
    (if (htleaf? t)
        (dict-insert (htleaf-sym t) (reverse path) dict)
        (aux (htnode-right t)
             (cons 1 path)
             (aux (htnode-left t) (cons 0 path) dict))
        )
    )
  (aux t null dict-empty))

;; kodowanie i dekodowanie

;; wykonaj mapowanie procedury zwracającej listy i połącz wyniki (spłaszcz listę list)
(define (concat-map f xs)
  (if (null? xs)
      null
      (append (f (car xs))
              (concat-map f (cdr xs)))))

(define (encode dict syms)
  (define (enc-sym s)
    (dict-find s dict))
  (concat-map enc-sym syms))

(define (decode ht bits)
  ;; zdekoduj znak i zwróć znak i pozostałą listę bitów (zachowując kolejność)
  (define (dec-sym ht bits)
    (if (htleaf? ht)
        (cons (htleaf-sym ht) bits)
        ;; zakładamy że bity są poprawnym ciągiem kodów
        (let* ((b  (car bits))
               (bs (cdr bits))
               (t  (if (= 0 b)
                       (htnode-left ht)
                       (htnode-right ht))))
          (dec-sym t bs))))
  ;; kontynuuj dekodowanie znaku aż skończy się lista bitów
  (if (null? bits)
      null
      (let ((p (dec-sym ht bits)))
        (cons (car p) (decode ht (cdr p))))))

(define plist '((a . 10) (b . 1) (c . 6) (d . 5) (e . 3) (f . 6)))
