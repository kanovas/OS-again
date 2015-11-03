#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
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
  int const CLIENTS = 5;
  int msqid;
  char pathname[] = "semaphore.c";
  key_t key;
  int i, len, maxlen;
  int s = 1; //semaphore
  struct queue clients;
  clients.l = 0; clients.r = 0;
  
  struct mymsgbuf {
    long mtype;
    struct {
      char operation; //'p', 'v', 'f'
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
    printf("Can\'t generate key\n");
    exit(-1);
  }
  
  //getting access to msg queue, with rw rights for all
  if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t get msqid\n");
    exit(-1);
  } 
  
  void send_message(struct mymsgbuf *msg) {
    int len = sizeof(msg);
    if (msgsnd(msqid, (struct msgbuf *) msg, len, 0) < 0) {
      printf("Can\'t send message to queue1\n");
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
      exit(-1);
    }  
  }
  
  maxlen = 81;
  
  for (i = 0; i < CLIENTS; i++) {
    int pid = fork();
    if (pid == -1) {
      printf("Can\'t create child process\n");
      exit(-1);
    }
    else if (pid == 0) { //child process action
      int cur_pid = getpid();
      //proberen
      send_message(mymsgbuf_init(&data, 1, 'p', cur_pid));
      if (len = msgrcv(msqid, (struct msgbuf *) &data, maxlen, cur_pid, 0) < 0) {
	  printf("Can\'t receive message from queue2\n");
	  exit(-1);
      }   
      //verhogen
      send_message(mymsgbuf_init(&data, 1, 'v', cur_pid));
    } 
  }
  
  send_message(mymsgbuf_init(&data, 1, 'f', 0));
  
  //start listening
  while (1) {
    if (len = msgrcv(msqid, (struct msgbuf *) &data, maxlen, 1, 0) < 0) {
      printf("Can\'t receive message from queue3\n");
      exit(-1);
    }
    if (data.info.operation == 'f') { //finish work 
      msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL); //delete msg queue
      exit(0);
    } 
    else if (data.info.operation == 'p') {
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
	  send_message(mymsgbuf_init(&answer, client, 'p', 0));
	}
      }
    }
    else {
      printf("Received wrong message\n");
      exit(-1);
    }
  }
  
  return 0;
}
	
