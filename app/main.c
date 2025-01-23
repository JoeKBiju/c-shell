#include <stdio.h>
#include <string.h>

int main() {
  // READ-EVAL-PRINT-LOOP
  while (1) {
    // Flush after every printf
    setbuf(stdout, NULL);

    printf("$ ");

    // Wait for user input
    char input[100];
    fgets(input, 100, stdin);

    //Handle Input
    //When inputing command the newline char is put into the buffer when pressing "ENTER".
    // '/0' is the NULL character whcich represents the end of a string.
    input[strlen(input)-1] = '\0';
    printf("%s: command not found\n", input);
  }
  return 0;
}
