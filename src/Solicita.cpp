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

int main(int argc, char** argv){
	Message msg;

	msg.msgtype = SND;

	msg.content.pid = getpid();
	msg.content.hour = 0;
	msg.content.minute = 0;
	msg.content.copies = 0;
	msg.content.priority = 0;
	strcpy(msg.content.processName, "Hello World");

	msgID = msgget (KEY, 0660);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n"<<strerror(errno)<<std::endl;
		exit(-1);
	}

	MSGSND(msgID, &msg, sizeof(Content), 0)
	else
	std::cout<<"Message sent!\n";
	
	AnswerMessage am;

	MSGRCV(msgID, &am, sizeof(long), RCV, 0)
	else
	std::cout<<"Message received! Job Number: "<<am.jobNumber<<std::endl;

	return 0;
}


