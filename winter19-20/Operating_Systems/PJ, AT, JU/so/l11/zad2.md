Największa możliwa wartość:
Najpierw przejedzie cały jeden total, a następnie cały drugi total. Więc
największa wartość to będzie 50 + 50.
Dla przypadku z k != 2:
50 * k

Najmniejsza wartość:
Proces 1 zaczyta sobie wartość tally i policzy na boku tally + 1.
W tym czasie proces 2 również zaczyta sobie wartość tally i policzy na boku
tally + 1. 
Najpierw zapisze proces 1 a potem 2. Wynik ostateczny to tally + 1. Więc
dostaniemy tally = 50.

Dla k procesów będzie tak samo.

Oczywiście są też inne scenariusze. Drugi proces może się zatrzymać po wczytaniu
tally i poczekać aż pierwszy się skończy. Wtedy wznowi pracę. Wynik końcowy to
znowu tally.


A w naszej chorej notacji

I1 tally -> do rejestru     tally = x I1 tally = x
I2 rejestr++                tally = x I2 tally = x
I3 rejestr -> do tally      tally = x I3 tally = x + 1
