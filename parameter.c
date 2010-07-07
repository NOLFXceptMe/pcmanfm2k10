/* Input: A string with parameters
 * Output: A string list with parameters substituted 
 * */

#include<glib.h>
#include<string.h>

#include<libfm/fm-file-info.h>

void print_gstring(gpointer data, gpointer user_data);

GPtrArray* substitute_parameters(gchar *input_string, FmFileInfoList *file_info_list)
{
	gsize i, j;
	gsize len_file_list = fm_list_get_length(file_info_list);
	GPtrArray *out_string_array = g_ptr_array_new();
	GString *out_string = g_string_new(NULL);
	//gchar *temp_string = NULL;
	gsize first_pos = strcspn(input_string, "%");
	gchar *pos = input_string + first_pos;
	g_string_append_len(out_string, input_string, first_pos);
	GString *g_string_i = NULL;
	gchar *file_name;
	gboolean array_is_init = FALSE;
	char temp[256];

	while((pos = strchr(pos, '%')) != NULL){
		switch(pos[1]){
			case 'b':
				if(array_is_init == FALSE)
					for(i=0; i<len_file_list; ++i)
						g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					file_name = (gchar *)fm_file_info_get_disp_name(fm_list_peek_nth(file_info_list, i));
					g_string_append(g_string_i, file_name);
					g_string_append_c(g_string_i, ' ');
				}
				break;

			case 'B':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				for(i=0; i<out_string_array->len; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					for(j=0; j<len_file_list; ++j){
						file_name = (gchar *)fm_file_info_get_disp_name(fm_list_peek_nth(file_info_list, j));
						g_string_append(g_string_i, file_name);
						g_string_append_c(g_string_i, ' ');
					}
				}
				break;

			case 'c':
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
				if(array_is_init == FALSE)
					for(i=0; i<len_file_list; ++i)
						g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case 'D':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case 'f':
				if(array_is_init == FALSE)
					for(i=0; i<len_file_list; ++i)
						g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case 'F':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case 'h':
				break;
			case 'n':
				break;
			case 'p':
				break;
			case 's':
				break;
			case 'u':
				if(array_is_init == FALSE)
					for(i=0; i<len_file_list; ++i)
						g_ptr_array_add(out_string_array, g_string_new(out_string->str));
				break;
			case 'U':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case 'w':
				break;
			case 'W':
				break;
			case 'x':
				if(array_is_init == FALSE)
					for(i=0; i<len_file_list; ++i)
						g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case 'X':
				if(array_is_init == FALSE)
					g_ptr_array_add(out_string_array, g_string_new(out_string->str));

				break;
			case '%':
				break;
			default:
				return NULL;
		}

		pos += 2;
		(array_is_init == FALSE)?array_is_init = TRUE:0;
	}
	/*
	out_string = g_strcpy(temp_string0);
	g_free(temp_string0);
	*/

	return out_string_array;
}
