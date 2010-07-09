#include<glib.h>
#include<libfm/fm.h>

int main()
{
	g_type_init();
	fm_init(NULL);

	//FmPath *folder_path = fm_path_new("/");
	//FmFolder *folder = fm_folder_get_for_path(folder_path);
	FmFolder *folder = fm_folder_get_for_path(fm_path_get_desktop());
	FmFileInfoList *file_list = fm_folder_get_files(folder);
	printf("%d number of files found\n", fm_list_get_length(file_list));
	gboolean same_type = fm_file_info_list_is_same_type(file_list);
	gboolean same_fs = fm_file_info_list_is_same_fs(file_list);

	if(same_type == TRUE) printf("Same type\n");
	else printf("Not the same type\n");

	if(same_fs == TRUE) printf("Same fs\n");
	else printf("Not the same fs\n");

	FmFileInfo *file_info;
	while(!fm_list_is_empty(file_list)){
		file_info = (FmFileInfo *)fm_list_pop_head(file_list);
		FmMimeType *mime = fm_file_info_get_mime_type(file_info);
		printf("%s is of mime type %s\n", fm_file_info_get_name(file_info), fm_mime_type_get_type(mime));
		printf("Description: %s\n", fm_mime_type_get_desc(mime));
	}

	return 0;
}

