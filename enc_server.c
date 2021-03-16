/*
    Patrick Furbert
    CS344 - Operating Systems 1
    March 4, 2021
*/


/* 
    Encryption Server
*/

/*
****order of network api calls****
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
int char_table[27]; //table of values for encoded characters

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

//store pid of child processes
void store_pid(pid_t pid){
    pids[pid_process_counter++] = pid;
}

//fill translation table for working with plaintext and keyfile
void fill_table(){
    //fill table
    for(int i=65; i<=90; i++){
        char_table[i-65] = i;
    }

    //put value for space in table(at the last index)
    char_table[26] = 32;
}

//get the position of character in the conversion string
int get_char_position(char letter){
    char* converstion = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    char* position = strchr(converstion, letter);
    return (int)(position - converstion);
}

//mod function that handles negative remainders
int mod(int a, int b){
    int c = a % b;
    return (c<0)?c+b:c;
}

char* encode_message(char* plain_text, char* key){

    char* ciphertext = (char*)malloc((sizeof(char) * (strlen(plain_text) + 3)));
    memset(ciphertext, '\0', strlen(plain_text)+1);

    //modular addition (encoding)
    for(int i=0;i<strlen(plain_text); i++){
        ciphertext[i] = char_table[mod(get_char_position(plain_text[i]) + get_char_position(key[i]), 27)];
    }

    return strcat(ciphertext, "!!");
}

//parse the plaintext and the key from the message
char* parse_message(char* message){
    char* save_pointer;
    char* plain_text;
    char* key;

    //parse plain_text and key from message recieved
    plain_text = strtok_r(message, "@@", &save_pointer);
    key = strtok_r(NULL, "@@", &save_pointer);

    if(plain_text == NULL){
        fprintf(stderr, "Error getting plaintext");
    }

    if(key == NULL){
        fprintf(stderr, "Error getting key");
    }

    if(plain_text != NULL && key != NULL){
        return encode_message(plain_text, key);
    }
}

/*
    All errors after startup must only be printed to stderr - but the program must still run
*/

int main(int argc, char** argv){
    
    fill_table(); //fill up the conversion table

    int connection_socket, characters_read, listen_socket, worker_number = 0;
    pid_t list_of_workers[POOL_SIZE];
    pid_t parent_pid = getpid();
    pid_t pid;

    char buffer[RECV_BUFFER_SIZE+1];
    char* payload = (char*)malloc(sizeof(char)*(RECV_BUFFER_SIZE + 1));
    memset(payload, '\0', RECV_BUFFER_SIZE + 1);
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
    listen(listen_socket, 10);


    ////////////SPAWN WORKERS//////////////
    for(int i=0;i<POOL_SIZE;i++){

        int r, pipeFDs[2];
        // Create the pipe with error check
        if (pipe(pipeFDs) == -1) {
            fprintf(stderr, "SERVER: ERROR on call to pipe()");
        } 

        //fork
        pid = fork();
        store_pid(pid); //store pid of child processes
        if(pid == 0){
            //send pid from the child to the parent through pipe
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
        close(pipeFDs[1]);
        char read_buffer[50];
        memset(read_buffer, '\0', sizeof(read_buffer));
        r = read(pipeFDs[0], read_buffer, sizeof(read_buffer) - 1);
        list_of_workers[i] = atoi(read_buffer);

    }

    if(pid == 0){ //code for children(workers)

    /*          LOOP                     */
        while(1){

            char* payload = (char*)malloc(sizeof(char)*(RECV_BUFFER_SIZE + 1));
            memset(payload, '\0', RECV_BUFFER_SIZE + 1);


        ////////////ACCEPT/////////////////////
            connection_socket = accept(listen_socket,
                            (struct sockaddr *)&client_address,
                            &size_of_client_info);
            if(connection_socket < 0) {
                fprintf(stderr, "ERROR on accept");
                exit(1);
            }
          
            //clear the buffer
            memset(buffer, '\0', RECV_BUFFER_SIZE+1);

        ////////////RECV///////////////////////
            while(strstr(buffer, "!!") == NULL) {
                
                memset(buffer, '\0', RECV_BUFFER_SIZE+1); //clear the buffer
                characters_read = recv(connection_socket, buffer, RECV_BUFFER_SIZE, 0);
                if(strstr(buffer, "dec:") != NULL){
                    characters_read = send(connection_socket, "^^", 2, 0);
                    close(connection_socket);
                }
                if(characters_read < 0){
                    fprintf(stderr, "ERROR on reading from the socket");
                    exit(1);
                }

                //build the payload 
                if(!need_to_realloc){ //if this is the first message chunk we don't need to call realloc
                    need_to_realloc = 1;
                    strcat(payload, buffer);
                }else{
                    payload = (char*)realloc(payload, sizeof(char)*(RECV_BUFFER_SIZE + payload_size));
                    strcat(payload, buffer);
                    payload_size += RECV_BUFFER_SIZE;
                }
            }

            payload[strlen(payload)-2] = '\0'; //remove the "!!"(end flag) from the message
            char* payload_reference = payload;
            payload = payload + 4; //move the pointer 4 characters to ahead to remove message prefix identifying where the message originated
            char* cipher_text = parse_message(payload);

            ////////////SEND///////////////////////
            characters_read = send(connection_socket, 
                                    cipher_text, strlen(cipher_text), 0);
            free(cipher_text);
            if(characters_read < 0){
                fprintf(stderr, "ERROR on writing to socket");
                exit(1);
            }

            free(payload_reference);

        ////////////CLOSE//////////////////////
            close(connection_socket);
        }//end loop
    }

    //wait for child processes to finish
    for(int i=0;i<POOL_SIZE;i++){
        while(waitpid(pids[i], NULL, 0) > 0);
    }

    close(listen_socket); //close the listening socket
    return EXIT_SUCCESS;
}