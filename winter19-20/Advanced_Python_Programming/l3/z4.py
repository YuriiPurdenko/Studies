import random


def uprosc_zdanie(tekst, x, y):
    leng = numb = 0
    word = new = ""
    t = [''] * 100 * y
    for i in tekst:
        if i == ' ' or i == ',' or i == '.' or i == '/n':
            if leng < x and leng > 0:
                numb += 1
                t[numb] = word
            word = ''
            leng = 0
        else:
            word += i
            leng += 1
    if leng < x:
        numb += 1
        t[numb] = word
    zm = numb
    while zm > y + 1:
        zm2 = random.randrange(numb)
        if(t[zm2] != ''):
            t[zm2] = ''
            zm -= 1
    for i in range(numb):
        if(t[i] != ''):
            new += ' ' + t[i]

    return new


tekst = "Podział peryklinalny inicjałów wrzecionowatych kambium charakteryzuje"
tekst += " się ścianą podziałową inicjowaną w płaszczyźnie maksymalnej."

print(uprosc_zdanie(tekst, 10, 5))
