#ifndef _PARSER_H_
#define _PARSER_H_

#include<string.h>
#include<glib.h>

#define UNKNOWN_ENTRY	1
#define ACTION_ENTRY	1<<1
#define PROFILE_ENTRY	1<<2
#define MENU_ENTRY		1<<3

typedef struct _FmActionEntry FmActionEntry;
typedef struct _FmProfileEntry FmProfileEntry;
typedef struct _FmMenuEntry FmMenuEntry;
typedef struct _FmConditions FmConditions;
typedef struct _FmDesktopEntry FmDesktopEntry;

struct _FmActionEntry {
	gchar *type;			/* string, not reqd */
	gchar *name; 			/* localestring, reqd */
	gchar *tooltip;			/* localestring, not reqd */
	gchar *icon;			/* localestring, not reqd */
	gchar *description;		/* localestring, not reqd */
	gchar *suggestedshortcut; /* string, not reqd */
	gboolean enabled;		/* boolean, not reqd */
	gboolean hidden;		/* boolean, not reqd */
	gboolean targetcontext;	/* boolean, not reqd */
	gboolean targetlocation;/* boolean, not reqd */
	gboolean targettoolbar;	/* boolean, not reqd */
	gchar *toolbarlabel; 	/* localestring, not reqd */
	gsize n_profiles;
	gchar **profiles;		/* strings list, reqd */

	FmConditions *conditions;
};

struct _FmProfileEntry {
	gchar *id;
	gchar *name;			/* localestring, not reqd */
	gchar *exec;			/* string, reqd */
	gchar *path;			/* string, not reqd */
	gchar *executionmode;	/* string, not reqd */
	gboolean startupnotify; /* boolean, not reqd */
	gchar *startupwmclass;	/* string, not reqd */
	gchar *executeas;		/* string, not reqd */

	FmConditions *conditions;
};

struct _FmMenuEntry {
	gchar *type;			/* string, reqd */
	gchar *name; 			/* localestring, reqd */
	gchar *tooltip;			/* localestring, not reqd */
	gchar *icon;			/* localestring, not reqd */
	gchar *description;		/* localestring, not reqd */
	gchar *suggestedshortcut; /* string, not reqd */
	gboolean enabled;		/* boolean, not reqd */
	gboolean hidden;		/* boolean, not reqd */
	gsize n_itemslist;
	gchar **itemslist;		/* strings list, reqd */

	FmConditions *conditions;
};

struct _FmConditions {
	gsize n_onlyshowin, n_notshowin;
	gchar **onlyshowin, **notshowin;	/* strings list, not reqd */
	gchar *tryexec;						/* string, not reqd */	
	gchar *showifregistered;			/* string, not reqd */
	gchar *showiftrue;					/* string, not reqd */
	gchar *showifrunning;				/* string, not reqd */
	gsize n_mimetypes, n_basenames;
	gchar **mimetypes;					/* strings list, not reqd */
	gchar **basenames;					/* strings list, not reqd */
	gboolean matchcase;					/* boolean, not reqd */
	gchar *selectioncount;				/* string, not reqd */
	gsize n_schemes, n_folderlist, n_capabilities;
	gchar **schemes;					/* strings list, not reqd */
	gchar **folderlist;					/* strings list, not reqd */
	gchar **capabilities;				/* strings list, not reqd */
};

struct _FmDesktopEntry {
	gchar *desktop_file_id;
	gsize n_action_entries, n_profile_entries, n_menu_entries;
	GPtrArray *fmActionEntries, *fmProfileEntries, *fmMenuEntries;
};

FmDesktopEntry* parse(gchar *fileName);
FmActionEntry* parse_action_entry(GKeyFile *, gchar *);
FmProfileEntry* parse_profile_entry(GKeyFile *, gchar *);
FmMenuEntry* parse_menu_entry(GKeyFile *, gchar *);
FmConditions* parse_conditions(GKeyFile *, gchar *);

#endif
