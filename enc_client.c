/*
    Patrick Furbert
    CS344 - Operating Systems 1
    March 4, 2021
*/

/*
    Client that sends the key and plain text to the encryption server
*/

/*
****order of network api calls****
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


#define RECV_BUFFER_SIZE 8192 //(2^13)


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
    char buffer[RECV_BUFFER_SIZE+1] = "Hi there! I am the baddest encryption client ever";
    char* cipher_text = (char*)malloc(sizeof(char)*(RECV_BUFFER_SIZE + 1));
    int need_to_realloc = 0;
    int cipher_text_size = RECV_BUFFER_SIZE + 1;

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

    //get size of the plaintext and key files
    long size_plaintext = ftell(plaintext_file);
    long size_keyfile = ftell(key_file);

    /*            If the key is shorter than the  plaintext error out                  */
    if(size_keyfile < size_plaintext){
        fprintf(stderr, "CLIENT: ERROR: size of <%s> must be >= size of <%s>\n", argv[2], argv[1]);
        exit(1);
    }
    
    //printf("Size of plaintext file is %ld\nSize of keyfile is %ld\n", size_plaintext, size_keyfile);

    fseek(plaintext_file, 0, SEEK_SET); //set the plaintext file position  to the beginning
    fseek(key_file, 0, SEEK_SET); //set the key file position to the beginning

    
    char *plain_text;
    char *key;
    size_t len = 0;
    ssize_t chars_read_from_files;

    
    while((chars_read_from_files = getline(&plain_text, &len, plaintext_file)) != -1){
    }

    len = 0; //reset length to zero

    while((chars_read_from_files = getline(&key, &len, key_file)) != -1){
    }

    //check the plaintext for characters that aren't allowed
    for(int i=0; i<size_plaintext-1;i++){
        if((plain_text[i] < 65 && plain_text[i] != 32 ) || plain_text[i] > 90){
            fprintf(stderr, "CLIENT: ERROR: <%s> contains BAD character(s) [%c] \n", argv[1], plain_text[i]);
            exit(1);
        }
    }

    //check the keyfile for characters that aren't allowed
    for(int i=0; i<size_keyfile-1;i++){
        if((key[i] < 65 && key[i] != 32 ) || key[i] > 90){
            fprintf(stderr, "CLIENT: ERROR: <%s> contains BAD character(s) [%c] \n", argv[2], key[i]);
            exit(1);
        }
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


    //housekeeping - free dynamically allocated memory and close files that were opened
    free(plain_text);
    free(key);
    fclose(plaintext_file);
    fclose(key_file);

    //concatenate the plaintext and the keyfile
    char payload[strlen(plain_text_arr) + strlen(key_arr) + 9];
    memset(payload, '\0', strlen(plain_text_arr) + strlen(key_arr) + 9);

    strcat(payload, "enc:");
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
        fprintf(stderr, "CLIENT: Error on opening socket\n");
        exit(1);
    }

    /*        End Startup               */

    /////////////////connect///////////////////

    if(connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){
        fprintf(stderr, "CLIENT: Error on connecting\n");
    }

    /////////////////send//////////////////////

    characters_written = send(socket_fd, payload, strlen(payload), 0);

    if(characters_written < 0){
        fprintf(stderr, "CLIENT: Error on writing to socket\n");
    }

    if(characters_written < strlen(payload)){
        fprintf(stderr, "CLIENT: Warning, not all data written to socket\n");
    }

    /////////////////recv//////////////////////
    memset(buffer, '\0', RECV_BUFFER_SIZE+1); // clear the buffer

    while(strstr(buffer, "!!") == NULL){
        memset(buffer, '\0', RECV_BUFFER_SIZE+1); //clear the buffer

        characters_read = recv(socket_fd, buffer, RECV_BUFFER_SIZE, 0); //read response from server

        if(characters_read < 0) {
            fprintf(stderr, "CLIENT: Error on reading from socket\n");
        }

        if(!need_to_realloc){
            need_to_realloc = 1;
            strcat(cipher_text, buffer);
        }else{
            cipher_text = (char*)realloc(cipher_text, sizeof(char)*(RECV_BUFFER_SIZE + cipher_text_size));
            strcat(cipher_text, buffer);
            cipher_text_size += RECV_BUFFER_SIZE;
        }
    }

    //set nul terminated character two characters before end of cipher_text to remove !!
    cipher_text[strlen(cipher_text)-2] = '\0';
    fprintf(stdout, "%s\n", cipher_text);

    free(cipher_text);
    /////////////////close/////////////////////
    close(socket_fd);


    return EXIT_SUCCESS;
}