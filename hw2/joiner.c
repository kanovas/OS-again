#include <sys/syscall.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define PERMS 0666

int print_err(char* error, int code) {
  write(2, error, strlen(error));
  return code;
}

int main(int argc, char* argv[]) {
  
  
  char* error0 = "Usage: join input_file1 input_file2 output_file\n";
  char* error1 = "Unable to open or create file due to permissions\n";
  char* error2 = "Read/write error\n"; 
  
  char buf[BUFSIZ];
  int n, m, input_file, output_file;
  
  if (argc != 4) {
    return print_err(error0, 1);
  }
  
  output_file = creat(argv[3], PERMS);
  if (output_file < 0) {
    return print_err(error1, 2);
  }
  
  //first file
  input_file = open(argv[1], O_RDONLY | O_CREAT, PERMS); 
  if (input_file < 0) {
    return print_err(error1, 2);
  }
  
   while ((n = read(input_file, buf, BUFSIZ)) > 0) {
     while (n > 0 && (m = write(output_file, buf, n)) > 0) {
       n -= m;
     }
     if (n != 0 || m == -1) {
       return print_err(error2, 3);
      }
    }   
  close(input_file);
  
  
  //second file
  input_file = open(argv[2], O_RDONLY | O_CREAT, PERMS); 
  if (input_file < 0) {
    return print_err(error1, 2);
  }
  
  while ((n = read(input_file, buf, BUFSIZ)) > 0) {
    while (n > 0 && (m = write(output_file, buf, n)) > 0) {
      n -= m;
    }
    if (n != 0 || m == -1) {
      return print_err(error2, 3);
    }
  }
  
  close(input_file);
  close(output_file);
  
  return 0;
  
}