Zakleszczenie(deadlock) - sytuacja, w której co najmniej dwie różne akcje czekają na siebie nawzajem, więc żadna nie może się zakończyć.

	W przypadku zbioru procesów/akcji do zakleszczenia dochodzi wtedy, kiedy każdy ze ze zbioru czeka na zdarzenie, ktore moze spowodowac inny proces ze zbioru

	improper resource allocation prevents forward progress

	wątek A czeka aż wątek B będzie w odpowiednim stanie (np. zwolni mutex do pliku x) a wątek B czeka na odpowiedni stan wątku A (np. zwolni mutex do pliku y). Wtedy kod nie wykonuje się a mamy pat
	
livelock: wątek A robi coś dopóki jakiś stan się nie zmieni (np. iteruje i do n), natomiast wątek B czeka na pewne warunki, żeby przejść do innej pracy(np. iteruje i w dół). Oba są w sytuacji, w której jeden odwraca skutki działania drugiego więc nie mogą się zakończyć, ale procesor tyka w przeciwieństwie do deadlocka (robotnik A zasypuje dziurę piachem a B wyrzuca z dziury piach. A kończy jak dziura zasypana, B jak pusta).

glodzenie(Starvation): external events and/or system scheduling decisions can prevent sub-task progress

	sytuacja: oczekiwanie procesu, aż zwolni się dostęp do danego zasobu, "wiecznie"/bardzo długo blokowany przez inny proces

	wątek A czeka na pewne współdzielone zasoby, ale nie może się do nich dostać, bo inne zachłanne wątki kradną mu dostęp (np. synchronizowana metoda obiektu zajmująca dużo czasu - jak jeden wątek często ją wywołuje, to kradnie czas innym wątkom, które też wymagają częstej synchronizacji).



zasob - wszystko to co trzeba uzyskac, wykorzystac i zwolnic 

	zależy o jakie chodzi. Zasobem może być urządzenie na wyłączność np. drukarka. Może być CPU. Może być plik. Może być pamięć. Ogólnie coś co musi zostać: zdobyte → użyte → oddane

Skrzyzowania

	deadlock - skrzyzowanie z 4 samochodami

	livelock - dwupasmowe rondo z 2 samochodami i 2 zjazdami, jak jeden chce zjechac drugi go blokuje, on wyprzedza go i blokuje zjazd pierwszemu (slabe)
		lub jak dwa samochody chca naraz zjechac na rondzie

	starvation - skrzyzowanie z pierwszenstwem, auto drodze podporzadkowanej stoi 

Wykrywanie i usuwanie:

	przeszukiwanie grafu zasobow i sprawdzanie czy nei ma cyklu

	1. przerwanie wszystkich procesow/watkow zakleszczonych oprocz jednego co skutkuje uwolnieniem zasobow
	2. wywlaszczanie zasobow

Zapobieganie:

	4 warunki z ksiazki (niedopuszczanie do ich 
		-kazdy zasob jest przypisany dokladnie do jednego procesu albo jest dostepny
		-procesy posiadajace przydzielone zasoby wczesniej moga zadac nowych
		-brak wywlaszczania
		-cykl w oczekiwaniu na zasoby
