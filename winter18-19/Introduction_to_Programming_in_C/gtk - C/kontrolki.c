#include<string.h>
#include<gtk/gtk.h>

// Prototypy własnych funkcji obsługi zdarzen srodowiska GTK+ 3.0

static void test_nacisniecia (GtkWidget *widget, gpointer data);
static void wyjscie_przyciskiem ( GtkWidget *widget, gpointer data);
static void wypisz_wprowadzony_tekst ( GtkWidget *widget, gpointer data);
static void dodaj_do_text( GtkWidget *widget, gpointer data);

static GtkWidget *text;// uzyte w funkcjach dodaj_do_text oraz main

int main (int argc,char *argv[] )
{
	gtk_init(&argc, &argv);
	
	GtkWidget *window =gtk_window_new(GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title(GTK_WINDOW(window), "GTK - panel numeryczny");
	gtk_window_set_position	(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width( GTK_CONTAINER(window), 10);
	
	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit), NULL);
	
	GtkWidget *box1 =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	gtk_container_add(GTK_CONTAINER(window), box1);
	GtkWidget *button = gtk_button_new_with_label("Kliknij tu");
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(test_nacisniecia),(gpointer)"Przycisk");
			
	gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
	
	text =	gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(text), 50);
	g_signal_connect(G_OBJECT(text),"activate",
			G_CALLBACK(wypisz_wprowadzony_tekst),
			(gpointer) text);
	gtk_entry_set_text(GTK_ENTRY(text),"Wprowadz");
	gint tmp_pos =	gtk_entry_get_text_length(GTK_ENTRY(text));
	gtk_editable_insert_text(GTK_EDITABLE(text)," jakis tekst", -1, &tmp_pos);
	gtk_editable_select_region(GTK_EDITABLE(text),
			0,gtk_entry_get_text_length(GTK_ENTRY(text)));
			
	gtk_box_pack_start(GTK_BOX(box1), text, TRUE, TRUE, 0);
	
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_column_homogeneous	(GTK_GRID(grid), TRUE);
	gtk_box_pack_start(GTK_BOX(box1), grid, TRUE, TRUE, 0);
	
	static struct przycisk {
		char *opis, *wyjscie;
		int posX,lenX,posY,lenY;
	} tab[] = {
		{"NumLock","",0,1,0,1}, {"/","/",1,1,0,1}, {"*","*",2,1,0,1}, {"-","-",3,1,0,1},
		{"7","7",0,1,1,1}, {"8","8",1,1,1,1}, {"9","9",2,1,1,1},{"+","+",3,1,1,2},
		{"4","4",0,1,2,1}, {"5","5",1,1,2,1}, {"6","6",2,1,2,1},
		{"1","1",0,1,3,1}, {"2","2",1,1,3,1}, {"3","3",2,1,3,1}, {"Enter","\n",3,1,3,2},
		{"0","0",0,2,4,1}, {".",".",2,1,4,1}
		};
	
	button=	gtk_label_new (tab[0].opis);
	gtk_grid_attach	(GTK_GRID(grid), button, tab[0].posX, tab[0].posY, tab[0].lenX, tab[0].lenY);
	
	for(int i = 1; i < 17; i++) {
		button =gtk_button_new_with_label(tab[i].opis);
		g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(dodaj_do_text),
			(gpointer) tab[i].wyjscie);
		gtk_grid_attach	(GTK_GRID(grid), button, tab[i].posX, tab[i].posY, tab[i].lenX, tab[i].lenY);
	}
	
	GtkWidget *box2 =gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	button =gtk_button_new_with_label("Koniec");
	g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(wyjscie_przyciskiem), NULL);
	gtk_box_pack_start(GTK_BOX(box2), button, TRUE, FALSE, 0);
	
	gtk_box_pack_start(GTK_BOX(box1), box2, TRUE, TRUE, 0);
	
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}


// definicje własnych funkcji obsługi zdarzen

static void test_nacisniecia ( GtkWidget *widget, gpointer data)
{
	g_print("GTK-test - %s zastal nacisniety\n", (gchar *) data);
}

static void wyjscie_przyciskiem( GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

static void wypisz_wprowadzony_tekst ( GtkWidget *widget, gpointer data)
{
	g_print("GTK-test wprowadzono: %s\n",gtk_entry_get_text(GTK_ENTRY(data)));
}

static void dodaj_do_text ( GtkWidget *widget, gpointer data)
{
	gint tmp_pos = gtk_entry_get_text_length(GTK_ENTRY(text));
	if(strcmp((gchar *)data,"\n") == 0)
		gtk_entry_set_text(GTK_ENTRY(text),"");
	else
		gtk_editable_insert_text(GTK_EDITABLE(text), (gchar *)data, -1, &tmp_pos);
}
	
