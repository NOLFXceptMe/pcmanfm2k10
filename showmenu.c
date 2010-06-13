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
GPtrArray* retrieve_valid_actions(GHashTable *fmActions);
void validate_profile(gpointer key, gpointer value, gpointer profile_array);
void validate_action(gpointer key, gpointer value, gpointer action_array);
gboolean validate_conditions(FmConditions *conditions);

gchar *environment = "LXDE";
gchar *mime_type = "text/plain";
GPtrArray *valid_profiles, *valid_actions;
gsize n_valid_profiles = 0, n_valid_actions = 0;

int main(int argc, char *argv[])
{
	gsize i/*, j*/;
	FmProfileEntry *fmProfileEntry;
	FmActionEntry *fmActionEntry;
	GPtrArray *fmProfileEntries, *fmActionEntries;
	GHashTable *fmProfiles, *fmActions;

	FmDesktopEntry *desktop_entry = parse(argv[1]);

	/* Extract data */
	/*
	printf("Desktop file_id = %s\n", desktop_entry->desktop_file_id);
	printf("Number of action definitions = %u\n", desktop_entry->n_action_entries);
	printf("Number of profile definitions = %u\n", desktop_entry->n_profile_entries);
	printf("Number of menu definitions = %u\n", desktop_entry->n_menu_entries);
	*/

	fmProfileEntries = desktop_entry->fmProfileEntries;
	fmActionEntries = desktop_entry->fmActionEntries;

	fmProfiles = g_hash_table_new(NULL, NULL);
	fmActions = g_hash_table_new(NULL, NULL);

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
	 * MimeType: text/plain
	 */

	/* Validate profiles */
	valid_profiles = retrieve_valid_profiles(fmProfiles);
	for(i=0;i<n_valid_profiles;++i){
		fmProfileEntry = g_ptr_array_index(valid_profiles, i);
		printf("%s is a valid profile\n", fmProfileEntry->id);
	}

	/* Validate actions */
	valid_actions = retrieve_valid_actions(fmActions);
	for(i=0;i<n_valid_actions;++i){
		fmActionEntry = g_ptr_array_index(valid_actions, i);
		printf("%s is a valid action\n", fmActionEntry->name);
	}

	/* Validate menus */

	/* Show context menu */
	/* Context menu will show valid actions and menus */

	return 0;
}

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles)
{
	GPtrArray *profile_array = g_ptr_array_new();
	g_hash_table_foreach(fmProfiles, (GHFunc) validate_profile, profile_array);

	return profile_array;
}

GPtrArray* retrieve_valid_actions(GHashTable *fmActions)
{
	GPtrArray *action_array = g_ptr_array_new();
	g_hash_table_foreach(fmActions, (GHFunc) validate_action, action_array);

	return action_array;
}

void validate_profile(gpointer key, gpointer value, gpointer profile_array)
{
	gchar *id = g_strdup(key);
	FmProfileEntry *profile = (FmProfileEntry *)value;

	printf("Validating profile %s:\t", id);
	FmConditions *conditions = profile->conditions;
	if(validate_conditions(conditions) == TRUE){
		printf("[OK]\n");
		g_ptr_array_add(profile_array, (gpointer) profile);
		n_valid_profiles++;
	} else {
		printf("[FAIL]\n");
	}
}

void validate_action(gpointer key, gpointer value, gpointer action_array)
{
	gchar *name = g_strdup(key);
	FmActionEntry *action = (FmActionEntry *)value;
	gsize i, j;

	printf("Validating action %s:\t\n", name);
	FmConditions *conditions = action->conditions;
	if(validate_conditions(conditions) == TRUE){
		/* Now validate action according to available profiles */
		for(i=0;i<action->n_profiles;++i){
			printf("Tyring to find profile %s in validated profiles\t", action->profiles[i]);
			for(j=0;j<n_valid_profiles;++j){
				if(g_strcmp0(g_strstrip(action->profiles[i]), g_strstrip(((FmProfileEntry *)g_ptr_array_index(valid_profiles, j))->id)) == 0){
					printf("[OK]\n");
					printf("%s is a valid action\n", name);
					g_ptr_array_add(action_array, action);
					return;
				} else {
					printf("[FAIL]\n");
				}
			}
		}
		printf("[FAIL]: No valid profile found\n");
	} else {
		printf("[FAIL]: Conditions not satisfied\n");
	}
}

gboolean validate_conditions(FmConditions *conditions)
{
	gsize i;
	gboolean isValid = TRUE;
	gboolean onlyshowin = TRUE, notshowin = TRUE;
	gboolean mimetypes = FALSE;

	/* OnlyShowIn/NotShowIn validation */
	if(conditions->onlyshowin != NULL){
		onlyshowin = FALSE;
		for(i=0;i<conditions->n_onlyshowin;++i)
			if(g_strcmp0(conditions->onlyshowin[i], environment) == 0) onlyshowin = TRUE;
	} else if(conditions->notshowin != NULL) {
		for(i=0;i<conditions->n_notshowin;++i)
			if(g_strcmp0(conditions->notshowin[i], environment) == 0) notshowin = FALSE;
	}

	if(onlyshowin == FALSE || notshowin == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* Mimetypes validation */
	if(conditions->n_mimetypes > 0){
		for(i=0;i<conditions->n_mimetypes;++i)
			if(g_strcmp0(conditions->mimetypes[i], mime_type) == 0)
				mimetypes = TRUE;
	} else {
		mimetypes = TRUE;
	}

	if(mimetypes == FALSE){
		isValid = FALSE;
		return isValid;
	}

	return isValid;
}
