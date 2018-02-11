//
// Created by mutter on 6/9/16.
//

#include "gtk/gtk.h"

static void radio_callback(GtkWidget *widget, gpointer data)
{
    gboolean is_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    if (is_active) {
        g_print("%s checked\n", widget->name);
    } else {
        g_print("%s unchecked\n", widget->name);
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *radio_layout;
    GtkWidget *radio_button_1;
    GtkWidget *radio_button_2;
    GtkWidget *radio_button_3;
    GtkWidget *radio_button_4;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    gtk_window_set_title(GTK_WINDOW(window), "Radio Buttons");
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    radio_layout = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), radio_layout);

    radio_button_1 = gtk_radio_button_new_with_label(NULL, "radio button 1");
    radio_button_2 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button_1)), "radio button 2");
    radio_button_3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button_2), "radio button 3");
    radio_button_4 = gtk_radio_button_new_with_label(gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button_3)), "radio button 4");

    radio_button_1->name = "button_1";
    radio_button_2->name = "button_2";
    radio_button_3->name = "button_3";
    radio_button_4->name = "button_4";

    gtk_box_pack_start_defaults(GTK_BOX(radio_layout), radio_button_1);
    gtk_box_pack_start_defaults(GTK_BOX(radio_layout), radio_button_2);
    gtk_box_pack_start_defaults(GTK_BOX(radio_layout), radio_button_3);
    gtk_box_pack_start_defaults(GTK_BOX(radio_layout), radio_button_4);

    g_signal_connect(radio_button_1, "toggled", G_CALLBACK(radio_callback), NULL);
    g_signal_connect(radio_button_2, "toggled", G_CALLBACK(radio_callback), NULL);
    g_signal_connect(radio_button_3, "toggled", G_CALLBACK(radio_callback), NULL);
    g_signal_connect(radio_button_4, "toggled", G_CALLBACK(radio_callback), NULL);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_button_3), TRUE);

    gtk_widget_show(radio_button_1);
    gtk_widget_show(radio_button_2);
    gtk_widget_show(radio_button_3);
    gtk_widget_show(radio_button_4);
    gtk_widget_show(radio_layout);
    gtk_widget_show(window);

    gtk_main();
    return GDK_OK;
}