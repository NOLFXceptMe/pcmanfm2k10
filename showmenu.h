#ifndef _SHOWMENU_H_
#define _SHOWMENU_H_

void print_file_info(gpointer, gpointer);
void print_base_names(gpointer data, gpointer user_data);

void add_to_mime_types(gpointer data, gpointer user_data);
void add_to_base_names(gpointer data, gpointer user_data);
void add_to_capabilities(gpointer data, gpointer user_data);
void add_to_schemes(gpointer data, gpointer user_data);

gchar *environment = "LXDE";
gsize selection_count = 0;
GPtrArray *mime_types = NULL, *base_names = NULL, *capabilities_array = NULL, *schemes_array = NULL;

#endif	/* _SHOWMENU_H_ */
