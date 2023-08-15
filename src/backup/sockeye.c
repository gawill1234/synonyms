#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>
#define PORT 8080

char *parse_it(char *, char *);
void gen_crc_table();
void set_up_synonyms();
void resetquerymark();
float elapsed(struct timeval, struct timeval);
char **split_string(char *, char);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *new_buffer;
    char *hello = "Hello from server";
    struct timeval st, et;
    //char *collection = "synonyms";
    char **myargs;
  
    gen_crc_table();

    set_up_synonyms();

      
    // Creating socket file descriptor
    if ((server_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    //                                              &opt, sizeof(opt)))
    //{
    //    perror("setsockopt");
    //    exit(EXIT_FAILURE);
    //}
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while ( 1 ) {
       if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                          (socklen_t*)&addrlen))<0)
       {
           perror("accept");
           exit(EXIT_FAILURE);
       }
       valread = read( new_socket , buffer, 1024);
       gettimeofday(&st, NULL);

       myargs = split_string(buffer, ';');

       new_buffer = parse_it(myargs[0], myargs[1]);

       gettimeofday(&et, NULL);
       printf("elapsed:  %f microseconds\n", (elapsed(st, et) * 1000000));

       bzero(buffer, 1024);
       send(new_socket , new_buffer , strlen(new_buffer) , 0 );
       printf("Hello message sent\n");

       free(new_buffer);

       close(new_socket);

       resetquerymark();
    }
    close(server_fd);
    return 0;
}
