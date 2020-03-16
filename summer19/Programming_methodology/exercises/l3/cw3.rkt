notatki Martyny Firgolskiej

#lang racket
;;Nie odpowiadam za poprawnosc tych kodow   ;[
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;definicje z wykladu
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; konkatenacja (łączenie) pary list
#|
(define (append as bs)
  (if (null? as)
      bs
      (cons (car as)
            (append (cdr as) bs))))
|#

(define (distance x y)
  (abs (- x y)))
(define (abs x)
  (if (< x 0)
      (- x)
      x))
(define (square x) (* x x))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;działania na listach
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define x (cons 2 3))
x
(car x)
(cdr x)
(define y (list 1 2 3 4 5 ))
(car y)
(define z (list ))
(null? z)
(null? y)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;zadania 1 i 2
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; point 
(define (make-point x y)
  (cons x y))
(define X (make-point 2 3))
(define (point-x P)
  (car P))
(define (point-y P)
  (cdr P))
(define (point? P)
  (and (pair? P) (number? (point-x P)) (number? (point-y P))))
(point-y X)
(point? X)
(point? (point-x X))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;vect
(define (make-vect A B);;-from-points
  (cons A B))
(define (make-vect-form-coordinates x1 y1 x2 y2)
  (make-vect (make-point x1 y1) (make-point x2 y2)))
(define (vect-begin V)
  (car V))
(define (vect-end V)
  (cdr V))
(define (vect? V)
  (and (pair? V) (point? (vect-begin V)) (point? (vect-end V))))
(define AB (make-vect X (make-point 4 5)))
(vect-begin AB)
(vect-end AB)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define (x-length V)
  (distance (point-x (vect-begin V)) (point-x (vect-end V))))
(define (y-length V)
  (distance (point-y (vect-begin V)) (point-y (vect-end V))))
(define (vect-length V)
  (sqrt (+ (square (x-length V)) (square (y-length V)))))

(define (vect-scale V k)
  (make-vect (vect-begin V) (make-point (+ (point-x (vect-begin V)) (* k (x-length V)))
                                           (+ (point-y (vect-begin V)) (* k (y-length V))))))
(define (vect-translate V P)
  (make-vect P (make-point (+ (point-x P) (x-length V))
                           (+ (point-y P) (y-length V)))))
AB
(vect-scale AB 2)
(vect-length AB)
(vect-translate AB (make-point 0 0))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define (make-vect2 P length dir)
  (list P length dir))
(define (vect2-P V)
  (car V))
(define (vect2-dir V)
  (cdr (cdr V)))
(define (vect2-length V)
  (car (cdr V)))
(define (vect2-begin V)
  (car V))
(define (vect2? V)
  (and (pair? V)
       (pair? (car V))))
#|
(define (make-vect2-from-coordinates A B)
  (let ((V (make-vect A B)))
    (make-vect2 A (vect-length V) (atan2 (length-x V) (length-y V))))
    )
|#
(define (vect2-end V)
  (make-point (+ (point-x (vect2-begin V)) (* (vect2-length V) (cos vect2-dir)))
              (+(point-y (vect2-begin V) (* (vect2-length V) (sin vect2-dir))))))
(define (vect2-scale V k)
  (make-vect2 (vect2-P V) (* k (vect2-length V)) (vect2-dir V)))
(define (vect2-translate V P)
  (make-vect2 P (vect2-length V) (vect2-dir V)))
  
;;ZAD 3 ----------------------------------------------------------------------------------------------------

(define (reverse-iter list-from list-to) ;;list-from (a,(b,(c,d)))
  (if (null? list-from)
      list-to
      (reverse-iter (cdr list-from) (cons (car list-to) list-to )))
  );;lista typu
;;(define (reverse ls)
;;  (reverse-iter ls null))
#|
(define (reverse-rec lista)
  (if (null? lista)
      lista
      (append (reverse-rec (cdr lista)) (list (car lista))));;(cons (reverse-rec
  )
|#
#|
dowod rownowaznosci procedur

\forall ( ls, acc) (list? ls) and (list? acc) \rightarrow (append (raverse-rec ls) acc) = (reverse-iter ls acc)
 dowod indukcyjny
BAZA
(ls = null):
L=(append null acc) = acc
(list (car ls))
P= acc (z definicji)

ZAL dla dowolnego acc zachodzi:
(append (reverse-rec xs) acc) = (reverse-iter xs acc)
chcę dowieść:
(append (reverse-rec (cons x xs) acc) = (reverse-iter (cons x xs) acc)

P=(reverse-iter xs (cons x acc))= (append(reverse-rec xc) (cons x acc)
L=(append (append (reverese-rec xs) (list x)) acc) = (z łączności appenda) = (append (reverse-rec xs) (append (list x) acc)) = L

|#


;;ZAD 4 ----------------------------------------------------------------------------------------------------

(define (insert-square xs n);;zakladam ze lista jest posortowana
  (define (iter l r)
    (if (or (null? r) (> (car r) n))
        (append l (list n) r)
        (iter (append l (list (car r))) (cdr r))))
  (iter null xs));;kwadratowy czas dzialania


(define (insert xs n);;zakladam ze lista jest posortowana
  (define (iter l r)
    (if (or (null? r) (> (car r) n))
        (append (reverse l) (list n) r)
        (iter (cons (car r) l) (cdr r))))
  (iter null xs));;liniowy czas dzialania
(insert (list 2 4 6 8 10) 5)


(define (insert-sort lst)
  (if (null? lst)
      null
      (insert (insert-sort (cdr lst)) (car lst))))
      
;;ZAD 5 ----------------------------------------------------------------------------------------------------

Dowod, ze append z lista pusta null tworza monoid
1)
jeśli (list? xs)

;;ZAD 6 ----------------------------------------------------------------------------------------------------

(define (permi p)
  (define (permif h t el ret);; h (heap) - stos 
    (let ((nr (cons (append (reverse h) (cons el t)) ret)))
      (if (null? t)
          nr
          (permif (cons (car t) h) ( cdr t) el nr))))
  (define (permit-it pps el ret);;pps lista poprzednich permutacji
    (if (null? pps)
        ret
        (permit-it (cdr pps) el (permif '() (car pps) el ret))))
  (if (null? x)
      '()
      (permi-it (permi (cdr x)) (car x ) '() )))

;;ZAD 7 ----------------------------------------------------------------------------------------------------
