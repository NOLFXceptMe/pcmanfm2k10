/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Test code for using GKeyFile to read .desktop files to Action definitions [ref. http://www.nautilus-actions.org/?q=node/377], and to work out a possible data structure
 */

#include<stdio.h>
#include "parser.h"

int main()
{
	gsize i, j;
	FmActionEntry *fmActionEntry;
	FmProfileEntry *fmProfileEntry;
	FmMenuEntry *fmMenuEntry;
	GPtrArray *fmActionEntries, *fmProfileEntries, *fmMenuEntries;

	FmDesktopEntry *desktop_entry = parse("open-terminal.desktop");

	/* Extract data */
	printf("Desktop file_id = %s\n", desktop_entry->desktop_file_id);
	printf("Number of action definitions = %u\n", desktop_entry->n_action_entries);
	printf("Number of profile definitions = %u\n", desktop_entry->n_profile_entries);
	printf("Number of menu definitions = %u\n", desktop_entry->n_menu_entries);

	fmActionEntries = desktop_entry->fmActionEntries;
	fmProfileEntries = desktop_entry->fmProfileEntries;
	fmMenuEntries = desktop_entry->fmMenuEntries;

	/* Extract action entries data */
	for(i=0;i<desktop_entry->n_action_entries;++i){
		fmActionEntry = g_ptr_array_index(fmActionEntries, i);

		/* Copy data into local variables first */

		printf("Required data\n");
		printf("Name = %s\n", fmActionEntry->name);
		printf("List of profiles attached to this action\n");
		for(j=0;j<fmActionEntry->n_profiles;++j){
			printf("%s\n", fmActionEntry->profiles[j]);
		}

		printf("Optional data\n");
		if(fmActionEntry->tooltip) printf("Tooltip = %s\n", fmActionEntry->tooltip);
		if(fmActionEntry->icon) printf("Icon = %s\n", fmActionEntry->icon);
		if(fmActionEntry->description) printf("Description = %s\n", fmActionEntry->description);
		/* TODO: Complete this */
		/*
	if(fmActionEntry->suggestedshortcut) printf("SuggestedShortcut = %s\n", fmActionEntry->suggestedShortcut);
	if(fmActionEntry->enabled == TRUE)
		printf(
	fmActionEntry->hidden = printf();
	fmActionEntry->targetcontext = printf();
	fmActionEntry->targetlocation = printf();
	fmActionEntry->targettoolbar = printf();
	fmActionEntry->toolbarlabel = printf();
	}
	*/

	/* Extract profile entries data */
	for(i=0;i<desktop_entry->n_profile_entries;++i){
		fmProfileEntry = g_ptr_array_index(fmProfileEntries, i);
	}

	/* Extract menu entries data */
	for(i=0;i<desktop_entry->n_menu_entries;++i){
		fmMenuEntry = g_ptr_array_index(fmMenuEntries, i);
	}

	return 0;
}
