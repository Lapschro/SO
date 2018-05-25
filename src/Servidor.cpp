#include <iostream>
#include <vector>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <wait.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <ctime>

#include "Structures.h"

#define NPROCESSES 20
#define NJOBS 20

long jobNumber = 1;
long currentProcess = -1;
unsigned int runningTime = 5;

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
	int jobID;
	bool priorityUp;
	bool ran;
} Process;

Process processes[NPROCESSES];

int msgID;

void Alarm(int a){
	runningTime = 5;
	//std::cout<<"Alarm.\n";
}

void WrapUp(int a){
	if(msgID >= 0){
		if(msgctl(msgID, IPC_RMID, NULL) < 0){
			std::cout<<"Error while deleting queue: "<<std::endl;
		}
	}

	exit(0);
}

void CreateProcess(int jobID){
	unsigned int freeSpaces = 0;
	for(int j = 0 ; j < NPROCESSES; j++){
		if(processes[j].pid == 0)
			freeSpaces++;
	}

	if(freeSpaces < jobs[jobID].copies){
		std::cout<<"Processes table is full.\n";
		return;
	}

	for(unsigned int i = 0; i < jobs[jobID].copies; i++){
		int j;
		for(j = 0; j < NPROCESSES; j++){
			if(processes[j].pid == 0)
				break;
		}
		
		Process p;
		p.priority = jobs[jobID].priority;
		p.jobID = jobID;
		p.priorityUp = false;
		p.ran = false;
		p.pid = fork();
		
		if(p.pid == 0){
			execl(jobs[jobID].processName, jobs[jobID].processName, (char*)NULL);
			std::cout<<strerror(errno)<<std::endl;
			jobs[jobID].jobNumber = 0;
			return;
		}
		
		//std::cout<<"Stopping pid: "<<p.pid<<std::endl;
		kill(p.pid, SIGSTOP);

		processes[j] = p;
	}
	jobs[jobID].running = true;
}

void Update(){
	if(currentProcess != -1){
		kill(processes[currentProcess].pid, SIGSTOP);

		int status;

		if(/*WIFEXITED(status)*/waitpid(currentProcess, &status, WNOHANG)>0){
			jobs[processes[currentProcess].jobID].copies--;

			processes[currentProcess].pid = 0;

			for(int i = 0;  i < NJOBS; i++){
				if(jobs[i].copies == 0){
					jobs[i].jobNumber = 0;
					jobs[i].running = false;
				}
			}
		}else{
			if(processes[currentProcess].ran){
				processes[currentProcess].ran = false;
				if(processes[currentProcess].priority == 3){
					processes[currentProcess].priorityUp = true;
				}
				if(processes[currentProcess].priority == 0){
					processes[currentProcess].priorityUp = false;
				}

				processes[currentProcess].priority += processes[currentProcess].priorityUp?-1:1;
			}else{
				processes[currentProcess].ran = true;
			}
			
			if(runningTime == 5){
				Process p = processes[currentProcess];
				memcpy(processes, &processes[1], (NPROCESSES-1)*sizeof(Process));
				processes[NPROCESSES -1] = p;
			}
		}
	}

	for(int i = 0; i < NJOBS; i++){
		if(jobs[i].jobNumber != 0 && !jobs[i].running ){
			double diff = difftime(mktime(&jobs[i].startTime), time(NULL));
			//std::cout<<diff<<std::endl;
			if(diff <= 0){
				CreateProcess(i);
			}
		}
	}
}

int ProcessCompare(const void* a, const void* b){
	Process pa = *(Process*)a;
	Process pb = *(Process*)b;

	if(pa.pid == 0 && pb.pid == 0)
		return 0;

	if(pa.pid == 0)
		return 1;
	
	if(pb.pid == 0)
		return -1;

	return (int)pa.priority - (int)pb.priority;
}

void Scheduler(){

	qsort((void*)processes, NPROCESSES, sizeof(Process), ProcessCompare);

	if(processes[0].pid == 0){
		currentProcess = -1;
		return;
	}
	if(currentProcess == -1)
		runningTime = 5;
	currentProcess = 0;
	kill(processes[0].pid, SIGCONT);
}

void MessageReceived(Message msg){
	//std::cout<<"Contents: \n";
	//std::cout<<msg.content.pid<<" "<<msg.content.processName<<std::endl;
	
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

void removeJob(long jobID)
{
	if(jobID>0){

		if(jobs[jobID-1].running == 0 and jobs[jobID-1].jobNumber != 0){

			jobs[jobID - 1].jobNumber = 0;
			AnswerMessage am;
			am.msgtype = RMV;
			am.jobNumber = jobID;
			msgsnd(msgID, &am, sizeof(am.jobNumber), 0);
			return;
		}
	}
	AnswerMessage am;
	am.msgtype = RMV;
	am.jobNumber = -1;
	msgsnd(msgID, &am, sizeof(am.jobNumber), 0);

}

void shutdown()
{
	/* Essa função precisa avisar para o usuário que mais nenhum processo será executado
	*  se houver processos não executados o processo avisa ao usuário que os mesmos não serão executados
	*  e imprime dados de cada processo que foi efetivamente executado no período de atividade do servidor, contendo
	*  pid do processo, nome do arquivo executável, tempo de submissão, tempo de início de execução, tempo de término de execução.
	*/
	raise(SIGTERM);
}

int main (int argc, char **argv){
	signal (SIGALRM, Alarm); /*Assincrono, usado quando tem uma chamada a funcao alarm*/
	signal (SIGTERM, WrapUp); /*TERMINATION*/
	signal (SIGINT, WrapUp); /*INTERRUPCAO*/

	Message msg;
	runningTime = 5;

	memset(jobs, 0, NJOBS*sizeof(Job));
	memset(processes, 0, NPROCESSES*sizeof(Process));

	msgID = msgget(KEY, IPC_CREAT | QUEUEPERMISSION);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n";
		exit(-1);
	}
	for(;;){
		alarm(runningTime);
		//std::cout<<runningTime<<std::endl;
		if(msgrcv(msgID, &msg, sizeof(Content), -1, 0) >= 0){
			if(msg.msgAct == SND){
				std::cout<<"certo";
				runningTime = alarm(0);
				//std::cout<<"Message received!\n";
				MessageReceived(msg);
			}
			else if(msg.msgAct == RMV){
				removeJob(msg.content.pid);

			}
			else if(msg.msgAct == SHUTDOWN){
				shutdown();
			}
		}
		Update();
		Scheduler();
	}	

	WrapUp(0);

	return 0;
}

