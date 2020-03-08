W shell.c w linijce 128.
Wykorzystane w linijce 159 i 161.

Dodałem dwie zmienne in oraz out, które przechowują wartość otrzymanych deskryptorów, pozwalają one sprawdzić czy redir zmienił deskryptory i uniknąć wielokrotnego zamknięcia deskryptorów w do_stage i do_pipeline.
Wynika to z tego, że w do_stage otrzymujemy wartość, a nie wskażnik przez co MaybeClose działa lokalnie i nie pozwala określić czy już zamkneliśmy dany deskryptor.

Jeśli zamykane byłyby deskryptory tylko w do_pipeline to nie zamykałbym deskryptorów otwartych przez do_readir, a jeśli tylko w do_stage to nie zamknąłbym deskryptora potoku, kiedy do_readir "podmieniłby" deskryptor.

W lexer.c w linijce 60 dodanie obsług ">>".
