#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "parser.h"

#define DESKTOP_FILES_FOLDER "/home/npower/.config/GSOC/"

GPtrArray* showmenu(GPtrArray *desktop_files_array);

int main()
{
	gsize i;
	gchar *extension = NULL;
	GPtrArray *desktop_files_array = g_ptr_array_new();
	gchar *desktop_files_array_entry = NULL;
	GPtrArray *valid_actions = NULL;
	DIR *desktop_files_folder = NULL;
	struct dirent *dir_entry = NULL;
	
	desktop_files_folder = opendir(DESKTOP_FILES_FOLDER);
	/* Lookup the $DESKTOP_FILES_FOLDER for .desktop files, and put the names in the desktop_files_array */
	while((dir_entry = readdir(desktop_files_folder)) != NULL){
		extension = strrchr(dir_entry->d_name, '.');
		if(extension == NULL || g_strcmp0(extension, ".desktop") != 0) continue;

		desktop_files_array_entry = g_strconcat(DESKTOP_FILES_FOLDER, dir_entry->d_name, NULL);
		//printf("%s\n", desktop_files_array_entry);
		g_ptr_array_add(desktop_files_array, desktop_files_array_entry);
	}
	closedir(desktop_files_folder);

	/* Now pass on the array to showmenu() */
	valid_actions = showmenu(desktop_files_array);
	for(i=0;i<valid_actions->len;++i){
		printf("%s is a valid action\n", (gchar *)(((FmActionEntry *)g_ptr_array_index(valid_actions, i))->id));
	}

	return 0;
}
