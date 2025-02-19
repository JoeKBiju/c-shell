#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Commands built into this shell
const char builtins[][10] = {"cd", "pwd", "type", "echo", "exit"};

// Gets the full path address of a command in PATH
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

// Executes a program which is in PATH
int executeProgram(char* command) {
  char* args;
  args = strtok(command, " ");
  
  char* argArr[10];
  int i = 0;
  while (args != NULL && i < 10) {
    argArr[i++] = args;
    args = strtok(NULL, " ");
  }
  argArr[i] = NULL;  // Need to end args array with NULL or will cause error
  
  char* programPath = checkPATH(argArr[0]);
  if (programPath == NULL) {                // ERROR: Program not found
    //printf("Program not in PATH");
    return 1;
  }
  if (args != NULL) {                       // There were more than 10 arguements
    //printf("ERROR: Too many arguements");
    return 1;
  }

  // Fork and execute program
  pid_t pid = fork();
  if (pid == 0) {
    execv(programPath, argArr);
    perror("execv");
    exit(1);
  } else if (pid < 0) {
    perror("fork");
    return 1;
  } else {
    int programExited;
    waitpid(pid, &programExited, 0);
  }

  return 0;
}

// Parses input string to identify commands and arguements
int parseCommand(char* command, int len) {
  char word[50];
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
    
    for(size_t j = 0; j < sizeof(builtins) / 10; j++) {
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

  } else if (!strcmp(word, "pwd")) {
    
    char currDir[1024];
    if (getcwd(currDir, sizeof(currDir)) != NULL) { 
      printf("%s\n", currDir);
    } else {
      perror("getcwd: ");
    }

 } else if (!strcmp(word, "cd")) {
    
    if(!strcmp(command+i+1, "~")) {
      char* username = getlogin();
      if(username == NULL) {
        perror("getlogin: ");
      }
      char userHomePath[1024];
      snprintf(userHomePath, sizeof(userHomePath), "/home/%s", username);

      if(chdir(userHomePath) == -1) {
        printf("cd: %s: Something went wrong!\n", userHomePath);
      }

    } else {

      if((chdir(command+i+1)) == -1) {
        printf("cd: %s: No such file or directory\n", command+i+1);
      }

    }

  } else {
    
    int programExecStatus = executeProgram(command);
    if (programExecStatus == 1) {
      printf("%s: command not found\n", command);
    }
  
  }

  fflush(stdout);
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
