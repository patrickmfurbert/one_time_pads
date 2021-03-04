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


int main(int argc, char** argv){

    printf("keygen requested length: %d", atoi(argv[2]));

    return EXIT_SUCCESS;
}