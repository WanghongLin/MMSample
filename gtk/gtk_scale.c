//
// Created by mutter on 6/11/16.
//

#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *scale;
    GtkObject *adjust;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Scale demo");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);

    adjust = gtk_adjustment_new(10, 0, 50, 1, 0, 0);

    scale = gtk_hscale_new(GTK_ADJUSTMENT(adjust));

    gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
    gtk_scale_set_digits(GTK_SCALE(scale), 8);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_BOTTOM);

    gtk_container_add(GTK_CONTAINER(window), scale);

    gtk_widget_show(scale);
    gtk_widget_show(window);

    gtk_main();
    return GDK_OK;
}
