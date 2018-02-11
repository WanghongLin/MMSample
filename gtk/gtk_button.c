//
// Created by mutter on 6/7/16.
//

#include <gtk/gtk.h>

static GtkWidget *xpm_label_box(gchar *xmp_filename, gchar *label_text)
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;

    box = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box), 2);

    image = gtk_image_new_from_file(xmp_filename);
    label = gtk_label_new(label_text);

    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 3);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 3);

    gtk_widget_show(image);
    gtk_widget_show(label);

    return box;
}

static void callback(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was clicked\n", (gchar *) data);
}

static void callback_pressed(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was pressed\n", (gchar *) data);
}

static void callback_released(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was released\n", (gchar *) data);
}

static void callback_enter(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was enter\n", (gchar *) data);
}

static void callback_leave(GtkWidget *widget, gpointer data)
{
    g_print("Hello again - %s was leave\n", (gchar *) data);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Pixmap's Button");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    button = gtk_button_new();
    g_signal_connect(button, "clicked", G_CALLBACK(callback), (gpointer) "cool button");
    g_signal_connect(button, "pressed", G_CALLBACK(callback_pressed), (gpointer) "cool button");
    g_signal_connect(button, "released", G_CALLBACK(callback_released), (gpointer) "cool button");
    g_signal_connect(button, "enter", G_CALLBACK(callback_enter), (gpointer) "cool button");
    g_signal_connect(button, "leave", G_CALLBACK(callback_leave), (gpointer) "cool button");

    box = xpm_label_box("info.xpm", "cool button");

    gtk_container_add(GTK_CONTAINER(button), box);
    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_show(box);
    gtk_widget_show(button);
    gtk_widget_show(window);

    gtk_main();
    return GDK_OK;
}
