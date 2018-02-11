//
// Created by mutter on 6/9/16.
//

#include "gtk/gtk.h"

static void toggle_callback(GtkWidget *widget, gpointer data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        g_print("active\n");
    } else {
        g_print("inactive\n");
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *check_button;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 100, 100);
    gtk_window_set_title(GTK_WINDOW(window), "Check button");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    check_button = gtk_check_button_new_with_label("check here");
    g_signal_connect(check_button, "toggled", G_CALLBACK(toggle_callback), NULL);

    gtk_container_add(GTK_CONTAINER(window), check_button);

    gtk_widget_show(check_button);
    gtk_widget_show(window);
    gtk_main();
    return GDK_OK;
}
