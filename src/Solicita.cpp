#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "Structures.h"

#define KEY 0x40304

int msgID;

int main(int argc, char** argv){
	Message msg;

	msg.msgtype = SND;

	if(argc < 4){
		std::cout<<"Erro na entrada: \nFormato de chamada:\nsolicita_execucao <hora:min> <copias> <prioridade> <nome do executavel>\n";
		return 1;
	}

	msg.content.pid = getpid();
	char* hourminute = argv[1];
	msg.content.hour = atoi(hourminute);

	char c ;
	do{
		c = *(hourminute++);
	}while(c != ':' && c!= '\0');

	if(c == 0){
		std::cout<<"Formato de hora errada\n";
		return -1;
	}

	msg.content.minute = atoi(++hourminute);
	msg.content.copies = atoi(argv[2]);
	msg.content.priority = (argc < 4) ? atoi(argv[3]) : 1;
	strcpy(msg.content.processName, argv[(argc < 4 )?3:4]);

	msgID = msgget (KEY, 0660);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n"<<strerror(errno)<<std::endl;
		exit(-1);
	}

	MSGSND(msgID, &msg, sizeof(Content), 0)

	AnswerMessage am;

	MSGRCV(msgID, &am, sizeof(long), RCV, 0)
	else
	std::cout<<"Job Number: "<<am.jobNumber<<std::endl<<"Process name: "<<msg.content.processName<<;

	return 0;
}


