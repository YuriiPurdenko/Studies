# Zadanie 1.8

**Wektory wyjątków (exception vectors) procesora m68k**

**Przerwanie** to radzaj **wyjątku procesora (exception)**.

Wektor wyjątku to miejsce w pamięci z którego procesor pobiera adres **procedury obsługi wyjątku (exception handler)**. Wektory wyjątku odpowiadające przerwaniom nazywane są **interrupt autovectors**.
Każdy typ wyjątku wymaga procedury obsługującej oraz unikalnego wektora wyjątku. Wszystkie wektory wyjątków mają dwa słowa długości (4 bajty) (oprócz wektora odpowiadającego wyjątkowi **RESET** który jest 8-bajtowy).

![](https://i.imgur.com/mfQ3ZZg.jpg)

**Mechanizmy kontroli przerwań**

- **Poziom priorytetu przerwań (interrupt priority level)** 
Istnieje siedem poziomów priorytetu przerwań numerowanych od 1 do 7 (im wyższy poziom, tym wyższy priorytet).
**Rejestr stanu SR (status register)** zawiera 3-bitową maskę poziomu priorytetu przerwania (**IPL**) (wartość maski IPL = 7 oznacza, iż wykonywane będą jedynie przerwania niemaskowalne). **Oczekujące (pending)** przerwanie jest obsługiwane jedynie wówczas, gdy wartość maski IPL jest ostro mniejsza od poziomu priorytetu przerwania odpowiadającemu temu przerwaniu.

- **Rejestry INTREQ (interrupt requests) oraz INTENA (interrupt enable)**
Rejestry **INTENA** (zapis) oraz **INTENAR** (odczyt) służą do decydowania, które przerwania mają być wykonywane, a które nie. Rejestr **INTREQ** (zapis) służy do zgłaszania przerwań, zaś rejestr **INTREQR** (odczyt) służy do odczytywania aktualnego stanu przerwań.

Znaczenie poszczególnych bitów w powyższych rejestrach:

![](https://i.imgur.com/XXC6yJN.jpg)


**Obsługa przerwań część I: Sprzęt (hardware)**

Procesor jest powiadamiany o wystąpieniu przerwania (**interrupt request**) poprzez kodowanie poziomu priorytetu przerwania na trzech przeznaczonych do tego liniach (**interrupt request lines**). Dostarczone żądania zyskują status **oczekujących (pending)** (odpowiedni bit jest włączony w INTREQR). Oczekujące przerwania są wykrywane pomiędzy wykonywaniem kolejnych intrukcji. Jeśli priorytet takiego przerwania jest nie większy od wartości maski IPL, to **obsługa wyjątku** jest odraczana na później. W przeciwnym wypadku (poziom priorytetu > IPL) procesor rozpoczyna sekwencję obsługi wyjątku:

1. Wykonywana jest kopia rejestru stanu (SR), procesor przechodzi w tryb jądra (S = 1), wyłączane jest **śledzenie (tracing)** (T = 0), oraz IPL ustawiane jest na poziom priorytetu obsługiwanego przerwania.
2. Procesor wykonuje **cykl magistrali potwierdzający przerwanie (interrupt acknowledge bus cycle)**. W efekcie poprzez magistralę danych, urządzenie peryferyje dostarcza procesorowi 8-bitowy **numer wektora**.![](https://i.imgur.com/gDiaKng.jpg)Następnie procesor oblicza **offset wektora (vector offset)** przesuwając numer wektora o dwie pozycje w lewo (ponieważ wektory są 4-bajtowe). Uzuskujemy w ten sposób bezwzględny adres wketora przerwania (na M68010+ jest to adres względny, dodatkowo używany jest VBR (vector base register)).![](https://i.imgur.com/YC3qZlO.jpg)
3. Na stos (jądra) odkładany jest zapisany rejest stanu oraz wartość **wskaźnika instrukcji (PC)** (procesory M68010+ zapisują dodatkowe informacje (w tym format i offset wektora)).![](https://i.imgur.com/RwrZEC4.jpg)
4. Procesor ustala nową wartość wskaźnika instrukcji z wektora wyjątku, oraz wznawia wykonywanie instrukcji.

**Obsługa przerwań część II: oprogramowanie (software)**

**Interrupt autovector**

Peryferia komputera Amiga 500 generują 14 różnych przerwań, które odwzorowane są w 6 przerwań zewnętrznych procesora (poziomy od 1 do 6).

![](https://i.imgur.com/wRuXjv6.jpg)

Zatem mając do dyspozycji 6 wketorów wyjątków (**interrupt autovector**), trzeba obsłużyć 14 różnych przerwań. Potrzebna jest zatem odpowiedna struktura procedur, których adresy przechowywane są w wektorach przerwań.

Co trzeba zrobić?

Dla każdego poziomu priorytetu wyjątku, w procedurze obsługującej należy sprawdzić kolejno odpowiednie bity w rejestrze INTREQR, i dla pierwszego włączonego bitu wywołać procedurą obsługującą konkretne przerwanie. Konieczne zatem jest skonstruowanie 6 funkcji obsługujących wyjątek (po jednej dla każdego wektora przerwania), oraz 14 funkcji obsługujących konkretne przerwania. 

**Implementacja**

Funkcje odpowiadające wektorom przerwań (w interrupt autovectors są adresy tych funkcji): [AmigaLvl1-6Handler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/intr.S?r=3c6ca3a9#22).
Inicjalizacja wektorów znajduje się w funkcji [vPortSetupExceptionVector()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#107)
```C
/* Initialize level 1-7 interrupt autovector in Amiga specific way. */
ExcVec[EXC_INTLVL(1)] = AmigaLvl1Handler;
ExcVec[EXC_INTLVL(2)] = AmigaLvl2Handler;
ExcVec[EXC_INTLVL(3)] = AmigaLvl3Handler;
ExcVec[EXC_INTLVL(4)] = AmigaLvl4Handler;
ExcVec[EXC_INTLVL(5)] = AmigaLvl5Handler;
ExcVec[EXC_INTLVL(6)] = AmigaLvl6Handler;
```

Funkcje obsługujące przerwania zostały zaimplementowane w postaci struktury

```C
IntVec_t IntVec;
```
gdzie typ definiowany jest następująco
```C
/* Interrupt Service Routine */
typedef void (*ISR_t)(void *);

/* Interrupt Vector Entry */
typedef struct IntVecEntry {
  ISR_t code;
  void *data;
} IntVecEntry_t;
  
typedef IntVecEntry_t IntVec_t[INTB_INTEN];
```

IntVec to 14 elementowa tablica, której poszczególne elementy to struktury złożone z adresu funkcji (**code**) oraz argumentu dla tej funkcji (**data**). Przerwaniu odpowiadającemu bitowi *N* w rejestrze INTENA, odpowiada element IntVec[*N*].

**Jaka procedura odpowiada danemu przerwaniu?**

Taka konstrukcja jest niepełna. W danej chwili każdemu przerwainu odpowiada jedna i tylko jedna procedura obsługi. A co jeśli do obsługi (np. VERTB) potrzebne są trzy różne proceduty?

**Łańcuchy przerwania (interrupt chain)**

**Serwerem przerwania (interrupt server)** nazywamy struckturę składającą się z wpisu listy (zawierającego argument dla funkcji obsługującej oraz priorytet określający pozycję na liście) oraz adresu procedury obsługującej przerwanie.
```C
/* Reuses following fields of ListItem_t:
  *  - (BaseType_t) pvOwner: data provided for IntSrvFunc_t,
  *  - (TickType_t) xItemValue: priority of interrupt server. */
typedef struct IntServer {
  ListItem_t node;
  IntFunc_t code;
} IntServer_t;
```

O serwerze przerwania należy myśleć jak o procedurze obsługującej (wraz z argumentem) oraz priorytecie określającym kiedy procedura ta się wykona względem innych procedur odpowiadających danemu przerwaniu.

Naturalna jest konieczność grupowania serwerów przerwania odpowiadających konkretnemu przerwaniu w strukturę (listę).

**Łańcuch przerwania (interrupt chain)** to lista serwerów przerwań (a ściśle, to wpisów zawartuch w takich serwerach) oraz flaga.

```C
/* List of interrupt servers. */
typedef struct IntChain {
  List_t list;
  uint16_t flag; /* interrupt enable/disable flag (INTF_*) */
} IntChain_t;
```

Dla każdego przerwania mamy łańcuch przerwania, który to składa się z wielu serwerów przerwania (każdy z nich zawiera funkcję do wywołania). Zatem przy obsłudze przerwania należy wywołać kolejno funkcje wszystkich serwerów w danym łańcuchu przerwania (serwery są uporządkowane malejąco względem priorytetu). Realizuje to funkcja [RunIntChain()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/intsrv.c?r=7be462d2#31). Zatem to jej adres powinien zawierać element IntVec[*N*] (pole data powinno zawierać adres łańcucha).

**Studium przypadku: przerwanie VERTB**

Przerwanie VERTB ma trzeci poziom priorytetu, zatem obsługując je, procesor wywkona procedurę [AmigaLvl3Handler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/intr.S?r=3c6ca3a9#38).

1. Rejestry d0, d1, a0, a1 zostają zapisane na stos (jądra).
2. Do rejestru a0 wpisujemy adres elementu struktury IntVec odpowiadający pierwszemu przerwaniu o poziomie priorytetu 3 (tj. **COPER**) (czyli adres struktury IntVec[INTB_COPER]) (struktura IntVecEntry_t jest 8-bajtowa (ponieważ architektura jest 32-bitowa)).
3. W rejestrze d1 zapisujemy numer bitu przerwania COPER.
4. W rejestrze d0 umieszczamy zawartość rejestru INTREQR.
5. W rejestrze INTREQR, dla bitów odpowiadających przerwaniom poziomu 3, szukamy pierwszego włączonego (przechodząc przy tym po kolejnych elementach tablicy IntVec) i w przechodzimy do bloku _handle (blok FINISH nie zawiera testu, bo jeśli do niego doszliśmy to konkretny bit musi być ustawiony (przecież przyszło przerwanie poziomu 3)).

Blok [_handle](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/intr.S?r=3c6ca3a9#59):

1. Czyścimy bit obsługiwanego przerwania (w rejestrze INTREQ).
2. Wykonujemy procedurę RunIntChain() (z adresem odpowiedniego łańcucha przerwania (dla VERTB jest to VertBlankChain)).
3. Jeżeli w zapisanym (na stosie) rejestrze SR maska IPL jest niezerowa, to wracamy z procedury AmigaLvl3Handler (powrót z zagnieżdżonego przerwania).
4. Blokujemy przerwania, aby zapobiec wyścigom do zmiennej **xNeedRescheduleTask**.
5. Jeśli wymagane jest przełączenie kontekstu (świadczy o tym zmienna xNeedRescheduleTask), to czyścimy flagę xNeedRescheduleTask i przechodzimy do procedury [vPortYieldHandler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#28) (przełączenie kontekstu).
6. Przywracamy rejestry ze stosu i wracamy z procedury AmigaLvl3Handler().

W przykładzie [preemption](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/examples/preemption/main.c?r=12be748b), do predefiniowanego łańcucha przerwania dla przerwania VERTB dodajemy serwer przerwania o nazwie SystemClockTick, którego procedura obsługująca to [SystemClockTickHandler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/examples/preemption/main.c?r=12be748b#22). Zatem w prypadku wystąpienia przerwania zegarowego w bloku _handle zostanie wywołana procedura RemIntServer(VertBlankChain), ale jedynym serwerem w łańcuchu VertBlankChain jest SystemClockTick, zatem zostanie wywołana funkcja SystemClockTickHandler(), w której potencjalnie włączona zostanie flaga xNeedRescheduleTask (procedura xTaskIncrementTick() zwraca wartość lokalnej stałej **xSwitchRequired**).