# one_time_pads(written in C)
Five programs used to encrypt and decrypt information using a [one-time pad](https://en.wikipedia.org/wiki/One-time_pad) system. This is the networking
assignment from CS344 - Operating Systems I at Oregon State University that utilizes networking and interprocess
communication APIs. The keygen generates a key that must be atleast as long as the size of the message. The encryption client sents the key and the message to the encyption server(responsible for encrypting the message) and recieves an ciphertext back. The decryption client sends the ciphertext and the key to the decryption server and gets back the original plaintext message. This implementation utilizes a real concurrency server design. There are pool of five processes created from the servers to handle simultatnous requests from different clients. There are 5 .c files, 2 bash scripts, and 5 plaintext files: 

-keygen.c <br/>
-dec_client.c <br/>
-dec_server.c <br/>
-enc_client.c <br/>
-enc_server.c <br/>
-compileall.sh <br/>
-p5testscript <br/>
-plaintext1 <br/>
-plaintext2 <br/>
-plaintext3 <br/>
-plaintext4 <br/>
-plaintext5 <br/>

#### Compile all the files
./compileall.sh

#### Run test script:
./p5testscript random_port_1 random_port_2 > mytestresults 2>&1 <br/>
ie: ./p5testscript 57162 57163 > mytestresults 2>&1