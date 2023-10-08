#include "version.h"

// Function Definitions
HttpVersion http_version(short major, short minor)
{
    HttpVersion version;
    version.major = major;
    version.minor = minor;
    return version;
}

char *http_version_to_string(HttpVersion version)
{
    char *version_string = malloc(14);
    int len = sprintf(version_string, "%d.%d", version.major, version.minor);
    version_string = realloc(version_string, len + 1);
    version_string[len] = '\0';
    return version_string;
}