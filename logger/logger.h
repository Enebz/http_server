#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* Global variables declaration */
extern FILE *log_file;

/* Function definitions */
int init_logger();
int close_logger();

int log_message(char *message, ...);
