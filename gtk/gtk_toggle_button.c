//
// Created by mutter on 6/9/16.
//

#include "gtk/gtk.h"

static void callback_delete(GtkWidget *widget, gpointer data)
{
    g_print("Hello click\n");
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *toggle_button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Toggle button");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    toggle_button = gtk_toggle_button_new_with_label("Toggle button");
    g_signal_connect(toggle_button, "clicked", G_CALLBACK(callback_delete), NULL);

    gtk_container_add(GTK_CONTAINER(window), toggle_button);

    gtk_widget_show(toggle_button);
    gtk_widget_show(window);
    gtk_main();

    return GDK_OK;
}