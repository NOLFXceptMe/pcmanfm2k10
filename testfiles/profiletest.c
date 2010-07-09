/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Test code for using GKeyFile to read .desktop files to Profile definitions [ref. http://www.nautilus-actions.org/?q=node/377], and to work out a possible data structure
 */

#include<stdio.h>
#include "parser.h"

int main(int argc, char *argv[])
{
	gsize i/*, j*/;
	//FmActionEntry *fmActionEntry;
	FmProfileEntry *fmProfileEntry;
	//FmMenuEntry *fmMenuEntry;
	GPtrArray *fmActionEntries, *fmProfileEntries, *fmMenuEntries;
	GHashTable *fmProfiles;

	FmDesktopEntry *desktop_entry = parse(argv[1]);

	/* Extract data */
	printf("Desktop file_id = %s\n", desktop_entry->desktop_file_id);
	printf("Number of action definitions = %u\n", desktop_entry->n_action_entries);
	printf("Number of profile definitions = %u\n", desktop_entry->n_profile_entries);
	printf("Number of menu definitions = %u\n", desktop_entry->n_menu_entries);

	fmActionEntries = desktop_entry->fmActionEntries;
	fmProfileEntries = desktop_entry->fmProfileEntries;
	fmMenuEntries = desktop_entry->fmMenuEntries;

	fmProfiles = g_hash_table_new(NULL, NULL);

	for(i=0;i<desktop_entry->n_profile_entries;++i){
		fmProfileEntry = g_ptr_array_index(fmProfileEntries, i);
		g_hash_table_insert(fmProfiles, fmProfileEntry->id, (gpointer) fmProfileEntry);
	}

	return 0;
}
