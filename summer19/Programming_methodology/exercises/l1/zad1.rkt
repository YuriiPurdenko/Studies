#lang racket

(define (abs x)
  (if (< x 0)
      (- x)
      x))

(define (distance x y)
  (abs (- x y)))

(define (square x)
  (* x x))

(define (cube x)
  (* x x x))

(define (cube-root x)
  (define (better guess)
    (/ (+ (/ x (square guess)) (* 2 guess)) 3)) ;;Newton method betterguess=(x/(guess*guess)+2guess)/3
  (define (is-ok? quess)
    (< (distance x (cube quess))
       0.0000000001))
  (define (find-root guess)
    (if (is-ok? guess)
        guess
        (find-root (better guess))))
  (find-root x))
  
  
  
  
 TESTY
 (cube-root -1000)
 (cube-root 27)
 (cube-root 15)
 (cube-root 1)
 (cube-root 0)
  
