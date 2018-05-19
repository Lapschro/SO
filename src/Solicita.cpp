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

int main(){
	Message msg;

	msg.msgtype = 1;

	msg.content.pid = getpid();
	msg.content.hour = 0;
	msg.content.minute = 0;
	msg.content.copies = 0;
	msg.content.priority = 0;
	strcpy(msg.content.processName, "Hello World");

	msgID = msgget (KEY, IPC_CREAT | 0660);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n"<<strerror(errno);
		exit(-1);
	}

	if(msgsnd(msgID, &msg, sizeof(Content), 0) < 0){
		std::cout<<strerror(errno)<<std::endl;
	}else
	std::cout<<"Message sent!\n";

	return 0;
}


