//
// Created by mutter on 6/10/16.
//

#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *scrollbar;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Scrollbar");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    scrollbar = gtk_vscrollbar_new(NULL);

    gtk_container_add(GTK_CONTAINER(window), scrollbar);

    gtk_widget_show(scrollbar);
    gtk_widget_show(window);

    gtk_main();
    return GDK_OK;
}