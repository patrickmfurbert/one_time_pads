/*
    Patrick Furbert 
    CS344 Operating Systems 1
    March 4, 2021
*/

/*

This program creates a key file of specified length. The characters in the 
file generated will be any of the 27 allowed characters, generated using the 
standard Unix randomization methods. Do not create spaces every five characters, 
as has been historically done. Note that you specifically do not have to do any 
fancy random number generation: we’re not looking for cryptographically secure 
random number generation. rand() (Links to an external site.) is just fine. 
The last character keygen outputs should be a newline. Any error text must 
be output to stderr.

*/

/*

The syntax for keygen is as follows:

keygen keylength

where keylength is the length of the key file in 
characters. keygen outputs to stdout. Here is an example run, 
which creates a key of 256 characters and redirects stdout a 
file called mykey (note that mykey is 257 characters long 
because of the newline):

$ keygen 256 > mykey

*/

//includes
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h> 

int get_random_num(){
    return rand()%27;
}


int main(int argc, char** argv){

    //table of values for random characters
    int table[27];

    //fill table
    for(int i=65; i<=90; i++){
        table[i-65] = i;
    }

    //put value for space in table(at the last index)
    table[26] = 32;

    //get the length of the key
    int key_length = atoi(argv[1]);

    //allocate space for the key
    char key[key_length+1];

    //clear the key buffer
    memset(key, '\0', sizeof(key));

    //seed random generator
    srand(time(NULL));


    //fill the buffer with nonsense
    for (int i=0; i<key_length; i++){
        int num = get_random_num();
        key[i] = table[num];
    }

    //print table of values for the allowed
    // for(int i=0; i<27; i++){
    //     printf("table[%d] = %d or char \'%c\'\n", i,table[i], table[i]);
    // }

    fprintf(stdout, "%s\n", key);

    return EXIT_SUCCESS;
}