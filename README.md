# one_time_pads(written in C)
Five programs used to encrypt and decrypt information using a [one-time pad](https://en.wikipedia.org/wiki/One-time_pad) system. This is the networking
assignment from CS344 - Operating Systems I at Oregon State University that utilizes networking and interprocess
communication APIs. The keygen generates a key that must be atleast as long as the size of the message. The encryption client sents the key and the message to the encyption server(responsible for encrypting the message) and recieves an ciphertext back. The decryption client sends the ciphertext and the key to the decryption server and gets back the original plaintext message. There are 5 .c files, 2 bash scripts, and 5 plaintext files: 

-keygen.c
-dec_client.c
-dec_server.c
-enc_client.c
-enc_server.c
-compileall.sh
-p5testscript
-plaintext1
-plaintext2
-plaintext3
-plaintext4
-plaintext5

#### Compile all the files
./compileall.sh

#### Run test script:
./p5testscript random_port_1 random_port_2 > mytestresults 2>&1
ie: ./p5testscript 57162 57163 > mytestresults 2>&1