#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <glib.h>
#include <libfm/fm-file-info.h>

GPtrArray* substitute_parameters(gchar *input_string, FmFileInfoList *file_info_list);

#endif	/* _PARAMETER_H_ */
