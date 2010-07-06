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

typedef struct _FmCapabilities FmCapabilities;

struct _FmCapabilities {
	gboolean isOwner;
	gboolean isReadable;
	gboolean isWritable;
	gboolean isExecutable;
	gboolean isLocal;
};

#endif	/* _VALIDATION_H_ */
