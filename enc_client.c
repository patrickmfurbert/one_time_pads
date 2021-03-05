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

int main(int argc, char** argv){

    if (argc<4){
        fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
        exit(0); 
        }

    return EXIT_SUCCESS;
}