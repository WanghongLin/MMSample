//
// Created by mutter on 6/11/16.
//

#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *button;
    GtkTooltips *tooltips;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    button = gtk_button_new_with_label("button 1");

    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_window_set_default_size(GTK_WINDOW(window), 100, 100);
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);

    tooltips = gtk_tooltips_new();
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tooltips), button, "This is button 1", NULL);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
