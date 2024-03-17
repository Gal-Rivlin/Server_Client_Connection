#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

int create_service(short port);
int accept_connection(int fd);
void run_service(int fd);
void handle_request(int nfd);
void sigchld_handler(int s);
int write_header(int fd , char *status , char *type , long length);