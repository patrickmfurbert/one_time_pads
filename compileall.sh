#!/bin/bash
gcc -std=gnu99 -g -o enc_server enc_server.c
gcc -std=gnu99 -g -o enc_client enc_client.c
gcc -std=gnu99 -g -o dec_server dec_server.c
gcc -std=gnu99 -g -o dec_client dec_client.c
gcc -std=gnu99 -g -o keygen keygen.c