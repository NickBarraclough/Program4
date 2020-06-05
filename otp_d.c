
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Global Variables */

#define BUFFER (int)104304

/* Function Prototypes */
char* decryptMessage(char*, char*);
char* encryptMessage(char*, char*);
void error(const char*);


int main(int argc, char const *argv[]){
    int listenSocketFD;
    int establishedConnectionFD;
    int portNumber;
    int charsRead;
    int pid;
    int messageLength;
    int keyLength;
    int i = 0;
    int postMode = 0;
    socklen_t sizeOfClientInfo;
    char buffer[BUFFER] = "";
    char message[BUFFER] = "";
    char key[BUFFER] = "";
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    if(argc != 2){  // Check usage & args
        fprintf(stderr,"USAGE: %s port\n", argv[0]);
        exit(1);
    }
    
    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress));  // Clear out the address struct
    portNumber = atoi(argv[1]);  // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET;  // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);  // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY;  // Any address is allowed for connection to this process

    // Set up the socket and create the socket endpoint
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    if(listenSocketFD < 0){  // Ensure that the socket has been opened
        error("ERROR opening socket");
    }

    if(bind(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){  // Enable the socket to begin listening and connect socket to port
        error("ERROR on binding");
    }
    
    listen(listenSocketFD, 5);  // Flip the socket on - it can now receive up to 5 connections

    while(1){
        char message[BUFFER] = "";
        char key[BUFFER] = "";
        
        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress);  // Get size of address for client that will connect
        
        // TODO: Check if there are already 5 processes running, if so deny the connection

        
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept

        sleep(2);
        
        if (establishedConnectionFD < 0){  // Check that connection has been established
            error("ERROR on accept");
        }
//        printf("We got connection \n");
        pid = fork();  // Retrieve the PID

        if (pid < 0){  // If PID is less then zero, error with the fork process
            fprintf(stderr, "HULL BREACH\n");
            exit(1);
        }

        if(pid == 0){  // If the pid is zero then continue process normally
            
            /* GET THE MESSAGE LENGTH */
            
            memset(buffer, '\0', BUFFER);  // Get the message from the client and display it
            charsRead = recv(establishedConnectionFD, buffer, BUFFER, 0);  // Read the client's message from the socket

            if (charsRead < 0){  // Check for an error when reading from socket
                error("ERROR reading from socket");
            }
//            printf("We read: %s\n", buffer);

            messageLength = atoi(buffer);  // Send a Success message back to the client
            charsRead = send(establishedConnectionFD, "xMarker", 1, 0); // Send success back

            if (charsRead < 0){  // Checks for an error when writing to the socket
                error("ERROR writing to socket");
            }

            /* GET THE KEY LENGTH */

            memset(buffer, '\0', BUFFER);  // Get the message from the client and display it

        
            charsRead = recv(establishedConnectionFD, buffer, messageLength, 0);
            
            if (charsRead < 0){  // Checks for an error when reading from the socket
                error("ERROR reading from socket");
            }
//            printf("We read: %s\n", buffer);


            keyLength = atoi(buffer);  // Send a Success message back to the client
            charsRead = send(establishedConnectionFD, "I am the server, and I got your keyLength", 39, 0); // Send success back

            if (charsRead < 0){  // Check for error when writing to socket
                error("ERROR writing to socket");
            }

            /* GET THE MESSAGE */

            memset(buffer, '\0', BUFFER);  // Get the message from the client and display it
            
            for (i = 0; i < messageLength; i+= recv(establishedConnectionFD, buffer + i, messageLength - i, 0)){
//                printf("Received: %d bytes in the message \n", i);
            }


            if (i < 0){  // Checks for an error when reading from the socket
                error("ERROR reading from socket");
            }
//            printf("We read: %s\n", buffer);

            
            FILE *f = fopen("ciphertext1", "w");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }

            /* print some text */
            fprintf(f, "%s", message);

            fclose(f);
            
            
            
            strcat(message, buffer);  // Adds read data to the destination of choice
            messageLength -= strlen(buffer);

            while (messageLength != 0){  // Loop until all data has been read correctly
                if (strlen(buffer) == 0){  // Check for when there is nothing in the file
                    break;
                } else {
                    memset(buffer, '\0', BUFFER);  // Clear out the buffer again for reuse
                    charsRead = recv(establishedConnectionFD, buffer, BUFFER, 0);  // Read data from the socket, leaving \0 at end

                    if (charsRead < 0){  // Checks for an error when reading from the socket
                        error("CLIENT: ERROR reading from socket");
                    }
                    messageLength -= strlen(buffer);  // Decrements the length
                    strcat(message, buffer);  // Adds read data to the destination of choice
                }
            }
        
            // Send a Success message back to the client
            charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

            if (charsRead < 0){  // Checks for an error when writing to the socket
                error("ERROR writing to socket");
            }

            /* GET THE KEY */

            memset(buffer, '\0', BUFFER);  // Get the message from the client and display it
//            charsRead = recv(establishedConnectionFD, buffer, BUFFER, 0);  // Read the client's message from the socket
            
            for (i = 0; i < keyLength; i+= recv(establishedConnectionFD, buffer + i, keyLength - i, 0)){
//                printf("Received: %d bytes in the message \n", i);
            }


            if (i < 0){  // Checks for an error when reading from the socket
                error("ERROR reading from socket");
            }

            strcat(key, buffer);  // Adds read data to the destination of choice
            keyLength -= strlen(buffer);

            while(keyLength != 0){  // Loop until all data has been read correctly
                if(strlen(buffer) == 0){  // Check for when there is nothing in the file
                    break;
                } else {
                    memset(buffer, '\0', BUFFER);  // Clear out the buffer again for reuse
                    charsRead = recv(establishedConnectionFD, buffer, BUFFER, 0);  // Read data from the socket, leaving \0 at end

                    if(charsRead < 0)  // Checks for an error when reading from the socket
                        error("CLIENT: ERROR reading from socket");
                
                    keyLength -= strlen(buffer);  // Decrements the length
                    strcat(key, buffer);  // Adds read data to the destination of choice
                }
            }
        
            // Send a Success message back to the client
            charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

            if (charsRead < 0)  // Checks for an error when writing to the socket
                error("ERROR writing to socket");

            /* SEND THE ENCRYPTED/DECRYPTED MESSAGE */
            char * messageDec = decryptMessage(key, message);
            printf("ciphertext1\n");
            charsRead = send(establishedConnectionFD, messageDec, strlen(messageDec), 0); // Send decrypted message back

            if (postMode == 0){
                messageDec = decryptMessage(key, message);
//                printf("%s\n",messageDec);
                //charsRead = send(establishedConnectionFD, messageDec, strlen(messageDec), 0); // Send decrypted message back
            } else {
                //charsRead = send(establishedConnectionFD, encryptMessage(key, message), strlen(message), 0); // Send encrypted message back
            }

            if(charsRead < 0)  // Checks for an error when writing to the socket
                error("ERROR writing to socket");

            if(charsRead < strlen(message))  // Checks that all data has transferred across socket
                printf("WARNING: Not all data written to socket!\n");
        
            close(establishedConnectionFD);  // Close the existing socket which is connected to the client
    
        }
    }
    
    close(listenSocketFD);  // Close the listening socket

    return 0;
}


char* encryptMessage(char* key, char* token){
    int tokenLength;
    int tokenValue;
    int keyLength;
    int keyValue;
    int i = 0;
    int j = 0;
    char saveEncrypt[BUFFER];
    char* encryptedToken;
    char* keyCharacters;

    tokenLength = strlen(token);  // Finds the length of the message
    keyLength = strlen(key);  // Finds the length of the key

    if(keyLength > tokenLength){  // Check to make sure the key is longer then the message
        encryptedToken = malloc(tokenLength * sizeof(char));  // Allocate memory because we cant return a local variable
        keyCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";  // Valid characters that we can choose from

        for(j = 0; j < tokenLength; j++){  // Loop through the length of the provided token
            if((token[i] < 65 || token[i] > 90) && (token[i] != ' ')){  // Check that all characters are valid
                fprintf(stderr, "otp_enc_d error: input contains bad characters\n");
                exit(1);
            }
        }

        while (i < tokenLength - 1){  // Loop through the length of the message
            if ((token[i] != ' ') && (key[i] != ' ')){  // Check if both characters are letters
                tokenValue = token[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                keyValue = key[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                saveEncrypt[i] = ("%c", keyCharacters[(tokenValue + keyValue) % 27]);  // Save the encrypted character
            }
            else if(token[i] != ' ' && key[i] == ' '){  // Check to see if message is a character and key is a space
                tokenValue = token[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                keyValue = key[i] - 6;  // Since this is a space subtract by the ascii value of 6
                saveEncrypt[i] = ("%c", keyCharacters[(tokenValue + keyValue) % 27]);  // Save the encrypted character
            }
            else if(token[i] == ' ' && key[i] != ' '){  // Check to see if key is a character and key is a character
                tokenValue = token[i] - 6;  // Since this is a space subtract by the ascii value of 6
                keyValue = key[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                saveEncrypt[i] = ("%c", keyCharacters[(tokenValue + keyValue) % 27]);   // Save the encrypted character
            }
            else {  // Check to see if both of the characters are a space
                tokenValue = token[i] - 6;  // Since this is a space subtract by the ascii value of 6
                keyValue = key[i] - 6;  // Since this is a space subtract by the ascii value of 6
                saveEncrypt[i] = ("%c", keyCharacters[(tokenValue + keyValue) % 27]);  // Save the encrypted character
            }

            i++;  // Increment i
        }

        saveEncrypt[tokenLength - 1] = '\n';  // Create a newline character at the end of the line
        strcpy(encryptedToken, saveEncrypt);
        saveEncrypt[tokenLength] = '\0';  // Create a null terminator at the end of the line
        strcpy(encryptedToken, saveEncrypt);

        return encryptedToken;  // Return the encrypted message
    }
    else {  // Check to make sure the key is longer then the message
        printf("Error: key is too short\n");
        exit(1);
    }
}


char* decryptMessage(char* key, char* token){
    int tokenLength;
    int tokenValue;
    int keyLength;
    int keyValue;
    int i = 0;
    char saveDecypt[BUFFER];
    char* decryptedToken;
    char* keyCharacters;

    tokenLength = strlen(token);  // Finds the length of the message
    keyLength = strlen(key);  // Finds the length of the key

    if (keyLength > tokenLength){  // Check to make sure the key is longer then the message
        decryptedToken = malloc(tokenLength * sizeof(char));  // Allocate memory because we cant return a local variable
        keyCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";  // Valid characters that we can choose from

        while (i < tokenLength - 1){  // Loop through the length of the message
            if (token[i] != ' ' && key[i] != ' '){  // Check to see if both of the characters are letter
                tokenValue = token[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                keyValue = key[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                if (tokenValue < keyValue) {  // If the token value is less adjust by 27 to pick the correct character
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue + 27) % 27]); // Save the decrypted character
                } else {  // If positive then pick character as normal
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue) % 27]);  // Save the decrypted character
                }
            } else if (token[i] != ' ' && key[i] == ' '){  // Check to see if token is a character and key is a character
                tokenValue = token[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                keyValue = key[i] - 6;  // Since this is a space subtract by the ascii value of 6
                if (tokenValue < keyValue){  // If the token value is less adjust by 27 to pick the correct character
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue + 27) % 27]); // Save the decrypted character
                } else {  // If positive then pick character as normal
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue) % 27]);  // Save the decrypted character
                }
            } else if (token[i] == ' ' && key[i] != ' '){  // Check to see if message is a character and key is a space
                tokenValue = token[i] - 6;  // Since this is a space subtract by the ascii value of 6
                keyValue = key[i] - 65;  // Since this is a letter subtract by the ascii value of A == 65
                if(tokenValue < keyValue){  // If the token value is less adjust by 27 to pick the correct character
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue + 27) % 27]); // Save the decrypted character
                } else {   // If positive then pick character as normal
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue) % 27]);  // Save the decrypted character
                }
            } else {  // Check to see if both of the characters are a space
                tokenValue = token[i] - 6;  // Since this is a space subtract by the ascii value of 6
                keyValue = key[i] - 6;  // Since this is a space subtract by the ascii value of 6
                if(tokenValue < keyValue){  // If the token value is less adjust by 27 to pick the correct character
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue + 27) % 27]); // Save the decrypted character
            
                } else {  // If positive then pick character as normal
                    saveDecypt[i] = ("%c", keyCharacters[(tokenValue - keyValue) % 27]);  // Save the decrypted character
                }
            }

            i++;  // Increment i
        }

        saveDecypt[tokenLength - 1] = '\n';  // Create a newline character at the end of the line
        strcpy(decryptedToken, saveDecypt);
        saveDecypt[tokenLength] = '\0';  // Create a null terminator at the end of the line
        strcpy(decryptedToken, saveDecypt);

        return decryptedToken;  // Return the decrypted message
      
    } else {  // Check to make sure the key is longer then the message
      printf("Error: key is too short\n");
      exit(1);
    }
}

/* Error function used for reporting issues */
void error(const char* message){
    perror(message);
    exit(1);
}
