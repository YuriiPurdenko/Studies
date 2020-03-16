#lang racket

(require racklog)

(define %rodzic ; (%rodzic x y) oznacza, że iks jest rodzicem igreka
  (%rel ()
        [('elżbieta2 'karol)]
        [('elżbieta2 'anna)]
        [('elżbieta2 'andrzej)]
        [('elżbieta2 'edward)]
        [('karol     'william)]
        [('karol     'harry)]
        [('anna      'piotr)]
        [('anna      'zara)]
        [('andrzej   'beatrycze)]
        [('andrzej   'eugenia)]
        [('edward    'james)]
        [('edward    'louise)]
        [('william   'george)]
        [('william   'charlotte)]
        [('william   'louis)]
        [('harry     'archie)]
        [('piotr     'savannah)]
        [('piotr     'isla)]
        [('zara      'mia)]
        [('zara      'lena)]))

(define %rok-urodzenia
  (%rel ()
        [('elżbieta2 1926)]
        [('karol     1948)]
        [('anna      1950)]
        [('andrzej   1960)]
        [('edward    1964)]
        [('william   1982)]
        [('harry     1984)]
        [('piotr     1977)]
        [('zara      1981)]
        [('beatrycze 1988)]
        [('euagenia  1990)]
        [('james     2007)]
        [('louise    2003)]
        [('george    2013)]
        [('charlotte 2015)]
        [('louis     2018)]
        [('archie    2019)]
        [('savannah  2010)]
        [('isla      2012)]
        [('mia       2014)]
        [('lena      2018)]))

(define %plec
  (%rel ()
        [('elżbieta2 'k)]
        [('karol     'm)]
        [('anna      'k)]
        [('andrzej   'm)]
        [('edward    'm)]
        [('william   'm)]
        [('harry     'm)]
        [('piotr     'm)]
        [('zara      'k)]
        [('beatrycze 'k)]
        [('euagenia  'k)]
        [('james     'm)]
        [('louise    'k)]
        [('george    'm)]
        [('charlotte 'k)]
        [('louis     'm)]
        [('archie    'm)]
        [('savannah  'k)]
        [('isla      'k)]
        [('mia       'k)]
        [('lena      'k)]))

(define %spadl-z-konia
  (%rel ()
        [('anna)]))


;; (*) jeśli zamienimy te dwa wywołania miejscami, to co się stanie?


;;ZAD 1 ---------------------------------------------------------------------------------

;;teoretycznie plci nie sprawdzamy ale przy okazji mamy prawnuka

(define %prababcia 
  (%rel (a x y z) [(a z) (%rodzic a x) (%rodzic x y) (%rodzic y z)]))

(%which (x) (%prababcia 'elżbieta2 x))

(define %praprababcia 
  (%rel (x y z) [(x z) (%prababcia x y) (%rodzic y z)]))

(%which (x) (%praprababcia 'elżbieta2 x))

(%which (x w r) (%prababcia 'elżbieta2 x)
                (%rok-urodzenia x r)
                (%is w (- 2019 r)))

(%which (x y z a)
        (%rodzic x 'Archie)
        (%rodzic y x)
        (%rodzic y z)
        (%rodzic z a))



;;ZAD 2 ---------------------------------------------------------------------------------

(define %rodzenstwo
  (%rel (x y z) [(x y) (%rodzic z x) (%rodzic z y)]))

(define %starszy
  (%rel (x y a b) [(x y) (%rok-urodzenia y b) (%rok-urodzenia x a) (%> a b)]))

(define %starsze-rodzenstwo-act
  (%rel (x y a b) [(x y) (%rodzenstwo x y) (%starszy x y) (%plec x a) (%plec y b) (%=:= a b)]
                  [(x y) (%rodzenstwo x y) (%plec x a) (%plec y b) (%=:= a 'm) (%=:= b 'k)]))


;;ZAD 3 ---------------------------------------------------------------------------------

                    
;;ZAD 4 ---------------------------------------------------------------------------------


(define %rev-acc
  (%rel (ys acc a b) [(null ys ys)]
                        [((cons a b) ys acc) (%rev-acc b ys (cons a acc))]))

(define %rev
  (%rel (x y) [(x y) (%rev-acc x y null)]))

;;ZAD 5 ---------------------------------------------------------------------------------



;;ZAD 6 ---------------------------------------------------------------------------------

(define %merge
  (%rel (xs ys zs) [(xs ys zs) (%merge-acc xs ys zs null)]))

(define %merge-acc
  (%rel (a b c d zs acc)
        [(null null zs acc)  (%rev zs acc)]
        
        [((cons a b) null zs acc) (%merge-acc b null zs (cons a acc))]

        [(null (cons c d) zs acc) (%merge-acc null d zs (cons c acc))]

        [((cons a b) (cons c d) zs acc)
         (%>= a c) (%merge-acc (cons a b) d zs (cons c acc))]

        [((cons a b) (cons c d) zs acc)
         (%< a c) (%merge-acc b (cons c d) zs (cons a acc))]))

;;ZAD 7 ---------------------------------------------------------------------------------



;;ZAD 8 ---------------------------------------------------------------------------------


;;ZAD 9 ---------------------------------------------------------------------------------