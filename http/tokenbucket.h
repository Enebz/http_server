#ifndef HTTP_TOKEN_BUCKET_H
#define HTTP_TOKEN_BUCKET_H

#include <time.h>
#ifndef _WIN32
    // TODO: Add support for linux
#else
    #include <Windows.h>
#endif

#define EPOCH_DIFFERENCE 116444736000000000ULL

typedef struct HttpTokenBucket HttpTokenBucket;

struct HttpTokenBucket
{
    int tokens;
    int max_tokens;
    int seconds;
    unsigned long long last_update;

    #ifdef _WIN32
		LPCRITICAL_SECTION cs;
	#endif
};

HttpTokenBucket* http_token_bucket_create(int max_tokens, int seconds);

unsigned long long GetMillisecondsSinceEpoch();

#endif // HTTP_TOKEN_BUCKET_H