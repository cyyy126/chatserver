#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <iostream>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

class ChatServer
{
public:
    // 初始化TcpServer
    ChatServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr,
               const string &nameArg)
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // 通过绑定器设置回调函数
        _server.setConnectionCallback(bind(&ChatServer::onConnection,
                                           this, _1));
        _server.setMessageCallback(bind(&ChatServer::onMessage,
                                        this, _1, _2, _3));
        // 设置EventLoop的线程个数
        _server.setThreadNum(4);
    }
    // 启动ChatServer服务
    void start()
    {
        _server.start();
    }

private:
    // TcpServer绑定的回调函数，当有新连接或连接中断时调用
    void onConnection(const muduo::net::TcpConnectionPtr &con)
    {
        if (con->connected())
        {
            cout << con->peerAddress().toIpPort() << "->" << con->localAddress().toIpPort() << "state:online" << endl;
        }
        else
        {
            cout << con->peerAddress().toIpPort() << "->" << con->localAddress().toIpPort() << "state:offline" << endl;
            con->shutdown();

        }
    }
    // TcpServer绑定的回调函数，当有新数据时调用
    void onMessage(const muduo::net::TcpConnectionPtr &con,
                   muduo::net::Buffer *buffer,
                   muduo::Timestamp time)
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        con->send(buf);
    }

private:
    muduo::net::TcpServer _server;
    EventLoop *_loop;
};

int main(){
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop();
}
