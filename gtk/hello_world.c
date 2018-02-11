//
// Created by mutter on 6/5/16.
//

#include <gtk/gtk.h>

static void hello(GtkWidget *widget, gpointer data)
{
    g_print("Hello World\n");
}

static gboolean delete_event(GtkWidget *widget,
                             GdkEvent *event, gpointer data)
{
    g_print("delete event occurred %d\n", event->key.keyval);

    return TRUE;
}

static void destroy(GtkWidget *widget,
                    gpointer data)
{
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    GtkWidget *widget;
    GtkWidget *button;

    gtk_init(&argc, &argv);

    widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    button = gtk_button_new();
    gtk_button_set_label((GtkButton *) button, "Hello world");

    g_signal_connect(widget, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(widget, "destroy", G_CALLBACK(destroy), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(widget), 10);
    g_signal_connect(button, "clicked", G_CALLBACK(hello), NULL);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), widget);

    gtk_container_add(GTK_CONTAINER(widget), button);
    gtk_widget_show(button);
    gtk_widget_show(widget);

    gtk_main();

    return 0;
}