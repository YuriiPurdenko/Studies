
#include"gra.h"

#define CZAS_ODKRYCIA 500
#define MAKS_DL_TEKSTU 1000

static Dane info;
static bool aktywna;
static GtkWidget *window,*window2;

void pokazBlad(char *komunikat){
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new (GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,"%s",komunikat);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}
static void zakoncz(){ closePipes(info.potoki); gtk_main_quit(); return;}
static void zakoncz2();
static gboolean pobierz_tekst();
static void klikniete(GtkWidget *widget, gpointer *data);
static void stworz_plansze(GtkWidget *widget, gpointer *data);


int main(int argc,char *argv[]){
	if ((info.potoki=initPipes(argc,argv)) == NULL)
	        return 1;
	gtk_init(&argc,&argv);

	if(argv[1][0] == 'B')
		info.gracz=1;
	gchar naglowek[10];
	sprintf(naglowek,"Player %c",argv[1][0]);
	window =gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window),naglowek);
	gtk_window_set_position	(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width( GTK_CONTAINER(window), 10);
	gtk_window_set_default_size(GTK_WINDOW(window),300,400);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(zakoncz),&info.potoki);

	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	GtkWidget *pt = gtk_label_new("Memory");
	GtkWidget *ee = gtk_button_new_with_label("Poziom:\nEasy - 4 x 4");
	GtkWidget *mm = gtk_button_new_with_label("Poziom:\nMedium - 6 x 6");
	GtkWidget *hh = gtk_button_new_with_label("Poziom:\nHard - 8 x 8");
	GtkWidget *ii = gtk_button_new_with_label("Instrukcja");
	int e=4,m=6,h=8;
	g_signal_connect(G_OBJECT(ee),"clicked",G_CALLBACK(stworz_plansze),&e);
	g_signal_connect(G_OBJECT(mm),"clicked",G_CALLBACK(stworz_plansze),&m);
	g_signal_connect(G_OBJECT(hh),"clicked",G_CALLBACK(stworz_plansze),&h);
	g_signal_connect(G_OBJECT(ii),"clicked",G_CALLBACK(pokaz_instrukcje),NULL);

	gtk_box_pack_start(GTK_BOX(box), pt, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), ee, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), mm, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), hh, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), ii, TRUE, TRUE, 0);

	gtk_widget_show_all(window);
	g_timeout_add(10,pobierz_tekst,NULL);
	gtk_main();
	return 0;
}

void stworz_plansze(GtkWidget *widget, gpointer *data){
	if(aktywna){
		char tekst[100]="Tylko jedna gra moze byc naraz uruchomiona";
		popup(tekst);
		return;
	}
	aktywna = 1;
	gchar naglowek[36];
	info.n = *((int*)data);
	gen(info.n,info.przyc);
	if(info.gracz)
		sprintf(naglowek,"Memory %dx%d - B",info.n,info.n);
	else
		sprintf(naglowek,"Memory %dx%d - A",info.n,info.n);

	window2 =gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *image,*button;

	gtk_window_set_title(GTK_WINDOW(window2), naglowek);
	gtk_window_set_position	(GTK_WINDOW(window2),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width( GTK_CONTAINER(window2), 10);

	g_signal_connect(G_OBJECT(window2),"destroy",G_CALLBACK(zakoncz2), &info);

	GtkWidget *box0 =gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(window2), box0);

	GtkWidget *box1 =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(box0), box1);

	GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,2);
	gtk_box_pack_start(GTK_BOX(box1), box2, TRUE, TRUE, 0);
	info.mv = gtk_label_new("Move:\nPlayer A");
	info.pas = gtk_label_new("Player A Score:\n0");
	info.pbs = gtk_label_new("Player B Score:\n0");
	gtk_box_pack_start(GTK_BOX(box2), info.mv, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box2), info.pas, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box2), info.pbs, TRUE, TRUE, 0);

	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
	gtk_grid_set_column_homogeneous	(GTK_GRID(grid), TRUE);
	gtk_box_pack_start(GTK_BOX(box1), grid, TRUE, TRUE, 0);

	GtkWidget *box3 =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *ng = gtk_button_new_with_label("Nowa gra");
	g_signal_connect(G_OBJECT(ng),"clicked",G_CALLBACK(nowa_gra),&info);
	GtkWidget *ps = gtk_button_new_with_label("Poddaj sie");
	g_signal_connect(G_OBJECT(ps),"clicked",G_CALLBACK(poddaj_sie),&info);
	GtkWidget *wg = gtk_button_new_with_label("Wczytaj gre");
	g_signal_connect(G_OBJECT(wg),"clicked",G_CALLBACK(wczytaj_gre),&info);

	gtk_box_pack_start(GTK_BOX(box3), ng, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box3), ps, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box3), wg, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(box0), box3, TRUE, TRUE, 0);

	for(int g=0;g<info.n;g++)
		for(int h=0;h<info.n;h++){
			button = gtk_button_new_with_label(NULL);
			image = gtk_image_new_from_file("obrazki/tlo.jpeg");
			info.przyc[g][h].guz=button;
			info.przyc[g][h].posX=g;
			info.przyc[g][h].posY=h;
			info.przyc[g][h].odkryty=0;
			g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(klikniete),&info.przyc[g][h]);
			gtk_grid_attach(GTK_GRID(grid),button,g,h,1,1);
			gtk_button_set_image(GTK_BUTTON(button),image);
		}
	przekaz_tekst(0,info);
	gtk_widget_show_all(window2);
	return;
}


static void klikniete(GtkWidget *widget, gpointer *data){
	Przycisk *zm = (Przycisk*) data;
	if(info.gracz!=info.ruchgracza){
		char tekst[100]="Kolej przeciwnika";
		popup(tekst);
		return;
	}
	if(zm->odkryty || info.odkryte>=2){
		return;
	}
	odkryj(*zm);
	zm->odkryty=1;
	info.ktoreodkryl[info.odkryte]=zm;
	info.odkryte++;
	info.ruchy+=2;
	if(info.odkryte==2){
		if((info.ktoreodkryl[0])->obraz!=(info.ktoreodkryl[1])->obraz){
			g_timeout_add(CZAS_ODKRYCIA,zakryj,info.ktoreodkryl[0]);
			g_timeout_add(CZAS_ODKRYCIA,zakryj,info.ktoreodkryl[1]);
			(info.ktoreodkryl[1])->odkryty=0;
			(info.ktoreodkryl[0])->odkryty=0;
			if(info.ruchgracza==1){
				gtk_label_set_text(GTK_LABEL(info.mv),"Move:\nPlayer A");
				info.ruchgracza=0;
			}
			else{
				gtk_label_set_text(GTK_LABEL(info.mv),"Move:\nPlayer B");
				info.ruchgracza=1;
			}
			przekaz_tekst(1,info);
		}
		else{
			char sl[19];
			if(info.ruchgracza==1){
				info.punktyB++;
				sprintf(sl,"Player B Score:\n%d",info.punktyB);
				gtk_label_set_text(GTK_LABEL(info.pbs),sl);
			}
			else{
				info.punktyA++;
				sprintf(sl,"Player A Score:\n%d",info.punktyA);
				gtk_label_set_text(GTK_LABEL(info.pas),sl);
			}
			bool czywygral=0;
			for(int g=0;g<info.n;g++)
				for(int h=0;h<info.n;h++){
					if(info.przyc[g][h].odkryty==0){
						czywygral=1;
						break;
					}
				}
			if(czywygral==0){
				if(info.gracz==0){
					if(info.punktyA>info.punktyB){
						char slowo[100]="Wygrales :D";
						popup(slowo);
					}
					if(info.punktyA<info.punktyB){
						char slowo[100]="Przegrales :(";
						popup(slowo);
					}
				}
				else{
					if(info.punktyA<info.punktyB){
						char slowo[100]="Wygrales :D";
						popup(slowo);
					}
					if(info.punktyA>info.punktyB){
						char slowo[100]="Przegrales :(";
						popup(slowo);
					}
				}
				if(info.punktyA==info.punktyB){
					char slowo[100]="Remis!!!";
					popup(slowo);
				}
				przekaz_tekst(4,info);
			}
			przekaz_tekst(0,info);
		}
		info.odkryte=0;
	}
	else
		przekaz_tekst(0,info);
	return;
}

gboolean pobierz_tekst(){
	char wejscie[MAKS_DL_TEKSTU+5];
	if(info.n==0){
		getStringFromPipe(info.potoki,wejscie,MAKS_DL_TEKSTU);
		return TRUE;
	}
	int zliczaj=0;														//chyba 342 jest maksymalne
	int RUCHY=0,N=0,PUNKTYA=0,PUNKTYB=0,RUCHGRACZA=0,licz=0;
	int xx=-1,yy=-1,x2=-1,y2=-1;
	if(getStringFromPipe(info.potoki,wejscie,MAKS_DL_TEKSTU)) {
		if(wejscie[0]=='z'){
			char slowo[100]="Uruchomiono naraz dwie rozne rozgrywki";
			popup(slowo);
			zakoncz2();
			return true;
		}
		if(wejscie[0]=='p'){
			char slowo[100]="Przeciwnik sie poddal :D";
			popup(slowo);
			info.ruchgracza=info.gracz;
			licz++;
			if(info.ruchgracza==0)
				gtk_label_set_text(GTK_LABEL(info.mv),"Move:\nPlayer A");
			else
				gtk_label_set_text(GTK_LABEL(info.mv),"Move:\nPlayer B");

			for(int g=0;g<info.n;g++){
				for(int h=0;h<info.n;h++){
					odkryj((info.przyc[g][h]));
					info.przyc[g][h].odkryty=1;
				}
			}
			if(info.gracz==1){
				char tekst[100];
				sprintf(tekst,"%s%d","Player B Score:\n",(info.n)*(info.n)/2);
				info.punktyB=(info.n)*(info.n)/2;
				info.punktyA=0;
				gtk_label_set_text(GTK_LABEL(info.pas),"Player A Score:\n0");
				gtk_label_set_text(GTK_LABEL(info.pbs),tekst);
			}
			else{
				char tekst[100];
				sprintf(tekst,"%s%d","Player A Score:\n",(info.n)*(info.n)/2);
				info.punktyA=(info.n)*(info.n)/2;
				info.punktyB=0;
				gtk_label_set_text(GTK_LABEL(info.pbs),"Player B Score:\n0");
				gtk_label_set_text(GTK_LABEL(info.pas),tekst);
			}
			return true;
		}
		if(wejscie[0]=='w'){
			for(int g=0;g<info.n;g++){
				for(int h=0;h<info.n;h++){
					odkryj((info.przyc[g][h]));
					info.przyc[g][h].odkryty=1;
				}
			}
				if(info.gracz==0){
					if(info.punktyA>(info.n)*(info.n)/2-info.punktyA){
						char slowo[100]="Wygrales :D";
						popup(slowo);
					}
					if(info.punktyA<(info.n)*(info.n)/2-info.punktyA){
						char slowo[100]="Przegrales :(";
						popup(slowo);
					}
					if(info.punktyA==(info.n)*(info.n)/2-info.punktyA){
						char slowo[100]="Remis!!!";
						popup(slowo);
					}
				}
				else{
					if((info.n)*(info.n)/2-info.punktyB<info.punktyB){
						char slowo[100]="Wygrales :D";
						popup(slowo);
					}
					if((info.n)*(info.n)/2-info.punktyB>info.punktyB){
						char slowo[100]="Przegrales :(";
						popup(slowo);
					}
					if(info.punktyB==(info.n)*(info.n)/2-info.punktyB){
						char slowo[100]="Remis!!!";
						popup(slowo);
					}
				}
		}
		if(wejscie[0]=='n'){															//jesli przeciwnik odkryl niedobrana pare to po sprawdzeniu pewnych warunkow zostana one zakryte
			xx=wejscie[1]-'0',yy=wejscie[2]-'0',x2=wejscie[3]-'0',y2=wejscie[4]-'0';
			licz=5;
		}																					//z pliku odczytuje informacje o rozgrywce przeciwnika
		while(wejscie[licz]!=' '){
			RUCHY=RUCHY*10+wejscie[licz]-'0';
			licz++;
		}
		licz++;
		N=wejscie[licz]-'0';
		licz+=2;
		while(wejscie[licz]!=' '){
			PUNKTYA=PUNKTYA*10+wejscie[licz]-'0';
			licz++;
		}
		licz++;
		while(wejscie[licz]!=' '){
			PUNKTYB=PUNKTYB*10+wejscie[licz]-'0';
			licz++;
		}
		licz++;
		RUCHGRACZA=wejscie[licz]-'0';
		licz++;
		if(N!=info.n){																			//jesli sa uruchomione naraz rozgrywki dla dwoch roznych plansz zostaja one wylaczone
			char slowo[100]="Uruchomiono naraz dwie rozne rozgrywki";
			popup(slowo);
			przekaz_tekst(2,info);
			zakoncz2();
			return TRUE;
		}
		if(info.ruchy>RUCHY && RUCHY!=0)															//jesli gracz, ktory wyslal informacje ma mniej ruchow od drugiego gracza uaktualnia informacje zgodne z drugim grazcem (nie liczac rozpoczecia nowej rozgrywki lub wznowienia starej dlatego rozna od zera)
			przekaz_tekst(0,info);
		else{
			if(info.ruchy==0 && RUCHY==1)														//jak wznawiam aby plansza ktora wznawiam nie zakryla sie cala
				return TRUE;
			for(int g=0;g<N;g++)
				for(int h=0;h<N;h++){
					(info.przyc[g][h]).obraz=0;
					while(wejscie[licz]==' ')
						licz++;
					while(wejscie[licz]!=' '){
						(info.przyc[g][h]).obraz=(info.przyc[g][h]).obraz*10+wejscie[licz]-'0';
						licz++;
					}
				}
			for(int g=0;g<N;g++)																//uaktualniam plansze graczy
				for(int h=0;h<N;h++){
					licz++;
					(info.przyc[g][h]).odkryty=wejscie[licz]-'0';
					if((info.przyc[g][h]).odkryty || (xx==g && yy==h) || (x2==g && y2==h)){
						odkryj((info.przyc[g][h]));
						zliczaj++;
					}
					else
						g_timeout_add(1,zakryj,&(info.przyc[g][h]));
					licz++;
				}
			info.ruchy=RUCHY;																		//ukatualniam dane
			info.punktyA=PUNKTYA;
			info.punktyB=PUNKTYB;
			info.ruchgracza=RUCHGRACZA;
			char sl[19];
			sprintf(sl,"Player A Score:\n%d",info.punktyA);
			gtk_label_set_text(GTK_LABEL(info.pas),sl);
			sprintf(sl,"Player B Score:\n%d",info.punktyB);
			gtk_label_set_text(GTK_LABEL(info.pbs),sl);
			if(info.ruchgracza==0)
				gtk_label_set_text(GTK_LABEL(info.mv),"Move:\nPlayer A");
			else
				gtk_label_set_text(GTK_LABEL(info.mv),"Move:\nPlayer B");
			if(wejscie[0]=='n'){										//zakrywam po jakims czasie zla pare kart, ktora odkryl przeciwnik
				g_timeout_add(CZAS_ODKRYCIA,zakryj,&info.przyc[xx][yy]);
				g_timeout_add(CZAS_ODKRYCIA,zakryj,&info.przyc[x2][y2]);
			}
			if(zliczaj%2==1 && info.gracz==info.ruchgracza){
				info.odkryte=1;
				int miejsce=bezpary(info.przyc,info.n);
				info.ktoreodkryl[0]=&info.przyc[miejsce%info.n][miejsce/info.n];
				}
		}
		if(info.ruchy==0)
			info.ruchy+=2;
	}
	return TRUE;
}
void zakoncz2(){
	FILE *fptr;
	char tekst[MAKS_DL_TEKSTU+5],tekst2[MAKS_DL_TEKSTU];
	char sl[24];
	sprintf(sl,".zapis%d.txt",info.n);
	fptr = fopen(sl,"w");
	sprintf(tekst,"%d %d %d %d %d",info.ruchy,info.n,info.punktyA,info.punktyB,info.ruchgracza);
	for(int g=0;g<info.n;g++)
		for(int h=0;h<info.n;h++){
			strncpy(tekst2,tekst,sizeof(tekst)-5);
			sprintf(tekst,"%s %d",tekst2,info.przyc[g][h].obraz);
		}
	for(int g=0;g<info.n;g++)
		for(int h=0;h<info.n;h++){
			strncpy(tekst2,tekst,sizeof(tekst)-5);
			sprintf(tekst,"%s %d",tekst2	,info.przyc[g][h].odkryty);
		}
	fprintf(fptr,"%s",tekst);
	fclose(fptr);
	for(int g=0;g<info.n;g++)
		for(int h=0;h<info.n;h++)
			info.przyc[g][h].odkryty=0;
	info.n=info.odkryte=info.punktyA=info.punktyB=0;
	info.ruchgracza=0;
	info.ruchy=1;
	aktywna = 0;
	gtk_widget_destroy(GTK_WIDGET(window2));
}
