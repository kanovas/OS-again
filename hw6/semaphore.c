#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <queue>

int main() {
  int const CLIENTS = 5;
  int msqid;
  char pathname[] = "semaphore.c";
  key_t key;
  int i, len, maxlen;
  int s; //semaphore
  std::queue<int> clients;
  
  struct mymsgbuf {
    long mtype;
    struct {
      char operation; //'p', 'v', 'f'
      int pid;
    } info;
  } mybuf;
  
  //generating IPC key from filename for queue #0
  if ((key = ftok(pathname, 0)) < 0) {
    printf("Can\'t generate key\n");
    exit(-1);
  }
  
  //getting access to msg queue, with rw rights for all
  if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  } 
  
  maxlen = 81;
  //start listening
  while (1) {
    if ((len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 0, 0) < 0) {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    if (mybuf.info.operation == 'f') { //finish work 
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL); //delete msg queue
      exit(0);
    } 
    else if (mybuf.info.operation == 'p') {
      if (s == 0) {
	clients.push(mybuf.info.pid);
      }
      else {
	//TODO SEND MESSAGE
      }
    }
    else if (mybuf.info.operation == 'v') {
      if (s == 0) {
	s++;
	if (!clients.empty()) {
	  int client = clients.front();
	  clients.pop();
	  //TODO SEND MESSAGE
      }
    }
    else {
      printf("Received wrong message\n");
      exit(-1);
    }
  }
  
  for (int i = 0; i < CLIENTS; i++) {
    int pid = fork();
    if (pid == -1) {
      printf("Can\'t create child process\n");
      exit(-1);
    }
    else if (pid == 0) { //child process action
      proberen(); //TODO
      verhogen(); //TODO
    } 
  }
  
  
}
	
