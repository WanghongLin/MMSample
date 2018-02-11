//
// Created by mutter on 6/11/16.
//

#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    static GtkWidget *window = NULL;
    GtkWidget *hbox;
    GtkWidget *vbox;
    GtkWidget *frame;
    GtkWidget *label;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Label");
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    hbox = gtk_hbox_new(FALSE, 5);
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);
    gtk_box_pack_start_defaults(GTK_BOX(hbox), vbox);

    frame = gtk_frame_new("Normal label");
    label = gtk_label_new("This is a normal label");
    gtk_container_add(GTK_CONTAINER(frame), label);
    gtk_box_pack_start_defaults(GTK_BOX(vbox), frame);

    frame = gtk_frame_new("Multi-line label");
    label = gtk_label_new("This is a Multi-line label.\nSecond line\nThird line");
    gtk_container_add(GTK_CONTAINER(frame), label);
    gtk_box_pack_start_defaults(GTK_BOX(vbox), frame);

    frame = gtk_frame_new("Left Justified Label");
    label = gtk_label_new("This is a Left-Justified Label\nMultiline label\nThird line");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_label_set_pattern(GTK_LABEL(label), "_");
    gtk_container_add(GTK_CONTAINER(frame), label);
    gtk_box_pack_start_defaults(GTK_BOX(vbox), frame);

    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}