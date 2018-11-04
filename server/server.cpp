


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


enum CMD
  {
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR,
    CMD_NEW_USER_JOIN
  };
std::vector<SOCKET> g_clients;

struct DataHeader
{
  int dataLength;//数据长度
  int cmd;//命令
};
struct Login : public DataHeader
{
  Login()
  {
    dataLength = sizeof(Login);
    cmd = CMD_LOGIN;

  }
  char username[32];
  char password[32];
};
struct NewUserJoin : public DataHeader
{
  NewUserJoin()
  {
    dataLength = sizeof(NewUserJoin);
    cmd = CMD_NEW_USER_JOIN;
    sock = 0;
  }
  int sock;
};
struct LoginResult : public DataHeader
{
  LoginResult()
  {
    dataLength = sizeof(LoginResult);
    cmd = CMD_LOGIN_RESULT;
    result = 0;
  }
  int result;
};
struct Logout : public DataHeader
{
  Logout()
  {
    dataLength = sizeof(Logout);
    cmd = CMD_LOGOUT;

  }
  char username[32];
};
struct LogoutResult : public DataHeader
{
  LogoutResult()
  {
    dataLength = sizeof(LogoutResult);
    cmd = CMD_LOGOUT_RESULT;
    result = 0;
  }
  int result;
};
int process(SOCKET client_socket)
{
  char recvBuffer[4096];
  DataHeader* header;

  //5.
   int nLen = recv(client_socket, recvBuffer, sizeof(DataHeader), 0);
  if (nLen <= 0)
    {
      std::cout << "接收到的数据小于等于0  "<<client_socket << std::endl;
      return -1;
    }
  header = (DataHeader*)recvBuffer;
  std::cout << "接收到来自： " << client_socket << "命令 cmd:" << header->cmd << " dataLength:" << header->dataLength << std::endl;
  switch (header->cmd)
    {
    case CMD_LOGIN:
      {
        Login* login;
        recv(client_socket, recvBuffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        login = (Login*)recvBuffer;
        std::cout << "接收到来自：" << client_socket << "用户名:  username: " << login->username << " password " << login->password << std::endl;


        LoginResult loginResult;

        send(client_socket, (const char*)&loginResult, sizeof(LoginResult), 0);
        break;
      }
    case CMD_LOGOUT:
      {
        Logout* logout;
        recv(client_socket, recvBuffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        logout = (Logout*)recvBuffer;
        std::cout << "接收到来自：" << client_socket << "logout消息用户名: username: " << logout->username << std::endl;


        LogoutResult logoutResult;

        send(client_socket, (const char*)&logoutResult, sizeof(LogoutResult), 0);
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
  return 0;

}
int main()
{
#ifdef _WIN32
  WORD ver = MAKEWORD(2, 2);
  WSADATA dat;
  WSAStartup(ver, &dat);
#endif

  ///
  //1. socket
  SOCKET _sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  //2.bind IP PORT
  sockaddr_in _in;
  _in.sin_family = AF_INET;
  _in.sin_port =htons( 4567);
#ifdef _WIN32
  _in.sin_addr.S_un.S_addr =INADDR_ANY;;
#else
  _in.sin_addr.s_addr=INADDR_ANY;;
#endif
  if (SOCKET_ERROR== bind(_sock, (sockaddr*)&_in, sizeof(_in)))
    {
      std::cout<<"绑定服务器socket失败"<<std::endl;
      return -1;
    }
  else
    {
      std::cout << "绑定服务器socket成功" << std::endl;
    }

  //3.listen
  if (SOCKET_ERROR == listen(_sock, 5))
    {
      std::cout << "listen failed" << std::endl;
      return -1;
    }
  else
    {
      std::cout << "listen success" << std::endl;
    }



  //char recvBuffer[128];
  while (true)
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



      timeval t = {2,0};
      int ret=select(maxSock+1,&fdRead,&fdWrite,&fdExp,&t);
      if (ret < 0)
        {
	  std::cout<<"select返回－1 失败 "<<std::endl;
	  break;
        }
      if (FD_ISSET(_sock, &fdRead))
        {

	  FD_CLR(_sock, &fdRead);
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
	      std::cout << "accept 接收到一个无效的socket" << std::endl;
	      //return -1;
            }
	  else {
	    std::cout << "接收到socket:" << client_socket << " ip : " << inet_ntoa(clientAddr.sin_addr) << ":" << clientAddr.sin_port << std::endl;

	    //
	    for (int n = (int)g_clients.size() - 1; n >= 0; n--)
	      {
		NewUserJoin newUserJoin;
		send(g_clients[n], (const char*)&newUserJoin, sizeof(NewUserJoin), 0);
	      }


	    g_clients.push_back(client_socket);
	  }

        }
      for (int n = (int)g_clients.size() - 1; n >= 0; n--)
        {
	  if (FD_ISSET(g_clients[n], &fdRead))
	    {
	      if (-1 == process(g_clients[n]))
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

      /*      for (size_t n = 0; n <fdRead.fd_count; n++)
        {
	  if (-1 == process(fdRead.fd_array[n]))
            {
	      auto iter = find(g_clients.begin(),g_clients.end(),fdRead.fd_array[n]);
	      if (iter != g_clients.end())
                {
		  g_clients.erase(iter);
                }
            }

	    }*/



    }

#ifdef _WIN32

  closesocket(_sock);




  WSACleanup();
#else
  close(_sock);
#endif


  std::cout<<"服务器退出"<<std::endl;
  return 0;
}
