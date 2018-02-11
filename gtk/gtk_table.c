//
// Created by mutter on 6/6/16.
//

#include <gtk/gtk.h>

static void callback(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was pressed\n", (gchar *)data);
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *table;
    GtkWidget *button;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Table");

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    table = gtk_table_new(2, 2, TRUE);
    gtk_container_add(GTK_CONTAINER(window), table);


    button = gtk_button_new_with_label("button 1");
    g_signal_connect(button, "clicked", G_CALLBACK(callback), "button 1");
    gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 1, 0, 1);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("button 2");
    g_signal_connect(button, "clicked", G_CALLBACK(callback), "button 2");
    gtk_table_attach_defaults(GTK_TABLE(table), button, 1, 2, 0, 1);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("Quit");
    g_signal_connect(button, "clicked", G_CALLBACK(delete_event), NULL);
    gtk_table_attach_defaults(GTK_TABLE(table), button, 0, 2, 1, 2);
    gtk_widget_show(button);

    gtk_widget_show(table);
    gtk_widget_show(window);
    gtk_main();

    return GDK_OK;
}
