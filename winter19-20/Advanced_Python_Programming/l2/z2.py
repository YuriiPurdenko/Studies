class Formula:
    def __init__(self, x):
        self.x = x
        self.zmienne = x.zmienne

    def zmienwart(self, symb, wart):
        self.zmienne[symb] = wart

    def oblicz(self, zmienne):
        return self.x.oblicz(zmienne)

    def __str__(self):
        return self.x.__str__()

    def dodajzmienna(self, symbol, wartosc):
        self.zmienne.append([symbol, wartosc])

    def taut(self):
        a = self.zmienne.copy()
        count = 2 ** len(self.zmienne) - 1
        while count >= 0:
            zm = count
            for i in self.zmienne:
                if zm % 2 == 1:
                    self.zmienne[i] = 1
                else:
                    self.zmienne[i] = 0
                zm /= 2
            if self.x.oblicz(self.zmienne):
                count -= 1
            else:
                return False
        self.zmienne = a
        return True


class Lub(Formula):
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.zmienne = x.zmienne.copy()
        self.zmienne.update(y.zmienne)

    def oblicz(self, zmienne):
        return self.x.oblicz(zmienne) or self.y.oblicz(zmienne)

    def __str__(self):
        return "(" + self.x.__str__() + ") v (" + self.y.__str__() + ")"


class Oraz(Formula):
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.zmienne = x.zmienne.copy()
        self.zmienne.update(y.zmienne)

    def oblicz(self, zmienne):
        return self.x.oblicz(zmienne) and self.y.oblicz(zmienne)

    def __str__(self):
        return "(" + self.x.__str__() + ") ^ (" + self.y.__str__() + ")"


class Impl(Formula):
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.zmienne = x.zmienne.copy()
        self.zmienne.update(y.zmienne)

    def oblicz(self, zmienne):
        zm = self.y.oblicz(zmienne)
        return (self.x.oblicz(zmienne) and zm) or zm

    def __str__(self):
        return "(" + self.x.__str__() + ") => (" + self.y.__str__() + ")"


class Nie(Formula):
    def __init__(self, x):
        self.x = x
        self.zmienne = x.zmienne.copy()

    def oblicz(self, zmienne):
        return not(self.x.oblicz(zmienne))

    def __str__(self):
        return "Not(" + self.x.__str__() + ")"


class Falsz(Formula):
    def __init__(self):
        self.zmienne = {}

    def oblicz(self, zmienne):
        return False

    def __str__(self):
        return "False"


class Prawda(Formula):
    def __init__(self):
        self.zmienne = {}

    def oblicz(self, zmienne):
        return True

    def __str__(self):
        return "True"


class Zmienna(Formula):
    def __init__(self, x, wart):
        self.x = x
        self.zmienne = {x: wart}

    def oblicz(self, zmienne):
        return zmienne[self.x]

    def __str__(self):
        return self.x


raz = Zmienna("one", 1)
dwa = Zmienna("two", 1)
a = Nie(raz)
print(a.zmienne)
b = Lub(a, dwa)
print(b.zmienne)

c = Oraz(raz, dwa)
d = Impl(c, b)
e = Lub(d, Prawda())
x = Formula(e)

print(x)
print(x.oblicz(x.zmienne))
print(x.taut())
f = Lub(raz, Falsz())
g = Formula(f)
print(raz.zmienne)
print(g.oblicz(g.zmienne))

print(g.taut())
