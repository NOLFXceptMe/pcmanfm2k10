#ifndef _VALIDATION_H_
#define _VALIDATION_H_

GPtrArray* retrieve_valid_profiles(GHashTable *fmProfiles);
GPtrArray* retrieve_valid_actions(GHashTable *fmActions, GPtrArray *valid_profiles);
void validate_profile(gpointer key, gpointer value, gpointer profile_array);
void validate_action(gpointer key, gpointer value, gpointer action_array);
gboolean validate_conditions(FmConditions *conditions);

typedef struct _FmCapabilities FmCapabilities;

struct _FmCapabilities {
	gboolean isOwner;
	gboolean isReadable;
	gboolean isWritable;
	gboolean isExecutable;
	gboolean isLocal;
};

#endif	/* _VALIDATION_H_ */
