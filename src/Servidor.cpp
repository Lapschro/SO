#include <iostream>
#include <vectror>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctime>

#include "Structures.h"

#define KEY 0x40304

#define NPROCESSES 5
#define NJOBS 5

long jobNumber = 1;

typedef struct job {
	long jobNumber;
	char processName[80];
	unsigned int copies;
	unsigned int priority;
	time_t startTime;
} Job;

Job jobs[NOFJOBS];

typedef struct process{
	long pid;
	unsigned int priority;
	bool priorityUp;
	bool ran;
} Process;

Process processes[NOFPROCESSES];

int msgID;

void Alarm(int a){
	std::cout<<"Alarm.\n";
}

void WrapUp(int a){
	if(msgID > 0){
		if(msgctl(msgID, IPC_RMID, NULL) < 0){
			std::cout<<"Error while deleting queue: "<<strerror(errno)<<std::endl;
		}
	}
}

int main (int argc, char **argv){
	signal (SIGALRM, Alarm);
	signal (SIGTERM, WrapUp);
	signal (SIGINT, WrapUp);

	Message msg;

	memset(jobs, 0, NOFJOBS*sizeof(Job));
	memset(processes, 0, NOFPROCESSES*sizeof(Process));

	msgID = msgget(KEY, IPC_CREAT | 0660);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n";
		exit(-1);
	}
	
	alarm(5);
	if(msgrcv(msgID, &msg, sizeof(Content), SND, 0) < 0){
		std::cout<<strerror(errno)<<std::endl;
	}else{
		std::cout<<"Message received!\n";
		alarm(0);
		std::cout<<"Contents: \n";
		std::cout<<msg.content.pid<<" "<<msg.content.processName<<std::endl;
	}

	AnswerMessage am;
	am.msgtype = RCV;
	am.jobNumber = 1.;

	msgsnd(msgID, &am, sizeof(am.jobNumber), 0);

	WrapUp(0);

	return 0;
}

