#include "plansza.h"

void gen(int n, Przycisk tab[8][8]){
	int uz[66]={0};
	int zm;
	srand(time(NULL));
	for(int g=0;g<n;g++)
		for(int h=0;h<n;h++){
			zm=rand()%(n*n/2);
			while(uz[zm%(n*n/2)]>=2)
				zm++;
			zm%=(n*n/2);
			tab[g][h].obraz=zm;
			uz[zm]++;
		}
	return;
}
void odkryj(Przycisk zm){
	char S[16];
	sprintf(S,"obrazki/00.png");
	int a=zm.obraz;
	if(a>0)
		S[9]+=(a%10);
	a/=10;
	if(a>0)
		S[8]+=(a%10);
	GtkWidget *image = gtk_image_new_from_file(S);
	gtk_button_set_image(GTK_BUTTON(zm.guz),image);
	return;
}
gboolean zakryj(gpointer data){
	Przycisk *zm =  data;
	GtkWidget *image = gtk_image_new_from_file("obrazki/tlo.jpeg");
	gtk_button_set_image(GTK_BUTTON(zm->guz),image);
	return FALSE;
}
int bezpary(Przycisk tab[8][8],int x){
	for(int g=0;g<x*x;g++)
		for(int h=g+1;h<x*x;h++){
			if(tab[g%x][g/x].obraz==tab[h%x][h/x].obraz && tab[g%x][g/x].odkryty!=tab[h%x][h/x].odkryty){
				if(tab[g%x][g/x].odkryty)
					return g;
				else
					return h;
				break;
			}
		}
	return -1;
}
