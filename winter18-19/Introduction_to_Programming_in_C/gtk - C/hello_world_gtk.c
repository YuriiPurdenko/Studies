#include <gtk/gtk.h>
int main (int argc,char *argv[])
{
    gtk_init (&argc, &argv);	//inicjalizacja gtk?

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);	//zadeklarowanie okienka w gtk

    gtk_window_set_title(GTK_WINDOW(window),"Zaczynamy z GTK+ 3.0");	//tyttul okienka
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);	//ustawienie pozycji okienka
    gtk_container_set_border_width (GTK_CONTAINER(window), 300);		//ustawienie szerokosci okienka
	gtk_container_set_border_height (GTK_CONTAINER(window), 30);
	
    g_signal_connect(G_OBJECT (window),"destroy",G_CALLBACK (gtk_main_quit),NULL);	//???

    GtkWidget *button= gtk_button_new_with_label ("Witamy w ́swiecie interfejsów GTK+");	//zadeklarowanie przycisku z napisem
    gtk_container_add (GTK_CONTAINER (window), button);	//do okienka dodaje przycisk
    gtk_widget_show (button);		

    gtk_widget_show (window);

    gtk_main();

    return 0;
}
