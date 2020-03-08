#include"plansza.h"
#include"fifo.h"
typedef struct dane{
	bool ruchgracza,gracz;
	int n,odkryte,punktyA,punktyB,ruchy;
	Przycisk przyc[8][8];
	Przycisk *ktoreodkryl[2];
	GtkWidget *mv,*pas,*pbs;
	PipesPtr potoki;
} Dane;
void pokaz_instrukcje(GtkWidget *widget, gpointer *data);
void zamknij(GtkWidget *widget, gpointer *data);
