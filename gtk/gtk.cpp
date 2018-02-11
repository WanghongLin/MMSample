#include <iostream>
#include <gtk/gtk.h>

using namespace std;

int main(int argc, char *argv[]) {
    cout << "Hello, World!" << endl;

    gtk_init(&argc, &argv);

    GtkWidget *widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_show(widget);

    gtk_main();
    return 0;
}