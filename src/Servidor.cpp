#include <iostream>
#include <vector>

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
	bool running;
	struct tm startTime;
} Job;

Job jobs[NJOBS];

typedef struct process{
	long pid;
	unsigned int priority;
	bool priorityUp;
	bool ran;
} Process;

Process processes[NPROCESSES];

int msgID;

void Alarm(int a){
	std::cout<<"Alarm.\n";
}

void WrapUp(int a){
	if(msgID >= 0){
		if(msgctl(msgID, IPC_RMID, NULL) < 0){
			std::cout<<"Error while deleting queue: "<<strerror(errno)<<std::endl;
		}
	}

	exit(0);
}

void CreateProcess(int jobID){
	std::cout<<"Process created\n";
}

void Update(){
	for(int i = 0; i < NJOBS; i++){
		if(jobs[i].jobNumber != 0 && !jobs[i].running ){
			double diff = difftime(mktime(&jobs[i].startTime), time(NULL));
			std::cout<<diff<<std::endl;
			if(diff <= 0){
				CreateProcess(i);
			}
		}
	}
}


void MessageReceived(Message msg){
	std::cout<<"Contents: \n";
	std::cout<<msg.content.pid<<" "<<msg.content.processName<<std::endl;
	
	int i;	
	for(i = 0; i < NJOBS; i++){
		if(jobs[i].jobNumber == 0)
			break;
	}
	
	if (i == NJOBS){
		AnswerMessage am;
		am.msgtype = RCV;
		am.jobNumber = 0;

		msgsnd(msgID, &am, sizeof(am.jobNumber), 0);
		
		return;
	}

	Job job;
	job.jobNumber = jobNumber++;
	strcpy(job.processName, msg.content.processName);
	job.priority = msg.content.priority;
	job.copies = msg.content.copies;

	time_t now;
	time(&now);

	job.startTime = *localtime(&now);
	job.startTime.tm_hour += msg.content.hour;
	job.startTime.tm_min += msg.content.minute;
	job.running = false;
	
	jobs[i] = job;
	
	AnswerMessage am;
	am.msgtype = RCV;
	am.jobNumber = job.jobNumber;

	msgsnd(msgID, &am, sizeof(am.jobNumber), 0);
}


int main (int argc, char **argv){
	signal (SIGALRM, Alarm);
	signal (SIGTERM, WrapUp);
	signal (SIGINT, WrapUp);

	Message msg;

	memset(jobs, 0, NJOBS*sizeof(Job));
	memset(processes, 0, NPROCESSES*sizeof(Process));

	msgID = msgget(KEY, IPC_CREAT | 0660);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n";
		exit(-1);
	}
	for(;;){
		alarm(5);
		if(msgrcv(msgID, &msg, sizeof(Content), SND, 0) < 0){
			std::cout<<strerror(errno)<<std::endl;
		}else{
			alarm(0);
			std::cout<<"Message received!\n";
			MessageReceived(msg);
		}
		Update();
		//Scheduler();
	}	

	WrapUp(0);

	return 0;
}

