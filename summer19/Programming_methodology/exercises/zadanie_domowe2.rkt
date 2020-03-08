#lang racket
(define (square x)
  (* x x))
(define (fraction-close an-2 an-1 bn-2 bn-1 num den k)
  (define (good-enough? x y)
    (< (abs (- x y)) 0.000001))
  (let ([an (+ (* (den k) an-1)
               (* (num k) an-2))]
        [bn (+ (* (den k) bn-1)
               (* (num k) bn-2))])
  (if (good-enough? (/ an bn) (/ an-1 bn-1))
      (/ an bn 1.0)
      (fraction-close an-1 an bn-1 bn num den (+ k 1)))))

(define (frac-close-it num den)
  (fraction-close 0 (num 1) 1 (den 1) num den  2))

;;Testy
;;1/fi
;;(frac-close-it (lambda(i) 1.0) (lambda(i) 1.0))
;;pi - 3
;;(frac-close-it (lambda(i) (square (- (* 2 i) 1))) (lambda(i) 6.0))
;;(atan 0)
;;(/ 0 (+ 1 (frac-close-it (lambda(i) (square (* 0 i))) (lambda(i) (+ (* 2 i) 1)))))
;;(atan 10)
;;(/ 10 (+ 1 (frac-close-it (lambda(i) (square (* 10 i))) (lambda(i) (+ (* 2 i) 1)))))
;;(atan x)
;;(/ x (+ 1 (frac-close-it (lambda(i) (square (* x i))) (lambda(i) (+ (* 2 i) 1)))))

