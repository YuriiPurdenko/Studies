#include<string.h>
#include<gtk/gtk.h>
#include<stdbool.h>
static void test_nacisniecia (GtkWidget *widget, gpointer data);

int t[1000]={0},n;
bool nastperm(int x,int y[]);

GtkWidget *T[6][6];

//Wiktor Pilarczyk lista 10 Zad 1
int main (int argc,char *argv[] )
{
	sscanf(argv[1],"%d",&n);
	for(int g=1;g<=n*n;g++)
        	t[g]=g;
	gtk_init(&argc, &argv);

	GtkWidget *window =gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW(window), "Magiczny kwadrat");
	gtk_window_set_position	(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width( GTK_CONTAINER(window), 10);

	g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit), NULL);

	GtkWidget *box1 =gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	gtk_container_add(GTK_CONTAINER(window), box1);
	GtkWidget *button = gtk_button_new_with_label("Kolejny kwadrat");
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(test_nacisniecia),(gpointer)"Przycisk");

	gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);

	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
	gtk_grid_set_column_homogeneous	(GTK_GRID(grid), TRUE);
	gtk_box_pack_start(GTK_BOX(box1), grid, TRUE, TRUE, 0);
	char slow[10];
	for(int h=0; h< n;h++)
		for(int i = 0; i < n; i++) {
			sprintf(slow,"%d",t[h*n+i+1]);
			char *opis=slow;
			T[h][i]=gtk_label_new (opis);
			gtk_grid_attach	(GTK_GRID(grid), T[h][i], i, h, 1, 1);
		}

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

static void test_nacisniecia ( GtkWidget *widget, gpointer data)
{
    	char pomoc[10];
	nastperm(n*n,t);
	for(int g=0;g<n;g++){
		for(int h=0;h<n;h++){
			sprintf(pomoc,"%d",t[g*n+h+1]);
			gtk_label_set_text(GTK_LABEL(T[g][h]),(gchar*)pomoc);
		}
	}
}
bool nastperm(int x,int y[])
{
    int zm;
    if(x==1)
        return false;
    if(y[1]==x)
    {
        for(int g=1;g<x;g++)
        {
            y[g]=y[g+1];
        }
        if(nastperm(x-1,y))
        {
            y[x]=x;
        }
        else
            return false;
    }
    else
    {
        for(int g=2;g<=x;g++)
            if(y[g]==x)
            {
                zm=y[g-1];
                y[g-1]=x;
                y[g]=zm;
                break;
            }
    }
    return true;
}
