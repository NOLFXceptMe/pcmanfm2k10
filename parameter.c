/* Input: A string with parameters
 * Output: A string list with parameters substituted 
 * */

/* TODO: Parsing URIs is a pain. Especially for host name, and port. A better way to do is split according to the grammar of RFC 3986. Later */

#include <glib.h>
#include <string.h>

#include <libfm/fm-file-info.h>

#include "parameter.h"

/* NOTE: Sticking to natural behavior of returning a list with the unchanged input_string, incase nothing can be substituted, however, this is expensive. A better way would be to return NULL incase of no changes, and let the caller handle the NULL case, which implies that there is no change in the input_string */
GPtrArray* substitute_parameters(gchar *input_string, FmFileInfoList *file_info_list)
{
	//printf("Input string is %s\n", input_string);
	GPtrArray *out_string_array = g_ptr_array_new();
	if(strchr(input_string, '%') == NULL){
		//printf("Found nothing to expand. Returning input_string.\n");
		g_ptr_array_add(out_string_array, g_string_new(input_string));
		return out_string_array;
	}

	gsize i, j;
	gsize len_file_list = fm_list_get_length(file_info_list);
	GString *out_string = g_string_new(NULL);
	gsize first_pos = strcspn(input_string, "%");
	gchar *pos = input_string + first_pos;
	g_string_append_len(out_string, input_string, first_pos);
	GString *g_string_i = NULL;
	gchar *base_name = NULL, *file_name = NULL, *host_name = NULL, *user_name = NULL, *port = NULL, *scheme = NULL, *uri = NULL, *file_name_wo_ext = NULL, *ext_pos = NULL;
	gboolean array_is_init = FALSE;
	FmFileInfo *file_info_i = NULL, *file_info_j = NULL;
	char temp[256], base_dir[1024];
	gboolean curr_dir_flag;

	while((pos = strchr(pos, '%')) != NULL){
		switch(pos[1]){
			case 'b':
				/* Works */
				if(array_is_init == FALSE){
					for(i=0; i<len_file_list; ++i){
						g_string_i = g_string_new(out_string->str);
						file_info_i = fm_list_peek_nth(file_info_list, i);
						base_name = (gchar *)fm_file_info_get_disp_name(file_info_i);
						g_string_append(g_string_i, base_name);
						g_string_append_c(g_string_i, ' ');

						g_ptr_array_add(out_string_array, g_string_new(g_string_i->str));
						g_string_free(g_string_i, TRUE);
					}
					break;
				}

				file_info_i = fm_list_peek_head(file_info_list);
				base_name = (gchar *)fm_file_info_get_disp_name(file_info_i);
				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, base_name);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'B':
				/* Works */
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_info_j= fm_list_peek_nth(file_info_list, j);
						base_name = (gchar *)fm_file_info_get_disp_name(file_info_j);
						g_string_append(g_string_i, base_name);
						g_string_append_c(g_string_i, ' ');
					}
				}

				break;
			case 'c':
				/* Works */
				memset(temp, 256, 0);
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					sprintf(temp, "%u", len_file_list);
					g_string_append(g_string_i, temp);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'd':
				/* Works */
				curr_dir_flag = FALSE;
				memset(base_dir, 1024, 0);
				if(array_is_init == FALSE){
					for(i=0; i<len_file_list; ++i){
						g_string_i = g_string_new(out_string->str);
						file_info_i = fm_list_peek_nth(file_info_list, i);
						//printf("File is %s\n", fm_file_info_get_disp_name(file_info_i));
						if(fm_file_info_is_dir(file_info_i) == TRUE){
							strcpy(base_dir, fm_path_to_str(fm_file_info_get_path(file_info_i)));
						} else {
							if(curr_dir_flag == FALSE){
								getcwd(base_dir, 1024);
								curr_dir_flag = TRUE;
							} else {
								continue;
							}
						}
						g_string_append(g_string_i, base_dir);
						g_string_append_c(g_string_i, ' ');

						//printf("Inserting %s\n", g_string_i->str);
						g_ptr_array_add(out_string_array, g_string_new(g_string_i->str));
						g_string_free(g_string_i, TRUE);
					}
					break;
				}

				file_info_i = fm_list_peek_head(file_info_list);
				if(fm_file_info_is_dir(file_info_i) == TRUE){
						strcpy(base_dir, fm_path_to_str(fm_file_info_get_path(file_info_i)));
				} else {
					if(curr_dir_flag == FALSE){
						getcwd(base_dir, 1024);
						curr_dir_flag = TRUE;
					} else {
						continue;
					}
				}
				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, base_dir);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'D':
				/* Works */
				memset(base_dir, 1024, 0);
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					curr_dir_flag = FALSE;
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_info_j= fm_list_peek_nth(file_info_list, j);
						if(fm_file_info_is_dir(file_info_j) == TRUE){
							strcpy(base_dir, fm_path_to_str(fm_file_info_get_path(file_info_j)));
						} else {
							if(curr_dir_flag == FALSE){
								getcwd(base_dir, 1024);
								curr_dir_flag = TRUE;
							} else {
								continue;
							}
						}
						g_string_append(g_string_i, base_dir);
						g_string_append_c(g_string_i, ' ');
					}
				}

				break;
			case 'f':
				/* Works */
				/* Is the same as %b for now */
				if(array_is_init == FALSE){
					for(i=0; i<len_file_list; ++i){
						g_string_i = g_string_new(out_string->str);
						file_info_i = fm_list_peek_nth(file_info_list, i);
						file_name = (gchar *)fm_path_to_str(fm_file_info_get_path(file_info_i));
						g_string_append(g_string_i, file_name);
						g_string_append_c(g_string_i, ' ');

						g_ptr_array_add(out_string_array, g_string_new(g_string_i->str));
						g_string_free(g_string_i, TRUE);
					}
					break;
				}

				file_info_i = fm_list_peek_head(file_info_list);
				file_name = (gchar *)fm_path_to_str(fm_file_info_get_path(file_info_i));
				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, file_name);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'F':
				/* Works */
				/* Is the same as %B for now */
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_info_j= fm_list_peek_nth(file_info_list, j);
						file_name = (gchar *)fm_path_to_str(fm_file_info_get_path(file_info_j));
						g_string_append(g_string_i, file_name);
						g_string_append_c(g_string_i, ' ');
					}
				}

				break;
			case 'h':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					file_info_i = fm_list_peek_nth(file_info_list, i);
					host_name = get_host_name(file_info_i);
					g_string_append(g_string_i, host_name);
					g_string_append_c(g_string_i, ' ');
				}
				break;
			case 'n':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					file_info_i = fm_list_peek_nth(file_info_list, i);
					user_name = get_user_name(file_info_i);
					g_string_append(g_string_i, user_name);
					g_string_append_c(g_string_i, ' ');
				}
				break;
			case 'p':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					file_info_i = fm_list_peek_nth(file_info_list, i);
					port = get_port(file_info_i);
					g_string_append(g_string_i, port);
					g_string_append_c(g_string_i, ' ');
				}
				break;
			case 's':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					file_info_i = fm_list_peek_nth(file_info_list, i);
					scheme = get_scheme(file_info_i);
					g_string_append(g_string_i, scheme);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'u':
				/* Works */
				if(array_is_init == FALSE){
					for(i=0; i<len_file_list; ++i){
						g_string_i = g_string_new(out_string->str);
						file_info_i = fm_list_peek_nth(file_info_list, i);
						uri = fm_path_to_uri(fm_file_info_get_path(file_info_i));
						g_string_append(g_string_i, uri);
						g_string_append_c(g_string_i, ' ');

						g_ptr_array_add(out_string_array, g_string_new(g_string_i->str));
						g_string_free(g_string_i, TRUE);
					}
					break;
				}

				file_info_i = fm_list_peek_head(file_info_list);
				uri = fm_path_to_uri(fm_file_info_get_path(file_info_i));
				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, uri);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'U':
				/* Works */
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_info_j= fm_list_peek_nth(file_info_list, j);
						uri = fm_path_to_uri(fm_file_info_get_path(file_info_j));
						g_string_append(g_string_i, uri);
						g_string_append_c(g_string_i, ' ');
					}
				}

				break;
			case 'w':
				/* Works */
				if(array_is_init == FALSE){
					for(i=0; i<len_file_list; ++i){
						g_string_i = g_string_new(out_string->str);
						file_info_i = fm_list_peek_nth(file_info_list, i);
						file_name = (gchar *)fm_file_info_get_disp_name(file_info_i);
						//printf("%s\n", file_name);

						ext_pos = g_strrstr(fm_file_info_get_disp_name(fm_list_peek_nth(file_info_list, i)), ".");
						if(ext_pos != NULL)
							file_name_wo_ext = g_strndup(file_name, strlen(file_name) - strlen(ext_pos));
						else
							file_name_wo_ext = g_strdup(file_name);

						g_string_append(g_string_i, file_name_wo_ext);
						g_string_append_c(g_string_i, ' ');
						g_ptr_array_add(out_string_array, g_string_new(g_string_i->str));

						g_free(file_name_wo_ext);
						g_string_free(g_string_i, TRUE);
					}
					break;
				}

				file_info_i = fm_list_peek_head(file_info_list);
				file_name = (gchar *)fm_file_info_get_disp_name(file_info_i);
				ext_pos = g_strrstr(file_name, ".");
				if(ext_pos != NULL)
					file_name_wo_ext = g_strndup(file_name, strlen(file_name) - strlen(ext_pos));
				else
					file_name_wo_ext = g_strdup(file_name);

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, file_name_wo_ext);
					g_string_append_c(g_string_i, ' ');
				}
				g_free(file_name_wo_ext);
				break;
			case 'W':
				/* Works */
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_info_j= fm_list_peek_nth(file_info_list, j);
						file_name = (gchar *)fm_file_info_get_disp_name(file_info_j);
						ext_pos = g_strrstr(file_name, ".");
						if(ext_pos != NULL)
							file_name_wo_ext = g_strndup(file_name, strlen(file_name) - strlen(ext_pos));
						else
							file_name_wo_ext = g_strdup(file_name);
						g_string_append(g_string_i, file_name_wo_ext);
						g_string_append_c(g_string_i, ' ');
						g_free(file_name_wo_ext);
					}
				}
				break;
			case 'x':
				/* Works */
				if(array_is_init == FALSE){
					for(i=0; i<len_file_list; ++i){
						file_info_i = fm_list_peek_nth(file_info_list, i);
						file_name = (gchar *)fm_file_info_get_disp_name(file_info_i);
						ext_pos = g_strrstr(file_name, ".");
						if(ext_pos != NULL){
							g_string_i = g_string_new(out_string->str);
							g_string_append(g_string_i, ext_pos);
							g_string_append_c(g_string_i, ' ');
							g_ptr_array_add(out_string_array, g_string_new(g_string_i->str));

							g_free(file_name_wo_ext);
							g_string_free(g_string_i, TRUE);
						}
					}
					break;
				}

				file_info_i = fm_list_peek_head(file_info_list);
				file_name = (gchar *)fm_file_info_get_disp_name(file_info_i);
				ext_pos = g_strrstr(file_name, ".");
				if(ext_pos == NULL)
					break;

				for(i=0; i<out_string_array->len;++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, ext_pos);
					g_string_append_c(g_string_i, ' ');
				}

				break;
			case 'X':
				/* Works */
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_info_j= fm_list_peek_nth(file_info_list, j);
						file_name = (gchar *)fm_file_info_get_disp_name(file_info_j);
						ext_pos = g_strrstr(file_name, ".");
						if(ext_pos != NULL){
							g_string_append(g_string_i, ext_pos);
							g_string_append_c(g_string_i, ' ');
						}
					}
				}

				break;
			case '%':
				/* Works */
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					g_string_append(g_string_i, "% ");
				}

				break;
			default:
				return NULL;
		}

		pos += 2;
		(array_is_init == FALSE)?array_is_init = TRUE:0;
	}

	return out_string_array;
}

gchar *get_host_name(FmFileInfo *file_info)
{
	/* TODO: Update to support IPv6. Different rules just for IPv6. Hence, ignoring */
	gchar *host_name = NULL;
	gsize host_length;
	gchar *uri = fm_path_to_uri(fm_file_info_get_path(file_info));
	gchar *user_pos = strchr(uri, ':');

	if(user_pos == NULL) return "";
	user_pos += 1;
	if(strncmp(user_pos, "//", 2) == 0)
		user_pos += 2;
	gchar *host_pos = strchr(uri, '@');
	if(host_pos == NULL)
		host_pos = user_pos;
	else
		host_pos += 1;

	if(strchr(host_pos, ':') != NULL){
		host_length = strcspn(host_pos, ":");
		host_name = g_strndup(host_pos, host_length);
	} else {
		printf("here\n");
		host_length = strcspn(host_pos, "/");
		host_name = g_strndup(host_pos, host_length);
	}

	return host_name;
}

gchar *get_user_name(FmFileInfo *file_info)
{
	gchar *uri = fm_path_to_uri(fm_file_info_get_path(file_info));
	gsize user_length;
	gchar *user_pos = NULL, *user_name = NULL;

	user_pos = strchr(uri, ':');
	if(user_pos == NULL) return "";
	user_pos += 1;
	if(strncmp(user_pos, "//", 2) == 0)
		user_pos += 2;
	if(strchr(user_pos, '@') != NULL){
		user_length = strcspn(user_pos, "@");
		user_name = g_strndup(user_pos, user_length);
	} else {
		user_name = "";
	}

	return user_name;
}

gchar *get_port(FmFileInfo *file_info)
{
	/* TODO: Update for IPv6 support */
	gchar *uri = fm_path_to_uri(fm_file_info_get_path(file_info));
	gchar *user_pos = strchr(uri, ':') + 1;
	user_pos += 1;
	if(strncmp(user_pos, "//", 2) == 0)
		user_pos += 2;
	gchar *port_pos = strchr(user_pos, ':');
	if(port_pos == NULL) return "";
	gsize port_length = strcspn(port_pos, "/");
	gchar *port = g_strndup(port_pos, port_length) + 1;

	return port;
}

gchar *get_scheme(FmFileInfo *file_info)
{
	gchar *uri = fm_path_to_uri(fm_file_info_get_path(file_info));
	gsize scheme_len = strcspn(uri, ":");
	gchar *scheme = g_strndup(uri, scheme_len);

	return scheme;
}

