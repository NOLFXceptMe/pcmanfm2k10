#include<glib.h>
#include<glib/gstdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#include "parser.h"
#include "validation.h"

extern gchar *environment;
extern gsize selection_count;
extern GPtrArray *mime_types, *base_names, *capabilities_array, *schemes_array, *folder_array;
typedef struct _valid_arrays_type valid_arrays_type;
struct _valid_arrays_type {
	GPtrArray *profile_array;
	GPtrArray *action_array;
	GPtrArray *menu_array;
};

/* Iterates through the profiles hash table and returns an array of valid ones */
GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles)
{
	GPtrArray *valid_profiles_array = g_ptr_array_new();
	g_hash_table_foreach(fmProfiles, (GHFunc) validate_profile, valid_profiles_array);

	return valid_profiles_array;
}

/* Iterates through the actions hash tables, and validates them using the array of valid profiles, and returns an array of valid actions */
GPtrArray* retrieve_valid_actions(GHashTable *fmActions, GPtrArray *valid_profiles)
{
	GPtrArray *valid_actions_array = g_ptr_array_new();

	valid_arrays_type *valid_arrays_temp = g_new(valid_arrays_type, 1);
	valid_arrays_temp->profile_array = valid_profiles;
	valid_arrays_temp->action_array = valid_actions_array;
	valid_arrays_temp->menu_array = NULL;

	g_hash_table_foreach(fmActions, (GHFunc) validate_action, valid_arrays_temp);

	return valid_actions_array;
}

/* Iterates through the menus hash tables, and validates them using the array of valid actions, and returns an array of valid menus */
GPtrArray* retrieve_valid_menus(GHashTable *fmMenus, GPtrArray *valid_actions)
{
	GPtrArray *valid_menus_array = g_ptr_array_new();

	valid_arrays_type *valid_arrays_temp = g_new(valid_arrays_type, 1);
	valid_arrays_temp->profile_array = NULL;
	valid_arrays_temp->action_array = valid_actions;
	valid_arrays_temp->menu_array = valid_menus_array;

	g_hash_table_foreach(fmMenus, (GHFunc) validate_menu, valid_arrays_temp);

	return valid_menus_array;
}

/* Validate a single profile and add to the valid_profiles_array */
void validate_profile(gpointer key, gpointer value, gpointer user_data)
{
	gchar *id = g_strdup(key);
	FmProfileEntry *profile = (FmProfileEntry *)value;
	GPtrArray *valid_profiles_array = (GPtrArray *)user_data;
	FmConditions *conditions = profile->conditions;

	gboolean profile_validity = validate_conditions(conditions);

	printf("Validating profile \"%s\":\t", id);
	if(profile_validity == TRUE){
		printf("[OK]\n");
		g_ptr_array_add(valid_profiles_array, (gpointer) profile);
	} else {
		printf("[FAIL]\n");
	}
	printf("\n");
}

/* Validate a single action and add to valid_actions_array */
void validate_action(gpointer key, gpointer value, gpointer user_data)
{
	gchar *name = g_strdup(key);
	FmActionEntry *action = (FmActionEntry *)value;
	valid_arrays_type *valid_arrays = (valid_arrays_type *)user_data;
	GPtrArray *valid_profiles_array = valid_arrays->profile_array;
	GPtrArray *valid_actions_array = valid_arrays->action_array;
	gsize i, j;

	FmConditions *conditions = action->conditions;
	gboolean action_validity = validate_conditions(conditions);
	printf("Validating action \"%s\"\n", name);
	if(action_validity == TRUE){
		/* Now validate action according to available profiles */
		for(i=0;i<action->n_profiles;++i){
			printf("Trying to find profile %s in validated profiles\t", action->profiles[i]);
			for(j=0;j<valid_profiles_array->len;++j){
				if(g_strcmp0(g_strstrip(action->profiles[i]), g_strstrip(((FmProfileEntry *)g_ptr_array_index(valid_profiles_array, j))->id)) == 0){
					printf("1: [OK]\n");
					//printf("%s is a valid action\n", name);
					g_ptr_array_add(valid_actions_array, action);
					return;
				}
			}
			printf("1: [FAIL]\n");
		}
		printf("[FAIL]: No valid profile found\n");
	} else {
		printf("[FAIL]: Conditions not satisfied\n");
	}
	printf("\n");
}

/* Validate a single menu item and add to valid_menus_array */
void validate_menu(gpointer key, gpointer value, gpointer user_data)
{
	gchar *name = g_strdup(key);
	FmMenuEntry *menu = (FmMenuEntry *)value;
	valid_arrays_type *valid_arrays = (valid_arrays_type *)user_data;
	GPtrArray *valid_actions_array = valid_arrays->action_array;
	GPtrArray *valid_menus_array = valid_arrays->menu_array;
	gsize i, j;

	FmConditions *conditions = menu->conditions;
	gboolean menu_validity = validate_conditions(conditions);
	printf("Validating menu \"%s\"\n", name);
	if(menu_validity == TRUE){
		/* Validate according to available actions */
		/* TODO: Even submenus are acceptable. Should support that */
		for(i=0; i<menu->n_itemslist; ++i){
			printf("Trying to find action %s in validated actions\t", menu->itemslist[i]);
			for(j=0; j<valid_actions_array->len; ++j){
				if(g_strcmp0(g_strstrip(menu->itemslist[i]), g_strstrip(((FmActionEntry *)g_ptr_array_index(valid_actions_array, j))->name)) == 0){
					printf("1: [OK]\n");
					printf("%s is a valid action\n", name);
					g_ptr_array_add(valid_menus_array, menu);
					return;
				}
			}
			printf("1: [FAIL]\n");
		}
		printf("[FAIL]: No valid item found\n");
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

	gsize i, j;
	gboolean isValid = TRUE;
	gboolean onlyshowin = TRUE, notshowin = TRUE;
	gboolean tryexec = TRUE;
	gboolean showifregistered = TRUE, showiftrue = TRUE, showifrunning = TRUE;
	gboolean mimetypes = TRUE;
	gboolean basenames = TRUE, matchcase = TRUE;
	gboolean selectioncount = TRUE;
	gboolean schemes = TRUE, folderlist = TRUE, capabilities = TRUE;

	FILE *fp;
	gchar line[255];
	gchar *selection_count_string;
	gsize selection_count_size;

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
		printf("Failed OnlyShowIn/NotShowIn validation\n");
		return isValid;
	}

	/* TryExec validation */
	if(conditions->tryexec != NULL){
		if(!(g_file_test(conditions->tryexec, G_FILE_TEST_EXISTS) == TRUE && g_file_test(conditions->tryexec, G_FILE_TEST_IS_EXECUTABLE) == TRUE))
			tryexec = FALSE;
	}

	if(tryexec == FALSE){
		isValid = FALSE;
		printf("Failed TryExec validation\n");
		return isValid;
	}

	/* ShowIfRegistered validation */
	/* TODO: How do I access D-Bus using Glib? */

	if(showifregistered == FALSE){
		isValid = FALSE;
		printf("Failed ShowIfRegistered validation\n");
		return isValid;
	}

	/* ShowIfTrue validation */
	/* TODO: Used popen, should that be fine? */
	if(conditions->showiftrue != NULL){
		showiftrue = FALSE;
		fp = popen(conditions->showiftrue, "r");
		memset(line, 255, 0);
		while(fgets(line, 255, fp)){
			if(g_strcmp0(g_strstrip(line), "true") == 0){
				showiftrue = TRUE;
				break;
			}
		}
		pclose(fp);
	}

	if(showiftrue == FALSE){
		isValid = FALSE;
		printf("Failed ShowIfTrue validation\n");
		return isValid;
	}

	/* ShowIfRunning validation */
	/* We use popen() and pgrep here */
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
		printf("Failed ShowIfRunning validation\n");
		return isValid;
	}

	/* Mimetypes validation */

	gboolean atleast_one_match = TRUE, atleast_one_negation = FALSE;
	gchar **mime_split_i = NULL, **mime_split_j = NULL;
	gchar *content_type = NULL;

	if(conditions->n_mimetypes > 0){
		for(i=0; i<conditions->n_mimetypes; ++i){									/* Iterate on the mimetypes supported, check if it matches any of the selection */
			if(conditions->mimetypes[i][0] == '!')									/* This is the first pass, skip all negations */
				continue;

			if(g_strcmp0(conditions->mimetypes[i], "*") == 0)						/* * matches everything, no further checks */
				break;

			mime_split_i = g_strsplit(conditions->mimetypes[i], "/", 2);
			//printf("\"%s/%s\"\n", mime_split_i[0], mime_split_i[1]);				/* mime_split_i[0] is the content type, mime_split_i[1] is the subtype */

			if(g_strcmp0(mime_split_i[0], "all") == 0){								/* all/? */
				if(g_strcmp0(mime_split_i[1], "*") == 0 || g_strcmp0(mime_split_i[1], "all") == 0){			/* all/ * and all/all is the same as * */
					break;
				}

				if(g_strcmp0(mime_split_i[1], "allfiles") == 0){					/* all/allfiles */
					/* Use the fact that all files start with anything other than inode
					 * Refer fm-mime-type.c fm_mime_type_get_for_native_file */

					for(j=0; j<mime_types->len; ++j){								/* Iterate on the mimetypes of selected files and check if there is any match */
						content_type = g_strndup(g_ptr_array_index(mime_types, j), 5);			/* Extract the first 5 chars of the mimetype, strlen("inode") = 5 :-) */
						//printf("%s\n", content_type);
						if(g_strcmp0(content_type, "inode") == 0){								/* If any non file type is found, check fails */
							mimetypes = FALSE;
							break;
						}
						g_free(content_type);
					}
					break;
				}
			}

			if(g_strcmp0(mime_split_i[0], "inode") == 0){										/* inode/? */
				for(j=0;j<mime_types->len;++j){													/* every mimetype of selection must match */
					(g_strcmp0(conditions->mimetypes[i], g_ptr_array_index(mime_types, j)) != 0)?mimetypes = FALSE:0;
					break;
				}
				break;
			}

			/* everything else */
			/* mime_split_i[0] is the mime group, and mime_split_i[1] is the mime specific type */
			atleast_one_match = FALSE;
			for(j=0; j<mime_types->len; ++j){
				mime_split_j = g_strsplit(g_ptr_array_index(mime_types, j), "/", 2);

				//printf("Content Matching: %s with %s\t", mime_split_i[0], mime_split_j[0]);
				if(g_strcmp0(mime_split_i[0], mime_split_j[0]) == 0){
					//printf("SUCCESS\n");
					//printf("Subtype Matching: %s with %s\t", mime_split_i[1], mime_split_j[1]);
					if(g_strcmp0(mime_split_i[1], mime_split_j[1]) == 0 || g_strcmp0(mime_split_i[1], "*") == 0 || g_strcmp0(mime_split_i[1], "all") == 0){
						//printf("SUCCESS\n");
						/* Matched */
						atleast_one_match = TRUE;
						break;
					} else {
						//printf("FAIL\n");
					}
				} else {
					//printf("FAIL\n");
				}
			}
			if(atleast_one_match == TRUE)
				break;
		}

		if(atleast_one_match == FALSE){
			mimetypes = FALSE;
		}

		if(mimetypes == TRUE){			/* Enter second pass, where we check for negations */
			atleast_one_negation = FALSE;
			for(i=0; i<conditions->n_mimetypes; ++i){
				//printf("MimeType %d is %s\n", i, conditions->mimetypes[i]);
				if(g_strstrip(conditions->mimetypes[i])[0] != '!')
					continue;

				conditions->mimetypes[i] = conditions->mimetypes[i] + 1;
				//printf("Trying to negate with %s\n", conditions->mimetypes[i]);

				/* Note that !* and !all/ * and !all/all shouldn't be possible, but we still test that case */
				if((g_strcmp0(conditions->mimetypes[i], "*") == 0) || (g_strcmp0(conditions->mimetypes[i], "all/*") == 0) || (g_strcmp0(conditions->mimetypes[i], "all/all") == 0)){
					mimetypes = FALSE;
					break;
				}

				/* We do not parse !all/inode and !all/allfiles. They should be represented as all/allfiles or all/inode. TODO: Is this ok? */

				mime_split_i = g_strsplit(conditions->mimetypes[i], "/", 2);
				//printf("\"%s/%s\"\n", mime_split_i[0], mime_split_i[1]);				/* mime_split_i[0] is the content type, mime_split_i[1] is the subtype */
				
				/* The only cases left now are !content_type/sub_type, and !content_type/ *  */
				atleast_one_negation = FALSE;
				for(j=0; j<mime_types->len; ++j){
					mime_split_j = g_strsplit(g_ptr_array_index(mime_types, j), "/", 2);

					if(g_strcmp0(mime_split_i[0], mime_split_j[0]) == 0){				/* Content type matches */
						if((g_strcmp0(mime_split_i[1], "*") == 0)|| (g_strcmp0(mime_split_i[1], "all") == 0) || (g_strcmp0(mime_split_i[1], mime_split_j[1]) == 0)){
							atleast_one_negation = TRUE;
							break;
						}
					}
				}
				if(atleast_one_negation == TRUE)
					break;
			}
			if(atleast_one_negation == TRUE)
				mimetypes = FALSE;
		}
	}

	/* Not extra code */
	if(mimetypes == FALSE){
		isValid = FALSE;
		printf("Failed MimeTypes validation\n");
		return isValid;
	}

	/* The following two conditions, matchcase and basenames are to be validated together */
	/* Matchcase and Basenames evalutation*/
	if(conditions->n_basenames > 0){
		matchcase = conditions->matchcase;

		/* Basenames validation */
		atleast_one_match = FALSE;
		for(i=0;i<conditions->n_basenames;++i){
			//printf("validating basename \"%s\"\n", conditions->basenames[i]);
			if(g_strstrip(conditions->basenames[i])[0] == '!')							/* First pass, only deal with non-negating conditions */
				continue;

			if(g_strcmp0(conditions->basenames[i], "*") == 0)
				break;

			/* Iterate over the basenames of the selected items and validate */
			for(j=0;j<base_names->len;++j){
				//printf("Matching %s with %s\n", conditions->basenames[i], (char *)g_ptr_array_index(base_names, j));
				if(matchcase == TRUE){
					if(g_strcmp0(conditions->basenames[i], g_ptr_array_index(base_names, j)) == 0){
						atleast_one_match = TRUE;
						break;
					}
				} else {
					if(g_ascii_strcasecmp(conditions->basenames[i], g_ptr_array_index(base_names, j)) == 0){
						atleast_one_match = TRUE;
						break;
					}
				}
			}
			if(atleast_one_match == TRUE)
				break;
		}

		if(atleast_one_match == FALSE)
			basenames = FALSE;

		if(basenames == TRUE){											/* Second pass, validate negating conditions */
			atleast_one_negation = FALSE;
			for(i=0; i<conditions->n_basenames; ++i){
				if(g_strstrip(conditions->basenames[i])[0] != '!')
					continue;

				conditions->basenames[i] = conditions->basenames[i]+1;

				/* Iterate over the basenames of the selected items and validate */
				for(j=0;j<base_names->len;++j){
					//printf("Negation: Matching %s with %s\n", conditions->basenames[i], (char *)g_ptr_array_index(base_names, j));
					if(matchcase == TRUE){
						if(g_strcmp0(conditions->basenames[i], g_ptr_array_index(base_names, j)) == 0){
							atleast_one_negation = TRUE;
							break;
						}
					} else {
						if(g_ascii_strcasecmp(conditions->basenames[i], g_ptr_array_index(base_names, j)) == 0){
							atleast_one_negation = TRUE;
							break;
						}
					}
				}
				if(atleast_one_negation == TRUE)
					break;
			}
			if(atleast_one_negation == TRUE)
				basenames = FALSE;
		}
	}



	if(basenames == FALSE){
		isValid = FALSE;
		printf("Failed BaseNames validation\n");
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
		printf("Failed SelectionCount validation\n");
		return isValid;
	}

	/* Schemes validation */
	gboolean negate_scheme;
	if(conditions->n_schemes > 0){
		for(i=0; i<conditions->n_schemes; ++i){			/* Iterate on schemes */
			atleast_one_match = FALSE;
			negate_scheme = FALSE;

			if(conditions->schemes[i][0] == '!'){
				negate_scheme = TRUE;
				conditions->schemes[i] = conditions->schemes[i] + 1;
				/* Mem leak, 1 byte? */
			}
			//printf("Validating scheme %s\n", conditions->schemes[i]);
			
			/* Now iterate on the schemes selected */
			for(j=0; j<schemes_array->len; ++j){
				//printf("Matching %s with %s\n", conditions->schemes[i], (char *)g_ptr_array_index(schemes_array, j));
				if(negate_scheme == FALSE && g_strcmp0(conditions->schemes[i], g_ptr_array_index(schemes_array, j)) == 0){
					atleast_one_match = TRUE;
					break;
				}

				if(negate_scheme == TRUE && g_strcmp0(conditions->schemes[i], g_ptr_array_index(schemes_array, j)) == 0){
					break;
				}
			}
			if((negate_scheme == FALSE && atleast_one_match == TRUE) || (negate_scheme == TRUE && atleast_one_match == FALSE))
				break;
		}

		/* Done matching all pairs */
		if(atleast_one_match == FALSE)
			schemes = FALSE;
	}

	if(schemes == FALSE){
		isValid = FALSE;
		printf("Failed Schemes validation\n");
		return isValid;
	}

	/* Folderlist validation */
	gchar current_directory[1024];

	if(conditions->n_folderlist >= 0){
		getcwd(current_directory, 1024);

		for(i=0; i<conditions->n_folderlist; ++i){				/* First Pass: Iterate over the folder list, to find if our current directory lies in any of them */
			if(g_strstrip(conditions->folderlist[i])[0] == '!')
				continue;
			printf("Validating wrt folder : %s\n", conditions->folderlist[i]);

			folderlist = match_folder_pair(conditions->folderlist[i], current_directory);
			
			if(folderlist == TRUE)
				break;
		}

		if(folderlist == TRUE){
			for(i=0; i<conditions->n_folderlist; ++i){				/* Second Pass: Iterate over the folder list, to find if our current directory should not lie in any of them */
				if(g_strstrip(conditions->folderlist[i])[0] != '!')
					continue;
				conditions->folderlist[i] = conditions->folderlist[i] + 1;
				printf("Validating negation wrt folder : %s\n", conditions->folderlist[i]);

				folderlist = (match_folder_pair(conditions->folderlist[i], current_directory) == TRUE)?FALSE:TRUE;

				if(folderlist == FALSE)
					break;
			}
		}
	}

	if(folderlist == FALSE){
		isValid = FALSE;
		printf("Failed FolderList validation\n");
		return isValid;
	}

	/* Capabilities validation */
	FmCapabilities *fm_capability = NULL;
	gboolean negate_capability;
	//printf("\nHave %d capabilities to verify\n", conditions->n_capabilities);
	if(conditions->n_capabilities> 0){
		for(i=0; i<conditions->n_capabilities; ++i){			/* Iterate on capabilities */
			//printf("Capability %d: %s\n", i, conditions->capabilities[i]);
			if(capabilities == FALSE)
				break;
			negate_capability = FALSE;
			if(conditions->capabilities[i][0] == '!'){
				negate_capability = TRUE;
				conditions->capabilities[i] = conditions->capabilities[i]+1;
				/* Memory leak of 1 byte here? */
			}

			/* Owner */
			if(g_strcmp0(conditions->capabilities[i], "Owner") == 0){
				for(j=0; j<capabilities_array->len; ++j){
					fm_capability = (FmCapabilities *)g_ptr_array_index(capabilities_array, j);
					/* Using the XOR here */
					if(!((fm_capability->isOwner == TRUE) ^ (negate_capability == TRUE))){
						capabilities = FALSE;
						break;
					}
				}
				continue;
			}

			if(g_strcmp0(conditions->capabilities[i], "Readable") == 0){
				for(j=0; j<capabilities_array->len; ++j){
					fm_capability = (FmCapabilities *)g_ptr_array_index(capabilities_array, j);
					if(!((fm_capability->isReadable == TRUE) ^ (negate_capability == TRUE))){
						capabilities = FALSE;
						break;
					}
				}
				continue;
			}

			if(g_strcmp0(conditions->capabilities[i], "Writable") == 0){
				for(j=0; j<capabilities_array->len; ++j){
					fm_capability = (FmCapabilities *)g_ptr_array_index(capabilities_array, j);
					if(!((fm_capability->isWritable == TRUE) ^ (negate_capability == TRUE))){
						capabilities = FALSE;
						break;
					}
				}
				continue;
			}

			if(g_strcmp0(conditions->capabilities[i], "Executable") == 0){
				for(j=0; j<capabilities_array->len; ++j){
					fm_capability = (FmCapabilities *)g_ptr_array_index(capabilities_array, j);
					if(!((fm_capability->isExecutable == TRUE) ^ (negate_capability == TRUE))){
						capabilities = FALSE;
						break;
					}
				}
				continue;
			}

			if(g_strcmp0(conditions->capabilities[i], "Local") == 0){
				for(j=0; j<capabilities_array->len; ++j){
					fm_capability = (FmCapabilities *)g_ptr_array_index(capabilities_array, j);
					if(!(fm_capability->isLocal == TRUE) ^ (negate_capability == TRUE)){
						capabilities = FALSE;
						break;
					}
				}
				continue;
			}
		}
	}

	if(capabilities == FALSE){
		isValid = FALSE;
		printf("Failed Capabilites validation\n");
		return isValid;
	}

	return isValid;
}

gboolean match_folder_pair(gchar *folderlist_i, gchar *cwd)
{
	gchar **folder_split_i = NULL, **cwd_split = NULL;
	gboolean folderlist = TRUE;
	gsize level_i = 0, level_j = 0;

	folder_split_i = g_strsplit(folderlist_i, "/", 0);
	cwd_split = g_strsplit(cwd, "/", 0);

	while(folder_split_i[level_i] != NULL){
		if((g_strcmp0(folder_split_i[level_i], "") == 0) && folder_split_i[level_i + 1] == NULL)
			break;

		if(g_strcmp0(folder_split_i[level_i], "*") == 0){
			while(g_strcmp0(folder_split_i[level_i], "*") == 0)			/* For the case when we write /path/to/ * / * / * */
				level_i++;

			if(folder_split_i[level_i] == NULL){
				break;
			}

			while(cwd_split[level_j] != NULL){
				if(g_strcmp0(folder_split_i[level_i], cwd_split[level_j]) != 0){
					level_j++;
				} else {
					break;
				}
			}
			if(cwd_split[level_j] == NULL){
				folderlist = FALSE;
				break;
			}
		}

		if(g_strcmp0(folder_split_i[level_i], cwd_split[level_j]) != 0){
			folderlist = FALSE;
			break;
		}

		++level_i, ++level_j;
	}

	return folderlist;
}
