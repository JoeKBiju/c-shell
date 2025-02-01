#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

const char builtins[][12] = {"type", "echo", "exit"};

char* checkPATH(char* command) {
  char path[1024];
  char* envPath = getenv("PATH");
  char* dirs;

  if (envPath != NULL) {
    strncpy(path, envPath, sizeof(path));
    dirs = strtok(path, ":");
  } else {
    return NULL;
  }

  static char fullCmdPath[1024];

  while(dirs) {
    snprintf(fullCmdPath, sizeof(fullCmdPath), "%s/%s", dirs, command);
    if (access(fullCmdPath, X_OK) == 0) {
      return fullCmdPath;
    }
    dirs = strtok(NULL, ":");
  }

  return NULL;
}

int parseCommand(char* command, int len) {
  char word[100];
  int i = 0;
  while (*(command+i) != ' ' && *(command+i) != '\0') {
    word[i] = *(command + i);
    i++;
  }
 
  word[i] = '\0';
  if (!strcmp(word, "exit")) {
    if (*(command + i+1) == '0') { return 1; }
  } else if (!strcmp(word, "echo")) {
    printf("%s\n", command+i+1);
  } else if (!strcmp(word, "type")) {
    
    for(size_t j = 0; j < sizeof(builtins) / 12; j++) {
      if(!strcmp(command+i+1, builtins[j])) {
        printf("%s is a shell builtin\n", builtins[j]);
        return 0;
      }
    }
    
    char* found = checkPATH(command+i+1);
    if (found) {
      printf("%s is %s\n", command+i+1, found);
      return 0;
    }

    printf("%s: not found\n", command+i+1);
  } else {
    printf("%s: command not found\n", command);
  }
  return 0;
}

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
    
    int output = parseCommand(input, strlen(input));
    if (output) { return 0; }
  }
  return 0;
}
