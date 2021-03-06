/*
    Patrick Furbert
    CS344 - Operating Systems 1
    March 4, 2021
*/

/*
    Client that sends the key and plain text to the encryption server
*/


/*
socket()
connect()
send()
recv()
close()
*/

//includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <netinet/in.h>
#include <arpa/inet.h>

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

/*
    All errors after startup must only be printed to stderr - but the program must still run
*/
int main(int argc, char** argv){

    int socket_fd, port_number, characters_written, characters_read;
    struct sockaddr_in server_address;
    char buffer[256] = "Hi there! I am the baddest encryption client ever";
    

    /*        Begin Startup             */
    if (argc<4){
        fprintf(stderr,"USAGE: %s plaintext_file key_file port\n", argv[0]); 
        exit(1); 
        }


    /////////////////////////////////////////////////////////////////////
    /*        Open the plain_text and key and create one big payload   */
    
    FILE* plaintext_file = fopen(argv[1], "r");
    FILE* key_file = fopen(argv[2], "r");

    

    if(plaintext_file == NULL){
        fprintf(stderr,"CLIENT: Error on opening plaintext file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(key_file == NULL){
        fprintf(stderr,"CLIENT: Error on opening key file: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    

    //handle plain text

    fseek(plaintext_file, 0, SEEK_END); //set the file position of the stream to the end
    fseek(key_file, 0, SEEK_END);

    
    long size_plaintext = ftell(plaintext_file);
    long size_keyfile = ftell(key_file);

    
    printf("Size of plaintext file is %ld\nSize of keyfile is %ld\n", size_plaintext, size_keyfile);

    fseek(plaintext_file, 0, SEEK_SET); //set the file position to the beginning
    fseek(key_file, 0, SEEK_SET);

    
    char *plain_text;
    char *key;
    size_t len = 0;
    ssize_t chars_read_from_files;

    
    while((chars_read_from_files = getline(&plain_text, &len, plaintext_file)) != -1){

    }

    len = 0; //reset length to zero

    while((chars_read_from_files = getline(&key, &len, key_file)) != -1){

    }

    
    //copy plaintext into char array for editing

    char plain_text_arr[size_plaintext];
    memset(plain_text_arr, '\0', size_plaintext);
    strcpy(plain_text_arr, plain_text);
    plain_text_arr[size_plaintext - 1] = '\0';

    
    //copy key into key array for editing
    char key_arr[size_keyfile];
    memset(key_arr, '\0', size_keyfile);
    strcpy(key_arr, key);
    key_arr[size_keyfile-1] = '\0';


    
    //housekeeping
    free(plain_text);
    free(key);
    fclose(plaintext_file);
    fclose(key_file);

    
    //concatenate the plaintext and the keyfile
    char payload[strlen(plain_text_arr) + strlen(key_arr) + 5];

    strcat(payload, plain_text_arr);
    strcat(payload, "@@");
    strcat(payload, key_arr);
    strcat(payload, "!!");

    

    /*        End payload creation                                     */
    /////////////////////////////////////////////////////////////////////


    //setup server address struct
    setupAddressStruct(&server_address, atoi(argv[3]));  

    /////////////////socket////////////////////
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "CLIENT: Error on opening socket");

        exit(1);
    }

    /*        End Startup               */

    /////////////////connect///////////////////
    printf("Client started with plaintext file: %s & key_file: %s\n", argv[1], argv[2]);


    if(connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        fprintf(stderr, "CLIENT: Error on connecting");
    }


    /////////////////send//////////////////////
    //characters_written = send(socket_fd, buffer, strlen(buffer), 0); //send message to server
    //characters_written = send(socket_fd, plain_text, strlen(plain_text), 0);
    characters_written = send(socket_fd, payload, strlen(payload), 0);

    printf("Characters written from send: %d\n", characters_written);

    if(characters_written < 0){
        fprintf(stderr, "CLIENT: Error on writing to socket");
    }

    if(characters_written < strlen(buffer)){
        fprintf(stderr, "CLIENT: Warning, not all data written to socket");
    }

    /////////////////recv//////////////////////
    memset(buffer, '\0', sizeof(buffer)); // clear the buffer

    characters_read = recv(socket_fd, buffer, sizeof(buffer) -1, 0); //read response from server

    if(characters_read < 0) {
        fprintf(stderr, "CLIENT: Error on reading from socket");
    }

    fprintf(stdout, "CLIENT: I recieved this message from the server: \"%s\"\n", buffer);

    /////////////////close/////////////////////
    close(socket_fd);


    return EXIT_SUCCESS;
}