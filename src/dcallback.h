#ifndef D_CALLBACK_
#define D_CALLBACK_

#include <string.h>
#include <gtk/gtk.h>

#include "soundex.h"
#include "bt-5.0.0/inc/btree.h"

#define WORD_LENGHT 255
#define MEANING_LENGHT 10000

extern  GObject *window, 
                *info_button, *delete_button, *update_button, *add_button, *search_entry,
                *info_bar, *notify_label, *yes_delete, *yes_add, *yes_update, *no_button,
                *word_label, *meaning_textview,
                //*popover, *pop_label,
                *status_label,
                //No Shape Objects
                *textbuffer,
                *comple, *liststore;


//Global DATA
extern BTA * data;

int convert_text_to_bt(char *);

void search_entry_activate (GtkEntry *, gpointer);

void delete_button_clicked (GtkButton *, gpointer);

void update_button_clicked (GtkButton *, gpointer);

void add_button_clicked (GtkButton *, gpointer);

void no_button_clicked (GtkButton *, gpointer);

void yes_delete_clicked (GtkButton *, gpointer);

void yes_add_clicked (GtkButton *, gpointer);

void yes_update_clicked (GtkButton *, gpointer);

gboolean search_entry_key_press (GtkEntry * entry, GdkEvent * event, gpointer NONE);

void show_about_dialog(GtkButton * button, gpointer none);

// gboolean cursor_match (GtkEntryCompletion *,GtkTreeModel *,GtkTreeIter *,gpointer );


#endif