#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <glib.h>
#include <libfm/fm-file-info.h>

gchar *get_host_name(FmFileInfo *file_info);
gchar *get_user_name(FmFileInfo *file_info);
gchar *get_port(FmFileInfo *file_info);
gchar *get_scheme(FmFileInfo *file_info);

GPtrArray* substitute_parameters(gchar *input_string, FmFileInfoList *file_info_list);

#endif	/* _PARAMETER_H_ */
