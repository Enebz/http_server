#ifndef HTTP_VERSION_H
#define HTTP_VERSION_H

#include <stdio.h>
#include <stdlib.h>

typedef struct HttpVersion HttpVersion;

struct HttpVersion {
    short major;
    short minor;
};

HttpVersion http_version(short major, short minor);
char *http_version_to_string(HttpVersion version);

#endif // HTTP_VERSION_H