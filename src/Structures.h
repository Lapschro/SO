#define KEY 0x40304
#define QUEUEPERMISSION 0660

#define MSGSND(msgID,ptr,size,flag) if(msgsnd(msgID,ptr,size,flag) < 0) std::cout<<"Error on send: "<<strerror(errno)<<std::endl;

#define MSGRCV(msgID,ptr,size,type,flag) if(msgrcv(msgID,ptr,size,type,flag) < 0) std::cout<<"Error on receive: "<<strerror(errno)<<std::endl;

enum MSGType{
	SND = 1, RCV, RMV, LIST, SHUTDOWN
};
typedef struct job
{
	long jobNumber;
	char processName[80];
	unsigned int copies;
	time_t subm_time;
	unsigned int priority;
	bool running;
	struct tm startTime;
} Job;
typedef struct Content{
	long pid;
	unsigned int hour, minute;
	unsigned int copies;
	unsigned int priority;
	char processName[80];
} Content;

typedef struct Message{
	long msgtype;
	int msgAct;
	Content content;
} Message;

typedef struct AnswerMessage{
	long msgtype;
	long jobNumber;
} AnswerMessage;

typedef struct ListMessage
{
	long msgtype;
	int nJobs;
	Job job[20];
} ListMessage;
