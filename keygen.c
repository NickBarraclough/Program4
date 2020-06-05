
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]){
    if (argc == 2){
        int commandArgLength;
        int buffer;
        int i;
        int pickedIndex;
        char* keyCharacters;
        srand(time(NULL));  // Set the seed of the random number generator.

        commandArgLength = atoi(argv[1]);  // Convert the string into an integer.
        buffer = commandArgLength + 1;  // Buffer is used to set the size of the key string.
        keyCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";  // String of characters that are used for random number selection.

        char keyGen[buffer];

        for(i = 0; i < commandArgLength; i++){  // Loop until the length of key has been met
            pickedIndex = rand() % 27;  // Generate a random number.
            keyGen[i] = keyCharacters[pickedIndex];  // Save random character into key string
        }

        keyGen[commandArgLength] = '\0';  // Once key has been created assign a NULL terminator to the end.

        printf("%s\n", keyGen);  // Print out the created key.
        
    } else {
        
        fprintf(stderr, "Too few command line arguments\n");  // Error handling, in which the number of arguments in not correct.
        exit(0);
    }
    
    return 0;
}
