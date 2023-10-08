#include "logger.h"
#include <Windows.h>

/* Global variables definition */
FILE *log_file;
CRITICAL_SECTION lock;

/* Function definitions */
int init_logger()
{
    InitializeCriticalSection(&lock);
    log_file = fopen("http_server.log", "w");
    if (log_file == NULL)
    {
        printf("Could not open log file.");
        exit(1);
    }

    return 0;
}

int close_logger()
{
    fclose(log_file);
    DeleteCriticalSection(&lock);
    return 0;
}

int log_message(char *message, ...)
{
    // We want to make sure that only one thread is writing to the log file at a time
    // so we use a critical section
    EnterCriticalSection(&lock);

    printf("Logging message: %s\n", message);

    va_list args;
    va_start(args, message);
    
    // Log with time
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *time = asctime(timeinfo);
    time[strlen(time) - 1] = '\0';

    fprintf(log_file, "[%s] ", time);
    vfprintf(log_file, message, args);
    fprintf(log_file, "\n");
    va_end(args);

    LeaveCriticalSection(&lock);
    return 0;
}
