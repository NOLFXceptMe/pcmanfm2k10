#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"

//#define DESKTOP_FILES_FOLDER "/home/npower/.config/GSOC/"

/* For testers, move the files in desktop_files folder in the current git repo to wherever and modify the above string */

GPtrArray* showmenu(GPtrArray *desktop_files_array);

int main()
{
	gsize i;
	GPtrArray *desktop_files_array = g_ptr_array_new();
	gchar *desktop_files_array_entry = NULL;
	GPtrArray *valid_actions = NULL;
	GDir *desktop_files_folder = NULL;
	gchar *dir_entry;
	GError *error = NULL;
	gchar *user_config_dir = (gchar *)g_get_user_config_dir();
	gchar *app_name = "GSOC";
	gchar *desktop_files_path = g_build_path("/", user_config_dir, app_name, NULL);

	//printf("User Config Dir = \"%s\"\n", user_config_dir);
	//printf("Desktop files path = \"%s\"\n", desktop_files_path);

	desktop_files_folder = g_dir_open(desktop_files_path, 0, &error);
	if(desktop_files_folder == NULL){
		fprintf(stderr, "%s\n", error->message);
		return -1;
	}

	/* Lookup the desktop_files_folder for .desktop files, and put the names in the desktop_files_array */
	while((dir_entry = (gchar *)g_dir_read_name(desktop_files_folder)) != NULL){
		if(g_str_has_suffix(dir_entry, ".desktop") == FALSE) continue;

		desktop_files_array_entry = g_build_path("/", desktop_files_path, dir_entry, NULL);
		//printf("%s\n", desktop_files_array_entry);
		g_ptr_array_add(desktop_files_array, desktop_files_array_entry);
	}
	g_dir_close(desktop_files_folder);

	/* Now pass on the array to showmenu() */
	valid_actions = showmenu(desktop_files_array);
	for(i=0;i<valid_actions->len;++i){
		printf("%s is a valid action\n", (gchar *)(((FmActionEntry *)g_ptr_array_index(valid_actions, i))->id));
	}

	g_ptr_array_free(desktop_files_array, TRUE);
	g_ptr_array_free(valid_actions, TRUE);
	g_free(user_config_dir);
	g_free(desktop_files_path);
	return 0;
}
