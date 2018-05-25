#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "Structures.h"

int main(int argc, char **argv)
{

    Message msg;

    msg.msgtype = SND;
    msg.msgAct = SHUTDOWN;
    int msgID = msgget(KEY, QUEUEPERMISSION);

    if (msgID < 0)
    {
        std::cout << "Could not get message queue.\n"
                  << std::endl;
        exit(-1);
    }

    MSGSND(msgID, &msg, sizeof(Content), 0)

    AnswerMessage am;

    MSGRCV(msgID, &am, sizeof(long), SHUTDOWN, 0)
    else
    {
        if (am.jobNumber == -1)
        {
            std::cout << "Could not shutdown.\n";
        }
        else
            std::cout << "Success." << std::endl;
    }
    return 0;
}