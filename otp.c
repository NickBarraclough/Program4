/* CLIENT */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Function Prototypes */
void error(const char*);
void postFunc(int, char**);
void getFunc(int, char**);


int main(int argc, char *argv[]){
    
    // otp post user plaintext key port
    if (strcmp(argv[1], "post") == 0){
        postFunc(argc, argv);
    }
    
    // otp get user key port
    if (strcmp(argv[1], "get") == 0){
        getFunc(argc, argv);
    }
    
    return 0;
}
    
void postFunc(int argc, char *argv[]){
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[80000];
    
    // Check usage & args
    if (argc < 6) {
        fprintf(stderr,"USAGE: %s hostname port\n", argv[1]);
        exit(0);
    }
    
    /* OPEN FIRST FILE AND SEND FILE SIZE TO SERVER*/
    char message[80000], key[80000];
    
    FILE * inFile = fopen(argv[3], "r");  // Open the file given by client

    if (inFile == NULL){  // Check that file opened successfully
      error("ERROR: Failed to open file.");
    }
    
    fgets(message, 80000, inFile);  // Put the file into the message variable

    FILE * openKey = fopen(argv[4], "r");  // Open the key given by client

    if (openKey == NULL){  // Check that key file opened successfully
      error("Failed");
    }
    
    fgets(key, 80000, openKey);  // Put key file into key variable

    FILE * getFileSize = fopen(argv[3], "r+");  // Get file size to determine when to stop recieving
    fseek(getFileSize, 0L, SEEK_END);
    long int messageLength = ftell(getFileSize);
    fclose(getFileSize);  // Close file when finished

    FILE * getKeySize = fopen(argv[4], "r+");  // Get key file size to determine when to stop recieving
    fseek(getKeySize, 0L, SEEK_END);
    long int keyLength = ftell(getKeySize);
    fclose(getKeySize);  // Close key file when finished

    char messageLengthChar[80000];
    char keyLengthChar[80000];
    sprintf(messageLengthChar, "%ld", messageLength);  // Convert message length into a character
    sprintf(keyLengthChar, "%ld", keyLength);  // Convert key length into a character


    if(keyLength < messageLength){
        error("Key is too short");
    } else {
//        printf("Key length = %d, message = %d\n", keyLength, messageLength);
    }

    
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[5]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
    
    if (serverHostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    
    // Copy in the address
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
    
    // Set up the socket and create the socket endpoint
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
        error("CLIENT: ERROR opening socket");
    
    // Connect socket to the server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        error("CLIENT: ERROR connecting");
    
    /* SEND THE MESSAGE LENGTH TO SERVER*/
    charsWritten = send(socketFD, messageLengthChar, strlen(messageLengthChar), 0);
    
    
    if (charsWritten < 0){  // Check for an error when writing to the socket
      error("CLIENT: ERROR writing to socket");
    }
    if (charsWritten < strlen(messageLengthChar)){  // Check that all data has transferred across socket
      printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out buffer for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from socket, leaving \0 at the end

    if (buffer[0] != 'x'){  // Check for valid connection between sockets
      fprintf(stderr, "ERROR Not A Valid Connection");
      exit(2);
    }

    if (charsRead < 0){  // Check for error when reading from socket
      error("CLIENT: ERROR reading from socket");
    }

    /* SEND THE KEY LENGTH TO SERVER*/
//    charsWritten = send(socketFD, keyLengthChar, strlen(keyLengthChar), 0);  // Send message to server and write to the server
    charsWritten = send(socketFD, keyLengthChar, strlen(keyLengthChar), 0);

    if (charsWritten < 0){  // Check for error when writing to socket
      error("CLIENT: ERROR writing to socket");
    }
    
    if (charsWritten < strlen(keyLengthChar)){  // Check that all data has transferred across socket
      fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out buffer for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from socket, leaving \0 at the end

    if (charsRead < 0){  // Check for error when reading from socket
      error("CLIENT: ERROR reading from socket");
    }

    /* SEND THE MESSAGE TO SERVER */
//    charsWritten = send(socketFD, message, strlen(message), 0);  // Send message to server and write to the server

    for (charsWritten = 0; charsWritten < strlen(message); charsWritten += send(socketFD, message, strlen(message) - charsWritten, 0)){
//        printf("Sent: %d bytes in the message \n", charsWritten);
    }

    if (charsWritten < 0){  // Check for error when writing to socket
      error("CLIENT: ERROR writing to socket");
    }

    if (charsWritten < strlen(message)){  // Check that all data has transferred across socket
      fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out buffer for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from socket, leaving \0 at the end

    if (charsRead < 0){  // Check for error when reading from socket
      error("CLIENT: ERROR reading from socket");
    }

    /* SEND THE KEY TO SERVER */
//    charsWritten = send(socketFD, key, strlen(key), 0);  // Send key to server and write to the server

    for (charsWritten = 0; charsWritten < strlen(key); charsWritten += send(socketFD, key, strlen(key) - charsWritten, 0)){
//
    }

    if (charsWritten < 0){  // Check for error when writing to socket
      error("CLIENT: ERROR writing to socket");
    }

    if (charsWritten < strlen(key)){  // Check that all data has transferred across socket
      fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out buffer for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from socket, leaving \0 at the end

    if (charsRead < 0){  // Check for error when reading from socket
      error("CLIENT: ERROR reading from socket");
    }

    /* SEND THE ENCRYPTED MESSAGE TO SERVER */
    memset(buffer, '\0', 1000);  // Get return message from server and clear out buffer for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from socket, leaving \0 at the end

    if (charsRead < 0){  // Checks for an error when reading from the socket
      error("CLIENT: ERROR reading from socket");
    }

//    printf("%s", buffer);               // Adds read data to the destination of choice
    messageLength -= strlen(buffer);    // Decrements the length

    while (messageLength != 0){          // Loop until all data has been read correctly
        if (strlen(buffer) == 0){           // Check for when there is nothing in the file
            break;
        } else {
            memset(buffer, '\0', 1000);     // Clear out buffer for reuse
            charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from socket, leaving \0 at end

            if(charsRead < 0){              // Checks for an error when reading from the socket
                error("CLIENT: ERROR reading from socket");
            }
            
            messageLength -= strlen(buffer);    // Decrements the length
//            printf("%s", buffer);               // Adds read data to the destination of choice
        }
    }

    close(socketFD); // Close the socket
}

    
void getFunc(int argc, char* argv[]){

    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[80000];
    char message[80000];
    char key[80000];
    char usern[100];

    if (argc != 5){   // Check usage & args
        fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
        exit(0);
    }
    
    FILE * openFile = fopen(argv[1], "r");  // Open the file provided from the client

    if(openFile == NULL)   // Check that file opened successfully
    {
        error("Failed");
    }
    fgets(message, 80000, openFile);   // Put the file into the message variable

    FILE * openKey = fopen(argv[2], "r");  // Open the key provided from the client

    if(openKey == NULL)  // Make sure the key was opened successfully
    {
        error("Failed");
    }
    fgets(key, 80000, openKey);  // Puts the key into the key variable

    long int messageLength;
    FILE * findFileSize = fopen(argv[1], "r+");  // Find message file size to determine when to stop receiving
    fseek(findFileSize, 0L, SEEK_END);
    messageLength = ftell(findFileSize);
    fclose(findFileSize);

    
    long int keyLength;
    FILE * findKeySize = fopen(argv[2], "r+");  // Find key file size to determine when to stop receiving
    fseek(findKeySize, 0L, SEEK_END);
    keyLength = ftell(findKeySize);
    fclose(findKeySize);

    if(keyLength < messageLength){
        error("Key is too short");
    } else {
//        printf("Key length = %d, message = %d\n");
    }

    
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress));  // Clear out the address struct
    portNumber = atoi(argv[4]);  // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET;  // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);  // Store the port number
    serverHostInfo = gethostbyname("localhost");  // Convert the machine name into a special form of address

    if(serverHostInfo == NULL)  // Ensure that a host has been specified
    {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }

    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    socketFD = socket(AF_INET, SOCK_STREAM, 0);  // Set up the socket and create the socket file descriptor
    
    if(socketFD < 0)
    {
        error("CLIENT: ERROR opening socket");
    }

    if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect to server and connect socket to address
    {
        error("CLIENT: ERROR connecting");
    }
    
    char messageLengthC[80000];
    char keyLengthC[80000];
    sprintf(messageLengthC, "%ld", messageLength);
    sprintf(keyLengthC, "%ld", keyLength);

      /* SEND THE MESSAGE LENGTH */

    charsWritten = send(socketFD, messageLengthC, strlen(messageLengthC), 0);  // Send message to server and write to the server

    if(charsWritten < 0)  // Checks for an error when writing to the socket
    {
        error("CLIENT: ERROR writing to socket");
    }
    if(charsWritten < strlen(messageLengthC))  // Checks to make sure all the data has transferred across socket
    {
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);   // Read data from the socket, leaving \0 at end

    if(buffer[0] != 'x')
    {
        fprintf(stderr, "ERROR Not A Valid Connection");
        exit(2);
    }

    if(charsRead < 0)  // Check for error when reading from socket
    {
        error("CLIENT: ERROR reading from socket");
    }

      /* SEND THE KEY LENGTH */

    charsWritten = send(socketFD, keyLengthC, strlen(keyLengthC), 0);  // Send message to server and write to the server

    if(charsWritten < 0)  // Checks for an error when writing to the socket
    {
        error("CLIENT: ERROR writing to socket");
    }
    if(charsWritten < strlen(keyLengthC))   // Checks to make sure all the data has transferred across socket
    {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from the socket, leaving \0 at end

    if(charsRead < 0)  // Checks for an error when reading from the socket
    {
        error("CLIENT: ERROR reading from socket");
    }

      /* SEND THE MESSAGE */

    for (charsWritten = 0; charsWritten < strlen(message); charsWritten += send(socketFD, message, strlen(message) - charsWritten, 0)){
//        printf("Sent: %d bytes in the message \n", charsWritten);
    }

    if(charsWritten < 0)  // Checks for an error when writing to the socket
    {
        error("CLIENT: ERROR writing to socket");
    }
    if(charsWritten < strlen(message))  // Checks to make sure all the data has transferred across socket
    {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from the socket, leaving \0 at end

    if(charsRead < 0)  // Checks for an error when reading from the socket
    {
        error("CLIENT: ERROR reading from socket");
    }

    /* SEND THE KEY */

    for (charsWritten = 0; charsWritten < strlen(key); charsWritten += send(socketFD, key, strlen(key) - charsWritten, 0)){
//
    }

    if(charsWritten < 0)  // Checks for an error when writing to the socket
    {
        error("CLIENT: ERROR writing to socket");
    }
    if(charsWritten < strlen(key))  // Checks to make sure all the data has transferred across socket
    {
        fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
    }

    memset(buffer, '\0', 1000);  // Get return message from server and clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, 1000, 0);  // Read data from the socket, leaving \0 at end

    if(charsRead < 0)  // Checks for an error when reading from the socket
    {
        error("CLIENT: ERROR reading from socket");
    }

      /* SEND THE ENC MESSAGE */

    memset(buffer, '\0', 1000);  // Get return message from server and clear out the buffer again for reuse
    charsRead = recv(socketFD, buffer, 1, 0);  // Read data from the socket, leaving \0 at end

    if(charsRead < 0)  // Checks for an error when reading from the socket
    {
        error("CLIENT: ERROR reading from socket");
    }

    //printf("%s", buffer);  // Adds read data to the destination of choice
    messageLength -= strlen(buffer);  // Decrements the length

    while(messageLength != 0)  // Loop until all data has been read correctly
    {
        if(strlen(buffer) == 0)   // Check for when there is nothing in the file
        {
            break;
        } else {
            memset(buffer, '\0', 1000);  // Clear out the buffer again for reuse
            charsRead = recv(socketFD, buffer, 1, 0);  // Read data from the socket, leaving \0 at end

            if(charsRead < 0)  // Checks for an error when reading from the socket
            {
                error("CLIENT: ERROR reading from socket");
            }
            messageLength -= strlen(buffer);  // Decrements the length
            printf("%s", buffer);  // Adds read data to the destination of choice
        }
    }

    close(socketFD);  // Close the socket

}

/* Error function used for reporting issues */
void error(const char *msg){
    perror(msg);
    exit(1);
}
