```
class Account {
    private int balance;
    synchronized void withdraw(int n) { balance -= n; }
    synchronized void deposit(int n) { balance += n; }
}

void transfer(Account from, Account to, int amount) {
    from.withdraw(amount);
    to.deposit(amount);
}
```

mamy taki kod i zakładamy, że funkcje withdraw i deposit robią locka.

Dlaczego to jest słabe?

Bo pomiędzy
from.withdraw(amount);
to.deposit(amount);
nie mamy locka, no i wtedy trochę słabo, bo pomiędzy nimi pieniądze "zniknęły",
no i wtedy jest kicha
inny problem, załóżmy locka na początku transfer na obu gości zróbmy równolegle, 
transfer(a, b, x) i transfer(b, a, x) -- no i jest deadlock

void transfer(Account from, Account to, int amount) {
    from.lock(); to.lock();
    from.withdraw(amount);
    to.deposit(amount);
    from.unlock(); to.unlock();
}

to co można zrobić, to 
if from < to
    then { from.lock(); to.lock(); }
    else { to.lock(); from.lock(); }

ale to rozwiązanie ma krótkie nogi, bo jeśli transfer byłoby bardziej
skomplikowane? musimy znaleźć prawidłową kolejność priorytetów bloków i musimy
eksponować wnętrze dla innych funkcji, co chcemy uniknąć
