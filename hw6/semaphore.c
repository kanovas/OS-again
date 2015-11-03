#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>

struct queue {
  int const SIZE = 1024;
  int l = 0, r = 0;
  int data [SIZE];
  
  int size() {
    return r - l;
  }
  
  bool empty() {
    return size() > 0;
  }
  
  int pop() {
    int ans = data[l];
    l++;
    return ans;
  }
  
  void push(int & val) {
    data[r] = val;
    r++;
  }
}

int main() {
  int const CLIENTS = 5;
  int msqid;
  char pathname[] = "semaphore.c";
  key_t key;
  int i, len, maxlen;
  int s = 1; //semaphore
  queue clients;
  
  struct mymsgbuf {
    long mtype;
    struct {
      char operation; //'p', 'v', 'f'
      int pid;
    } info;
    
    mymsgbuf(long& mtype_, char& operation_, int& pid_) {
      mtype = mtype_;
      operation = operation_;
      pid = pid_;
    }
  } data;
  
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
  
  void send_message(mymsgbuf & msg) {
    int len = sizeof(msg);
    if (msgsnd(msqid, (struct msgbuf *) &msg, len, 0) < 0) {
      printf("Can\'t send message to queue\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }  
  }
  
  maxlen = 81;
  //start listening
  while (1) {
    if ((len = msgrcv(msqid, (struct msgbuf *) &data, maxlen, 1, 0) < 0) {
      printf("Can\'t receive message from queue\n");
      exit(-1);
    }
    if (data.info.operation == 'f') { //finish work 
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL); //delete msg queue
      exit(0);
    } 
    else if (data.info.operation == 'p') {
      if (s == 0) {
	clients.push(data.info.pid);
      }
      else {
	s--;
	send_message(mymsgbuf(data.info.pid, 'p', 0));
      }
    }
    else if (data.info.operation == 'v') {
      if (s == 0) {
	s++;
	if (!clients.empty()) {
	  int client = clients.pop();
	  send_message(mymsgbuf(client, 'p', 0));
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
      int cur_pid = getpid();
      //proberen
      send_message(mymsgbuf(1, 'p', cur_pid));
      if ((len = msgrcv(msqid, (struct msgbuf *) &data, maxlen, cur_pid, 0) < 0) {
	  printf("Can\'t receive message from queue\n");
	  exit(-1);
      }   
      //verhogen
      send_message(mymsgbuf(1, 'v', cur_pid));
    } 
  }
  
  send_message(mymsgbuf(1, 'f', 0););
}
	
