//
// Created by mutter on 6/11/16.
//

#include <gtk/gtk.h>

static GtkWidget *create_arrow_button(GtkArrowType arrowType, GtkShadowType shadowType)
{
    GtkWidget *button;
    GtkWidget *arrow;
    button = gtk_button_new();
    arrow = gtk_arrow_new(arrowType, shadowType);

    gtk_container_add(GTK_CONTAINER(button), arrow);
    gtk_widget_show(button);
    gtk_widget_show(arrow);

    return button;
}

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Arrow buttons");
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_hbox_new(FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER(box), 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    button = create_arrow_button(GTK_ARROW_UP, GTK_SHADOW_IN);
    gtk_box_pack_start_defaults(GTK_BOX(box), button);

    button = create_arrow_button(GTK_ARROW_DOWN, GTK_SHADOW_OUT);
    gtk_box_pack_start_defaults(GTK_BOX(box), button);

    button = create_arrow_button(GTK_ARROW_LEFT, GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start_defaults(GTK_BOX(box), button);

    button = create_arrow_button(GTK_ARROW_RIGHT, GTK_SHADOW_ETCHED_OUT);
    gtk_box_pack_start_defaults(GTK_BOX(box), button);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
