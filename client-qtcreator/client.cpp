

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define  _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR (-1)
#endif

#include <iostream>
#include <thread>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#include "EasyTcpClient.hpp"

void cmdThread2(EasyTcpClient *client1)
{
    char cmdBuffer[128];
    while (true)
    {





        //3.
        cin >> cmdBuffer;
        //4.
        if (0 == strcmp(cmdBuffer, "exit"))
        {
            cout << " 输入 exit 即将退出" << endl;
            client1->closeSocket();
            break;

        }
        else if (0 == strcmp(cmdBuffer, "login"))
        {
            Login login;
            strcpy(login.username, "wu");
            strcpy(login.password, "123456");
            client1->sendData(&login);


        }
        else if (0 == strcmp(cmdBuffer, "logout"))
        {
            Logout logout;
            strcpy(logout.username, "wu");

            client1->sendData(&logout);
        }
        else
        {
            cout << "不支持的命令" << endl;
            continue;
        }
    }


}

int main2()
{
    EasyTcpClient client1;
    client1.connectServer("192.168.2.18",4567);

   // EasyTcpClient client2;
    //client2.connectServer("192.168.12.1",4568);

    // thread t1(cmdThread, &client1);
    //t1.detach();



  //  thread t2(cmdThread, &client2);
    //t2.detach();
    Logout logout;
    strcpy(logout.username, "wu");
    while (client1.isRun())
    {
       // client1.sendData(&logout);
        //client2.sendData(&logout);

        client1.onRun();
        client1.sendData(&logout);
        //client2.onRun();
    }

    client1.closeSocket();
    //client2.closeSocket();
    return 0;
}
