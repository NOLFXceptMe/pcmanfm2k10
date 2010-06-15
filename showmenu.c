/* AUTHOR:				Naveen Kumar Molleti
 * EMAIL:				nerd.naveen@gmail.com
 * GIT Read-only URL:	git://github.com/NOLFXceptMe/pcmanfm2k10.git
 *
 * Test code, to simulate a rightclick for the sake of displaying a menu. Instead of showing up a graphical menu, this will write to stdout
 */

#include<glib.h>
#include<glib/gstdio.h>
#include<stdio.h>
#include<features.h>
#include<stdlib.h>

#include "parser.h"

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles);
GPtrArray* retrieve_valid_actions(GHashTable *fmActions);
void validate_profile(gpointer key, gpointer value, gpointer profile_array);
void validate_action(gpointer key, gpointer value, gpointer action_array);
gboolean validate_conditions(FmConditions *conditions);

gchar *environment = "LXDE";
gchar *mime_type = "text/plain";
gsize n_base_names = 2;
gchar *base_names[] = { "h", "c" };
gsize selection_count = 3;
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
	/* Please note notation
	 * conditions->fieldname is of type as defined as in parser.h
	 * fieldname is a gboolean to be used in this function
	 * field_name is a global variable in this file to be used as the "environment" setup for validating conditions
	 * TODO: Find a better notation :)
	 */

	gsize i;
	gboolean isValid = TRUE;
	gboolean onlyshowin = TRUE, notshowin = TRUE;
	gboolean tryexec = TRUE;
	gboolean showifregistered = TRUE, showiftrue = TRUE, showifrunning = TRUE;
	gboolean mimetypes = FALSE;
	gboolean basenames = TRUE, matchcase = TRUE;
	gboolean selectioncount = TRUE;
	gboolean schemes = TRUE, folderlist = TRUE, capabilities = TRUE;

	FILE *fp;
	gchar line[255];
	gchar *selection_count_string;
	gsize selection_count_size;
	//gchar *base_name_string;


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

	/* TryExec validation */
	if(conditions->tryexec != NULL){
		if(!(g_file_test(conditions->tryexec, G_FILE_TEST_EXISTS) == TRUE && g_file_test(conditions->tryexec, G_FILE_TEST_IS_EXECUTABLE) == TRUE))
			tryexec = FALSE;
	}

	if(tryexec == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* ShowIfRegistered validation */
	/* TODO: How do I access D-Bus using Glib? */

	if(showifregistered == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* ShowIfTrue validation */
	/* TODO: Used popen, should that be fine? */
	if(conditions->showiftrue != NULL){
		printf("Here 1");
		fp = popen(conditions->showiftrue, "r");
		memset(line, 255, 0);
		while(fgets(line, 255, fp)){
			if(g_strcmp0(g_strstrip(line), "true") != 0){
				showiftrue = FALSE;
				break;
			}
		}
		pclose(fp);
	}

	if(showiftrue == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* ShowIfRunning validation */
	/* We use popen() and pgrep here */
	printf("Here 2");
	if(conditions->showifrunning != NULL){
		fp = popen(g_strconcat("pgrep ", conditions->showifrunning, NULL), "r");
		memset(line, 255, 0);
		fgets(line, 255, fp);
		if(g_strcmp0(g_strstrip(line), "") == 0){
			showifrunning = FALSE;
		}
	}
	
	if(showifrunning == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* Mimetypes validation */
	if(conditions->n_mimetypes > 0){
		for(i=0;i<conditions->n_mimetypes;++i)
			if(g_strcmp0(conditions->mimetypes[i], mime_type) == 0)
				mimetypes = TRUE;
	} else {						/* No mimetypes specified, defaults to *, so valid */
		mimetypes = TRUE;
	}

	if(mimetypes == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* The following two conditions, matchcase and basenames are to be validated together */
	/* MatchCase evalutation*/
	if(conditions->n_basenames > 0){
		matchcase = conditions->matchcase;

		/* Basenames validation */
		/*
		for(i=0;i<conditions->n_basenames;++i){
			base_name_string = g_strstrip(conditions->basenames[i]);
			for(j=0;j<n_base_names;++j){
				if(matchcase == FALSE){
					if(base_name_string[0] == '!' && g_ascii_strcasecmp(base_name_string+1, base_names[j]))
				}
			}
		}
		*/
	}

	if(basenames == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* Selection count validation */
	if(conditions->selectioncount == NULL){
		if(selection_count == 0)
			selectioncount = FALSE;
		else
			selectioncount = TRUE;
	} else {
		/* Parse the selection count string */
		selection_count_string = g_strstrip(g_strdup(conditions->selectioncount));
		selection_count_size = atoi(selection_count_string + 1);
		switch(selection_count_string[0]){
			case '<':
				if(!(selection_count < selection_count_size))
					selectioncount = FALSE;
				break;
			case '=':
				if(!(selection_count == selection_count_size))
					selectioncount = FALSE;
				break;
			case '>':
				if(!(selection_count > selection_count_size))
					selectioncount = FALSE;
				break;
			default:
				selectioncount = TRUE;
		}
	}

	if(selectioncount == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* Schemes validation */
	if(conditions->n_schemes != 0){
	}

	if(schemes == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* Folderlist validation */
	if(conditions->n_folderlist != 0){
	}

	if(folderlist == FALSE){
		isValid = FALSE;
		return isValid;
	}

	/* Capabilities validation */
	if(conditions->n_capabilities!= 0){
	}

	if(capabilities == FALSE){
		isValid = FALSE;
		return isValid;
	}

	return isValid;
}
