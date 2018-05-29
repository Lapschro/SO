#include <iostream>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <wait.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctime>
#include <iomanip>
#include <vector>
#include "Structures.h"
#include <stdlib.h>
#include <sstream>
#define NPROCESSES 20
#define NJOBS 20

long jobNumber = 1;
long currentProcess = -1;
long currentpid = 0;
unsigned int runningTime = 5;
Job jobs[NJOBS];

typedef struct process{
	std::string exec_name;
	long pid;
	long oldpid;
	unsigned int priority;
	int jobID;
	bool priorityUp;
	bool ran;
	time_t begin, end, subm_time;

} Process;

Process processes[NPROCESSES];
std::vector<Process> ranProcesses;
int msgID;
std::string leftAlign(const std::string s, const int w)
{
	std::stringstream ss, spaces;
	int padding = w - s.size();
	for (int i = 0; i < padding; ++i)
		spaces << " ";
	
	ss << s << spaces.str(); 
	return ss.str();
}
void Alarm(int a){
	runningTime = 5;
	//std::cout<<"Alarm.\n";
}

void WrapUp(int a){
	/* Os indices abaixo são os tamanhos entre as |, pra organizar a tabela*/
	/*pid 11 ; arq_exec 32 ; submission 39 ; begin 29 ; end 27*/

	std::cout<<"\n\n\n";
	std::cout << "|    pid    |      arq_exec      |   submission_time   |    begin    |    end    |"<<std::endl;
	char buff[20];

	for(auto it:ranProcesses){
		std::cout<<"|"<<leftAlign(std::to_string(it.oldpid), 11);
		std::cout<<"|"<<leftAlign(it.exec_name, 20);
		strftime(buff, 20, "%T", localtime(&it.subm_time));
		std::cout<<"|"<<leftAlign(buff, 21);
		strftime(buff, 20, "%T", localtime(&it.begin));
		std::cout<<"|"<<leftAlign(buff, 13);
		strftime(buff, 20, "%T", localtime(&it.end));
		std::cout<<"|"<<leftAlign(buff, 11)<<"|";
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
	if (msgID >= 0)
	{
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
		p.exec_name = jobs[jobID].processName;
		p.priority = jobs[jobID].priority;
		p.jobID = jobID;
		p.priorityUp = false;
		p.subm_time =jobs[jobID].subm_time;
		p.ran = false;
		time(&p.begin);
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

		if(waitpid(currentProcess, &status, WNOHANG)>0){
			jobs[processes[currentProcess].jobID].copies--;
			time(&processes[currentProcess].end);
			processes[currentProcess].oldpid = processes[currentProcess].pid;
			processes[currentProcess].pid = 0;
			ranProcesses.push_back(processes[currentProcess]);
			for(int i = 0;  i < NJOBS; i++){
				if(jobs[i].copies == 0){
					jobs[i].jobNumber = 0;
					jobs[i].running = false;
				}
			}
		}else if (runningTime == 5){
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
	if(processes[0].pid != currentpid)
		runningTime = 5;
	currentpid = processes[0].pid;
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
	time(&job.subm_time);
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
	AnswerMessage am;

	if(jobID>0){

		if(jobs[jobID-1].running == 0 and jobs[jobID-1].jobNumber != 0){

			jobs[jobID - 1].jobNumber = 0;
			am.msgtype = RMV;
			am.jobNumber = jobID;
			msgsnd(msgID, &am, sizeof(am.jobNumber), 0);
			return;
		}
	}
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
	std::cout<<"\nFrom now on no job will execute\n\nEXECUTED JOBS\n";
	raise(SIGTERM);
}

void listar_postergados(){
	/*Essa funcao vai enviar todos os dados dos jobs já adicionado na lista de JOBS de acordo com o exemplo*/
	int i;
	ListMessage lm;

	for (i = 0; i < NJOBS; i++)
	{
		if (jobs[i].jobNumber == 0)
			break;
	}

	if(i==0){
		lm.nJobs = 0;
		lm.msgtype = LIST;
		msgsnd(msgID, &lm, sizeof(lm),0);
		return;
	}else{
		lm.msgtype = LIST;
		lm.nJobs = i;
		for(int j = 0;j <i ;j++){
			lm.job[j] = jobs[j];
		}
		msgsnd(msgID, &lm, sizeof(lm), 0);
		return;
	}
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
				runningTime = alarm(0);
				//std::cout<<"Message received!\n";
				MessageReceived(msg);
			}
			else if(msg.msgAct == RMV){
				removeJob(msg.content.pid);

			}
			else if(msg.msgAct == SHUTDOWN){
				shutdown();
			}else if(msg.msgAct == LIST){
				listar_postergados();
			}
		}
		Update();
		Scheduler();
	}	

	WrapUp(0);

	return 0;
}

