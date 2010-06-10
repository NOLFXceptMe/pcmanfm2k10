/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Parser module for .desktop files
 * To be integrated into PCManFM
 * Work as a part of GSoC 2010
 *
 */

#include <stdio.h>
#include<glib.h>

int parse(gchar* file_name);

int main()
{
	int ret_val = parse("test.desktop");
	if(ret_val >0) printf("Successfully parsed test.desktop");
	printf("Test 1 completed");

	return 0;
}

int parse(gchar* file_name)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	gsize n_groups, n_keys;
	gchar **group_names, **key_names;
	gsize i, j;

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS;

	if(!g_key_file_load_from_file(keyfile, file_name, flags, &error)){
		fprintf(stderr, error->message);
		return -1;
	}

	group_names = g_key_file_get_groups(keyfile, &n_groups);
	printf("Number of groups = %u\n", n_groups);

	/* Print extracted key pairs */
	for(i=0;i<n_groups;++i){
		printf("Name of the group is %s\n", group_names[i]);

		key_names = g_key_file_get_keys(keyfile, group_names[i], &n_keys, NULL);
		printf("Number of keys = %u\n", n_keys);

		for(j=0;j<n_keys;++j){
			printf("Extracted key-value pair %s = %s\n", key_names[j], g_key_file_get_value(keyfile, group_names[i], key_names[j], &error));
		}
	}

	return 0;
}
