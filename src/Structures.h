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
}


