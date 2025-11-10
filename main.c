#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 16

int main(void) {
    printf("Hello!\nEnter File Location\n");
    char filename[256];
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';

    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL) {
        printf("Sorry, couldn't open file location\n");
        return EXIT_FAILURE;
    }
    char buffer[MAX_LINE_LENGTH];
    printf("Domain Names excepted are:\n");
    while (fgets(buffer, sizeof(buffer), fptr) != NULL) { // reads each line
        printf(buffer);
    }


    return EXIT_SUCCESS;
}