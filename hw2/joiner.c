#include <sys/syscall.h>
#include <fcntl.h>
#include <stdio.h>

#define PERMS 0666

int main(int argc, char* argv[]) {
  
  char buf[BUFSIZ];
  int n, m, input_file, output_file;
  
  //TODO: check number of arguments
  
  output_file = creat(argv[3], PERMS);
  if (output_file < 0) {
    //TODO: report error to 2
    return 1;
  }
  
  input_file = open(argv[1], O_RDONLY | O_CREAT, PERMS); 
  if (input_file < 0) {
    //TODO: report error to 2
    return 1;
  }
  
   while ((n = read(input_file, buf, BUFSIZ)) > 0) {
     while (n > 0 && (m = write(output_file, buf, n)) > 0) {
       n -= m;
     }
     if (n != 0 || m == -1) {
       return 2;
      }
    }
  
  input_file = open(argv[2], O_RDONLY | O_CREAT, PERMS); 
  if (input_file < 0) {
    //TODO: report error to 2
    return 1;
  }
  
  while ((n = read(input_file, buf, BUFSIZ)) > 0) {
    while (n > 0 && (m = write(output_file, buf, n)) > 0) {
      n -= m;
    }
    if (n != 0 || m == -1) {
      return 2;
    }
  }
  
  close(input_file);
  close(output_file);
  
  return 0;
  
}