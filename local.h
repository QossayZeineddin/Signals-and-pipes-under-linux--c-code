
/*
 * local header file for pipe client-server
 */
#ifndef __LOCAL_H_
#define __LOCAL_H_


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define B_SIZ (PIPE_BUF / 2)

struct message {
char   fifo_name[B_SIZ];
};




void red ()
{
    printf("\033[1;31m");
     fflush(stdout);

}
void green ()
{
    printf("\033[1;32m");
    fflush(stdout);

}
void yellow()
{
    printf("\033[1;33m");
    fflush(stdout);

}

void blue()
{
    printf("\033[0;34m");
    fflush(stdout);

}
void normals()
{
    printf("\033[0;37m");
    fflush(stdout);

}
void cyan()
{
    printf("\033[1;36m");
    fflush(stdout);

}


void Purple()
{
    printf(" \033[0;35m");
    fflush(stdout);

}

#endif
