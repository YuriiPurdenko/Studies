#lang racket

(struct document (title author body))
(struct chapter (title body))
(struct subchapter (title body))
(struct paragraph (strings))

(define (chpt-elem? b)
  (or (paragraph?? b)
      (chapter?? b)))

(define (document?? ar)
  (match ar
    [(document t a b) (and (string? t)
                          (string? a)
                          (list? b)
                          (andmap chapter? b))]
    [_ false]))

(define (chapter?? ch)
  (match ch
    [(chapter t b) (and (string? t)
                        (list? b)
                        (andmap chpt-elem? b))]
    [_ false]))

(define (paragraph?? p)
  (match p
    [(paragraph t) (and (cons? t) ; żeby nie była pusta
                        (list? t)
                        (andmap string t))]
    [_ false]))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (text->html text)
  (define (gen-chapters chap lvl)
    (if (eq? chap null)
        " "
        (string-append (gen-chapter (car chap) lvl) (gen-chapters (cdr chap) lvl))))
  (define (gen-chapter chap lvl)
    (let ((tag (string-append "h" (number->string lvl))))
      (string-append "<" tag ">"
                     (chapter-title chap)
                     "</" tag ">"
                     (gen-body (chapter-body chap) (+ lvl 1)))))
  (define (gen-body body lvl)
    (if (eq? body null)
        " "
        (string-append (gen-elem (car body) lvl) (gen-body (cdr body) lvl))))
  (define (gen-elem elem lvl)
    (match elem
      [(chapter t _) (gen-chapter elem lvl)]
      [(paragraph b) (string-append "<p>" (apply string-append b) "</p>")]))
  (let
      [(title (document-title text)) 
       (author (document-author text))
       (body (document-body text))]
       (string-append "<html>" "<head>"
                      "<author>" author "</author>"
                      "<title>" title "</title>"
                      "</head>" "<body>"
                      (gen-chapters body 1)
                      "</body>" "</html>")))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (factorize n)
  (define (iter n k acc)
    (cond
      [(< n 2) acc]
      [(= (modulo n k) 0) (iter (quotient n k) k (cons k acc))]
      [else (iter n (+ k 1) acc)]))
  (match n
    [0 '(0)]
    [1 '(1)]
    [n (iter n 2 '())]))

(define (gen-fact-doc n)
  (let* [(primes (reverse (factorize n)))
         (primes-str (string-join (map number->string primes) " * "))]
    (document (string-append "Rozklad na czynniki pierwsze liczby " (number->string n))
              "Autor: Ja"
              (list (chapter "rozdział pierwszy"
                       (list (paragraph `("n = " ,primes-str))))))))