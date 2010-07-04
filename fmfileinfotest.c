#include<gio/gio.h>
#include<glib.h>

#include<libfm/fm.h>

int main()
{
	g_type_init();
	fm_init(NULL);

	FmPath *path = fm_path_new("/home/npower/Makefile");
	GFile *file = fm_path_to_gfile(path);
	GFileInfo *inf = g_file_query_info(file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
	FmFileInfo *fi = fm_file_info_new_from_gfileinfo(path, inf);
	printf("%s\n", fm_file_info_get_disp_name(fi));

	gboolean isDeskEntry = fm_file_info_is_desktop_entry(fi);
	if(isDeskEntry)
		printf("yes\n");
	else
		printf("no\n");

	g_free(path);
	path = fm_file_info_get_path(fi);
	printf("%s\n", fm_path_to_str(path));
	printf("%s\n", fm_path_to_uri(path));

	return 0;
}
