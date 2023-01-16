#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  // Get expression
  char* code = NULL;
  size_t len = 0;
  ssize_t nread = getline(&code, &len, stdin);
  // Save the '%' symbol to a variable because it's so frustrating to screen it
  char percent = '%';
  // Write a C program that prints our expression to a file 
  FILE* c_file = fopen("code.c", "w");
  fprintf(c_file, "#include <stdio.h>\nint main() {\nprintf(\"%cd\", (%s));\nreturn 0;\n}", percent, code);
  fclose(c_file);
  free(code);
  // Create two processes:
  pid_t pid = fork();
  // Check for errors
  if (pid == -1) {
    perror("fork");
    _exit(1);
  }
  // 1) The child process will execute gcc to compile our code
  if (pid == 0) {
    execlp("/usr/bin/gcc", "/usr/bin/gcc", "-o", "code", "code.c", NULL);
  }
  // 2) The parent process will wait for the child to complete and then execute the newly compiled program
  else {
    waitpid(pid, NULL, 0);
    execlp("./code", "./code", NULL);
  }

  // Why are we still here? Surely, execlp has failed.
  perror("execlp");
  _exit(1);
  return 0;
}
