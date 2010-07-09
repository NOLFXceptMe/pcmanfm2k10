#include<glib.h>
#include<stdio.h>
#include<string.h>

gchar *get_host_name(gchar *uri)
{
	gchar *user_pos = strchr(uri, ':');
	gsize host_length;
	gchar *host_name = NULL, *host_pos = NULL;

	if(user_pos == NULL) return "";
	user_pos += 1;
	if(strncmp(user_pos, "//", 2) == 0)
		user_pos += 2;
	host_pos = strchr(uri, '@');
	if(host_pos == NULL)
		host_pos = user_pos;
	else
		host_pos += 1;

	if(strchr(host_pos, ':') != NULL){
		host_length = strcspn(host_pos, ":");
		host_name = g_strndup(host_pos, host_length);
	} else {
		host_length = strcspn(host_pos, "/");
		host_name = g_strndup(host_pos, host_length);
	}

	return host_name;
}

gchar *get_user_name(gchar *uri)
{
	gchar *user_pos = strchr(uri, ':'), *user_name = NULL;
	gsize user_length;

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

gchar *get_port(gchar *uri)
{
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

gchar *get_scheme(gchar *uri)
{
	gsize scheme_len = strcspn(uri, ":");
	gchar *scheme = g_strndup(uri, scheme_len);

	return scheme;
}

int main()
{
	//gchar *uri = "ssh://npower@localhost:22/index.html";
	//gchar *uri = "ftp://ftp.is.co.za/rfc/rfc.txt";
	//gchar *uri = "ldap://[2001:db8::7]/c=GB?objectClass?one"; /* FAIL */
	//gchar *uri = "urn:oasis:names:specification:docbook:dtd:xml:4.1.2"; /* FAIL */
	
	printf("Scheme: %s\n", get_scheme(uri));
	printf("User Name: %s\n", get_user_name(uri));
	printf("Host Name: %s\n", get_host_name(uri));
	printf("Port: %s\n", get_port(uri));

	return 0;
}

