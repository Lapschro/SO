#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "Structures.h"




int msgID;

int main(int argc, char** argv){
	Message msg;

	msg.msgtype = SND;

	if(argc < 4){
		std::cout<<"Erro na entrada: \nFormato de chamada:\nsolicita_execucao <hora:min> <copias> [<prioridade>] <nome do executavel>\n";
		return 1;
	}

	msg.content.pid = getpid();
	char* hourminute = argv[1];

	if(strlen(hourminute) != 5){
		std::cout << "Formato de hora errada\n";
		return -1;
	}else{
		/*cast hour and minute*/
		const char delimiters[] = ":";
		char *hours;
		char *minutes;
		minutes = strdupa(hourminute);
		hours = strsep(&minutes, delimiters); /* token => "words" */
		msg.content.hour = atoi(hours);
		msg.content.minute = atoi(minutes);
	}

	
	msg.content.copies = atoi(argv[2]);
	msg.content.priority = (argc < 4) ? 1 : atoi(argv[3]) ;

	if (msg.content.priority <1){
		msg.content.priority = 1;
	}else if(msg.content.priority > 3){
		msg.content.priority = 3;
	}
	
	strcpy(msg.content.processName, argv[(argc < 4 )?3:4]);

	msgID = msgget(KEY, QUEUEPERMISSION);

	if(msgID < 0){
		std::cout<<"Could not get message queue.\n"<<std::endl;
		exit(-1);
	}
	MSGSND(msgID, &msg, sizeof(Content), 0)

	AnswerMessage am;

	MSGRCV(msgID, &am, sizeof(long), RCV, 0)
	else{
		if(am.jobNumber == 0){
			std::cout<<"Could not create job.\n";
		}else
			std::cout << "Job Number: " << am.jobNumber << std::endl
					  << " Process name: " << msg.content.processName << std::endl
					  << " Time: " << msg.content.hour << ":" << msg.content.minute << std::endl
					  << " Priority: " << msg.content.priority << std::endl;
	}
	return 0;
}


