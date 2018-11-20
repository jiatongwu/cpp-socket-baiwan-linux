#ifndef _EasyTcpClient_hpp_
#define  _EasyTcpClient_hpp_


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
#include "MessageHeader.hpp"
using namespace std;
class EasyTcpClient
{
private:
    SOCKET _sock;
public:
    EasyTcpClient()
    {
        _sock = INVALID_SOCKET;
    }

    virtual ~EasyTcpClient()
    {
        closeSocket();
    }

    void initSocket()
    {

#ifdef _WIN32
        WORD ver = MAKEWORD(2, 2);
        WSADATA dat;
        WSAStartup(ver, &dat);
#endif
        if (INVALID_SOCKET != _sock)
        {
            closeSocket();
            cout<<"_sock是有效的socket 重新初始化它"<<endl;
        }
        ///
        //1.socket
        _sock = socket(AF_INET, SOCK_STREAM, 0);
        if (INVALID_SOCKET == _sock)
        {
            cout << "create socket failed" << endl;

        }
        else
        {
            cout << "create socket success" << endl;
        }



    }
    //
    int connectServer(const char* ip,unsigned short port)
    {
        int ret = -1;
        if (INVALID_SOCKET == _sock)
        {
            initSocket();
        }
        //2.
        sockaddr_in _in;
        _in.sin_family = AF_INET;
        _in.sin_port = htons(port);
#ifdef _WIN32
        _in.sin_addr.S_un.S_addr = inet_addr(ip);
#else
        _in.sin_addr.s_addr = inet_addr(ip);
#endif
        ret = connect(_sock, (sockaddr*)&_in, sizeof(_in));
        if (SOCKET_ERROR == ret)
        {
            cout << "connect failed" << endl;

        }
        else
        {
            cout << "connect success" << endl;
        }

        return  ret;

    }
    //socket
    void closeSocket()
    {
        if(INVALID_SOCKET!=_sock)
        {
            //4.socket
#ifdef _WIN32
            closesocket(_sock);
            WSACleanup();
#else
            close(_sock);
#endif
            _sock = INVALID_SOCKET;
            cout << "close socket" << endl;
        }




    }
    //select 处理
    bool onRun()
    {
        if(isRun())
        {

            fd_set fdReads;
            FD_ZERO(&fdReads);

            FD_SET(_sock, &fdReads);
            timeval t = {0,0};
            int ret = select(_sock+1, &fdReads, NULL, NULL,&t);
            if (ret < 0)
            {
                cout<<"select return <0   "<<endl;

                _sock = INVALID_SOCKET;
                return false;
            }
            if (FD_ISSET(_sock, &fdReads))
            {
                FD_CLR(_sock,&fdReads);
                if (-1 == recvData())
                {
                    closeSocket();
                    cout<<" 接收数据时，返回－1 "<<endl;
                    return false;
                }
                return true;

            }
        }
        return  false;

    }
    //处理粘包 半包
#define RECV_BUFFER_SIZE 10240
    //recv buffer
    char _recvBuffer[RECV_BUFFER_SIZE]={};
    //message buffer
    char _messageBuffer[RECV_BUFFER_SIZE*10]={};
    int _lastPos=0;
    int recvData()
    {

        // DataHeader* header;
        //copy recv buffer to message buffer
        int nLen = recv(_sock, _recvBuffer,RECV_BUFFER_SIZE, 0);
        if (nLen <= 0)
        {
            cout << "接收到的数据<=0 " << endl;
            return -1;
        }
        memcpy(_messageBuffer+_lastPos,_recvBuffer,nLen);
        _lastPos+=nLen;

        //test message buffer while has a message enough
        while(_lastPos>=sizeof(DataHeader))
        {
            DataHeader* dataHeader=(DataHeader*)_messageBuffer;
            if(_lastPos>=dataHeader->dataLength)
            {
                // process the enough message and change new message buffer and _lastPos
                int newLastPos=_lastPos-dataHeader->dataLength;
                onNetMessage(dataHeader);
                memcpy(_messageBuffer,_messageBuffer+dataHeader->dataLength,newLastPos);
                _lastPos=newLastPos;
            }else
            {
                break;
            }
        }



        return 0;
    }
    void onNetMessage(DataHeader* header)
    {

        switch (header->cmd)
        {
        case CMD_LOGIN_RESULT:
        {
            LoginResult* loginResult=NULL;

            loginResult = (LoginResult*)header;
           // cout << "接收到logoutResult: " << loginResult ->result<< endl;

            break;
        }
        case CMD_LOGOUT_RESULT:
        {
            LogoutResult* logoutResult=NULL;
            logoutResult = (LogoutResult*)header;
           // cout << "接收到logoutResult:" << logoutResult->result << endl;
            break;
        }
        case CMD_NEW_USER_JOIN:
        {
            NewUserJoin* newUserJoin = NULL;

            newUserJoin = (NewUserJoin*)header;
           // cout << "接收到NewUserJoin:" << newUserJoin->sock << endl;

            break;
        }
        case CMD_ERROR:
        {
            cout << "recevie cmd_error message length:" << header->dataLength<< endl;

            break;
        }
        default:
        {
            cout << "receive no define message message length:" << header->dataLength<< endl;

            break;
        }
        }


    }

    bool isRun()
    {
        return INVALID_SOCKET!=_sock;
    }

    int  sendData(DataHeader *header)
    {
        if(isRun() && header)
        {
            return send(_sock, (const char*)header,header->dataLength, 0);
        }
        return SOCKET_ERROR;
    }


private:

};
#endif
