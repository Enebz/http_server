#include "tokenbucket.h"


HttpTokenBucket* http_token_bucket_create(int max_tokens, int seconds)
{
    HttpTokenBucket* bucket = malloc(sizeof(HttpTokenBucket));
    bucket->max_tokens = max_tokens;
    bucket->tokens = max_tokens;
    bucket->seconds = seconds;
    bucket->last_update = GetMillisecondsSinceEpoch();

    // Initialize the critical section
    bucket->cs = (LPCRITICAL_SECTION)malloc(sizeof(CRITICAL_SECTION));
    InitializeCriticalSection(bucket->cs);
    return bucket;
}

unsigned long long GetMillisecondsSinceEpoch()
{
    FILETIME fileTime;
    ULARGE_INTEGER largeInt;

    GetSystemTimePreciseAsFileTime(&fileTime);

    largeInt.LowPart = fileTime.dwLowDateTime;
    largeInt.HighPart = fileTime.dwHighDateTime;

    // Convert to milliseconds since the Unix epoch
    unsigned long long milliseconds = (largeInt.QuadPart - EPOCH_DIFFERENCE) / 10000ULL;

    return milliseconds;
}