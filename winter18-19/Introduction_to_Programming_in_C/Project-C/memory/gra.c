#include"gra.h"

#define CZAS_ODKRYCIA 500
#define MAKS_DL_TEKSTU 10000

void nowa_gra(GtkWidget *widget, gpointer *data){
	Dane *wsk = (Dane *) data;
	bool czywszystkozakryte=0;
	for(int g=0;g<wsk->n;g++)
		for(int h=0;h<wsk->n;h++){
			if((wsk->przyc[g][h]).odkryty==0){
				czywszystkozakryte=1;
			}
		}
	if(czywszystkozakryte){
		char tekst[100]="Najpierw nalezy dokonczyc rozgrywke lub sie poddac";
		popup(tekst);
		return;
	}
	if(wsk->ruchgracza!=wsk->gracz){
		char tekst[100]="Tylko zwyciezca moze rozpoczac nowa gre lub osoba ktora doprowadzila do remisu";
		popup(tekst);
		return;
	}
	wsk->punktyA=wsk->punktyB=wsk->odkryte=0;
	wsk->ruchy=0;
	gtk_label_set_text(GTK_LABEL(wsk->pas),"Player A Score:\n0");
	gtk_label_set_text(GTK_LABEL(wsk->pbs),"Player B Score:\n0");
	for(int g=0;g<wsk->n;g++){
		for(int h=0;h<wsk->n;h++){
			g_timeout_add(1,zakryj,&wsk->przyc[g][h]);
			(wsk->przyc[g][h]).odkryty=0;
		}
	}
		gen(wsk->n,wsk->przyc);
	przekaz_tekst(0,*wsk);
	wsk->ruchy+=2;
		return;
}

void przekaz_tekst(int zak,Dane zm){
	char tekst[MAKS_DL_TEKSTU+100],tekst2[MAKS_DL_TEKSTU];
	if(zak==4){
		sprintf(tekst,"w");
		sendStringToPipe(zm.potoki, tekst);
		return;
	}
	if(zak==3){
		sprintf(tekst,"p");
		sendStringToPipe(zm.potoki, tekst);
		return;
	}
	if(zak==2){
		sprintf(tekst,"z");
		sendStringToPipe(zm.potoki, tekst);
		return;
	}
	if(zak==1){
		sprintf(tekst,"n%d%d%d%d",(zm.ktoreodkryl[0])->posX,(zm.ktoreodkryl[0])->posY,(zm.ktoreodkryl[1])->posX,(zm.ktoreodkryl[1])->posY);
		strncpy(tekst2,tekst,sizeof(tekst)-100);
		sprintf(tekst,"%s%d %d %d %d %d",tekst2,zm.ruchy,zm.n,zm.punktyA,zm.punktyB,zm.ruchgracza);
	}
	else
		sprintf(tekst,"%d %d %d %d %d",zm.ruchy,zm.n,zm.punktyA,zm.punktyB,zm.ruchgracza);
	for(int g=0;g<zm.n;g++)
		for(int h=0;h<zm.n;h++){
			strncpy(tekst2,tekst,sizeof(tekst)-100);
			sprintf(tekst,"%s %d",tekst2,(zm.przyc[g][h]).obraz);
		}
	for(int g=0;g<zm.n;g++)
		for(int h=0;h<zm.n;h++){
			strncpy(tekst2,tekst,sizeof(tekst)-100);
			sprintf(tekst,"%s %d",tekst2,(zm.przyc[g][h]).odkryty);
		}
	sendStringToPipe(zm.potoki, tekst);
	return;
}
void poddaj_sie(GtkWidget *widget,gpointer *data){
	Dane *zm = (Dane *) data;
	bool czymoge=1;
	for(int g=0;g<zm->n;g++)
		for(int h=0;h<zm->n;h++)
			if(zm->przyc[g][h].odkryty==0){
				czymoge=0;
				break;
			}
	if(czymoge){
		char tekst[100]="Nie mozna poddac zakonczonej rozgrywki";
		popup(tekst);
		return;
	}
	for(int g=0;g<zm->n;g++)
		for(int h=0;h<zm->n;h++){
			odkryj((zm->przyc[g][h]));
			zm->przyc[g][h].odkryty=1;
		}
	zm->ruchy++;
	if(zm->ruchgracza==zm->gracz)
		zm->ruchgracza=(zm->gracz+1)%2;
	if(zm->gracz==0){
		char tekst[100]="Player B Score:\n",tekst2[95];
		strncpy(tekst2,tekst,sizeof(tekst)-5);
		sprintf(tekst,"%s%d",tekst2,(zm->n)*(zm->n)/2);
		zm->punktyB=(zm->n)*(zm->n)/2;
		zm->punktyA=0;
		gtk_label_set_text(GTK_LABEL(zm->pas),"Player A Score:\n0");
		gtk_label_set_text(GTK_LABEL(zm->pbs),tekst);
	}
	else{
		char tekst[100]="Player A Score:\n",tekst2[95];
		strncpy(tekst2,tekst,sizeof(tekst)-5);
		sprintf(tekst,"%s%d",tekst2,(zm->n)*(zm->n)/2);
		zm->punktyA=(zm->n)*(zm->n)/2;
		zm->punktyB=0;
		gtk_label_set_text(GTK_LABEL(zm->pbs),"Player B Score:\n0");
		gtk_label_set_text(GTK_LABEL(zm->pas),tekst);
	}
	if(zm->ruchgracza==0)
		gtk_label_set_text(GTK_LABEL(zm->mv),"Move:\nPlayer A");
	else
		gtk_label_set_text(GTK_LABEL(zm->mv),"Move:\nPlayer B");
	przekaz_tekst(3,*zm);
}
void wczytaj_gre(GtkWidget *widget,gpointer *data){
	Dane *wart = (Dane*) data;
	bool sprawdz=0;
	if(wart->ruchy>1){
		char tekst[100]="Gre mozna wczytac tylko na poczatku";
		popup(tekst);
		return;
	}
	for(int g=0;g<wart->n;g++)
		for(int h=0;h<wart->n;h++)
			if(wart->przyc[g][h].odkryty==1){
				sprawdz=1;
				break;
			}
	if(sprawdz){
		char tekst[100]="Gre mozna wczytac tylko na poczatku rozgrywki";
		popup(tekst);
		return;
	}

	int zliczaj=0;
	FILE *fptr;
	char sl[15];
	int zm;
	sprintf(sl,".zapis%d.txt",wart->n);
	fptr = fopen(sl,"r");
	fscanf(fptr,"%d %d %d %d %d",&wart->ruchy,&wart->n,&wart->punktyA,&wart->punktyB,&zm);
	wart->ruchgracza=zm;
	for(int g=0;g<wart->n;g++)
		for(int h=0;h<wart->n;h++)
			fscanf(fptr,"%d",&wart->przyc[g][h].obraz);
	for(int g=0;g<wart->n;g++)
		for(int h=0;h<wart->n;h++){
			fscanf(fptr,"%d",&zm);
			wart->przyc[g][h].odkryty=zm;
			if(wart->przyc[g][h].odkryty){
				odkryj(wart->przyc[g][h]);
				zliczaj++;
			}
			else
				g_timeout_add(1,zakryj,&wart->przyc[g][h]);
		}
	if(zliczaj%2==1 && wart->gracz==wart->ruchgracza){
		wart->odkryte=1;
		int miejsce=bezpary(wart->przyc,wart->n);
		wart->ktoreodkryl[0]=&wart->przyc[miejsce%wart->n][miejsce/wart->n];
	}
	fclose(fptr);
	wart->ruchy=0;
	char wtf[19];
	sprintf(wtf,"Player A Score:\n%d",wart->punktyA);
	gtk_label_set_text(GTK_LABEL(wart->pas),wtf);
	sprintf(wtf,"Player B Score:\n%d",wart->punktyB);
	gtk_label_set_text(GTK_LABEL(wart->pbs),wtf);
	if(wart->ruchgracza==0)
		gtk_label_set_text(GTK_LABEL(wart->mv),"Move:\nPlayer A");
	else
		gtk_label_set_text(GTK_LABEL(wart->mv),"Move:\nPlayer B");
	przekaz_tekst(0,*wart);
	wart->ruchy+=2;
	return;
}
void popup(char *t){
	GtkWidget *window =gtk_window_new(GTK_WINDOW_TOPLEVEL),*lab;
	gtk_window_set_title(GTK_WINDOW(window),"Komunikat");
	gtk_window_set_position	(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window),200,50);
	gtk_container_set_border_width( GTK_CONTAINER(window), 10);
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(zamknij), window);

	gchar tekst[100];
	sprintf(tekst,"%s",t);

	GtkWidget *box =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	lab=gtk_label_new(tekst);
	GtkWidget *ok = gtk_button_new_with_label("Ok");

	g_signal_connect(G_OBJECT(ok),"clicked",G_CALLBACK(zamknij),window);


	gtk_container_add(GTK_CONTAINER(box), lab);
	gtk_container_add(GTK_CONTAINER(box), ok);
	gtk_container_add(GTK_CONTAINER(window), box);

	gtk_widget_show_all(window);
	return;
}
