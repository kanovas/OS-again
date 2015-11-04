#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#define QUEUE_SIZE 1024

struct queue {
  int l, r;
  int data [QUEUE_SIZE];
};

int queue_size(const struct queue *q) {
  return q->r - q->l;
}
  
int queue_pop(struct queue *q) {
  int ans = q->data[q->l];
  (q->l)++;
  return ans;
}
  
void queue_push(struct queue *q, int val) {
  q->data[q->r] = val;
  (q->r)++;
}

int main() {
  int const CLIENTS = 10;
  int msqid;
  char pathname[] = "semaphore.c";
  key_t key;
  int i, len, maxlen;
  int s = 1; //semaphore
  struct queue clients;
  clients.l = 0; clients.r = 0;
  
  struct mymsgbuf {
    long mtype;
    struct inf {
      char operation; //'p', 'v'
      int pid;
    } info;
  } data, answer;
  
  struct mymsgbuf * mymsgbuf_init(struct mymsgbuf *buf, long mtype, char operation, int pid) {   
    buf->mtype = mtype;
    buf->info.operation = operation;
    buf->info.pid = pid;
    return buf;
  }
  //generating IPC key from filename for queue #0
  if ((key = ftok(pathname, 0)) < 0) {
    perror("Can\'t generate key\n");
    exit(-1);
  }
  
  //getting access to msg queue, with rw rights for all
  if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    perror("Can\'t get msqid\n");
    exit(-1);
  } 
  
  void send_message(struct mymsgbuf *msg) {
    int len = sizeof(msg->info);
    if (msgsnd(msqid, msg, len, 0) < 0) {
      perror("Can\'t send message to queue1\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }  
  }
  
  maxlen = 81;
  
  for (i = 0; i < CLIENTS; i++) {
    int pid = fork();
    if (pid == -1) {
      perror("Can\'t create child process\n");
      exit(-1);
    }
    else if (pid == 0) { //child process action
      int cur_pid = getpid();
      //proberen
      send_message(mymsgbuf_init(&data, 1, 'p', cur_pid));
      if (len = msgrcv(msqid, &data, maxlen, cur_pid, 0) < 0) {
	  perror("Can\'t receive message from queue2\n");
	  exit(-1);
      }  
      printf("p operation made\n");
      //verhogen
      send_message(mymsgbuf_init(&data, 1, 'v', cur_pid));
      printf("v operation made\n");
      exit(0);
    } 
  } 
  
  //start listening
  while (1) {
    if (waitpid(-1, NULL, WNOHANG) < 0) {    //children are dead, finish work
      msgctl(msqid, IPC_RMID, NULL);        //delete msg queue
      exit(0);
    }
    
    if (len = msgrcv(msqid, &data, maxlen, 1, IPC_NOWAIT) < 0) {    //not blocking msgrcv
      if (errno == ENOMSG) continue;
      perror("Can\'t receive message from queue3\n");
      exit(-1);
    }
    
    if (data.info.operation == 'p') {
      if (s == 0) {
	queue_push(&clients, data.info.pid);
      }
      else {
	s--;
	send_message(mymsgbuf_init(&answer, data.info.pid, 'p', 0));
      }
    }
    else if (data.info.operation == 'v') {
      if (s == 0) {
	s++;
	if (queue_size(&clients) > 0) {
	  int client = queue_pop(&clients);
	  s--;
	  send_message(mymsgbuf_init(&answer, client, 'p', 0));
	}
      }
    }
    else {
      perror("Received wrong message\n");
      exit(-1);
    }
  } 
  
  return 0;
}
	
