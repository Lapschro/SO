#define KEY 0x40304
#define QUEUEPERMISSION 0660
#define MSGSND(msgID,ptr,size,flag) if(msgsnd(msgID,ptr,size,flag) < 0) std::cout<<"Error on send: "<<strerror(errno)<<std::endl;

#define MSGRCV(msgID,ptr,size,type,flag) if(msgrcv(msgID,ptr,size,type,flag) < 0) std::cout<<"Error on receive: "<<strerror(errno)<<std::endl;

enum MSGType{
	SND = 1, RCV
};

typedef struct Content{
	long pid;
	unsigned int hour, minute;
	unsigned int copies;
	unsigned int priority;
	char processName[80];
} Content;

typedef struct Message{
	long msgtype;
	Content content;
} Message;

typedef struct AnswerMessage{
	long msgtype;
	long jobNumber;
} AnswerMessage;


