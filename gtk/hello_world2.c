//
// Created by mutter on 6/5/16.
//

#include <gtk/gtk.h>

static void callback(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was pressed\n", (gchar *) data);
}

static gboolean delete_event(GtkWidget *widget, GdkEvent* event, gpointer data)
{
    gtk_main_quit();
    return FALSE;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkButton *button;
    GtkHBox *gtkHBox;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title((GtkWindow *) window, "Hello buttons!");

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    gtkHBox = (GtkHBox *) gtk_hbox_new(FALSE, 0);
    gtk_container_add((GtkContainer *) window, (GtkWidget *) gtkHBox);
    button = (GtkButton *) gtk_button_new_with_label("Button 1");

    g_signal_connect(button, "clicked", G_CALLBACK(callback), (gpointer) "button 1");

    gtk_box_pack_start(gtkHBox, button, TRUE, TRUE, 0);

    gtk_widget_show(button);

    button = (GtkButton *) gtk_button_new_with_label("Button 2");
    g_signal_connect(button, "clicked", G_CALLBACK(callback), (gpointer) "button 2");
    gtk_box_pack_start(gtkHBox, button, TRUE, TRUE, 0);

    gtk_widget_show(button);
    gtk_widget_show(gtkHBox);
    gtk_widget_show(window);

    gtk_main();

    return GDK_OK;
}