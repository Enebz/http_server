#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

typedef struct HttpStatusCode HttpStatusCode;

struct HttpStatusCode {
    int code;
    const char* reason;
};

HttpStatusCode http_status_code(int code, const char *reason);

#endif // HTTP_STATUS_H