#ifndef EASYTCPSERVER_HPP
#define EASYTCPSERVER_HPP



#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define  _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <windows.h>
    #include <WinSock2.h>
    #pragma comment(lib,"ws2_32.lib")
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
#include <vector>


#include "MessageHeader.hpp"
using namespace std;
class EasyTcpServer
{
private:
    std::vector<SOCKET> g_clients;
    SOCKET _sock;
public:

    EasyTcpServer()
    {
        _sock=INVALID_SOCKET;
    }
    virtual ~EasyTcpServer()
    {
       closeSocket();

    }

    //初始化Socket
    SOCKET initSocket()
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
        _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == _sock)
        {
            cout << "create socket failed,socket:"<<_sock << endl;

        }
        else
        {
            cout << "create socket success,socket:"<<_sock << endl;
        }
        return _sock;
    }
    //绑定端口号
    int bindSocket(unsigned short port)
    {
        if (INVALID_SOCKET == _sock)
        {
            initSocket();
        }
        int ret=-1;
        if (INVALID_SOCKET != _sock)
        {
            //2.bind IP PORT
            sockaddr_in _in;
            _in.sin_family = AF_INET;
            _in.sin_port =htons( port);
#ifdef _WIN32
            _in.sin_addr.S_un.S_addr =INADDR_ANY;;
#else
            _in.sin_addr.s_addr=INADDR_ANY;;
#endif
            ret=bind(_sock, (sockaddr*)&_in, sizeof(_in));
            if (SOCKET_ERROR== ret)
            {
                std::cout<<"绑定服务器socket失败,socket:"<<_sock<<std::endl;
            }
            else
            {
                std::cout << "绑定服务器socket成功,socket:"<<_sock << std::endl;
            }
        }
        return ret;
    }
    //监听端口
    int Listen(int n)
    {
        int ret=listen(_sock,n);
        if (SOCKET_ERROR == ret)
        {
            std::cout << "listen failed,socket:" <<_sock<< std::endl;

        }
        else
        {
            std::cout << "listen success,socket:"<<_sock << std::endl;
        }
        return ret;
    }
    //接收网络连接
    SOCKET acceptSocket()
    {
        sockaddr_in clientAddr;
        int clientAddrLength = sizeof(clientAddr);
        //4.
        SOCKET client_socket = INVALID_SOCKET;
#ifdef _WIN32
        if ((client_socket = accept(_sock, (sockaddr*)&clientAddr, &clientAddrLength)) == INVALID_SOCKET)
#else
        if ((client_socket = accept(_sock, (sockaddr*)&clientAddr,(socklen_t*) &clientAddrLength)) == INVALID_SOCKET)
#endif
        {
            std::cout <<_sock<< " accept 接收到一个无效的socket" << std::endl;

        }
        else
        {
            std::cout <<_sock<< " 接收到socket:"<<client_socket << client_socket << " ip : " << inet_ntoa(clientAddr.sin_addr) << ":" << clientAddr.sin_port << std::endl;
            NewUserJoin newUserJoin;
            sendDataToAll(&newUserJoin);


            g_clients.push_back(client_socket);
            return client_socket;

        }
    }
    //关闭socket
    void closeSocket()
    {
        if(INVALID_SOCKET!=_sock)
        {

#ifdef _WIN32
            for (int n = (int)g_clients.size() - 1; n >= 0; n--)
            {
                closesocket(g_clients[n]);
            }
            closesocket(_sock);
            WSACleanup();
#else
            for (int n = (int)g_clients.size() - 1; n >= 0; n--)
            {
                close(g_clients[n]);
            }
            close(_sock);
#endif
            _sock=INVALID_SOCKET;
        }

    }
    //处理网络消息
    bool onRun()
    {
        if(isRun())
        {
            fd_set fdRead;
            fd_set fdWrite;
            fd_set fdExp;

            FD_ZERO(&fdRead);
            FD_ZERO(&fdWrite);
            FD_ZERO(&fdExp);

            FD_SET(_sock,&fdRead);
            FD_SET(_sock, &fdWrite);
            FD_SET(_sock, &fdExp);
            SOCKET maxSock=_sock;
            for (int n = (int)g_clients.size() - 1; n >= 0; n--)
            {
                FD_SET(g_clients[n],&fdRead);
                if(maxSock< g_clients[n])
                {
                    maxSock=g_clients[n];
                }
            }



            timeval t = {1,0};
            int ret=select(maxSock+1,&fdRead,&fdWrite,&fdExp,&t);
            if (ret < 0)
            {
                std::cout<<"select返回－1 失败 "<<std::endl;
                closeSocket();
                return false;
            }
            if (FD_ISSET(_sock, &fdRead))
            {

                FD_CLR(_sock, &fdRead);
                acceptSocket();
            }


            for (int n = (int)g_clients.size() - 1; n >= 0; n--)
            {
                if (FD_ISSET(g_clients[n], &fdRead))
                {
                    if (-1 == RECVDATA(g_clients[n]))
                    {
                        auto iter = g_clients.begin()+n;
                        if(iter!=g_clients.end())
                        {
                            g_clients.erase(iter);
                        }
                    }
                    //FD_CLR(_sock, &fdRead);
                }


            }
            return true;
        }
        return false;

    }
    //是否工作中
    bool isRun()
    {
        return INVALID_SOCKET!=_sock;
    }
    //接收数据 处理粘包 拆分包
    int RECVDATA(SOCKET client_socket)
    {
        char recvBuffer[4096];
        DataHeader* header;

        //5.
        int nLen = recv(client_socket, recvBuffer, sizeof(DataHeader), 0);
        if (nLen <= 0)
        {
            std::cout << "接收到的数据小于等于0  "<<client_socket << " 客户端退出"<<std::endl;
            return -1;
        }
        header=(DataHeader*) recvBuffer;
        recv(client_socket, recvBuffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        std::cout << "接收到来自： " << client_socket << "命令 cmd:" << header->cmd << " dataLength:" << header->dataLength << std::endl;
        onNetMessage( client_socket,header);
        return 0;

    }
    //响应网络消息
    virtual void onNetMessage(SOCKET client_socket,DataHeader *header)
    {

        switch (header->cmd)
        {
        case CMD_LOGIN:
        {
            Login* login;
            login = (Login*)header;
            std::cout << "接收到来自：" << client_socket << "用户名:  username: " << login->username << " password " << login->password << std::endl;


            LoginResult loginResult;
            sendData(client_socket,&loginResult);
            break;
        }
        case CMD_LOGOUT:
        {
            Logout* logout;

            logout = (Logout*)header;
            std::cout << "接收到来自：" << client_socket << "logout消息用户名: username: " << logout->username << std::endl;


            LogoutResult logoutResult;
            sendData(client_socket,&logoutResult);
            break;
        }
        case CMD_ERROR:
        {
            break;
        }
        default:
        {
            break;
        }
        }
    }
    //发送给指定客户端数据
    int  sendData(SOCKET client_socket,DataHeader *header)
    {
        if(isRun() && header)
        {
            return send(client_socket, (const char*)header,header->dataLength, 0);
        }
        return SOCKET_ERROR;
    }
    //群发客户端数据
    void  sendDataToAll(DataHeader *header)
    {
        for (int n = (int)g_clients.size() - 1; n >= 0; n--)
        {
            sendData(g_clients[n],header);
        }
    }
};
#endif // EASYTCPSERVER_HPP
