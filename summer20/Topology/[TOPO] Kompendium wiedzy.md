<span style="font-family:Papyrus; font-size:9pt;">

# [TOPO] Kompendium wiedzy

### Baza topologii
Rodzina $\mathcal{B}$ jest bazą, jeżeli dla każdego $U$ i $x \in U$ istnieje jakiś $B \in \mathcal{B}$ że $x \in B \subseteq U$.

**Tw 1.2.6**: Jeżeli $\bigcup \mathcal{B} = X$ oraz dla dowolnych $B_1, B_2 \in \mathcal{B}$ i $x \in B_1 \cap B_2$ istnieje $B \in \mathcal{B}$ takie, że $x \in B \subseteq B_1 \cap B_2$, to $\mathcal{B}$ generuje topologię (jest bazą) $\{U : x \in U \implies x \in B \subseteq U \text{ dla pewnego B} \in \mathcal{B}\}$.

### Otoczenie punktu
$S$ jest otoczeniem $x$ jeżeli istnieje $U$ spełniający $x \in U \subseteq S$.
W przestrzeni metrycznej otoczenie $x$ to zbiór zawierający kulę o środku w $x$.

### Przestrzeń $T_1$
Przestrzeń jest $T_1$ jeżeli dla każdych $x, y$ mamy $U$ takie, że $x\in U, y \notin U$.
Równoważnie przestrzeń jest $T_1$ jeśli każdy singleton jest domknięty.

### Przestrzeń Hausdorffa ($T_2$)
$(X, \mathcal{T})$ jest $T_2 \iff$ dla każdych $x_1, x_2 \in X$ istnieją rozłączne otoczenia $x_1$ i $x_2$.
* Zwarte podzbiory są domknięte.
* Dla każdych zwartych $Z_1, Z_2 \subseteq X$ istnieją rozłączne otoczenia $Z_1$ i $Z_2$.
* Lokalna zwartość + $T_2 \implies T_3$.
* Zwartość + $T_2 \implies T_4$.

### Przestrzeń regularna ($T_3$)
$(X, \mathcal{T})$ jest $T_3 \iff$ dla każdego $x \in X$ i domkniętego $S \subseteq X$ ($x \not\in S$) istnieją rozłączne otoczenia $x$ i $S$.
* Dla każdego $x \in X$ i otoczenia $G$ istnieje domknięte otoczenie $S$ takie, że $S \subseteq G$.

### Przestrzeń normalna ($T_4$)
$(X, \mathcal{T})$ jest $T_4 \iff$ dla każdych domkniętych $S_1, S_2 \subseteq X$  istnieją rozłączne otoczenia $S_1$ i $S_2$.
* Wszystkie metryzowalne topologie są $T_4$.
* Każdy domknięty podzbiór $X$ jest $T_4$.

### Domknięcie zbioru

Domknięcie $\overline{S}$ zbioru $S$ jest zbiorem punktów $x$ takich, że każde otoczenie $x$ przecina $S$.
W przestrzeni metrycznej punkt $x \in \overline{S}$ jeżeli istnieje ciąg punktów $x_n \in S$ zbieżny do $x$.

$S$ jest domknięty jeśli $\overline{S} = S$.
* Domknięcie dowolnego zbioru jest domknięte.
* S domknięty $\iff$ $X \setminus S$ otwarte.
* $\overline{S} = \bigcap \{F: F \text{ jest zbiorem domkniętym w } X \text{ zawierającym } S\}$

### Zbieżność ciągu punktów

W przestrzeni metrycznej ciąg punktów $(x_n)_{n=1}^{\infty}$ jest zbieżny do $x$, jeśli $d(x_n, x) \rightarrow 0$.

### Wnętrze zbioru

Wnętrzem $Int(S)$ zbioru $S$ nazywamy zbiór punktów, których pewne otoczenia są zawarte w $S$.
Równoważnie $Int(S) = \bigcup_{U \subseteq S} U$ oraz $Int(S) = X \setminus \overline{(X \setminus S)}$.

### Przekształcenie ciągłe

#### W przestrzeniach metrycznych
$f$ jest ciągłe jeśli $\forall_{a\in X} \forall_{\epsilon >0} \exists_{\delta>0} \forall_{x\in X} d_X(a, x) < \delta \implies d_Y(f(a), f(x)) < \epsilon$ (tak jak na analizie)

Równoważnie $f$ jest ciągłe jeśli $x_n \rightarrow x_0 \implies f(x_n) \rightarrow f(x_0)$

#### Ogólnie
$f$ jest ciągłe jeśli dla każdego $U \in \mathcal{T}_Y$ mamy $f^{-1}(U) \in \mathcal{T}_X$

NWSR:
* $f$ jest przekształceniem ciągłym
* jeśli $F$ domknięty to $f^{-1}(F)$ jest domknięty
* dla każdego $A \subseteq X$ mamy $f(\overline{A}) \subseteq \overline{f(A)}$
* dla każdego $x$ i otoczenia $U$ punktu $f(x)$ istnieje otoczenie $V$ punktu $x$ takie, że $f(V) \subseteq U$

### Homeomorfizm
$f$ jest homeomorfizmem, jeśli jest bijekcją oraz $f$ i $f^{-1}$ są ciągłe.

### Iloczyn kartezjański przestrzeni topologicznych
Niech $(X_i, \mathcal{T}_i)$ są przestrzeniami topologicznymi

#### Iloczyn skończony
Rodzina $\mathcal{B} = \{ V_1 \times ... \times V_n :V_i \in \mathcal{T}_i \}$ jest bazą przestrzeni $(X_1 \times ... \times X_n, \mathcal{T})$
Jeżeli $X_i$ mają metryki $d_i$, to $\mathcal{T}$ jest generowana przez $d(a, b) = max_i \{d_i(a_i, b_i) \}$

#### Iloczyn przeliczalny
Rodzina $\mathcal{B} = \{ V_1 \times ... \times V_n \times X_{n+1} \times ... :V_i \in \mathcal{T}_i \}$ jest bazą przestrzeni $(X_1 \times X_2 \times ... , \mathcal{T})$
Jeżeli $X_i$ mają metryki $d_i$, to $\mathcal{T}$ jest generowana przez $d(a, b) = \sum_{i=1}^\infty 2^{-i} min(d_i, 1)$

### Twierdzenie Tietzego o przedłużaniu przekształceń
Niech $A\subseteq X$ domknięta (X - metryzowalna), $f: A \rightarrow [a, b]$ funkcja ciągła. Istnieje wówczas ciągła $\overline{f}: X \rightarrow [a, b]$ t.że $\forall_{x\in A} \overline{f}(x) = f(x)$.

### Twierdzenie o rozkładach jedynki
Niech $X$ metryzowalna i $X = W_1 \bigcup ... \bigcup W_m$, gdzie $W_i$ są zbiorami otwartymi. Istnieją wówczas funkcje ciągłe $\lambda_i$ t.że $\{x: \lambda_i(x)>0\} \subseteq W_i$ oraz dla każdego $x$ mamy $\sum_{i=1}^m \lambda_i(x) = 1$

### Ośrodkowość
Zbiór $S$ jest gęsty, jeśli $\overline{S} = X$.
Przestrzen jest ośrodkowa, jeśli zawiera przeliczalny podzbiór gęsty.
W przestrzeniach metryzowalnych istnienie przeliczalnej bazy jest równoważne ośrodkowości.

### Punkt skupienia ciągu
Punkt jest punktem skupienia ciągu, jeśli każde jego otoczenie zawiera nieskonczenie wiele wyrazów tego ciągu.

### Przestrzenie zwarte
Rodzina zbiorów otwartych $\mathcal{U}$ jest otwartym pokryciem $S$, jeśli $S \subseteq \bigcup \mathcal{U}$.

Przestrzeń jest zwarta, jeżeli jest Hausdorffa oraz z każdego otwartego pokrycia $X$ możemy wybrać pokrycie skończone (podzbiór, który też jest pokryciem $X$).

W przestrzeniach metryzowalnych są równoważne warunki zwartości:
* Z każdego ciągu punktów można wybrać podciąg zbieżny w tej przestrzeni.
* Każdy zstępujący ciąg niepustych zbiorów domkniętych ma niepuste przecięcie.

### Zbiór ograniczony to taki, który zawiera się w jakiejś kuli

### Zbiór zwarty
Zbiór $K$ jest zwarty, jeśli podprzestrzeń $(K, \mathcal{T}_K)$ jest zwarta.
Zbiory domknięte w przestrzeni zwartej są zwarte.
Podzbiór przestrzeni euklidesowej jest zwarty $\iff$ jest domknięty i ograniczony.

### Zbiór zwarty w przestrzeni Hausdorffa jest domknięty

### Przekształcenia ciągłe przestrzeni zwartych
Przekształcenia ciągłe przestrzeni Hausdorffa w przestrzeń Hausdorffa przeprowadza zbiory zwarte w zbiory zwarte.

### Iloczyn przeliczalny przestrzeni zwartych jest zwarty

### Iloczyn kartezjański skończenie wielu przestrzeni zwartych jest zwarty

### Zupełność
Ciąg $x_n$ jest ciągiem Cauchy'ego, jeśli $\forall_{\epsilon>0} \exists_{n_0} \forall{n,m \geq n_0} d(x_n, x_m) < \epsilon$
Przestrzeń metryczna jest zupełna, jeśli każdy ciąg Cauchy'ego w tej przestrzeni jest zbieżny.

### Przestrzeń Hilberta $(l_2, d_h)$
Punktami są ciągi $a_n$ sumowalne z kwadratem, czyli $\sum_{i=1}^\infty a_i^2 < \infty$, metryka to $d_h(a, b) = \sqrt{\sum_{i=1}^\infty (a_i-b_i)^2}$

### Twierdzenie Banacha o punkcie stałym
Przekształcenie $T$ jest zwężające, jeśli dla pewnej stałej $c<1$ dla każdego $x, y$ $d(T(x), T(y)) \leqslant c \cdot d(x, y)$.
$x$ jest punktem stałym $T$, jeśli $T(x) = x$
Jeśli $T$ jest przekształceniem zwężającym przestrzeni zupełnej w siebie to $T$ ma dokładnie jeden punkt stały. Ponadto $T(a), T(T(a)), ...$ zbiega do tego punktu stałego. 
### Zbiór jest brzegowy, jeśli ma puste wnętrze.
### Twierdzenie Baire'a
W przestrzeni metrycznej zupełnej, przeliczalna suma domkniętych zbiorów brzegowych jest zbiorem brzegowym.

### Warunek Cantora
Średnica zbioru: $diam(A) = sup\{d(x, y): x, y \in A\}$
Przestrzeń jest zupełna $\iff$ każdy zstępujący ciąg niepustych zbiorów domkniętych o średnicach dążących do zera ma niepuste przecięcie

### Całkowita ograniczoność
Zbiór  w  przestrzeni  metrycznej $(X,d)$ jest  całkowicie  ograniczony,  jeśli  dla  każdego $\varepsilon > 0$ można go  pokryć  skończenie  wieloma  zbiorami o średnicach $\leqslant \varepsilon$.

- W  przestrzeni  euklidesowej zbiory  ograniczone  są  całkowicie  ograniczone (nie działa dla przestrzeni Hilberta).
- Domknięte i ograniczone podprzestrzenie przestrzeni euklidesowych są więc zupełne i całkowicie ograniczone.

### Przestrzeń metryczna jest zwarta wtedy i tylko wtedy, gdy jest zupełna i całkowicie ograniczona

### Twierdzenie Ascoliego-Arzeli
Jeśli $(X,\mathcal{T})$ jest przestrzenią zwartą, to każde  przekształcenie  ciągłe w przestrzeń euklidesową jest ograniczone. 

Dla przestrzeni zwartych $X$, przestrzeń $C_b(X, \mathbb{R}^n)$ (funkcji ciągłych, ograniczonych) pokrywa się z przestrzenią $C(X, \mathbb{R}^n)$ wszystkich przekształceń ciągłych z $X$ w $\mathbb{R}^n$.

### Jednakowa ciągłość
Rodzina przekształceń ciągłych $F \subseteq C(X, \mathbb{R}^n) : (X, \mathcal{T})\to (\mathbb{R}^n, d_e)$ jest **jednakowo ciągła**, jeśli $\forall x \in X, \varepsilon > 0$ istnieje otoczenie $U$ punktu $x$ takie, że $\forall f \in F$, $diamf(U) \leqslant \varepsilon$.

### Twierdzenie
Rodzina $F \subseteq C(X, \mathbb{R}^n)$ jest  ograniczona,  jeśli $\exists r > 0$, że obrazy $f(X)$ leżą w kuli $B(0, r)$.
Niech $(X, \mathcal{T})$ - przestrzeń zwarta i $\mathcal{F} \subseteq C(X, \mathbb{R}^n)$ - rodzina jednakowo ciągła i ograniczona, wówczas  domknięcie $\mathcal{F}$ w przestrzeni metrycznej $(C(X,\mathbb{R}^n),d_{sup})$ jest zwarte.

### Przestrzeń spójna
Przestrzeń jest spójna, jeśli nie da się jej rozłożyć na sumę dwóch rozłącznych, niepustych zbiorów domkniętych (równoważnie otwartych).
* Przekształcenia ciągłe przeprowadzają zbiory spójne na zbiory spójne.
* Jeżeli rodzina zbiorów spójnych przecina się niepusto, to jej suma jest spójna
* Iloczyn kartezjański skończenie wielu przestrzeni spójnych jest spójny
* Jeżeli $S \subseteq T \subseteq \overline{S}$ i $S$ spójny to $T$ spójny.

### Przestrzeń łukowo spójna
Droga łącząca punkty $a$ i $b$ to przekształcenie $f: [0, 1] \rightarrow X$ ciągłe takie, że $f(0) = a$ i $f(1) = b$
Przestrzeń jest łukowo spójna, jeśli każdą parę punktów z X można połączyć drogą w $X$.
Przestrzeń łukowo spójna jest spójna.

### Składowa przestrzeni topologicznej
Składowa przestrzeni topologicznej to taki zbiór spójny $S$, że żaden zbiór $A \subset S$ nie jest spójny.
Każdy punkt przestrzeni należy do pewnej składowej, a składowe są parami rozłączne.
Wszystkie składowe są domknięte.

### Składowa łukowej spójności przestrzeni 
Składową łukowej spójności przestrzeni nazywamy maksymalny w sensie inkluzji łukowo spójny podzbiór $X$.
Składowe łukowej spójności są parami rozłączne i pokrywają całą przestrzeń.
Składowe łukowej spójności nie muszą być domknięte.

</span>