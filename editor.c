/*
    This file is part of Rite Editor.

    Rite Editor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Rite Editor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rite Editor.  If not, see <http://www.gnu.org/licenses/>.

*/



/*
*-------------------------------------------------------
*	Rite Editor.
*	Made By (c)Rohith Srivathsav
*	MIS : 111508059
*	Branch : SY IT
*	Batch : S3
*	College of Engineering, Pune
*	(All Rights Reserved)	
*-------------------------------------------------------
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <errno.h>
#include <sys/dir.h>
#include <gdk/gdkkeysyms.h>
#include <time.h>
#include <ctype.h>


//Widget for Search Bar
typedef struct _SearchBar {
	GtkWidget *search_entry;
	GtkWidget *sbutton;
	GtkWidget *nbutton;
	GtkWidget *qbutton;
	GtkWidget *text_view;
}SearchBar;

//Widget for open-dialog
typedef struct _OpenDialog{
  GtkWidget *window;
  GtkTextBuffer *buffer;
  gchar *filename;
} OpenDialog;

//Widget for new dialog
typedef struct _NewDialog{
  GtkWidget *window;
  GtkWidget *text_view;
  GtkTextBuffer *buffer;
  GtkWidget *statusbar;
} NewDialog;

//Widget for bug report/feedback
typedef struct _Bug{
  GtkWidget *window;
  GtkWidget *text_view;
  GtkWidget *combo;
  GtkTextBuffer *buffer;
  GtkWidget *entry1;
} Bugr;

//Icon Creation
GdkPixbuf *create_pixbuf(const gchar * filename) {
    
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   
   if (!pixbuf) {
       
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}

//Doubly Linked List Implementation
typedef struct Node  {
	char* data;
	struct Node* next;
	struct Node* prev;
}Node;

struct Node* head = NULL, *tail = NULL, *curr = NULL; 

//Creates a new Node and returns pointer to it. 
struct Node* GetNewNode(char *x) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = (char *)malloc(strlen(x) + 1);
	strcpy(newNode->data, x);
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(char *x) {
	struct Node* temp = head;
	struct Node* newNode = GetNewNode(x);
	if(head == NULL) {
		head = newNode;
		tail = head;
		curr = tail;
		return;
	}
	
	tail->next = newNode;
	newNode->prev = tail;
	tail = newNode;
	
}

//Prints all the elements in linked list in forward traversal order
void Print() {
	struct Node* temp = head;
	
	while(temp != NULL) {
		printf("%s ",temp->data);
		temp = temp->next;
	}
	printf("\n");
}

void Delete() {
	struct Node* temp;
	
	while(head != NULL) {
		temp = head;
		head = head->next;
		
		free(temp->data);
		free(temp);
		
	}
	
}

void undo_selected(GtkWidget *widget, gpointer data) {
	
	gchar *contents;
        GtkTextIter start, end;
	gtk_text_buffer_get_bounds(data, &start, &end);
	contents = gtk_text_buffer_get_text(data, &start, &end, FALSE);
	
	if(strcmp(contents, "\0") == 0) {
		return;
	}
	
	if(curr == NULL) 	
		return;
	
	
	if(strcmp(curr->data, tail->data) == 0) {
		const gchar* text = (curr->prev)->data;
		gint len = strlen(text);
		gtk_text_buffer_set_text (data, text, len);	
		curr = curr->prev;
		
		return;
	}
		
	else {
		curr = tail;
		const gchar* text = (curr->prev)->data;
		gint len = strlen(text);
		gtk_text_buffer_set_text (data, text, len);	
		curr = curr->prev;
		
		return;	
	}
	
}


//Function invoked when save menu item is clicked
void save_dialog_selected(GtkWidget *widget, OpenDialog *sdlog){
  GtkWidget *dialog;
  gint response;
  GtkTextBuffer *buffer;

  if (strcmp( gtk_window_get_title(GTK_WINDOW(sdlog->window)) , "Untitled") == 0)
  {

    dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(sdlog->window),
     GTK_FILE_CHOOSER_ACTION_SAVE,
     GTK_STOCK_SAVE, GTK_RESPONSE_APPLY,
     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     NULL);

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_APPLY)
    {
     gchar *filename;
     gchar *contents;
     GtkTextIter start, end;
     filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
     buffer = sdlog->buffer;
     gtk_text_buffer_get_bounds(buffer, &start, &end);
     contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
     g_file_set_contents(filename, contents, -1, NULL);
     gtk_window_set_title(GTK_WINDOW(sdlog->window), filename);
     sdlog->filename = filename;
   }
   else if (response == GTK_RESPONSE_CANCEL)
   {
     gtk_widget_destroy(dialog);
     return;
   }
   gtk_widget_destroy(dialog);
 }
 else
 {
   GtkTextIter start, end;
   const gchar *filename = gtk_window_get_title(GTK_WINDOW(sdlog->window)); 
   gchar *contents;
   gtk_text_buffer_get_bounds(sdlog->buffer, &start, &end);
   contents = gtk_text_buffer_get_text(sdlog->buffer, &start, &end, FALSE);
   g_file_set_contents(filename, contents, -1, NULL);
 }
}

//Function invoked when new menu item is clicked
void new_dialog_selected(GtkWidget *widget, NewDialog *ndlog){
 gtk_widget_show(ndlog->text_view);
 gtk_widget_show(ndlog->statusbar);
 gtk_window_set_title(GTK_WINDOW(ndlog->window), "Untitled");
 gtk_text_buffer_set_text(ndlog->buffer, "", -1);
}

//Function invoked when open menu item is clicked
void open_dialog_selected(GtkWidget *widget, OpenDialog *odlog){

  GtkWidget *dialog;

  dialog = gtk_file_chooser_dialog_new ("Open File",
    GTK_WINDOW(odlog->window),
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
  {
   gchar *filename;
   gchar *contents;
   filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
   g_file_get_contents(filename, &contents, NULL, NULL);
   gtk_text_buffer_set_text(odlog->buffer, contents, -1);
   gtk_window_set_title(GTK_WINDOW(odlog->window), filename);
   odlog->filename=filename;
   g_free (filename);
 }
 gtk_widget_destroy(dialog);
}

//Functions related to search-bar
void find (GtkTextView *text_view, const gchar *text, GtkTextIter *iter)
{
  GtkTextIter mstart, mend;
  gboolean found;
  GtkTextBuffer *buffer;
  GtkTextMark *last_pos;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
  found = gtk_text_iter_forward_search (iter, text, 0, &mstart, &mend, NULL);

  if (found)
  {
    gtk_text_buffer_select_range (buffer, &mstart, &mend);
    last_pos = gtk_text_buffer_create_mark (buffer, "last_pos", 
      &mend, FALSE);
    gtk_text_view_scroll_mark_onscreen (text_view, last_pos);
  }
}

void close_button_clicked (GtkWidget *close_button, SearchBar *sbar)
{
  gtk_widget_hide(sbar->search_entry);
  gtk_widget_hide(sbar->sbutton);
  gtk_widget_hide(sbar->nbutton);
  gtk_widget_hide(sbar->qbutton);
}

void find_menu_selected(GtkWidget *widget, SearchBar *sbar)
{
  gtk_widget_show(sbar->search_entry);
  gtk_widget_show(sbar->sbutton);
  gtk_widget_show(sbar->nbutton);
  gtk_widget_show(sbar->qbutton);
}


void search_button_clicked (GtkWidget *search_button, SearchBar *sbar)
{
  const gchar *text;
  GtkTextBuffer *buffer;
  GtkTextIter iter;

  text = gtk_entry_get_text (GTK_ENTRY (sbar->search_entry));

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sbar->text_view));
  gtk_text_buffer_get_start_iter (buffer, &iter);
  
  find (GTK_TEXT_VIEW (sbar->text_view), text, &iter);
}

void next_button_clicked (GtkWidget *next_button, SearchBar *sbar)
{
  const gchar *text;
  GtkTextBuffer *buffer;
  GtkTextMark *last_pos;
  GtkTextIter iter;

  text = gtk_entry_get_text (GTK_ENTRY (sbar->search_entry));
  
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sbar->text_view));

  last_pos = gtk_text_buffer_get_mark (buffer, "last_pos");
  if (last_pos == NULL)
    return;

  gtk_text_buffer_get_iter_at_mark (buffer, &iter, last_pos);
  find (GTK_TEXT_VIEW (sbar->text_view), text, &iter);
}

//Function invoked when font menu item selected
void select_font(GtkWidget *widget, GtkWidget *widget1) {
  GtkResponseType result;
  GtkWidget *dialog = gtk_font_selection_dialog_new("Select Font");
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  if (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_APPLY) {

   PangoFontDescription *font_desc;
   gchar *fontname = gtk_font_selection_dialog_get_font_name(
    GTK_FONT_SELECTION_DIALOG(dialog));

   font_desc = pango_font_description_from_string(fontname);

   gtk_widget_modify_font(GTK_WIDGET(widget1), font_desc);

   g_free(fontname);
 }

 gtk_widget_destroy(dialog);
}


char* randomDigits() {
	int r, count = 0;
	 char *a= (char*)malloc (sizeof (char) * 5);
	while(count < 4) {
		r = rand() % 10;
		a[count++] = (char)('0' + r);
	
	}
	a[count] = '\0';
	return a;

}
char filename[128];
void submit_clicked(GtkWidget *widget, gpointer data) {
		 
	const gchar *username;
	gchar *contents;
	GtkTextIter start, end;
	  	
	GtkWidget *textEntry = ((Bugr *)data)->entry1;
	username = gtk_entry_get_text (GTK_ENTRY (textEntry));
	GtkWidget *comboBox = ((Bugr *)data)->combo;
	GtkWidget *window = ((Bugr *)data)->window;
	GtkWidget *text_view = ((Bugr *)data)->text_view;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	 
	if(username[0] =='\0') {
		 GtkWidget *dialog0 = gtk_about_dialog_new();
  		   gtk_widget_set_size_request (dialog0, 500, 200);
		  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog0), "Error!\nUsername cannot be left empty.");
		 
		   gtk_dialog_run(GTK_DIALOG (dialog0));
		  gtk_widget_destroy(dialog0);
		  return;
	}
	 
	gchar *text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(comboBox));
	
	filename[0] = '\0';
	if(strcmp(text, "Bug Report") == 0) {
		  
		  strcpy(filename, "Bug_Reports/bug_report_");
		  char* rd = randomDigits();
		  strcat(filename, rd);
		  strcat(filename, ".txt");
		  free(rd); 
		  FILE *fp1;
		  fp1 = fopen(filename, "w");
		  fprintf(fp1, "Username : %s\n-------------------------\n", username);
		 
		  gtk_text_buffer_get_bounds(buffer, &start, &end);
		  contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		
		  
		  fprintf(fp1, "%s\n", contents);
		  
		  fclose(fp1);
		  
		  //Success dialog
  		  GtkWidget *dialog1 = gtk_about_dialog_new();
  		   gtk_widget_set_size_request (dialog1, 300, 200);
		  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog1), "Bug Report\n Submitted Succesfully!");
		  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog1), 
		   "\nThank you\n");
		   gtk_dialog_run(GTK_DIALOG (dialog1));
		  gtk_widget_destroy(dialog1);
		  free(data);
		  gtk_widget_hide(window);
		 
	}
	
	else if(strcmp(text, "Feedback") == 0) {
		
		  strcpy(filename, "Feedbacks/feedback_");
		  char* rd = randomDigits();
		  strcat(filename, rd);
		  strcat(filename, ".txt");
		  free(rd);
		  
		 
		  FILE *fp2;
		  fp2 = fopen(filename, "w");
		  fprintf(fp2, "Username : %s\n-------------------------\n", username);
		  
		  
		  gtk_text_buffer_get_bounds(buffer, &start, &end);
		  contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		
		  
		  fprintf(fp2, "%s\n", contents);
		  
		  fclose(fp2);
		  
		  //Success dialog
		  GtkWidget *dialog2 = gtk_about_dialog_new();
  		   gtk_widget_set_size_request (dialog2, 300, 200);
		  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog2), "Feedback\n Submitted Succesfully!");
		  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog2), 
		   "\nThank you\n");
		   gtk_dialog_run(GTK_DIALOG (dialog2));
		  gtk_widget_destroy(dialog2);
		  free(data);
		gtk_widget_hide(window);
	}
	
	g_free(text);
}
//Function invoked when bug report/feedback menu item is clicked
void bug_selected(GtkWidget *widget, gpointer data) {
  GtkWidget *window;
  GtkWidget *table;

  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;

  GtkWidget *entry1;
  GtkWidget *combo;
  GtkWidget *text_view;
  GtkWidget *button; 
  
  GtkTextBuffer *buffer;
  
  
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_title(GTK_WINDOW(window), "Bug Report/Feedback");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_widget_set_size_request (window, 300, 270);
  gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
  
  table = gtk_table_new(4, 2, FALSE);
  gtk_container_add(GTK_CONTAINER(window), table);

  label1 = gtk_label_new("Username :");
  label2 = gtk_label_new("Type :");
  label3 = gtk_label_new("Message :");

  gtk_table_attach(GTK_TABLE(table), label1, 0, 1, 0, 1, 
      GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), label2, 0, 1, 1, 2, 
      GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), label3, 0, 1, 2, 3, 
      GTK_FILL, GTK_FILL, 5, 5);

  entry1 = gtk_entry_new();
  text_view = gtk_text_view_new ();
  gtk_widget_set_size_request (text_view, 100, 100);
  combo = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Bug Report");
  gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Feedback");
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
   button = gtk_button_new_with_label("Submit");
  gtk_widget_set_size_request(button, 70, 30);
  
   gtk_table_attach(GTK_TABLE(table), entry1, 1, 2, 0, 1, 
      GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  
   gtk_table_attach(GTK_TABLE(table), combo, 1, 2, 1, 2, 
      GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), text_view, 1, 2, 2, 3, 
      GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(table), button, 1, 2, 3, 4, 
      GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 30, 5);
      
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  

   Bugr *p = (Bugr *)malloc(sizeof(Bugr));      
   p->combo = combo;
   p->entry1 = entry1;
   p->text_view = text_view;
   p->window = window;
   g_signal_connect(G_OBJECT(button), "clicked", 
        G_CALLBACK(submit_clicked), p);
  

   
     
  gtk_widget_show_all(window);
	
}
//Add date and time internal plugin
void add_dnt(GtkWidget *widget, gpointer data) {
	 
	 if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        	
        	 time_t t = time(NULL);
   		 struct tm *tm = localtime(&t);
    		char s[64];
    		strftime(s, sizeof(s), "%c", tm);
   		
   		strcat(s, "\n");
   		gint len = strlen(s);
   		const gchar* text = s;
   		
   		if(data != NULL) 
   			gtk_text_buffer_insert_at_cursor (data, text, len);
   	}
        
}
//lower case internal plugin
void lower_case(GtkWidget *widget, gpointer data) {
	 gchar *contents;
        GtkTextIter start, end;
        int i;
        gtk_text_buffer_get_bounds(data, &start, &end);
   	contents = gtk_text_buffer_get_text(data, &start, &end, FALSE);
	
	 if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        	
   		
   		for( i = 0; contents[i]; i++) {
 			 contents[i] = tolower(contents[i]);
		}
   		const gchar* text = contents;
   		gint len = strlen(contents);
   		gtk_text_buffer_set_text (data, text, len);
        	
   	}
        
}
//Reverse internal plugin
void reverse_text(GtkWidget *widget, gpointer data) {
	
   	
	
	 if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        	
        	 gchar *contents;
		GtkTextIter start, end;
		 int i, j;
		 char temp;       	
	   	gtk_text_buffer_get_bounds(data, &start, &end);
	   	contents = gtk_text_buffer_get_text(data, &start, &end, FALSE);
   		 i = 0;
		 j = strlen(contents) - 1;
		 
		   while (i < j) {
		      temp = contents[i];
		      contents[i] = contents[j];
		      contents[j] = temp;
		      i++;
		      j--;
		   }
   		const gchar* text = contents;
   		gint len = strlen(contents);
   		gtk_text_buffer_set_text (data, text, len);
   	}
   
        
}
void help_clicked(GtkWidget *widget, gpointer window) {
    
 
	  GtkWidget *dialog = gtk_about_dialog_new();
	  gtk_widget_set_size_request (dialog, 700, 700);
	  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Quick Guide\nto create a custom external plugin\n");
	   gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"\nCopyright (c) 2016 Rohith Srivathsav\nunder GNU GPLv3 License");
	  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
	   "HOW IT WORKS:\n\n \
	    Basically, there are two different programs running simultaneously - the main program (editor.c) and your plugin program (foobar.c). The main program creates a text file (tmp.txt) and copies the data from the text buffer into this newly created file. Then the plugin program is executed which takes the data from this file (tmp.txt) , performs desirable funtions and puts back the modified text into the file. Finally, the main program reads the file (tmp.txt) and replaces the previous buffer with the modified data. The tmp.txt file is then deleted.\n\
	    \nPLUGIN CREATION:\n\n \
	    A prototype file to create your own plugin can be found in the link given below:\nhttp://pastebin.com/pPBVjtdT\nor at /Plugins/plugin_prototype.txt\n\n\
	    IMPORTANT NOTES:\n\n\
	    1. Make sure to place your plugin file (foobar.c) inside the 'Plugins' folder.\n\
	    2. Include GNU GPLv3 License and the name of the Plugin Creator inside the plugin file (foobar.c).\n\n \
	    Good Luck!");
	   gtk_dialog_run(GTK_DIALOG (dialog));
	  gtk_widget_destroy(dialog);
}
char pluginfile[128];

void external_plugin(GtkWidget *widget, gpointer data) {
	
	gchar *contents;
	
	GtkTextIter start, end;
        char ch, run[128], x[128];
        int i = 0, k, l;
  	gtk_text_buffer_get_bounds(data, &start, &end);
   	contents = gtk_text_buffer_get_text(data, &start, &end, FALSE);
	gchar *newContents = (gchar *) malloc(strlen(contents));
	
	//Step 1 : Copy data into tmp.txt
	FILE *fp1;
	fp1 = fopen("Plugins/tmp.txt", "w");
	fprintf(fp1, "%s", contents);
	fclose(fp1);
	
	//Step 2 : Run the chosen file and make changes to tmp.txt as wanted
	//system("gcc ./Plugins/uppercase.c -o plugin; ./plugin");
	
	run[0] = '\0';
	
	//Getting ONLY FILE NAME 'x' from directory 'pluginfile'	
	k = strlen(pluginfile) - 1;
	l = 0;

	
	while(k > 0) {
		if(pluginfile[k] == '/')
			break;
		else {
			x[l] = pluginfile[k];
			l++;
			k--;
		}
	}
	
	x[l] = '\0';	
	 
	l = 0;
	k = strlen(x) - 1;
	char temp;
	
	while (l < k) {
		temp = x[l];
	        x[l] = x[k];
	        x[k] = temp;
	        l++;
	        k--;
	}
	
	strcpy(run, "gcc ./Plugins/");
	strcat(run, x);
	strcat(run, " -o plugin; ./plugin");
	system(run);
		
	//Step 3 : Open tmp.txt and take the contents and initialize it to buffer
	FILE *fp2;
	fp2 = fopen("Plugins/tmp.txt", "r");
	while((ch = getc(fp2)) != EOF) {
    		newContents[i] = ch;
    		i++;
    	}
    	newContents[i] = '\0';
    	fclose(fp2);
	
	
   	gint len = strlen(newContents);
   	gtk_text_buffer_set_text (data, (gchar *) newContents, len);

	
	
	//Step 4 : Delete tmp.txt
	
	int status = remove("Plugins/tmp.txt");
	
	g_free(newContents);
	
 
}

//Function called when open file from plugin menu is selected
void open_plugin_selected(GtkWidget *widget, gpointer data){
  pluginfile[0] = '\0';
  GtkWidget *dialog;
 gchar *file;
  dialog = gtk_file_chooser_dialog_new ("Open File",
    GTK_WINDOW(data),
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    NULL);
  gint resp = gtk_dialog_run (GTK_DIALOG (dialog));
  if (resp == GTK_RESPONSE_ACCEPT) {
  	file = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	 strcpy(pluginfile,file);
   	 
   }
  
  
 gtk_widget_destroy(dialog);
}

//Function invoked when plugin menu item selected
void select_plugin(GtkWidget *widget, gpointer buffer) {
	
	GtkWidget *window;
	GtkWidget *table;
	
	GtkWidget *label1;
 	GtkWidget *label2;
  	GtkWidget *label3;
  	GtkWidget *label4;
  	
  	GtkWidget *inbuilt;
  	GtkWidget* external;

	GtkWidget *cb1;
	GtkWidget *cb2;
	GtkWidget *cb3;
	
	GtkWidget *choose;
	GtkWidget *run;
	GtkWidget *help;
	
	inbuilt = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(inbuilt), "\n<b>Inbuilt Plugins</b>\n");
	label1 = gtk_label_new("Adds current date and time at the cursor positon");
  	label2 = gtk_label_new("Converts text to lower case");
  	label3 = gtk_label_new("Reverses the entire text");
  	label4 = gtk_label_new("For help about adding your own external plugin click here :");
  	external = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(external), "\n<b>External Plugins</b>\n");
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 400);
	gtk_window_set_title(GTK_WINDOW(window), "Plugins");
	gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
	table = gtk_table_new(7, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(window), table);
        
        
	cb1 = gtk_check_button_new_with_label("Insert Date & Time");
	cb2 = gtk_check_button_new_with_label("Lower Case");
	cb3 = gtk_check_button_new_with_label("Reverse");
	
	choose = gtk_button_new_with_label("Choose");
	run = gtk_button_new_with_label("Run");
	help = gtk_button_new_with_label("Help");

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb1), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb2), FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb3), FALSE);
	  
	GTK_WIDGET_UNSET_FLAGS(cb1, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS(cb2, GTK_CAN_FOCUS);
	GTK_WIDGET_UNSET_FLAGS(cb3, GTK_CAN_FOCUS);

	g_signal_connect(cb1, "toggled", 
	    G_CALLBACK(add_dnt), (gpointer) buffer);
	
	g_signal_connect(cb2, "toggled", 
	    G_CALLBACK(lower_case), (gpointer) buffer);

	g_signal_connect(cb3, "toggled", 
	    G_CALLBACK(reverse_text), (gpointer) buffer);
	
	

	gtk_table_attach(GTK_TABLE(table), inbuilt, 0, 1, 0, 1, 
	    GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach(GTK_TABLE(table), cb1, 0, 1, 1, 2, 
	    GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach(GTK_TABLE(table), cb2, 0, 1, 2, 3, 
	    GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);
	gtk_table_attach(GTK_TABLE(table), cb3, 0, 1, 3, 4, 
	    GTK_FILL, GTK_FILL, 5, 5);
	gtk_table_attach(GTK_TABLE(table), external, 0, 1, 4, 5, 
	    GTK_FILL, GTK_FILL, 5, 5);
	gtk_table_attach(GTK_TABLE(table), choose, 0, 1, 5, 6, 
	    GTK_FILL, GTK_FILL, 20, 20);
	gtk_table_attach(GTK_TABLE(table), label4, 0, 1, 6, 7, 
	    GTK_FILL, GTK_FILL, 5, 5);
	 
	
	gtk_table_attach(GTK_TABLE(table), label1, 1, 2, 1, 2, 
      		GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 20, 20);
    	 gtk_table_attach(GTK_TABLE(table), label2, 1, 2, 2, 3, 
      		GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 20, 20);
  	gtk_table_attach(GTK_TABLE(table), label3, 1, 2, 3, 4, 
     		GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 20, 20);
     	gtk_table_attach(GTK_TABLE(table), run, 1, 2, 5, 6, 
     		GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 100, 20);
     	gtk_table_attach(GTK_TABLE(table), help, 1, 2, 6, 7, 
     		GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 100, 20);
     	
        g_signal_connect(G_OBJECT(choose), "clicked", 
     	   G_CALLBACK(open_plugin_selected), (gpointer) window);
     	   g_signal_connect(G_OBJECT(run), "clicked", 
        G_CALLBACK(external_plugin), (gpointer) buffer);
         g_signal_connect(G_OBJECT(help), "clicked", 
        G_CALLBACK(help_clicked), NULL);
  
  
     	gtk_widget_show_all(window);
	
	
}

//Function invoked when about menu item is clicked
void show_about(GtkWidget *widget, gpointer data) {
  
 
   GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Rite Editor 1.0.0\n");
   gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"\nCopyright (c) 2016 Rohith Srivathsav\nunder GNU GPLv3 License");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
   "'Rite Editor' is a minimal text editor which runs on Linux.It is implemented using Gtk+2.0 and C programming language.\nRite Editor is an OPEN SOURCE Software. You can add custom external plugins to the editor to add new funtionalities.");
   gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
  
}


//Function invoked when copy option is used
void copy_to_clipboard(GtkWidget *widget, GtkTextBuffer *buffer){
  GtkClipboard *clipboard;
  clipboard = gtk_clipboard_get(GDK_NONE);
  gtk_text_buffer_copy_clipboard(buffer, clipboard);
}

//Function invoked when cut option is used
void cut_to_clipboard(GtkWidget *widget, GtkTextBuffer *buffer){
  GtkClipboard *clipboard;
  clipboard = gtk_clipboard_get(GDK_NONE);
  gtk_text_buffer_cut_clipboard(buffer, clipboard,TRUE);
}

//Function invoked when paste option is used
void paste_from_clipboard(GtkWidget *widget, GtkTextBuffer *buffer){
  GtkClipboard *clipboard;
  clipboard = gtk_clipboard_get(GDK_NONE);
  gtk_text_buffer_paste_clipboard(buffer, clipboard,NULL,TRUE);
}



//Function invoked when text buffer is changed
void update_statusbar(GtkTextBuffer *buffer,
  GtkStatusbar  *statusbar) {
  gchar *msg;
  gint row, col;
  GtkTextIter iter;
  
  gtk_statusbar_pop(statusbar, 0); 

  gtk_text_buffer_get_iter_at_mark(buffer,
    &iter, gtk_text_buffer_get_insert(buffer));

  row = gtk_text_iter_get_line(&iter);
  col = gtk_text_iter_get_line_offset(&iter);

  msg = g_strdup_printf("Col: %d Ln: %d", col+1, row+1);

  gtk_statusbar_push(statusbar, 0, msg);

  g_free(msg);
}

//Function invoked when cursor changes
void mark_set_callback(GtkTextBuffer *buffer, 
  const GtkTextIter *new_location, GtkTextMark *mark, gpointer data) {

  update_statusbar(buffer, GTK_STATUSBAR(data));
}

void append_list(GtkWidget *widget, gpointer data) {
	gchar *contents;
	
	GtkTextIter start, end;
	
  	gtk_text_buffer_get_bounds(data, &start, &end);
   	contents = gtk_text_buffer_get_text(data, &start, &end, FALSE);
	InsertAtTail(contents);
		
}

int main(int argc, char *argv[]) {
  //--help / -h / -v implementation
  if(argc == 2) {
  
  	if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
  		printf("\nWelcome to Rite Editor. Here are some useful commands-\n");
  		printf("Help : Prints this message.\n./program --help\t\t ./program -h\n\n");
  		printf("Version : Prints current version of the editor.\n./program --version\t\t ./program -V\n\n");
  		return 0;
  	
  	}
  	
  	else if(strcmp(argv[1], "-V") == 0 || strcmp(argv[1], "--version") == 0) {
  		printf("\nRite Editor - Version 1.0.0\n\n");
  		return 0;
  	}
  	
  	else {
  		
  		perror("Invalid Arguement.\nUse : ./program --help OR ./program -h for help\n");
  		return EINVAL;
  	}
  		
  }
  
  //Definitions

  //Contains definitions for layout related items
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *hbox;
  
  //Icon Definition
  GdkPixbuf *icon;
  
  //Contains Menubar, Menu and menuitems inside it in an iterative way
  GtkWidget *menubar; 
  GtkWidget *fileMenu;
  GtkWidget *editMenu;
  GtkWidget *optionsMenu;
  GtkWidget *helpMenu;
  GtkWidget *fileMi;
  GtkWidget *newMi;
  GtkWidget *openMi;
  GtkWidget *saveMi;
  GtkWidget *quitMi;
  GtkWidget *editMi;
  GtkWidget *cutMi;
  GtkWidget *copyMi;
  GtkWidget *pasteMi;
  GtkWidget *undoMi;
  GtkWidget *redoMi;
  GtkWidget *optionsMi;
  GtkWidget *findMi;
  GtkWidget *fontMi;
  GtkWidget *pluginMi;
  GtkWidget *cb;
  GtkWidget *helpMi;
  GtkWidget *aboutMi;
  GtkWidget *bugMi;
  
  //Contains statusBar definition
  GtkWidget *statusbar;
  
  //init
  InsertAtTail("\0");
  
  //contains Text view and related definitions
  GtkWidget *text_view;
  GtkTextBuffer *buffer;
  GtkWidget* scrolledwindow;

  //searchbar definition
  SearchBar sbar;

  //Miscellaneous declaration
  // GtkClipboard *clipboard;
  GtkAccelGroup *accel_group = NULL;

  //initialize our gtk engine
  gtk_init(&argc, &argv);
	
  //properties for the main window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 1280, 720);
  gtk_window_set_title(GTK_WINDOW(window), "Rite Editor");
  
  icon = create_pixbuf("code.png");  
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  //Setting up vbox and hbox
  vbox = gtk_vbox_new(FALSE, 2);
  hbox = gtk_hbox_new(FALSE, 2);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  //initilize accel group which enables shortcut(accelerator) keys
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  //initialize menu bar
  menubar = gtk_menu_bar_new();

  // Setting File Menu //

  //initialize file menu and define file items in it
  fileMenu = gtk_menu_new();
  fileMi = gtk_menu_item_new_with_mnemonic("_File");
  newMi  = gtk_menu_item_new_with_label("New");
  openMi  = gtk_menu_item_new_with_label("Open");
  saveMi  = gtk_menu_item_new_with_label("Save");
  quitMi  = gtk_menu_item_new_with_label("Quit");

  //linking menu and its respective menu items
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), newMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), saveMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);

  //Defining shortcut keys for menu items
  gtk_widget_add_accelerator(newMi, "activate", accel_group, 
    GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
  gtk_widget_add_accelerator(openMi, "activate", accel_group, 
    GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
  gtk_widget_add_accelerator(saveMi, "activate", accel_group, 
    GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
  gtk_widget_add_accelerator(quitMi, "activate", accel_group, 
    GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 

  //attaching FileMenu to menubar
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi); 

  
  //Setting Edit Menu //

  //initialize edit menu and define file items in it
  editMenu = gtk_menu_new();
  editMi = gtk_menu_item_new_with_mnemonic("_Edit");
  cutMi  = gtk_menu_item_new_with_label("Cut");
  copyMi  = gtk_menu_item_new_with_label("Copy");
  pasteMi  = gtk_menu_item_new_with_label("Paste");
  undoMi  = gtk_menu_item_new_with_label("Undo");
  

  //linking edit menu and its respective menu items
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(editMi), editMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), cutMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), copyMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), pasteMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), undoMi);
  

  //Defining shortcut keys for edit menu items
  gtk_widget_add_accelerator(cutMi, "activate", accel_group, 
    GDK_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
  gtk_widget_add_accelerator(copyMi, "activate", accel_group, 
    GDK_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
  gtk_widget_add_accelerator(pasteMi, "activate", accel_group, 
    GDK_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
     gtk_widget_add_accelerator(undoMi, "activate", accel_group, 
    GDK_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
     

  //attaching EditMenu to menubar
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), editMi); 


  //Setting Options Menu //
  
  //initialize options menu and define file items in it
  optionsMenu = gtk_menu_new();
  optionsMi = gtk_menu_item_new_with_mnemonic("_Options");
  findMi  = gtk_menu_item_new_with_label("Find");
  fontMi  = gtk_menu_item_new_with_label("Font");
  pluginMi = gtk_menu_item_new_with_label("Plugins");

  //linking options menu and its respective menu items
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(optionsMi), optionsMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionsMenu), findMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionsMenu), fontMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(optionsMenu), pluginMi);
  

  //Defining shortcut keys for edit menu items
  gtk_widget_add_accelerator(findMi, "activate", accel_group, 
    GDK_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE); 
   gtk_widget_add_accelerator(pluginMi, "activate", accel_group, 
    GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);   

  //attaching optionsMenu to menubar
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), optionsMi); 

  //Setting Help Menu //
  
  //initialize help menu and define file items in it
  helpMenu = gtk_menu_new();
  helpMi = gtk_menu_item_new_with_mnemonic("_Help");
  aboutMi  = gtk_menu_item_new_with_label("About");
  bugMi = gtk_menu_item_new_with_label("Bug Report/Feedback");	
  
  //linking help menu and its respective menu items
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMi), helpMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), aboutMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), bugMi);
  
  //attaching helpMenu to menubar
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), helpMi); 


  //connecting layout of verticalbox to menubar
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

  //Initializing scrolledwindows
  scrolledwindow = gtk_scrolled_window_new(NULL,NULL);

  //Initializing textviewer and displaying it.
  text_view = gtk_text_view_new ();
  gtk_container_add(GTK_CONTAINER(scrolledwindow),text_view);
  gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, 1, 1, 0);

  // Initializing buffer
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

  //Set up search bar
  sbar.text_view = text_view;
  sbar.search_entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), sbar.search_entry, TRUE, TRUE, 0);

  sbar.sbutton = gtk_button_new_with_label ("Search");  
  gtk_box_pack_start (GTK_BOX (hbox),sbar.sbutton, FALSE, FALSE, 0);
  g_signal_connect (G_OBJECT (sbar.sbutton), "clicked", 
    G_CALLBACK (search_button_clicked), &sbar);

  sbar.nbutton = gtk_button_new_with_label ("Next");
  gtk_box_pack_start (GTK_BOX (hbox), sbar.nbutton, FALSE, FALSE, 0);
  g_signal_connect (G_OBJECT (sbar.nbutton), "clicked",
    G_CALLBACK (next_button_clicked), &sbar);

  sbar.qbutton = gtk_button_new_with_label ("Close");
  gtk_box_pack_start (GTK_BOX (hbox), sbar.qbutton, FALSE, FALSE, 0);
  g_signal_connect (G_OBJECT (sbar.qbutton), "clicked",
    G_CALLBACK (close_button_clicked), &sbar);

  //initializing open dialog
  OpenDialog odlog;
  odlog.window = window;
  odlog.buffer= buffer;

  //initializing new dialog
  NewDialog ndlog;
  ndlog.window=window;
  ndlog.buffer = buffer;
  ndlog.text_view = text_view;


  //initializing save dialog
  OpenDialog sdlog;
  sdlog.window=window;
  sdlog.buffer=buffer;

  //Initializing statusbar and displaying it.
  statusbar = gtk_statusbar_new();
  gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

  //initializing status bar to new dialog
  ndlog.statusbar = statusbar;
  
  //connect functions to events that is signalled by gtk

  g_signal_connect(G_OBJECT(window), "destroy",
    G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(quitMi), "activate",
    G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(copyMi),"activate",
    G_CALLBACK(copy_to_clipboard),buffer);

  g_signal_connect(G_OBJECT(cutMi),"activate",
    G_CALLBACK(cut_to_clipboard),buffer);


  g_signal_connect(G_OBJECT(aboutMi),"activate",
    G_CALLBACK(show_about),(gpointer) window);


  g_signal_connect(G_OBJECT(pasteMi),"activate",
    G_CALLBACK(paste_from_clipboard),buffer);
    
  g_signal_connect(G_OBJECT(undoMi),"activate",
    G_CALLBACK(undo_selected), buffer);

  g_signal_connect(G_OBJECT(fontMi),"activate",
    G_CALLBACK(select_font),text_view);
    
  g_signal_connect(G_OBJECT(pluginMi),"activate",
    G_CALLBACK(select_plugin),(gpointer) buffer);  

  g_signal_connect(G_OBJECT(findMi),"activate",
    G_CALLBACK(find_menu_selected), &sbar);

  g_signal_connect(G_OBJECT(openMi),"activate",
    G_CALLBACK(open_dialog_selected), &odlog);

  g_signal_connect(G_OBJECT(newMi),"activate",
    G_CALLBACK(new_dialog_selected), &ndlog);

 g_signal_connect(G_OBJECT(bugMi),"activate",
    G_CALLBACK(bug_selected), (gpointer) window);
    
  g_signal_connect(G_OBJECT(saveMi),"activate",
    G_CALLBACK(save_dialog_selected), &sdlog);

  g_signal_connect(buffer, "changed",
   G_CALLBACK(update_statusbar), statusbar);

  g_signal_connect(buffer, "changed",
    G_CALLBACK(append_list), (gpointer) buffer);

  g_signal_connect_object(buffer, "mark_set", 
    G_CALLBACK(mark_set_callback), statusbar, 0);

  // display all the windows and enter into event loop
  gtk_widget_show_all(window);

  //Hide find options at the start
  gtk_widget_hide(sbar.search_entry);
  gtk_widget_hide(sbar.sbutton);
  gtk_widget_hide(sbar.nbutton);
  gtk_widget_hide(sbar.qbutton);

  //hide the text_viewer in the beginning
  gtk_widget_hide(text_view);

  //hide status bar
  gtk_widget_hide(statusbar);

  //Display line and column number in status bar
  update_statusbar(buffer, GTK_STATUSBAR(statusbar));
  
  g_object_unref(icon);  
  gtk_main();
  Delete();
  
  return 0;
}
