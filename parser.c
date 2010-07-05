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

FmDesktopEntry* parse(gchar* file_name)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	GError *error = NULL;
	gsize n_groups;
	gchar **group_names;
	gsize i;
	gchar *type_string;
	gchar *profile_id;
	guint type;
	FmDesktopEntry *fmDesktopEntry = NULL;
	FmActionEntry *fmActionEntry;
	FmProfileEntry *fmProfileEntry;
	FmMenuEntry *fmMenuEntry;

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS;

	if(!g_key_file_load_from_file(keyfile, file_name, flags, &error)){
		fprintf(stderr, "%s\n", error->message);
		return NULL;
	}

	/* Initialize desktop entry structure */
	fmDesktopEntry = g_slice_new0(FmDesktopEntry);
	fmDesktopEntry->desktop_file_id = g_strndup(file_name, strlen(file_name) - strlen(strrchr(file_name, '.')));
	fmDesktopEntry->n_action_entries = 0;
	fmDesktopEntry->n_profile_entries = 0;
	fmDesktopEntry->n_menu_entries = 0;
	fmDesktopEntry->fmActionEntries = g_ptr_array_new();
	fmDesktopEntry->fmProfileEntries = g_ptr_array_new();
	fmDesktopEntry->fmMenuEntries = g_ptr_array_new();

#ifndef NDEBUG
	//printf("Extracted file_id is %s\n", fmDesktopEntry->desktop_file_id);
#endif
	
	group_names = g_key_file_get_groups(keyfile, &n_groups);

	/* Store action entries, profile entries, and menu entries in respective data structures */
	for(i=0;i<n_groups;++i){
#ifndef NDEBUG
		//printf("Name of the group is %s\n", group_names[i]);
#endif

		/* Action entry or Menu entry
		 * Also have one issue to be clarified on XDG mailing list
		 * - Keys and values are mentioned to be case sensitive
		 * - Nothing as such is given for group headers. So, is "desktop Entry", the same as "Desktop Entry"?
		 * For now assuming that group headers are case insensitive, hence using g_ascii_strcasecmp().  If case sensitive, shift to g_strcmp0() */
		if(g_ascii_strcasecmp(group_names[i], "Desktop Entry") == 0){
			type_string = g_key_file_get_string(keyfile, group_names[i], "Type", NULL);

			if(type_string == NULL || g_strcmp0(type_string, "Action") == 0){
				type = ACTION_ENTRY;
				fmActionEntry = parse_action_entry(keyfile, group_names[i]);
				if(fmActionEntry != NULL){
					fmDesktopEntry->n_action_entries++;
					g_ptr_array_add(fmDesktopEntry->fmActionEntries, (gpointer) fmActionEntry);
				}
			}
			
			else if(g_strcmp0(type_string, "Menu") == 0){
				type = MENU_ENTRY;
				fmMenuEntry = parse_menu_entry(keyfile, group_names[i]);
				if(fmMenuEntry != NULL){
					fmDesktopEntry->n_menu_entries++;
					g_ptr_array_add(fmDesktopEntry->fmMenuEntries, (gpointer) fmMenuEntry);
				}
			}
			
			else if(g_strcmp0(type_string, "Application") == 0){
				type = APPLICATION_ENTRY;
			}
			
			else if(g_strcmp0(type_string, "Link") == 0){
				type = LINK_ENTRY;
			}

			else if(g_strcmp0(type_string, "Directory") == 0){
				type = DIRECTORY_ENTRY;
			}
			
			else {
				type = UNKNOWN_ENTRY;
			}
		}
		
		else if(g_str_has_prefix(group_names[i], "X-Action-Profile ") == TRUE){
			type = PROFILE_ENTRY;
			profile_id = g_strdup(group_names[i]+ 17);		/* "X-Action-Profile " is 17 characters long */
			fmProfileEntry = parse_profile_entry(keyfile, group_names[i]);
			if(fmProfileEntry != NULL){
				fmProfileEntry->id = g_strdup(profile_id);
				fmDesktopEntry->n_profile_entries++;
				g_ptr_array_add(fmDesktopEntry->fmProfileEntries, (gpointer) fmProfileEntry);
			}
		} 
		
		else {
			type = UNKNOWN_ENTRY;
		}
		
#ifndef NDEBUG
		/*
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
		*/
#endif
	}

	return fmDesktopEntry;
}

FmActionEntry* parse_action_entry(GKeyFile *keyfile, gchar *group_name)
{
	FmActionEntry *ae = g_slice_new0(FmActionEntry);
	GError *error = NULL;

	/* TODO: We have a problem here with parsing locale strings */
	ae->name = g_key_file_get_locale_string(keyfile, group_name, "Name", "en_US.UTF-8", &error);
	if(ae->name == NULL){
#ifndef NDEBUG
		fprintf(stderr, "%s\n", error->message);
#endif
		return NULL;
	}
	ae->type = g_key_file_get_string(keyfile, group_name, "Type", NULL);
	if(ae->type == NULL){
		ae->type = "Action";
	}
	ae->tooltip = g_key_file_get_locale_string(keyfile, group_name, "Tooltip", NULL, NULL);
	ae->icon = g_key_file_get_locale_string(keyfile, group_name, "Icon", NULL, NULL);
	ae->description = g_key_file_get_locale_string(keyfile, group_name, "Description", NULL, NULL);
	ae->suggestedshortcut = g_key_file_get_locale_string(keyfile, group_name, "SuggestedShortcut", NULL, NULL);
	ae->enabled = g_key_file_get_boolean(keyfile, group_name, "Enabled", NULL);
	ae->hidden = g_key_file_get_boolean(keyfile, group_name, "Hidden", NULL);
	ae->targetcontext = g_key_file_get_boolean(keyfile, group_name, "TargetContext", NULL);
	ae->targetlocation = g_key_file_get_boolean(keyfile, group_name, "TargetLocation", NULL);
	ae->targettoolbar = g_key_file_get_boolean(keyfile, group_name, "TargetToolbar", NULL);
	ae->toolbarlabel = g_key_file_get_locale_string(keyfile, group_name, "ToolbarLabel", NULL, NULL);
	ae->profiles = g_key_file_get_string_list(keyfile, group_name, "Profiles", &ae->n_profiles, NULL);

	/* Parse the conditions associated with this entry */
	ae->conditions = parse_conditions(keyfile, group_name);

	return ae;
}

FmProfileEntry* parse_profile_entry(GKeyFile *keyfile, gchar *group_name)
{
	FmProfileEntry *pe = g_slice_new0(FmProfileEntry);
	GError *error;

	pe->exec = g_key_file_get_string(keyfile, group_name, "Exec", &error);
	if(pe->exec == NULL){
#ifndef NDEBUG
		fprintf(stderr, "%s\n", error->message);
#endif
		return NULL;
	}
	pe->name = g_key_file_get_locale_string(keyfile, group_name, "Name", NULL, NULL);
	pe->path = g_key_file_get_string(keyfile, group_name, "Path", NULL);
	pe->executionmode = g_key_file_get_string(keyfile, group_name, "ExecutionMode", NULL);
	pe->startupnotify = g_key_file_get_boolean(keyfile, group_name, "StartupNotify", NULL);
	pe->startupwmclass = g_key_file_get_string(keyfile, group_name, "StartupWMClass", NULL);
	pe->executeas = g_key_file_get_string(keyfile, group_name, "ExecuteAs", NULL);
	pe->conditions = parse_conditions(keyfile, group_name);

	return pe;
}

FmMenuEntry* parse_menu_entry(GKeyFile *keyfile, gchar *group_name)
{
	FmMenuEntry *me = g_slice_new0(FmMenuEntry);
	GError *error;

	me->type = g_key_file_get_string(keyfile, group_name, "Type", &error);
	if(me->type == NULL || g_ascii_strcasecmp(me->type, "Menu") != 0){
#ifndef NDEBUG
		fprintf(stderr, "%s\n", error->message);
#endif
		return NULL;
	}

	me->name = g_key_file_get_locale_string(keyfile, group_name, "Name", NULL, NULL);
	me->tooltip = g_key_file_get_locale_string(keyfile, group_name, "Tooltip", NULL, NULL);
	me->icon = g_key_file_get_locale_string(keyfile, group_name, "Icon", NULL, NULL);
	me->description = g_key_file_get_locale_string(keyfile, group_name, "Description", NULL, NULL);
	me->suggestedshortcut = g_key_file_get_string(keyfile, group_name, "SuggestedShortcut", NULL);
	me->enabled = g_key_file_get_boolean(keyfile, group_name, "Enabled", NULL);
	me->hidden = g_key_file_get_boolean(keyfile, group_name, "Hidden", NULL);
	me->itemslist = g_key_file_get_string_list(keyfile, group_name, "ItemsList", &me->n_itemslist, NULL);
	me->conditions = parse_conditions(keyfile, group_name);
	return me;
}

FmConditions* parse_conditions(GKeyFile *keyfile, gchar *group_name)
{
	FmConditions *conditions = g_slice_new0(FmConditions);
	GError *error = NULL;

	conditions->onlyshowin = g_key_file_get_string_list(keyfile, group_name, "OnlyShowIn", &conditions->n_onlyshowin,  NULL);
	conditions->notshowin = g_key_file_get_string_list(keyfile, group_name, "NotShowIn", &conditions->n_notshowin, NULL);
	conditions->tryexec = g_key_file_get_string(keyfile, group_name, "TryExec", NULL);
	conditions->showifregistered = g_key_file_get_string(keyfile, group_name, "ShowIfRegistered", NULL);
	conditions->showiftrue = g_key_file_get_string(keyfile, group_name, "ShowIfTrue", NULL);
	conditions->showifrunning = g_key_file_get_string(keyfile, group_name, "ShowIfRunning", NULL);
	conditions->mimetypes = g_key_file_get_string_list(keyfile, group_name, "MimeTypes", &conditions->n_mimetypes, NULL);
	conditions->basenames = g_key_file_get_string_list(keyfile, group_name, "Basenames", &conditions->n_basenames, NULL);
	conditions->matchcase = g_key_file_get_boolean(keyfile, group_name, "Matchcase", &error);
	if(error){
		if(error->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND|| error->code == G_KEY_FILE_ERROR_INVALID_VALUE){
			conditions->matchcase = TRUE;
			g_error_free(error);
			error = NULL;
		}
	}

	conditions->selectioncount = g_key_file_get_string(keyfile, group_name, "SelectionCount", NULL);
	conditions->schemes = g_key_file_get_string_list(keyfile, group_name, "Schemes", &conditions->n_schemes, NULL);
	conditions->folderlist = g_key_file_get_string_list(keyfile, group_name, "Folders", &conditions->n_folderlist, NULL);
	conditions->capabilities = g_key_file_get_string_list(keyfile, group_name, "Capabilities", &conditions->n_capabilities, NULL);

	return conditions;
}
