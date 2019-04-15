NAME: DAI HUYNH
CSCI 3761
Student ID: 107057706

                                                                LAB 2
Description:
In this programming assignment, you are to build a simple file copy service that consists of a client and server. The server and client program should run on separate machines.
A server should be able to spawn a process (or create thread) to handle each incoming connection from clients and let a client download or upload requested files.
    • A server MUST support at least 3 clients concurrently.
    • A server should support the following request from clients:
1) Show files at the server’s current directory: ‘catalog’
2) Download files: ‘download’
    • Synopsis: download source-filename dest-filename
3) Upload files: ‘upload’
    • Synopsis: upload source-filename dest-filename
4) Display current directory of the server: ‘spwd’
    • Synopsis: spwd [no-argument]
5) Terminating the server: “bye”
    • Synopsis: bye [no-argument]
How to build and run program:

The homework file is compressed in zip file. To uncompress, we need to follow commands % unzip huy7706.zip

Now you should see 2 directories named client and server with the files: client.c and server.c respectively

Build the program.

1) RUN SERVER
We change to the directory: %cd server

Then compile the program: % make

Run the program: %./server "PORT NUMBER"

Delete all the objectives files: % ./make clean

2) RUN CLIENT
We change to the directory: %cd client

Then compile the program: % make

Run the program: %./client "HOSTNAME"

Delete all the objectives files: % ./make clean