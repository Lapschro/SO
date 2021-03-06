#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "Structures.h"
#include <sstream>

std::string leftAlign(const std::string s, const int w)
{
    std::stringstream ss, spaces;
    int padding = w - s.size(); // count excess room to pad
    for (int i = 0; i < padding; ++i)
        spaces << " ";
    ss << s << spaces.str(); // format with padding
    return ss.str();
}

int main(int argc, char **argv)
{
    Message msg;

    msg.msgtype = SND;
    msg.msgAct = LIST;

    int msgID = msgget(KEY, QUEUEPERMISSION);

    if (msgID < 0)
    {
        std::cout << "Could not get message queue.\n"
                  << std::endl;
        exit(-1);
    }

    MSGSND(msgID, &msg, sizeof(Content), 0)
    ListMessage lm;
    MSGRCV(msgID, &lm, sizeof(lm), LIST, 0)
    else
    {
        if (lm.nJobs == 0)
        {
            std::cout << "No jobs to List\n\n";
        }
        else{
            std::cout << "\n\n\n";
            std::cout << "|    job    |      arq_exec      |   hhmm   |    copias    |    pri    |" << std::endl;
            char buff[10];
            std::string hhmm;
            for (int i = 0; i < lm.nJobs; i++)
            {
                
                std::cout << "|" << leftAlign(std::to_string(lm.job[i].jobNumber), 11);
                std::cout << "|" << leftAlign(lm.job[i].processName, 20);
                strftime(buff, 10, "%H:%M:%S", &lm.job[i].startTime);
                std::cout << "|" << leftAlign(buff, 10);
                std::cout << "|" << leftAlign(std::to_string(lm.job[i].copies), 14);
                std::cout << "|" << leftAlign(std::to_string(lm.job[i].priority), 11) << "|";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
    return 0;
}