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

int main(int argc, char *argv[])
{
	g_type_init();
	fm_init(NULL);

	gsize i;
	FmProfileEntry *fmProfileEntry;
	FmActionEntry *fmActionEntry;
	GPtrArray *fmProfileEntries, *fmActionEntries;
	GHashTable *fmProfiles, *fmActions;

	FmDesktopEntry *desktop_entry = parse(argv[1]);

	fmProfileEntries = desktop_entry->fmProfileEntries;
	fmActionEntries = desktop_entry->fmActionEntries;

	fmProfiles = g_hash_table_new(NULL, NULL);
	fmActions = g_hash_table_new(NULL, NULL);
	mime_types = g_ptr_array_new();
	base_names = g_ptr_array_new();
	capabilities_array = g_ptr_array_new();
	schemes_array = g_ptr_array_new();

	for(i=0;i<desktop_entry->n_profile_entries;++i){
		fmProfileEntry = g_ptr_array_index(fmProfileEntries, i);
		g_hash_table_insert(fmProfiles, fmProfileEntry->id, fmProfileEntry);
	}

	for(i=0;i<desktop_entry->n_action_entries;++i){
		fmActionEntry = g_ptr_array_index(fmActionEntries, i);
		g_hash_table_insert(fmActions, fmActionEntry->name, fmActionEntry);
	}

	/* Set some base conditions */
	/* Environment: LXDE
	 */
	/* Make entries into the path_list, manually */
	//path_list = fm_path_list_new_from_uris((const char **)uris);
	FmPathList *path_list = fm_path_list_new();
	/*
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/parser.c"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/parser.h"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/README"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/showmenu.c"));
	*/
	/*
	 * "examples" is a directory, and "home" is a link, both of them have the mime type inode/directory */
	/*
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/examples"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/home"));
	*/
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/parser.c"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/Einstein_german.ogg"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/Roggan.mp3"));
	//fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/action"));
	
	/* Make a list of file infos from the path_list */
	/* I can't get to use fm_file_info_job_new(), for some reason, it does not fill in the file info data structures */
	FmJob *job = fm_file_info_job_new(path_list, FM_FILE_INFO_JOB_NONE);
	fm_job_run_sync(job);
	FmFileInfoList *file_info_list = ((FmFileInfoJob *)job)->file_infos;

	/* If fm_file_info_job_new() doesn't work for some reason, manually enter the file infos into the file_info_list */
	//FmFileInfo *file_info = NULL;
	//fm_list_foreach(path_list, add_to_file_info_list, file_info_list);
	
	/*
	if(file_info_list){
		printf("Have %d files selected\n", fm_list_get_length(file_info_list));
		fm_list_foreach(file_info_list, print_file_info, NULL);
	}
	*/

	/* Now evaluate other data depending on the files_selected */
	selection_count = fm_list_get_length(file_info_list);
	fm_list_foreach(file_info_list, add_to_mime_types, mime_types);
	fm_list_foreach(file_info_list, add_to_base_names, base_names);
	fm_list_foreach(file_info_list, add_to_capabilities, capabilities_array);
	fm_list_foreach(file_info_list, add_to_schemes, schemes_array);
	fm_list_foreach(file_info_list, add_to_folders, folder_array);
	/* Pre-processing done */

	/* Validate profiles */
	GPtrArray *valid_profiles = retrieve_valid_profiles(fmProfiles);
	for(i=0;i<valid_profiles->len;++i){
		fmProfileEntry = g_ptr_array_index(valid_profiles, i);
		printf("%s is a valid profile\n", fmProfileEntry->id);
	}
	printf("\n");

	/* Validate actions */
	GPtrArray *valid_actions = retrieve_valid_actions(fmActions, valid_profiles);
	for(i=0;i<valid_actions->len;++i){
		fmActionEntry = g_ptr_array_index(valid_actions, i);
		printf("%s is a valid action\n", fmActionEntry->name);
	}

	/* Validate menus */

	/* Show context menu */
	/* Context menu will show valid actions and menus */

	return 0;
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
	gchar *base_name_temp = g_strrstr(display_name, ".");
	if(base_name_temp == NULL)
		return;
	gchar *base_name = g_strconcat("*", base_name_temp, NULL);

	guint i;
	gchar *base_name_i;
	for(i = 0; i<base_names->len; i++){
		base_name_i = (gchar *)g_ptr_array_index(base_names, i);
		if(g_strcmp0(base_name, base_name_i) == 0)
			return;
	}

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
