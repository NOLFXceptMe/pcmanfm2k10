#ifndef _SUBSTITUTION_H_
#define _SUBSTITUTION_H_

void substitute_profile_params(gpointer key, gpointer value, gpointer user_data);
void substitute_action_params(gpointer key, gpointer value, gpointer user_data);
void substitute_menu_params(gpointer key, gpointer value, gpointer user_data);
void substitute_condition_params(FmConditions *conditions, FmFileInfoList *file_info_list);

#endif	/* _SUBSTITUTION_H_ */
