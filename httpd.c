#include "header.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        perror("not enough args provided");
        exit(1);
    }
    int portnum = atoi(argv[1]);
    int fd = create_service(portnum);

    printf("listening on port %d\n", portnum);


    run_service(fd);
    close(fd);

    

    return 0;

}


void run_service(int fd)
{
   while (1)
   {
      int nfd = accept_connection(fd);
      if (nfd != -1)
      {
        printf("Connection established\n");
        int pid = fork();
        if (pid == 0) {
            close(fd);
            handle_request(nfd);
            close(nfd);
            exit(0);
        } else if (pid > 0) {
            struct sigaction sa;
            sa.sa_handler = sigchld_handler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_RESTART; 
            if (sigaction(SIGCHLD, &sa, NULL) == -1) {
                perror("sigaction");
                exit(1);
            }
            close(nfd); //parent will just keep circulating and spawn child processes when needed
        } else {
            perror("fork");
        }
      }
   }
}


void handle_request(int nfd)
{

//just reading the request
    FILE *network = fdopen(nfd, "r");
    char *line = NULL;
    size_t size;
    ssize_t num;

    if (network == NULL)
    {
        perror("fdopen");
        close(nfd);
        return;
    }
        
    if ((num = getline(&line, &size, network)) < 0) {
        perror("getline");
        fclose(network);
        close(nfd);
        return;
    }

//parsing request
    char request[6];
    char filename[300];
    if (sscanf(line, "%s %s", request, filename) != 2) {
        char response[] = "HTTP/1.0 400 Bad Request : format incorrect\r\n\r\n";
        write(nfd , response , sizeof(response));
        fclose(network);
        close(nfd);
        return;
    }
    request[5] = '\0';
    request[299] = '\0';

    if (filename[0] == '/') { 
            memmove(filename, filename + 1, strlen(filename));
    }   //since we are assuming the path is basically the file, we can get filename by doing this


    struct stat fileinfo;
    if (stat(filename, &fileinfo) != 0) {
            // File not found, send 404 response
            char response[] = "HTTP/1.0 404 Not Found\r\n\r\n";
            write(nfd , response , sizeof(response));
            fclose(network);
            close(nfd);
            return;
    }



    if (strcmp(request, "GET") == 0){

        if(write_header(nfd , "200 OK" , "text/html" , fileinfo.st_size)){
            char response[] = "HTTP/1.0 500 Internal Error: could not parse header\r\n\r\n";
            write(nfd , response , sizeof(response));
            fclose(network);
            close(nfd);
            return;
        }
        FILE *file3 = fopen(filename, "r");
        if (file3 == NULL){
            char response[] = "HTTP/1.0 404 Not Found\r\n\r\n";
            write(nfd , response , sizeof(response));
            fclose(network);
            close(nfd);
            return;
        }
        char *line2 = NULL;
        size_t size2;

        while (getline(&line2, &size2, file3) > 0){
            int size3 = strlen(line2);
            if (write(nfd, line2, size3) == -1) {
                perror("write to client failed");
            }
        }
        free(line2);
        fclose(file3);
    }

    else if (strcmp(request, "HEAD") == 0){
        write_header(nfd , "200 OK" , "text/html" , fileinfo.st_size);  
    }

    else{
        char response[] = "HTTP/1.0 501 Not Implemented : only HEAD and GET commands\r\n\r\n";
        write(nfd , response , sizeof(response));
        fclose(network);
        close(nfd);
        return;
    }

    free(line);
    fclose(network);
}

int write_header(int fd , char *status , char *type , long length){
    char header[500];
    int header_length = snprintf(header , sizeof(header) , 
    "HTTP/1.0 %s\r\n"
    "Content-Type: %s\r\n"
    "COntent-Length: %ld\r\n"
    "\r\n", status, type , length);

    if (header_length > 0 ){
        write(fd , header, header_length);
        return 0;
    }
    return 1;
}


void sigchld_handler(int s) {
    // Waitpid with -1 waits for any child process. WNOHANG means return immediately if no child has exited.
    while(waitpid(-1, NULL, WNOHANG) > 0);
}