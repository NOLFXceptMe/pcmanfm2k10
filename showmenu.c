/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Test code, to simulate a rightclick for the sake of displaying a menu. Instead of showing up a graphical menu, this will write to stdout
 */

#include<glib.h>
#include<stdio.h>

#include "parser.h"

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles);
void validate_profile(gpointer key, gpointer value, gpointer profile_array);
gboolean validate_conditions(FmConditions *conditions);
void printprofile(gpointer data, gpointer user_data);

gchar *environment = "LXDE";
gchar *mime_type = "text/plain";

int main(int argc, char *argv[])
{
	gsize i/*, j*/;
	FmProfileEntry *fmProfileEntry;
	GPtrArray *fmProfileEntries;
	GHashTable *fmProfiles;
	GPtrArray *valid_profiles;

	FmDesktopEntry *desktop_entry = parse(argv[1]);

	/* Extract data */
	/*
	printf("Desktop file_id = %s\n", desktop_entry->desktop_file_id);
	printf("Number of action definitions = %u\n", desktop_entry->n_action_entries);
	printf("Number of profile definitions = %u\n", desktop_entry->n_profile_entries);
	printf("Number of menu definitions = %u\n", desktop_entry->n_menu_entries);
	*/

	fmProfileEntries = desktop_entry->fmProfileEntries;

	fmProfiles = g_hash_table_new(NULL, NULL);

	for(i=0;i<desktop_entry->n_profile_entries;++i){
		fmProfileEntry = g_ptr_array_index(fmProfileEntries, i);
		g_hash_table_insert(fmProfiles, fmProfileEntry->id, (gpointer) fmProfileEntry);
	}
	/* Set some base conditions */
	/* Environment: LXDE
	 * MimeType: text/plain
	 */

	/* Validate profiles */
	valid_profiles = retrieve_valid_profiles(fmProfiles);
	g_ptr_array_foreach(valid_profiles, printprofile, NULL);
	/* Validate actions */

	/* Show menu */

	return 0;
}

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles)
{
	GPtrArray *profile_array = g_ptr_array_new();
	g_hash_table_foreach(fmProfiles, (GHFunc) validate_profile, profile_array);

	return profile_array;
}

void printprofile(gpointer data, gpointer user_data)
{
	FmProfileEntry *pe = (FmProfileEntry *)data;
	printf("%s is a valid profile\n", pe->id);
}

void validate_profile(gpointer key, gpointer value, gpointer profile_array)
{
	//gchar *id = g_strdup(key);
	FmProfileEntry *profile = (FmProfileEntry *)value;

	FmConditions *conditions = profile->conditions;
	if(validate_conditions(conditions) == TRUE){
		g_ptr_array_add(profile_array, (gpointer) profile);
	} else
		return;
}

gboolean validate_conditions(FmConditions *conditions)
{
	gsize i;
	gboolean isValid = TRUE;
	gboolean onlyshowin = TRUE, notshowin = TRUE;
	gboolean mimetypes = FALSE;

	if(conditions->onlyshowin != NULL){
		onlyshowin = FALSE;
		for(i=0;i<conditions->n_onlyshowin;++i)
			if(g_strcmp0(conditions->onlyshowin[i], environment) == 0) onlyshowin = TRUE;
	} else if(conditions->notshowin != NULL) {
		for(i=0;i<conditions->n_notshowin;++i)
			if(g_strcmp0(conditions->notshowin[i], environment) == 0) notshowin = FALSE;
	}

	/* OnlyShowIn/NotShowIn validation */
	if(onlyshowin == FALSE || notshowin == FALSE){
		isValid = FALSE;
		return isValid;
	}

	for(i=0;i<conditions->n_mimetypes;++i)
		if(g_strcmp0(conditions->mimetypes[i], mime_type) == 0)
			mimetypes = TRUE;

	/* Mimetypes validation */
	if(mimetypes == FALSE){
		isValid = FALSE;
		return isValid;
	}

	return isValid;
}
