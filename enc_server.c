/*
    Patrick Furbert
    CS344 - Operating Systems 1
    March 4, 2021
*/


/* 
    Encryption Server
*/

/*
socket()
bind()
listen()
accept()
recv()
send()
close()
*/

//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h> //to get localhost
#include <arpa/inet.h> //to get localhost

//defines
#define POOL_SIZE 5
#define RECV_BUFFER_SIZE 8192 //(2^13)
pid_t pids[POOL_SIZE];
int pid_process_counter = 0;

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = inet_addr("127.0.0.1");
  //address->sin_addr.s_addr = INADDR_ANY; 

}

void store_pid(pid_t pid){
    pids[pid_process_counter++] = pid;
}

void encode_message(char* message){
    char* save_pointer;
    char* plain_text;
    char* key;

    plain_text = strtok_r(message, "@@", &save_pointer);
    key = strtok_r(NULL, "@@", &save_pointer);

    if(plain_text == NULL){
        fprintf(stderr, "Error getting plaintext");
    }

    if(key == NULL){
        fprintf(stderr, "Error getting key");
    }

    if(plain_text != NULL && key != NULL){
        printf("Plaintext:\n%s\n", plain_text);
        printf("key:\n%s\n", key);
        printf("Size of plaintext: %ld\nSize of key: %ld\n", strlen(plain_text), strlen(key));
    }
}

/*
    All errors after startup must only be printed to stderr - but the program must still run
*/

int main(int argc, char** argv){
    
    int connection_socket, characters_read, listen_socket, worker_number = 0;
    pid_t list_of_workers[POOL_SIZE];
    pid_t parent_pid = getpid();
    pid_t pid;

    char buffer[RECV_BUFFER_SIZE];
    char* payload = (char*)malloc(sizeof(char)*(RECV_BUFFER_SIZE + 1));
    int need_to_realloc = 0;
    int payload_size = RECV_BUFFER_SIZE + 1;
    struct sockaddr_in server_address, client_address;
    socklen_t size_of_client_info = sizeof(client_address);


    /*        Begin Startup             */
    
    if(argc < 1) { //check for incorrect number of arguments
        fprintf(stderr, "USAGE: %s port\n", argv[0]);
        exit(1);
    }

    setupAddressStruct(&server_address, atoi(argv[1])); //setup the address struct for the server socket

    ////////////SOCKET/////////////////////
    if((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "ERROR opening socket");
        exit(1);
    }

    ////////////BIND///////////////////////
    if(bind(listen_socket, 
            (struct sockaddr *)&server_address,
            sizeof(server_address)) < 0){
        fprintf(stderr, "ERROR on binding");                                        
    }

    /*        End Startup                */

    ////////////LISTEN/////////////////////
    listen(listen_socket, 5);

    //printf("Server started running on http://localhost:%d\n", atoi(argv[1]));


    ////////////SPAWN WORKERS//////////////
    for(int i=0;i<POOL_SIZE;i++){

        int r, pipeFDs[2];
        // Create the pipe with error check
        if (pipe(pipeFDs) == -1) {
            fprintf(stderr, "SERVER: ERROR on call to pipe()");
        } 

        //fork
        pid = fork();
        store_pid(pid);
        if(pid == 0){
            close(pipeFDs[0]); 
            char my_pid[50];
            memset(my_pid, '\0', sizeof(my_pid));
            sprintf(my_pid,"%d", getpid());
            write(pipeFDs[1], my_pid, strlen(my_pid)); 
            break;
        }
        if(pid == -1){
            fprintf(stderr, "SERVER: Error on forking\n");
        }
        // Parent process will read from the pipe, so close the output file desriptor.
       // printf("in parent block\n");
        close(pipeFDs[1]);
        char read_buffer[50];
        memset(read_buffer, '\0', sizeof(read_buffer));
        r = read(pipeFDs[0], read_buffer, sizeof(read_buffer) - 1);
       list_of_workers[i] = atoi(read_buffer);

    }

    if(pid == 0){

      // _exit(0);
      //  printf("Do we make it here?\n");
    

    /*          LOOP                     */
        while(1){


        ////////////ACCEPT/////////////////////
            connection_socket = accept(listen_socket,
                            (struct sockaddr *)&client_address,
                            &size_of_client_info);
            if(connection_socket < 0) {
                fprintf(stderr, "ERROR on accept");
                exit(1);
            }

            printf("SERVER(PID:%d): Connected to client running at host %d port %d\n",getpid(),
                                    ntohs(client_address.sin_addr.s_addr),
                                    ntohs(client_address.sin_port));


            //clear the buffer
            memset(buffer, '\0', sizeof(buffer));

        ////////////RECV///////////////////////
            while(strstr(buffer, "!!") == NULL) {
                memset(buffer, '\0', sizeof(buffer)); //clear the buffer
                characters_read = recv(connection_socket, buffer, sizeof(buffer), 0);
                if(characters_read < 0){
                    fprintf(stderr, "ERROR on reading from the socket");
                    exit(1);
                }
               // printf("SERVER: I received this from the client: \"%s\"\n", buffer);

                //build the payload 
                if(!need_to_realloc){
                    need_to_realloc = 1;
                    strcat(payload, buffer);
                }else{
                    payload = (char*)realloc(payload, sizeof(char)*(RECV_BUFFER_SIZE + payload_size));
                    strcat(payload, buffer);
                    payload_size += RECV_BUFFER_SIZE;
                }
            }

            printf("SERVER: Finished recv\n");
            payload[strlen(payload)-2] = '\0';
           // printf("Contents of payload:\n%s\n", payload);
            printf("Size of Payload = %ld\n", strlen(payload));
            encode_message(payload);

            free(payload);
            ////////////SEND///////////////////////
            characters_read = send(connection_socket, 
                                    "I am the server, and I got your message", 39, 0);
            if(characters_read < 0){
                fprintf(stderr, "ERROR on writing to socket");
                exit(1);
            }
        ////////////CLOSE//////////////////////
            close(connection_socket);
        }
    }

    //wait for child processes to finish
    for(int i=0;i<POOL_SIZE;i++){
        while(waitpid(pids[i], NULL, 0) > 0);
    }

    close(listen_socket); //close the listening socket
    return EXIT_SUCCESS;
}