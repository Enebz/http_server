#include "response.h"

// Function Definitions
HttpResponse *http_response_create(HttpRequest *request)
{
    HttpResponse *response = malloc(sizeof(HttpResponse));
    response->request = request;
    response->status.code = 0;
    response->status.reason = NULL;
    response->headers = ht_create(30);
    response->body = NULL;
    return response;
}

void http_response_destroy(HttpResponse *response)
{
    ht_destroy(response->headers);
    free(response);
}

int http_response_set_file(HttpResponse *response, char* file_path)
{
    char * buffer = 0;
	long length;
	FILE * f = fopen (file_path, "rb");

	if (f != NULL)
	{
		fseek (f, 0, SEEK_END);
		length = ftell (f);
		fseek (f, 0, SEEK_SET);
		buffer = malloc (length + 1); // +1 for null terminator
		if (buffer)
		{
			fread (buffer, 1, length, f);
		}
		fclose (f);
	}
    else 
    {
        return -1;
    }

    // Null terminate the buffer
    buffer[length] = '\0';
    response->body = buffer;

    // Get mime type from file extension
    char *mime_type = NULL;
    char *file_ext = strrchr(file_path, '.');

    if (file_ext != NULL)
    {
        if (strcmp(file_ext, ".html") == 0)
        {
            mime_type = "text/html";
        }
        else if (strcmp(file_ext, ".css") == 0)
        {
            mime_type = "text/css";
        }
        else if (strcmp(file_ext, ".js") == 0)
        {
            mime_type = "text/javascript";
        }
        else if (strcmp(file_ext, ".jpg") == 0)
        {
            mime_type = "image/jpeg";
        }
        else if (strcmp(file_ext, ".png") == 0)
        {
            mime_type = "image/png";
        }
        else if (strcmp(file_ext, ".gif") == 0)
        {
            mime_type = "image/gif";
        }
        else if (strcmp(file_ext, ".ico") == 0)
        {
            mime_type = "image/x-icon";
        }
        else if (strcmp(file_ext, ".txt") == 0)
        {
            mime_type = "text/plain";
        }
        else if (strcmp(file_ext, ".pdf") == 0)
        {
            mime_type = "application/pdf";
        }
        else if (strcmp(file_ext, ".json") == 0)
        {
            mime_type = "application/json";
        }
        else if (strcmp(file_ext, ".xml") == 0)
        {
            mime_type = "application/xml";
        }
        else if (strcmp(file_ext, ".zip") == 0)
        {
            mime_type = "application/zip";
        }
        else if (strcmp(file_ext, ".gz") == 0)
        {
            mime_type = "application/gzip";
        }
        else if (strcmp(file_ext, ".tar") == 0)
        {
            mime_type = "application/x-tar";
        }
        else if (strcmp(file_ext, ".mp3") == 0)
        {
            mime_type = "audio/mpeg";
        }
        else if (strcmp(file_ext, ".mp4") == 0)
        {
            mime_type = "video/mp4";
        }
        else if (strcmp(file_ext, ".avi") == 0)
        {
            mime_type = "video/x-msvideo";
        }
        else if (strcmp(file_ext, ".mpeg") == 0)
        {
            mime_type = "video/mpeg";
        }
        else if (strcmp(file_ext, ".webm") == 0)
        {
            mime_type = "video/webm";
        }
        else if (strcmp(file_ext, ".ogv") == 0)
        {
            mime_type = "video/ogg";
        }
        else if (strcmp(file_ext, ".flv") == 0)
        {
            mime_type = "video/x-flv";
        }
        else if (strcmp(file_ext, ".wmv") == 0)
        {
            mime_type = "video/x-ms-wmv";
        }
        else if (strcmp(file_ext, ".wav") == 0)
        {
            mime_type = "audio/x-wav";
        }
        else if (strcmp(file_ext, ".ogg") == 0)
        {
            mime_type = "audio/ogg";
        }
        else if (strcmp(file_ext, ".aac") == 0)
        {
            mime_type = "audio/aac";
        }
        else if (strcmp(file_ext, ".midi") == 0)
        {
            mime_type = "audio/midi";
        }
        else if (strcmp(file_ext, ".wma") == 0)
        {
            mime_type = "audio/x-ms-wma";
        }
        else if (strcmp(file_ext, ".weba") == 0)
        {
            mime_type = "audio/webm";
        }
        else if (strcmp(file_ext, ".webp") == 0)
        {
            mime_type = "image/webp";
        }
        else if (strcmp(file_ext, ".svg") == 0)
        {
            mime_type = "image/svg+xml";
        }
        else if (strcmp(file_ext, ".tif") == 0)
        {
            mime_type = "image/tiff";
        }
        else if (strcmp(file_ext, ".tiff") == 0)
        {
            mime_type = "image/tiff";
        }
        else if (strcmp(file_ext, ".bmp") == 0)
        {
            mime_type = "image/bmp";
        }
        else if (strcmp(file_ext, ".ttf") == 0)
        {
            mime_type = "font/ttf";
        }
        else if (strcmp(file_ext, ".otf") == 0)
        {
            mime_type = "font/otf";
        }
        else {
            mime_type = "application/octet-stream";
        }
    }

    ht_insert(response->headers, "Content-Type", mime_type);
    return 0;
}

char* http_response_to_string(HttpResponse *response)
{
    // Create the response string
    char *response_str = (char*)malloc(4096);
    sprintf(response_str, "HTTP/1.1 %d %s\r\n", response->status.code, response->status.reason);

    // Add headers
    char **header_keys = (char**)ht_keys(response->headers);
    char **header_vals = (char**)ht_values(response->headers);

    for (int i = 0; i < ht_count(response->headers); i++)
    {
        strcat(response_str, header_keys[i]);
        strcat(response_str, ": ");
        strcat(response_str, header_vals[i]);
        strcat(response_str, "\r\n");
    }

    // Add CRLF
    strcat(response_str, "\r\n");

    // Add body
    if (response->body != NULL)
    {
        strcat(response_str, response->body);
    }
    return response_str;
}