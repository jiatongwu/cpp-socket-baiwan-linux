#include "EasyTcpServer.hpp"
using namespace std;
int main()
{

    EasyTcpServer server1;
    server1.initSocket();
    server1.bindSocket(4567);
    server1.Listen(5);



    while (server1.isRun())
    {
        server1.onRun();
    }

    server1.closeSocket();



    std::cout<<"服务器退出"<<std::endl;
    return 0;
}
