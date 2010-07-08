/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Test code, to simulate a rightclick for the sake of displaying a menu. Instead of showing up a graphical menu, this will write to stdout
 */

#include<glib.h>
#include<glib/gstdio.h>
#include<stdio.h>
#include<features.h>
#include<stdlib.h>

#include<libfm/fm.h>
#include<libfm/fm-file-info.h>
#include<libfm/fm-file-info-job.h>
#include<libfm/fm-path.h>
#include<libfm/fm-list.h>

#include "parser.h"
#include "validation.h"
#include "parameter.h"
#include "substitution.h"

void print_file_info(gpointer, gpointer);
//void add_to_file_info_list(gpointer data, gpointer user_data);
void print_base_names(gpointer data, gpointer user_data);

void add_to_mime_types(gpointer data, gpointer user_data);
void add_to_base_names(gpointer data, gpointer user_data);
void add_to_capabilities(gpointer data, gpointer user_data);
void add_to_schemes(gpointer data, gpointer user_data);
void add_to_folders(gpointer data, gpointer user_data);

gchar *environment = "LXDE";
gsize selection_count = 0;
GPtrArray *mime_types = NULL, *base_names = NULL, *capabilities_array = NULL, *schemes_array = NULL, *folder_array = NULL;

//int main(int argc, char *argv[])
GPtrArray* showmenu(GPtrArray *desktop_files_array)
{
	/*
	if(argc<2){
		fprintf(stderr, "Usage: ./showmenu <filename>");
		return -1;
	}
	*/

	g_type_init();
	fm_init(NULL);

	gsize i;
	FmProfileEntry *fmProfileEntry;
	FmActionEntry *fmActionEntry;
	FmMenuEntry *fmMenuEntry;
	GPtrArray *fmProfileEntries, *fmActionEntries, *fmMenuEntries;
	GHashTable *fmProfiles, *fmActions, *fmMenus;

	fmProfiles = g_hash_table_new(NULL, NULL);
	fmActions = g_hash_table_new(NULL, NULL);
	fmMenus = g_hash_table_new(NULL, NULL);

	mime_types = g_ptr_array_new();
	base_names = g_ptr_array_new();
	capabilities_array = g_ptr_array_new();
	schemes_array = g_ptr_array_new();

	//FmDesktopEntry *desktop_entry = parse(argv[1]);
	FmDesktopEntry *desktop_entry = NULL;

	/*
	if(desktop_entry == NULL){
		fprintf(stderr, "Failed to open %s\n", argv[1]);
		return -1;
	}
	*/

	for(i=0; i<desktop_files_array->len; ++i){
		desktop_entry = parse((gchar *)g_ptr_array_index(desktop_files_array, i));

		fmProfileEntries = desktop_entry->fmProfileEntries;
		fmActionEntries = desktop_entry->fmActionEntries;
		fmMenuEntries = desktop_entry->fmMenuEntries;

		for(i=0;i<desktop_entry->n_profile_entries;++i){
			fmProfileEntry = g_ptr_array_index(fmProfileEntries, i);
			g_hash_table_insert(fmProfiles, fmProfileEntry->id, fmProfileEntry);
		}

		for(i=0;i<desktop_entry->n_action_entries;++i){
			fmActionEntry = g_ptr_array_index(fmActionEntries, i);
			g_hash_table_insert(fmActions, fmActionEntry->name, fmActionEntry);
		}

		for(i=0;i<desktop_entry->n_menu_entries;++i){
			fmMenuEntry = g_ptr_array_index(fmMenuEntries, i);
			g_hash_table_insert(fmMenus, fmMenuEntry->name, fmMenuEntry);
		}
	}

	/* Set some base conditions */
	/* Environment: LXDE
	 */
	/* Make entries into the path_list, manually */
	FmPathList *path_list = fm_path_list_new();
	/* "examples" is a directory, and "home" is a link, both of them have the mime type inode/directory */
	//fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/examples"));
	//fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/home"));
	//fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/parser.c"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/Einstein_german.ogg"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/Roggan.mp3"));
	//fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/action"));
	//fm_list_push_tail(path_list, fm_path_new("http://localhost:8000/README"));
	
	/* Make a list of file infos from the path_list */
	/* I can't get to use fm_file_info_job_new(), for some reason, it does not fill in the file info data structures */
	FmJob *job = fm_file_info_job_new(path_list, FM_FILE_INFO_JOB_NONE);
	fm_job_run_sync(job);
	FmFileInfoList *file_info_list = ((FmFileInfoJob *)job)->file_infos;

	/* Now evaluate other data depending on the files_selected */
	selection_count = fm_list_get_length(file_info_list);
	fm_list_foreach(file_info_list, add_to_mime_types, mime_types);
	fm_list_foreach(file_info_list, add_to_base_names, base_names);
	fm_list_foreach(file_info_list, add_to_capabilities, capabilities_array);
	fm_list_foreach(file_info_list, add_to_schemes, schemes_array);
	fm_list_foreach(file_info_list, add_to_folders, folder_array);
	/* Pre-processing done */

	/* Substitute parameters */

	/* Validate profiles */
	g_hash_table_foreach(fmProfiles, substitute_profile_params, file_info_list);
	g_hash_table_foreach(fmActions, substitute_action_params, file_info_list);
	g_hash_table_foreach(fmMenus, substitute_menu_params, file_info_list);

	GPtrArray *valid_profiles = retrieve_valid_profiles(fmProfiles);
	for(i=0;i<valid_profiles->len;++i){
		fmProfileEntry = g_ptr_array_index(valid_profiles, i);
		printf("%s is a valid profile\n", fmProfileEntry->id);
	}
	printf("\n");

	/* Validate actions */
	GPtrArray *valid_actions = retrieve_valid_actions(fmActions, valid_profiles);
	//GPtrArray *valid_actions_names = g_ptr_array_new();
	for(i=0;i<valid_actions->len;++i){
		fmActionEntry = g_ptr_array_index(valid_actions, i);
		//printf("%s is a valid action\n", fmActionEntry->name);
		//g_ptr_array_add(valid_actions_names, fmActionEntry->id);
	}

	/* Validate menus */
	GPtrArray *valid_menus = retrieve_valid_menus(fmMenus, valid_actions);
	for(i=0;i<valid_menus->len;++i){
		fmMenuEntry = g_ptr_array_index(valid_menus, i);
		printf("%s is a valid menu\n", fmMenuEntry->id);
	}

	/* Show context menu */
	/* Context menu will show valid actions and menus */

	//return 0;
	//return valid_actions_names;
	return valid_actions;
}

/*
void add_to_file_info_list(gpointer data, gpointer user_data)
{
	FmPath *path = (FmPath *)data;
	FmFileInfoList *file_info_list = (FmFileInfoList *)user_data;
	GFile *file = fm_path_to_gfile(path);
	GFileInfo *info = g_file_query_info(file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
	FmFileInfo *fi = fm_file_info_new();
	fi->path = fm_path_new("");
	fm_file_info_set_from_gfileinfo(fi, info);

	fm_list_push_tail(file_info_list, fi);
}
*/

void print_file_info(gpointer data, gpointer user_data)
{
	FmFileInfo *file_info = (FmFileInfo *)data;
	printf("%s\n", fm_file_info_get_disp_name(file_info));
	printf("%s\n", fm_mime_type_get_type(fm_file_info_get_mime_type(file_info)));
}

void print_base_names(gpointer data, gpointer user_data)
{
	gchar *basename = (gchar *)data;
	printf("%s\n", basename);
}

void add_to_mime_types(gpointer data, gpointer user_data)
{
	FmFileInfo *file_info = (FmFileInfo *)data;
	GPtrArray *mime_types = (GPtrArray *)user_data;
	FmMimeType *mime_type = fm_file_info_get_mime_type(file_info);
	if(mime_type == NULL)
		return;

	gchar *mime_type_string = (gchar *)fm_mime_type_get_type(mime_type);
	gchar *mime_type_string_i = NULL;
	guint i;
	for(i = 0; i<mime_types->len; i++){
		mime_type_string_i = (gchar *)g_ptr_array_index(mime_types, i);
		if(g_strcmp0(mime_type_string, mime_type_string_i) == 0)
			return;
	}

	g_ptr_array_add(mime_types, (gpointer) mime_type_string);
}

void add_to_base_names(gpointer data, gpointer user_data)
{
	FmFileInfo *file_info = (FmFileInfo *)data;
	GPtrArray *base_names = (GPtrArray *)user_data;
	gchar *display_name = (gchar *)fm_file_info_get_disp_name(file_info);
	gchar *base_name = g_strdup(display_name);

	//printf("Adding %s to base_names\n", base_name);
	g_ptr_array_add(base_names, (gpointer) base_name);
}

void add_to_capabilities(gpointer data, gpointer user_data)
{
	FmFileInfo *file_info = (FmFileInfo *)data;
	GPtrArray *capabilities = (GPtrArray *)user_data;
	FmCapabilities *fm_capability = (FmCapabilities *)g_new0(FmCapabilities, 1);
	mode_t mode = file_info->mode;
	
	fm_capability->isOwner = (file_info->uid == getuid())?TRUE:FALSE;
	fm_capability->isReadable = (S_IRUSR & mode)?TRUE:FALSE;
	fm_capability->isWritable = (S_IWUSR & mode)?TRUE:FALSE;
	fm_capability->isExecutable = (S_IXUSR & mode)?TRUE:FALSE;

	gchar *scheme_restricted = g_strndup(fm_path_to_uri(fm_file_info_get_path(file_info)), 4);
	fm_capability->isLocal = (g_strcmp0(scheme_restricted, "file") == 0)?TRUE:FALSE;

	g_ptr_array_add(capabilities, (gpointer) fm_capability);
}

void add_to_schemes(gpointer data, gpointer user_data)
{
	FmFileInfo *fi = (FmFileInfo *)data;
	GPtrArray *schemes_array = (GPtrArray *)user_data;
	gchar *uri = fm_path_to_uri(fm_file_info_get_path(fi));
	gsize i;

	/* Now parse the scheme */
	/* Using string.h functions. GLib doesn't have strcspn()? :O */
	size_t colon_pos = strcspn(uri, ":");
	gchar *scheme = g_strndup(uri, colon_pos), *scheme_i = NULL;

	for(i=0; i<schemes_array->len; ++i){
		scheme_i = (gchar *)g_ptr_array_index(schemes_array, i);
		if(g_strcmp0(scheme, scheme_i) == 0){
			g_free(scheme);
			g_free(uri);
			return;
		}
	}

	g_ptr_array_add(schemes_array, (gpointer) scheme);
	g_free(uri);
}

void add_to_folders(gpointer data, gpointer user_data)
{
	//FmFileInfo *fi = (FmFileInfo *)data;
	//GPtrArray *folders_array = (GPtrArray *)user_data;

}
