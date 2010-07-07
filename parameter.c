/* Input: A string with parameters
 * Output: A string list with parameters substituted 
 * */

#include<glib.h>
#include<string.h>

#include<libfm/fm-file-info.h>

void print_gstring(gpointer data, gpointer user_data);

GPtrArray* substitute_parameters(gchar *input_string, FmFileInfoList *file_info_list)
{
	gsize i;
	gsize len_file_list = fm_list_get_length(file_info_list);
	GPtrArray *out_string_array = g_ptr_array_new();
	GString *out_string = g_string_new(NULL);
	//gchar *temp_string = NULL;
	gsize first_pos = strcspn(input_string, "%");
	gchar *pos = input_string + first_pos;
	g_string_append_len(out_string, input_string, first_pos);
	GString *g_string_i = NULL;
	gchar *file_name;

	for(i=0; i<len_file_list; ++i){
		g_ptr_array_add(out_string_array, g_string_new(out_string->str));
	}
	//g_ptr_array_foreach(out_string_array, print_gstring, NULL);

	while((pos = strchr(pos, '%')) != NULL){
		switch(pos[1]){
			case 'b':
				for(i=0; i<len_file_list; ++i){
					g_string_i = (GString *)g_ptr_array_index(out_string_array, i);
					file_name = (gchar *)fm_file_info_get_disp_name(fm_list_peek_nth(file_info_list, i));
					g_string_append(g_string_i, file_name);
				}
				break;
			case 'B':
				break;
			case 'c':
				break;
			case 'C':
				break;
			case 'd':
				break;
			case 'D':
				break;
			case 'f':
				break;
			case 'F':
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
				break;
			case 'U':
				break;
			case 'w':
				break;
			case 'W':
				break;
			case 'x':
				break;
			case 'X':
				break;
			case '%':
				break;
			default:
				return NULL;
		}

		/*
		g_free(temp_string0);
		temp_string0 = g_strcpy(temp_string1);
		g_free(temp_string1);
		*/

		pos += 2;
	}
	/*
	out_string = g_strcpy(temp_string0);
	g_free(temp_string0);
	*/

	return out_string_array;
}
