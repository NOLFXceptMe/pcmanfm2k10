/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Parser module for .desktop files
 * To be integrated into PCManFM
 * Work as a part of GSoC 2010
 *
 */

#include<stdio.h>
#include<glib.h>
#include "parser.h"

int parse(gchar* file_name)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	gsize n_groups, n_keys;
	gchar **group_names, **key_names;
	gsize i, j;
	gchar *type_string;
	gchar *profile_id;
	guint type;


	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS;

	if(!g_key_file_load_from_file(keyfile, file_name, flags, &error)){
		fprintf(stderr, error->message);
		return -1;
	}

	group_names = g_key_file_get_groups(keyfile, &n_groups);

	/* Store action entries, profile entries, and menu entries in respective data structures */
	for(i=0;i<n_groups;++i){
#ifndef NDEBUG
		printf("Name of the group is %s\n", group_names[i]);
#endif

		/* Action entry or Menu entry
		 * Also have one issue to be clarified on XDG mailing list
		 * - Key names are mentioned to be case sensitive
		 * - Nothing as such is given for group headers. So, is "desktop Entry", the same as "Desktop Entry"?
		 * For now assuming that group headers are case insensitive, hence using g_ascii_strcasecmp().  If case sensitive, shift to g_strcmp0() */
		if(g_ascii_strcasecmp(group_names[i], "Desktop Entry") == 0){
			type_string = g_key_file_get_string(keyfile, group_names[i], "Type", NULL);
			if(type_string == NULL || g_strcmp0(type_string, "Action") == 0)
				type = ACTION_ENTRY;
			else if(g_strcmp0(type_string, "Menu") == 0)
				type = MENU_ENTRY;
			else
				type = UNKNOWN_ENTRY;
		} else if(g_str_has_prefix(group_names[i], "X-Action-Profile ") == TRUE){
			type = PROFILE_ENTRY;
			profile_id = g_strdup(group_names[i]+ 17);		/* "X-Action-Profile " is 17 characters long */
		} else {
			type = UNKNOWN_ENTRY;
		}
		

#ifndef NDEBUG
		switch(type){
			case ACTION_ENTRY:
				printf("Action Entry\n");
				break;
			case MENU_ENTRY:
				printf("Menu Entry\n");
				break;
			case PROFILE_ENTRY:
				printf("Profile Entry with profile_id %s\n", profile_id);
				break;
			case UNKNOWN_ENTRY:
				printf("Unknown Entry\n");
				break;
			default:
				break;
		};
	}
#endif

	return 0;
}
