#include <gtk/gtk.h>
#include "bt-5.0.0/inc/btree.h"
#include "dcallback.h"

//Global Object
GObject *window, 
            *info_button, *delete_button, *update_button, *add_button, *search_entry,
            *info_bar, *notify_label, *yes_delete, *yes_add, *yes_update, *no_button,
            *word_label, *meaning_textview,
            // *popover, *pop_label,
            *status_label,
            //No Shape Objects
            *textbuffer,
            *comple, *liststore;


//Global DATA
BTA * data = NULL;

//Global variable

//testing callback function
static void hello_word (GtkWidget * widget, gpointer data)
{
    printf("hola\n");
} 

int main (int argc, char *argv[])
{
    //Chuyen Txt than Tu Dien BTA
    if (argc == 2)    
    {
        convert_text_to_bt(argv[1]);
        g_print("export to resource.dat\n");
    }

    //Khoi tao data
    data = btopn ("resource.dat", 0, 1);

    //Khai bao cac doi tuong >> Local->Global
    GtkBuilder *builder;
    /*
    GObject *window, 
            *info_button, *delete_button, *update_button, *search_entry,
            *info_bar, *notify_label, *yes_delete, *yes_add, *yes_update, *no_button,
            *word_label, *meaning_textview, 
            //No Shape Objects
            *textbuffer; */

    //Khoi tao giao dien nguoi dung
    gtk_init (&argc, &argv);

    // Khoi tao Soundex Func
    soundex_init();

    //Khoi tao GtkBuilder
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "glade/newW.ui", NULL);

    //Khoi tao Window >> Tin hieu "tat cua so"
    window = gtk_builder_get_object (builder, "window");
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    // g_signal_connect (window, "key-press-event", G_CALLBACK (window_key_press), NULL);

    /* TODO Khoi tao tin hieu cua cac Object khac o day*/

        
        

        //Khoi tao Word Label
        word_label = gtk_builder_get_object (builder, "word_label");
        // Khoi tao status label
        status_label = gtk_builder_get_object (builder, "status_label");
        //Khoi tao Meaning TextView && TextBuffer
        meaning_textview = gtk_builder_get_object (builder, "meaning_textview");
        textbuffer = gtk_builder_get_object (builder, "textbuffer");
        //Khoi tao Info Button
        info_button = gtk_builder_get_object (builder, "info_button");
        //Khoi tao Delete Button
        delete_button = gtk_builder_get_object (builder, "delete_button");
        //Khoi tao Update Button
        update_button = gtk_builder_get_object (builder, "update_button");
        //Khoi tao Add Button
        add_button = gtk_builder_get_object (builder, "add_button");
        //Khoi tao Search Entry
        search_entry = gtk_builder_get_object (builder, "search_entry");
         //Khoi tao Info Bar
        info_bar = gtk_builder_get_object (builder, "info_bar");
        //Khoi tao Notify Label
        notify_label = gtk_builder_get_object (builder, "notify_label");

        //Khoi tao Yes Button >> NO CALLBACK function 
        yes_delete = gtk_builder_get_object (builder, "yes_delete");
        yes_add = gtk_builder_get_object (builder, "yes_add");
        yes_update = gtk_builder_get_object (builder, "yes_update");
        no_button = gtk_builder_get_object (builder, "no_button");

        // Khoi tao completion
        comple = gtk_entry_completion_new();
        gtk_entry_completion_set_text_column(comple, 0);
        liststore = gtk_list_store_new(10, G_TYPE_STRING, G_TYPE_STRING,
                                        G_TYPE_STRING, G_TYPE_STRING,
                                        G_TYPE_STRING, G_TYPE_STRING,
                                        G_TYPE_STRING, G_TYPE_STRING,
                                        G_TYPE_STRING, G_TYPE_STRING);
        gtk_entry_completion_set_model(comple, GTK_TREE_MODEL(liststore));
        gtk_entry_set_completion(GTK_ENTRY(search_entry), comple);

        // Khoi tao popover >> pop label
        // popover = gtk_builder_get_object (builder, "popover");
        // pop_label = gtk_builder_get_object (builder, "pop_label");

        // Ket noi Cac tin hieu 
        g_signal_connect (info_button, "clicked", G_CALLBACK (show_about_dialog), NULL);

        g_signal_connect (delete_button, "clicked", G_CALLBACK (delete_button_clicked), NULL);   
        g_signal_connect (update_button, "clicked", G_CALLBACK (update_button_clicked), NULL);     
        g_signal_connect (add_button, "clicked", G_CALLBACK (add_button_clicked), NULL);

            
        g_signal_connect (search_entry, "key-press-event", G_CALLBACK (search_entry_key_press), NULL);
        g_signal_connect (search_entry, "activate", G_CALLBACK (search_entry_activate), NULL);
        g_signal_connect (search_entry, "grab-focus", G_CALLBACK (no_button_clicked), NULL);

           
        g_signal_connect (yes_delete, "clicked", G_CALLBACK (yes_delete_clicked), NULL); 
        g_signal_connect (yes_add, "clicked", G_CALLBACK (yes_add_clicked), NULL);
        g_signal_connect (yes_update, "clicked", G_CALLBACK (yes_update_clicked), NULL);

        //khoi tao No Button >> NO CALLBACK function 
        g_signal_connect (no_button, "clicked", G_CALLBACK (no_button_clicked), NULL);

        
        
        //g_signal_connect (comple, "cursor-on-match", G_CALLBACK (cursor_match), NULL);
        
    //Hien thi Window va "Child"
    gtk_widget_show (window); 

    //Ngung tham chieu Builder (free)
    g_object_unref (builder);

    //Chay vong lap main (the main loop) cho den khi gtk_main_quit() duoc goi
    gtk_main ();

    //Ngung tham chieu DATA >> Tat chuong trinh
    btcls (data);

    return 0;
}