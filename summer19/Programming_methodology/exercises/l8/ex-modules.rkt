#lang racket

(module my-module-1 racket
  (provide (all-defined-out)) ; wszystkie definicje z modułu są widoczne na zewnątrz
  (define (try-me) 4)
)

(module my-module-2 racket
  (provide (all-defined-out)) ; wszystkie definicje z modułu są widoczne na zewnątrz
  (define (try-me) 5)
)

;; Ponieważ w modułach są procedury o takich samych nazwach,
;; importując je, każemy dodać prefiks "mod...:" (nie jest to słowo
;; kluczowe, a identyfikator wiązany przez konstrukcję "prefix-in")
;; żeby odróżnić, z którego modułu pochodzi procedura.
(require (prefix-in mod1: 'my-module-1))
(require (prefix-in mod2: 'my-module-2))

(define (test)
  (cond [(> (mod1:try-me) (mod2:try-me)) (display "modul nr 1 jest lepszy, bo produkuje wieksza liczbe")]
        [(< (mod1:try-me) (mod2:try-me)) (display "modul nr 2 jest lepszy, bo produkuje wieksza liczbe")]
        [else (display "oba moduly sa rownie dobre")]))
