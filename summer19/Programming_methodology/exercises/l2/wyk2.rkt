#lang racket

;;silnie, uogólniona suma + punkty stałe i metoda Newtona

(define (inc n)
  (+ n 1))

(define (square n)
  (* n n))

(define (cube n)
  (* n n n))

(define (identity x)
  x)

(define (average x y)
  (/ (+ x y) 2))

;; dwie definicji silni
;; pierwsza generuje proces rekurencyjny, druga iteracyjny
(define (fact n)
  (if (<= n 0)
      1
      (* n (fact (- n 1)))))

(define (fact-iter n i c)
  (if (> i n)
      c
      (fact-iter n (+ i 1) (* i c))))

(define (fact-it n)
  (fact-iter n 1 1))

;; trzy definicje sumujące pewne ciągi liczbowe
(define (sum-of-nats start end)
  (if (> start end)
      0
      (+ start (sum-of-nats (inc start) end))))

(define (sum-cubes start end)
  (if (> start end)
      0
      (+ (cube start) (sum-cubes (+ start 2) end))))

(define sum-pi
  ;; "odcukrzona" definicja procedury: define wiąże nazwę procedury,
  ;; ale sama procedura zdefiniowana jest przy użyciu formy lambda
  (lambda (start end)
    (if (> start end)
        0
        (+ (/ 1.0 (* start (+ start 2)))
           (sum-pi (+ start 4) end)))))

;; przekazywanie procedur jako parametrów pozwala nam na wyabstrahowanie
;; od konkretnych aspektów powyższych definicji, i zdefiniowanie wspólnej,
;; ogólnej procedury sumującej
(define (sum val next start end)
  (if (> start end)
      0
      (+ (val start)
         (sum val next (next start) end))))

(define (good-enough? x y)
  (< (abs (- x y)) 0.0001))

;; inny przykład użycia procedur jako argumentów: obliczanie punktu stałego
;; transformacji, podobnie jak przy liczeniu pierwiastka tydzień temu
(define (fixed-point f s)
  (define (iter k)
    (let ((new-k (f k)))
      (if (good-enough? k new-k)
          k
          (iter new-k))))
  (iter s))

;; dodatkowe przykłady

;; próba obliczania pierwiastka kwadratowego z x jako punktu stałego funkcji
;; y ↦ x / y (wziętej bezpośrednio z zależności y^2 = x) zapętla się (sprawdź!)
;; stosujemy tłumienie z uśrednieniem: procedurę wyższego rzędu zwracającą procedurę jako wynik
(define (average-damp f)
  (lambda (x) (average x (f x))))

(define (sqrt-ad x)
  (fixed-point (average-damp (lambda (y) (/ x y))) 1.0))

;; obliczanie pochodnej funkcji z definicji przyjmując dx za "odpowiednio małą" wartość (zamiast "prawdziwej" granicy)
(define (deriv f)
  (let ((dx 0.000001))
    (lambda (x) (/ (- (f (+ x dx)) (f x)) dx))))

;; przekształcenie Newtona: x ↦ x - f(x) / f'(x)
;; klasyczna metoda numeryczna pozwala obliczyć miejsce zerowe f jako punkt stały tej transformacji
(define (newton-transform f)
  (lambda (x)
    (- x
       (/ (f x)
          ((deriv f) x)))))

(define (newtons-method f x)
  (fixed-point (newton-transform f) x))

;; zastosowania
(define pi (newtons-method sin 3.0))

(define (sqrt-nm x)
  (newtons-method (lambda (y) (- x (square y))) 1.0))

;; możemy wyabstrahować wzorzec widoczny w definicjach sqrt: znaleźć punkt stały pewnej funkcji *przy użyciu* transformacji
;; argumentem fix-point-of-transform jest procedura przetwarzająca procedury w procedury!
(define (fixed-point-of-transform transform f x)
  (fixed-point (transform f) x))



