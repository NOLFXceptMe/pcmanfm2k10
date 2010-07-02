#include<gio/gio.h>
#include<glib.h>

#include<libfm/fm.h>

int main()
{
	g_type_init();
	fm_init(NULL);

	gchar *file_name = "open-terminal.desktop";	 //Some desktop file
	GFile *file = g_file_new_for_path(file_name);
	GFileInfo *info = g_file_query_info(file, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
	//printf("%s\n", g_file_info_get_display_name(info));
	FmFileInfo* fi = fm_file_info_new();
	fi->path = fm_path_new("");
	fm_file_info_set_from_gfileinfo(fi, info);
	printf("%s\n", fm_file_info_get_disp_name(fi));

	gboolean isDeskEntry = fm_file_info_is_desktop_entry(fi);
	if(isDeskEntry)
		printf("yes\n");
	else
		printf("no\n");

	return 0;
}
