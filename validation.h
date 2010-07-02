#ifndef _VALIDATION_H_
#define _VALIDATION_H_

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles);
GPtrArray* retrieve_valid_actions(GHashTable *fmActions, GPtrArray *valid_profiles);
void validate_profile(gpointer key, gpointer value, gpointer profile_array);
void validate_action(gpointer key, gpointer value, gpointer action_array);
gboolean validate_conditions(FmConditions *conditions);

#endif	/* _VALIDATION_H_ */
