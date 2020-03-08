#include "stdio.h"
#include <stdlib.h>
#include "stdbool.h"
#include "math.h"
#include<ctype.h>

typedef struct wezel{
	bool czyzm;
	char op;
	int wspol;
	int pot;
	struct wezel *lewy,*prawy;
}	*NaWezel, Wezel;
typedef struct{
	bool czyjed;
	int jed1;
	int jed2;
	char dzial;
}	Kol;
void czytaj();
int prior(char x);

/*
dodawaj();
odejmuj();
mnoz();
rozniczkuj();
wypisz();
*/
