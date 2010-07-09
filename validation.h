#ifndef _VALIDATION_H_
#define _VALIDATION_H_

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles);
GPtrArray* retrieve_valid_actions(GHashTable *fmActions, GPtrArray *valid_profiles);
GPtrArray* retrieve_valid_menus(GHashTable *fmMenus, GPtrArray* valid_actions);
void validate_profile(gpointer key, gpointer value, gpointer profile_array);
void validate_action(gpointer key, gpointer value, gpointer user_data);
void validate_menu(gpointer key, gpointer value, gpointer user_data);
gboolean validate_conditions(FmConditions *conditions);

gboolean match_folder_pair(gchar *, gchar *);
gchar* prepare_for_regex_matching(gchar *, gchar *, gchar *);

typedef enum _FmCapabilities FmCapabilities;

enum _FmCapabilities {
	FM_CAP_OWNER = 1<<0,
	FM_CAP_READABLE = 1<<1,
	FM_CAP_WRITABLE = 1<<2,
	FM_CAP_EXECUTABLE = 1<<3,
	FM_CAP_LOCAL = 1<<4
};

#endif	/* _VALIDATION_H_ */
