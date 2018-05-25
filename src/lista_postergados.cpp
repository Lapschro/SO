#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "Structures.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "remove_postergado <id>";
        return 0;
    }

    Message msg;

    msg.msgtype = SND;
    msg.msgAct = RMV;
    if (argc < 2)
    {
        std::cout << "Erro na entrada: \nFormato de chamada:\nremove_postergado <jobId>\n";
        return 1;
    }
    long jobId = atoi(argv[1]);
    msg.content.pid = jobId; /*adicionar jobId depois*/

    int msgID = msgget(KEY, QUEUEPERMISSION);

    if (msgID < 0)
    {
        std::cout << "Could not get message queue.\n"
                  << std::endl;
        exit(-1);
    }

    MSGSND(msgID, &msg, sizeof(Content), 0)

    AnswerMessage am;

    MSGRCV(msgID, &am, sizeof(long), RMV, 0)
    else
    {
        if (am.jobNumber == -1)
        {
            std::cout << "Could not remove job - job running or doesn't exist.\n";
        }
        else
            std::cout << "Job Number: " << am.jobNumber << std::endl
                      << " was removed: " << std::endl;
    }
    return 0;
}