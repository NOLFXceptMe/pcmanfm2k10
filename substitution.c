#include <glib.h>
#include <libfm/fm-file-info.h>

#include "parser.h"
#include "parameter.h"
#include "substitution.h"

void substitute_profile_params(gpointer key, gpointer value, gpointer user_data)
{
	//gchar *profile_id = g_strdup(key);
	FmProfileEntry *profile = (FmProfileEntry *)value;
	FmFileInfoList *file_info_list = (FmFileInfoList *)user_data;
	GPtrArray *exec_sub = NULL, *path_sub = NULL;
	
	/* Start substitution */
	if(profile->exec != NULL){
		exec_sub = substitute_parameters(profile->exec, file_info_list);
		profile->exec_list = exec_sub;
	}

	if(profile->path != NULL){
		path_sub = substitute_parameters(profile->path, file_info_list);

		g_free(profile->path);
		profile->path = g_strdup(((GString *)g_ptr_array_index(path_sub, 0))->str);
		g_ptr_array_free(path_sub, TRUE);
	}

	substitute_condition_params(profile->conditions, file_info_list);
}

void substitute_action_params(gpointer key, gpointer value, gpointer user_data)
{
	//gchar *action_id = g_strdup(key);
	gsize i;
	FmActionEntry *action = (FmActionEntry *)value;
	FmFileInfoList *file_info_list = (FmFileInfoList *)user_data;
	GPtrArray *name_sub = NULL, *tooltip_sub = NULL, *icon_sub = NULL, *toolbarlabel_sub = NULL, *profiles_sub = NULL;

	if(action->name != NULL){
		name_sub = substitute_parameters(action->name, file_info_list);

		g_free(action->name);
		action->name = g_strdup(((GString *)g_ptr_array_index(name_sub, 0))->str);
		g_ptr_array_free(name_sub, TRUE);
	}

	if(action->tooltip != NULL){
		tooltip_sub = substitute_parameters(action->tooltip, file_info_list);
		g_free(action->tooltip);
		action->tooltip = g_strdup(((GString *)g_ptr_array_index(tooltip_sub, 0))->str);
		g_ptr_array_free(tooltip_sub, TRUE);
	}

	if(action->icon != NULL){
		icon_sub = substitute_parameters(action->icon, file_info_list);
		g_free(action->icon);
		action->icon = g_strdup(((GString *)g_ptr_array_index(icon_sub, 0))->str);
		g_ptr_array_free(icon_sub, TRUE);
	}

	if(action->toolbarlabel != NULL){
		toolbarlabel_sub = substitute_parameters(action->toolbarlabel, file_info_list);
		g_free(action->toolbarlabel);
		action->toolbarlabel = g_strdup(((GString *)g_ptr_array_index(toolbarlabel_sub, 0))->str);
		g_ptr_array_free(toolbarlabel_sub, TRUE);
	}

	for(i=0; i<action->n_profiles; ++i){
		//printf("Expanding %s ", action->profiles[i]);
		profiles_sub = substitute_parameters(action->profiles[i], file_info_list);
		g_free(action->profiles[i]);
		action->profiles[i] = g_strdup(((GString *)g_ptr_array_index(profiles_sub, 0))->str);
		//printf("to %s\n", action->profiles[i]);
		g_ptr_array_free(profiles_sub, TRUE);
	}

	substitute_condition_params(action->conditions, file_info_list);
}

void substitute_menu_params(gpointer key, gpointer value, gpointer user_data)
{
	//gchar *menu_id = g_strdup(key);
	gsize i; FmMenuEntry *menu = (FmMenuEntry *)value;
	FmFileInfoList *file_info_list = (FmFileInfoList *)user_data;
	GPtrArray *name_sub = NULL, *tooltip_sub = NULL, *icon_sub = NULL, **itemslist_sub = g_new(GPtrArray *, menu->n_itemslist);

	if(menu->name != NULL){
		name_sub = substitute_parameters(menu->name, file_info_list);
		g_free(menu->name);
		menu->name = g_strdup(((GString *)g_ptr_array_index(name_sub, 0))->str);
		g_ptr_array_free(name_sub, TRUE);
	}

	if(menu->tooltip != NULL){
		tooltip_sub = substitute_parameters(menu->tooltip, file_info_list);
		g_free(menu->tooltip);
		menu->tooltip = g_strdup(((GString *)g_ptr_array_index(tooltip_sub, 0))->str);
		g_ptr_array_free(tooltip_sub, TRUE);
	}

	if(menu->icon != NULL){
		icon_sub = substitute_parameters(menu->icon, file_info_list);
		g_free(menu->icon);
		menu->icon = g_strdup(((GString *)g_ptr_array_index(icon_sub, 0))->str);
		g_ptr_array_free(icon_sub, TRUE);
	}

	for(i=0; i<menu->n_itemslist; ++i){
		itemslist_sub[i] = substitute_parameters(menu->itemslist[i], file_info_list);
		g_free(menu->itemslist[i]);
		menu->itemslist[i] = g_strdup(((GString *)g_ptr_array_index(itemslist_sub[i], 0))->str);
		g_ptr_array_free(itemslist_sub[i], TRUE);
	}

	substitute_condition_params(menu->conditions, file_info_list);
}

void substitute_condition_params(FmConditions *conditions, FmFileInfoList *file_info_list)
{
	conditions->tryexec_list = (conditions->tryexec)?substitute_parameters(conditions->tryexec, file_info_list):NULL;
	conditions->reg_list = (conditions->showifregistered)?substitute_parameters(conditions->showifregistered, file_info_list):NULL;
	conditions->true_list = (conditions->showiftrue)?substitute_parameters(conditions->showiftrue, file_info_list):NULL;
	conditions->running_list = (conditions->showifrunning)?substitute_parameters(conditions->showifrunning, file_info_list):NULL;
}
