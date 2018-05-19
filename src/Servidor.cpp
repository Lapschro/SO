#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "Structures.h"


#define KEY 0xb00b

int msgID;

void Alarm(int a){
	std::cout<<"Alarm.\n";
}

int main (int argc, char **argv){
	signal (SIGALRM, Alarm);
	signal (SIGTERM, Alarm);
	signal (SIGINT, Alarm);

	Message msg;
	
	msgID = msgget(KEY, IPC_CREAT | 0660);
	
	if(msgID < 0){
		std::cout<<"Could not get message queue.\n";
		exit(-1);
	}
	
	alarm(5);
	if(msgrcv(msgID, &msg, sizeof(Content), 0, 0) < 0){
		std::cout<<strerror(errno)<<std::endl;
	}else{
		std::cout<<"Message received!\n";
		std::cout<<"Contents: \n";
		std::cout<<msg.content.pid<<" "<<msg.content.processName<<std::endl;
	}

	msgctl(msgID, IPC_RMID, NULL);

	return 0;
}
