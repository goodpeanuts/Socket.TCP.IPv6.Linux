#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 65035
#define BUFFER_SIZE 1024

using namespace std;

char msg[] = "连接成功，请选择需要的文件\n\ttest1.txt\n\ttest2.txt\n\ttest3.txt\n";  //发给客户端的信息

int main() {
    int sock_server, newsock;   //监听套接字，已连接套接字
    struct sockaddr_in6 addr{};       //填写绑定地址
    struct sockaddr_in6 client_addr{};  //接收客户端发来的信息
    
    char buffer[BUFFER_SIZE];
    
    //创建服务端套接字
    sock_server = socket(AF_INET6, SOCK_STREAM, 0);
    
    //填写要绑定的地址
    socklen_t addr_len = sizeof(struct sockaddr_in6);
    memset((void*)&addr, 0, addr_len);
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(PORT);
    addr.sin6_addr = in6addr_any;
    
    //给监听套接字绑定地址
    bind(sock_server, (struct sockaddr*)&addr, sizeof(addr));
    
    //将套接字设为监听状态
    listen(sock_server, 0);
    
    //接收连接请求并收发数据
    int size;
    while (true) {
        cout << "服务器正在运行，监听连接请求中\n";
        newsock = accept(sock_server, (struct sockaddr*)&client_addr, &addr_len);  //接收请求
        cout << "成功接受一个连接请求！\n";
        
        // 发送连接成功消息
        size = send(newsock, msg, sizeof(msg), 0);
        if (size == -1) {
            cout << "发送连接成功消息失败！错误代码：" << strerror(errno) << endl;
            close(newsock);
            continue;
        }
        
        // 接收文件名
        size = recv(newsock, buffer, BUFFER_SIZE, 0);
        if (size == 0) {
            cout << "客户端断开连接\n";
            close(newsock);
            continue;
        }
        else {
            buffer[size] = '\0';
            string filename = buffer;
            cout << "客户端请求文件: " << filename << endl;
            
            // 打开文件
            ifstream file(filename, ios::binary | ios::ate);
            if (!file) {
                cout << "无法打开文件" << endl;
                close(newsock);
                continue;
            }
            
            // 获取文件大小
            streampos fileSize = file.tellg();
            file.seekg(0, ios::beg);
            
            // 发送文件大小
            size = send(newsock, (char*)&fileSize, sizeof(fileSize), 0);
            if (size == -1) {
                cout << "发送文件失败，错误代码：" << strerror(errno) << endl;
                close(newsock);
                file.close();
                continue;
            }
            
            // 发送文件内容
            while (fileSize > 0) {
                size = file.readsome(buffer, BUFFER_SIZE);
                if (size == 0) {
                    cout << "读取文件内容失败" << endl;
                    break;
                }
                else {
                    size = send(newsock, buffer, size, 0);
                    if (size == -1) {
                        cout << "发送文件内容失败，错误代码：" << strerror(errno) << endl;
                        break;
                    }
                    fileSize -= size;
                }
            }
            
            file.close();
            cout << "文件发送完成" << endl;
            cout << "---------------- \n"<<endl;
        }
        
        close(newsock);
    }
    
    close(sock_server);
    return 0;
}

