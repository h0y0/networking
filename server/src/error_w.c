#include "error_w.h"
#include <stdio.h>
#include <stdlib.h>

void err_e(const char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

void err_c(int c, const char *s)
{
    if(c)
    {
        perror(s);
        exit(EXIT_FAILURE);
    }
}
