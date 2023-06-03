#ifndef HTTP_VERSION_H
#define HTTP_VERSION_H

typedef struct HttpVersion HttpVersion;

struct HttpVersion {
    short major;
    short minor;
};

HttpVersion http_version(short major, short minor);

#endif // HTTP_VERSION_H