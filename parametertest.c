#include<glib.h>

#include<libfm/fm.h>
#include<libfm/fm-file-info.h>
#include<libfm/fm-job.h>
#include<libfm/fm-file-info-job.h>

#include "parameter.h"

void print_gstring(gpointer data, gpointer user_data);

int main()
{
	g_type_init();
	fm_init(NULL);

	GPtrArray *out_string_array = NULL;
	gchar *input_string = "cat %%";

	FmPathList *path_list = fm_path_list_new();
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/parser.c"));
	fm_list_push_tail(path_list, fm_path_new("http://localhost:8000/"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/Einstein_german.ogg"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code/GSOC/pcmanfm2k10/Roggan.mp3"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Code"));
	fm_list_push_tail(path_list, fm_path_new("/home/npower/Music"));
	
	/* Make a list of file infos from the path_list */
	/* I can't get to use fm_file_info_job_new(), for some reason, it does not fill in the file info data structures */
	FmJob *job = fm_file_info_job_new(path_list, FM_FILE_INFO_JOB_NONE);
	fm_job_run_sync(job);
	FmFileInfoList *file_info_list = ((FmFileInfoJob *)job)->file_infos;

	out_string_array = substitute_parameters(input_string, file_info_list);

	g_ptr_array_foreach(out_string_array, print_gstring, NULL);

	return 0;
}

void print_gstring(gpointer data, gpointer user_data)
{
	GString *gstring = (GString *)data;
	printf("%s\n", gstring->str);
}
