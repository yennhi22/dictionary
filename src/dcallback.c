#include <string.h>
#include <gtk/gtk.h>

#include "soundex.h"
#include "bt-5.0.0/inc/btree.h"
#include "dcallback.h"

int isBlank(char * text)
{
    int i=0, l=strlen(text);
    while (i<l)
    {
        if (!((text[i]==' ') || (text[i]=='\n'))) 
        {  
            return 0;
        }
        else i++;
    }
    return 1;
}

void find_next_word (char * word)
{
    int rsize, value;
    if (!bfndky(data, word, &rsize)) 
    {
        bnxtky(data, word, &value);
        gtk_entry_set_text(search_entry, word);
        search_entry_activate(search_entry, NULL);
    }
    else 
    {
        char nextword[WORD_LENGHT];
        strcpy(nextword, word);
        //printf("%s\n", word);
        btins(data, nextword, "", 1);
        bnxtky(data, nextword, &value);
        gtk_entry_set_text(search_entry, nextword);
        search_entry_activate(search_entry, NULL);
        btdel(data, word);
        //printf("%s\n", word);
    }
}

void separate_mean(char* mean) 
{
    int i = 0, j = 1;
    while (mean[j] != '\0') 
    {
        if (mean[j] == '\\' && mean[j + 1] == 'n') 
        {
            mean[i++] = '\n';
            j += 2;
        }
        else {
            if (i != j)
                mean[i++] = mean[j++];
            else 
            {
                i++; j++;
            }
        }
    }
    mean[i] = '\0';
}

int convert_text_to_bt(char * filename)
{
    FILE * datafile;
    if ((datafile = fopen(filename, "r")) == NULL) 
    {
        printf("Loi mo file\n");
        return 0;;
    };

    BTA *convertfile;
    convertfile = btcrt("resource.dat", 0, 0);

    char word[WORD_LENGHT], mean[MEANING_LENGHT];
    int line;

    while (fscanf(datafile, "%[^\t]", word) == 1) 
    {
        fgets(mean, MEANING_LENGHT, datafile);
        line++;
        separate_mean(mean);
        btins(convertfile, word, mean, strlen(mean) + 1);    
    }

    printf("%d lines.\n",line);
    btcls(convertfile);
    fclose(datafile);
    return 1;
}

void show_matching_soundex_word (char * word)
{
    char * soundex_code = soundex (word);
    char nxt_word[WORD_LENGHT];
    char prv_word[WORD_LENGHT];
    char soundex_list[1024] = {0};
    // printf("%s\n", soundex_code);
    int value, i, n=5;

    for (i=0; i<n && n<100; i++)
    {
        if (!bprvky (data, prv_word, &value))
            if (!strcmp (soundex_code, soundex(prv_word)))
            {   
                strcat (soundex_list, prv_word);
                strcat (soundex_list, "\n");
            }
        else n++; 
    }

    n=5;
    for (i=0; i<n && n<100; i++)
    {
        if (!bnxtky (data, nxt_word, &value))
            if (!strcmp (soundex_code, soundex(nxt_word)))
            {   
                strcat (soundex_list, nxt_word);
                strcat (soundex_list, "\n");
            }
        else n++; 
    }
    // printf("%s\n", soundex_list);
    if (soundex_list[0] != 0)  
    {
        //Cap Nhat Buffer
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(textbuffer), soundex_list, -1);
        //Set Buffer moi vao Text View
        gtk_text_view_set_buffer(GTK_TEXT_VIEW (meaning_textview), GTK_TEXT_BUFFER( textbuffer));

        gtk_label_set_label (GTK_LABEL (status_label), "Không tìm thấy. Ý bạn là...");
    }
}

void search_entry_activate (GtkEntry * entry, gpointer NONE)
{
    gtk_widget_set_visible (GTK_LABEL (status_label), FALSE);

    char word[WORD_LENGHT];
    char meaning[MEANING_LENGHT];
    strcpy (word, gtk_entry_get_text (search_entry));

    int rsize = 0;
    btsel (data, word, meaning, MEANING_LENGHT, &rsize);

    //GtkTextBuffer *buffer;
    //buffer = gtk_text_buffer_new (NULL);

    if (!rsize) 
    {
        //"Khong tim thay"
        gtk_label_set_label(GTK_LABEL(word_label), "");
        //Cap Nhat Buffer
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(textbuffer), "", -1);
        //Set Buffer moi vao Text View
        gtk_text_view_set_buffer(GTK_TEXT_VIEW (meaning_textview), GTK_TEXT_BUFFER( textbuffer));
        
        //Notify Popup 
        // gtk_label_set_label (GTK_LABEL(pop_label), "Không tìm thấy");
        // gtk_popover_set_relative_to (GTK_POPOVER(popover), GTK_WIDGET(search_entry)) ;
        // gtk_popover_popup (GTK_POPOVER(popover));

        gtk_label_set_label (GTK_LABEL (status_label), "Không tìm thấy");
        gtk_widget_set_visible (GTK_LABEL (status_label), TRUE);

        //An Update Button  "Sửa"
        gtk_widget_set_sensitive (GTK_WIDGET(update_button), FALSE);
        gtk_widget_set_sensitive (GTK_WIDGET(add_button), TRUE);


        // TODO: Them Soundex Func "neu muon"
        show_matching_soundex_word (word);
            

    }
    else 
    {
        //Tim thay Word
        gtk_label_set_label(GTK_LABEL(word_label), word);
        //New Meaning >> Buffer
        gtk_text_buffer_set_text(GTK_TEXT_BUFFER(textbuffer), meaning, -1);
        //
        gtk_text_view_set_buffer(GTK_TEXT_VIEW (meaning_textview) ,GTK_TEXT_BUFFER(textbuffer));

        //Update Button: "Them"
        gtk_widget_set_sensitive (GTK_WIDGET(update_button), TRUE);
        gtk_widget_set_sensitive (GTK_WIDGET(add_button), FALSE);

        
    }
}

void delete_button_clicked (GtkButton * button, gpointer NONE)
{
    gtk_label_set_label (GTK_LABEL(notify_label), "Do you want to Delete this word?");
    gtk_widget_set_visible (GTK_WIDGET(info_bar), TRUE);
    gtk_widget_set_visible (GTK_WIDGET(yes_delete), TRUE);
    gtk_widget_set_visible (GTK_WIDGET(yes_add), FALSE);
    gtk_widget_set_visible (GTK_WIDGET(yes_update), FALSE);
}

void add_button_clicked (GtkButton * button, gpointer NONE)
{
    char * word = gtk_entry_get_text (GTK_ENTRY(search_entry));

    gtk_label_set_label (GTK_LABEL(word_label), word);

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(textbuffer), "", -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW (meaning_textview), GTK_TEXT_BUFFER( textbuffer));
    gtk_widget_set_visible (GTK_LABEL (status_label), FALSE);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (meaning_textview), TRUE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (meaning_textview), TRUE);

    gtk_label_set_label (GTK_LABEL(notify_label), "Do you want to Add this word?");
    gtk_widget_set_visible (GTK_WIDGET(info_bar), TRUE);
    gtk_widget_set_visible (GTK_WIDGET(yes_delete), FALSE);
    gtk_widget_set_visible (GTK_WIDGET(yes_add), TRUE);
    gtk_widget_set_visible (GTK_WIDGET(yes_update), FALSE);
}

void update_button_clicked (GtkButton * button, gpointer NONE)
{
    // char * word = gtk_entry_get_text (GTK_ENTRY(search_entry));

    // gtk_label_set_label (GTK_LABEL(word_label), word);

    gtk_text_view_set_editable (GTK_TEXT_VIEW (meaning_textview), TRUE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW     (meaning_textview), TRUE);
    
    gtk_label_set_label (GTK_LABEL(notify_label), "Do you want to Update this word?");
    gtk_widget_set_visible (GTK_WIDGET(info_bar),TRUE );
    gtk_widget_set_visible (GTK_WIDGET(yes_delete), FALSE);
    gtk_widget_set_visible (GTK_WIDGET(yes_add), FALSE);
    gtk_widget_set_visible (GTK_WIDGET(yes_update), TRUE);
}

void no_button_clicked (GtkButton * button, gpointer NONE)
{
    gtk_text_view_set_editable (GTK_TEXT_VIEW (meaning_textview), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (meaning_textview), FALSE);

    gtk_widget_set_visible (GTK_WIDGET(info_bar), FALSE);
    gtk_widget_set_visible (GTK_WIDGET(status_label), FALSE);
}

void yes_delete_clicked (GtkButton * button, gpointer NONE)
{
    char * word = gtk_label_get_text (GTK_LABEL(word_label));
    int fail = btdel (data, word);
    
    static char notify[255];
    if (!fail) strcpy (notify, "Đã xóa \"");
    else strcpy (notify, "Không thể xóa  \"");
    strcat (notify, word);
    strcat (notify, "\"  khỏi từ điển!");

    // gtk_label_set_label (GTK_LABEL(pop_label), notify);
    // gtk_popover_set_relative_to (GTK_POPOVER(popover), GTK_WIDGET(delete_button)) ;
    // gtk_popover_popup (GTK_POPOVER(popover));

    gtk_label_set_label (GTK_LABEL (status_label), notify);
    gtk_widget_set_visible (GTK_LABEL (status_label), TRUE);

    gtk_widget_set_visible (GTK_WIDGET(info_bar), FALSE);
}

void yes_add_clicked (GtkButton * button, gpointer NONE)
{
    char * word = gtk_label_get_text (GTK_LABEL(word_label));

    GtkTextIter startIter;
	GtkTextIter endIter;

    gtk_text_buffer_get_start_iter (textbuffer, &startIter);
    gtk_text_buffer_get_end_iter (textbuffer, &endIter);
    
    char * meaning = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(textbuffer), &startIter, &endIter, FALSE);
    if (isBlank(meaning)) 
    {
        // gtk_label_set_label (GTK_LABEL(pop_label), "Không được để trống");
        // gtk_popover_set_relative_to (GTK_POPOVER(popover), GTK_WIDGET(add_button)) ;
        // gtk_popover_popup (GTK_POPOVER(popover));

        gtk_label_set_label (GTK_LABEL (status_label), "Không được để trống");
        gtk_widget_set_visible (GTK_LABEL (status_label), TRUE);

        return;
    }

    int fail = btins (data, word, meaning, strlen(meaning)+1);

    static char notify[255];
    if (!fail) strcpy (notify, "Đã thêm \"");
    else strcpy (notify, "Không thể thêm  \"");
    strcat (notify, word);
    strcat (notify, "\"  vào từ điển!");

    // gtk_label_set_label (GTK_LABEL(pop_label), notify);
    // gtk_popover_set_relative_to (GTK_POPOVER(popover), GTK_WIDGET(add_button)) ;
    // gtk_popover_popup (GTK_POPOVER(popover));

    gtk_label_set_label (GTK_LABEL (status_label), notify);
    gtk_widget_set_visible (GTK_LABEL (status_label), TRUE);
    

    gtk_text_view_set_editable (GTK_TEXT_VIEW (meaning_textview), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (meaning_textview), FALSE);

    gtk_widget_set_visible (GTK_WIDGET(info_bar), FALSE);
}

void yes_update_clicked (GtkButton * button, gpointer NONE)
{
    char * word = gtk_label_get_text (GTK_LABEL(word_label));

    GtkTextIter startIter;
	GtkTextIter endIter;

    gtk_text_buffer_get_start_iter (textbuffer, &startIter);
    gtk_text_buffer_get_end_iter (textbuffer, &endIter);

    char * meaning = gtk_text_buffer_get_text (GTK_TEXT_BUFFER(textbuffer), &startIter, &endIter, FALSE);
    if (isBlank(meaning)) 
    {
        // gtk_label_set_label (GTK_LABEL(pop_label), "Không được để trống");
        // gtk_popover_set_relative_to (GTK_POPOVER(popover), GTK_WIDGET(add_button)) ;
        // gtk_popover_popup (GTK_POPOVER(popover));

        gtk_label_set_label (GTK_LABEL (status_label), "Không được để trống");
        gtk_widget_set_visible (GTK_LABEL (status_label), TRUE);

        return;
    }

    int fail = btupd (data, word, meaning, strlen(meaning)+1);

    static char notify[255];
    if (!fail) strcpy (notify, "Đã cập nhật \"");
    else strcpy (notify, "Không thể cập nhật  \"");
    strcat (notify, word);
    strcat (notify, "\"  vào từ điển!");

    // gtk_label_set_label (GTK_LABEL(pop_label), notify);
    // gtk_popover_set_relative_to (GTK_POPOVER(popover), GTK_WIDGET(update_button)) ;
    // gtk_popover_popup (GTK_POPOVER(popover));

    gtk_label_set_label (GTK_LABEL (status_label), notify);
    gtk_widget_set_visible (GTK_LABEL (status_label), TRUE);

    gtk_text_view_set_editable (GTK_TEXT_VIEW (meaning_textview), FALSE);
    gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (meaning_textview), FALSE);

    gtk_widget_set_visible (GTK_WIDGET(info_bar), FALSE);
}

int prefix(const char * big, const char * small) 
{
	int small_len = strlen(small);
	int big_len = strlen(big);
	int i;
	if (big_len < small_len)
		return 0;
	for (i = 0; i < small_len; i++)
		if (big[i] != small[i])
			return 0;
	return 1;
}

void show_completion (char * word)
{
    GtkTreeIter Iter;
    
    gtk_list_store_clear(liststore);
    
    
    //else printf("dech");
    char nextword[WORD_LENGHT];
    strcpy(nextword, word);

    int rsize;
    int existed = !bfndky(data, word, &rsize);
    if (existed)
    {
        gtk_list_store_append(liststore, &Iter);
		gtk_list_store_set(liststore, &Iter, 0, nextword, -1 );
    }
    else btins(data, word, "#", 1);

    int i, n=10;
    for (i = 0; (i <= n) && (n < 100); i++) 
    {
        int value;
		if (!bnxtky (data, nextword, &value))
            if (prefix(nextword, word))
            {
                gtk_list_store_append(liststore, &Iter);
                gtk_list_store_set(liststore, &Iter, 0, nextword, -1 );
            }
            else n++;
	}
    if (!existed) btdel(data, word);
}

gboolean search_entry_key_press (GtkEntry * entry, GdkEvent * event, gpointer NONE)
{
    GdkEventKey *keyEvent = (GdkEventKey *)event;
    char word[WORD_LENGHT];

    strcpy(word, gtk_entry_get_text(GTK_ENTRY(entry)));
   
    if (keyEvent->keyval == GDK_KEY_Tab) 
    {
        // gtk_widget_set_state_flags (search_entry, GTK_STATE_FLAG_NORMAL, FALSE);
        
        gtk_widget_grab_focus (update_button);
        
        //gtk_entry_grab_focus_without_selecting (search_entry);
        // gtk_widget_activate (search_entry);
        find_next_word (word);
    }
    else 
    {
        //printf("%d\n", keyEvent->keyval);
            
        if (keyEvent->keyval != GDK_KEY_BackSpace)
        {
            // Phải khởi tạo biến cho strlen() ở ngoài vì sau khi gán phần tử cuốí cùng của word[] là key_valua thì không có '\0' kí tử rác ở sau làm strlen() không ổn định
            int l = strlen(word);
            if ((keyEvent->keyval != 65364) && (keyEvent->keyval != 65362)) // Up & Down Arrow
                word[l] = keyEvent->keyval;
            word[l+1] = '\0';
        } 
        else word[strlen(word)-1] = '\0';
        
        show_completion(word);
    }

    //Return FALSE to allow signal to another widget
    return FALSE;
}

void show_about_dialog(GtkButton * button, gpointer none)
{
    GdkPixbuf *pixbuf=gdk_pixbuf_new_from_file_at_size("images/about.png",150,150,NULL);
    GtkWidget *dialog=gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),"DICTIONARY");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),"v1.0");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"C Avanced Team: \n Nguyễn Đức Anh 20165735\nNguyễn Thế Vinh 20167446\nLê Thị Hải Yến 20164753\n");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),"A project by C Avanced Team");
    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG(dialog),"https://github.com/ruanshiron");
    gtk_about_dialog_set_website_label (GTK_ABOUT_DIALOG(dialog),"Github");
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),pixbuf);
    gtk_window_set_transient_for (dialog, window);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

