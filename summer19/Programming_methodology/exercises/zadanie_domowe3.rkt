#lang racket

(define (average x y)
  (/ (+ x y) 2))

(define (compose f g)
  (lambda (x) (f (g x))))

(define (repeated p n)
  (if (= n  0)
      identity
      (compose p (repeated p (- n 1)))))

(define (nth-root n x)
  
  (define (good-enough? x y)
    (< (abs (- x y)) 0.0000001))
  
  (define (fixed-point f s)
    (define (iter k)
      (let ((new-k (f k)))
        (if (good-enough? k new-k)
            k
            (iter new-k))))
    (iter s))

  (define (average-damp f)
    (lambda (x) (average x (f x))))
  
  (fixed-point ((repeated average-damp (floor (log n 2)))
                (lambda (y) (/ x
                               (expt y (- n 1)))))
               1.0001))


;;Funkcja fixed-point i average-damp na podstawie wykładu

;;TESTY
;;(nth-root 1 0)
;;(nth-root 2 4)  Nie działa bez tłumienia
;;(nth-root 3 -100)
;;(nth-root 4 18)  Nie działa dla jednokrotnego tłumienia            
;;(nth-root 8 1000000)  Nie działa dla dwukrotnego tłumienia
;;(nth-root 16 1000000) Nie działa dla trzykrotnego tłumienia
;;(nth-root 32 1000000) Nie działa dla czterokrotnego tłumienia
;;(nth-root 64 1000000) Nie działa dla pięciokrotnego tłumienia
