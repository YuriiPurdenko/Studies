#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <time.h>
#include<gtk/gtk.h>
typedef struct przycisk{
	GtkWidget *guz;
	int posX,posY;
	int obraz;
	bool odkryty;
}	Przycisk;

void gen(int n, Przycisk tab[8][8]);
void odkryj(Przycisk zm);
gboolean zakryj(gpointer data);
int bezpary(Przycisk tab[8][8],int x);
