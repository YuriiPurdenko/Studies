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

