#include "version.h"

// Function Definitions
HttpVersion http_version(short major, short minor)
{
    HttpVersion version;
    version.major = major;
    version.minor = minor;
    return version;
}