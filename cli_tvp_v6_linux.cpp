#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 65035
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock_client;
    struct sockaddr_in6 server_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in6);
    char buffer[BUFFER_SIZE];

    // 创建套接字
    sock_client = socket(AF_INET6, SOCK_STREAM, 0);

    // 填写服务器地址
    memset((void*)&server_addr, 0, addr_len);
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(PORT);
    if (inet_pton(AF_INET6, "fd15:4ba5:5a2b:1008:1c65:f815:7213:e616", &(server_addr.sin6_addr)) != 1) {
        cout << "无效的服务器地址" << endl;
        close(sock_client);
        return 0;
    }

    connect(sock_client, (struct sockaddr*)&server_addr, addr_len);

    // 接收连接成功消息
    int size = recv(sock_client, buffer, BUFFER_SIZE, 0);
    if (size == 0) {
        cout << "连接失败" << endl;
        close(sock_client);
        return 0;
    } else {
        cout << "服务器消息: " << buffer << endl;
    }

    // 发送文件名
    string filename;
    cout << "请输入要发送的文件名：";
    cin >> filename;

    size = send(sock_client, filename.c_str(), filename.length(), 0);
    if (size == -1) {
        cout << "发送文件名失败" << endl;
        close(sock_client);
        return 0;
    }

    // 接收文件大小
    streampos fileSize;
    size = recv(sock_client, (char*)&fileSize, sizeof(fileSize), 0);
    if (size == 0) {
        cout << "与服务器断开连接" << endl;
        close(sock_client);
        return 0;
    } else {
        cout << "接收到的文件大小为: " << fileSize << " bytes" << endl;
    }

    // 创建文件
    ofstream file("received_" + filename, ios::binary);

    // 接收文件内容
    int remainingSize = fileSize;
    while (remainingSize > 0) {
        size = recv(sock_client, buffer, min(remainingSize, BUFFER_SIZE), 0);
        if (size == 0) {
            cout << "对方已关闭连接" << endl;
            break;
        } else if (size == -1) {
            cout << "接收文件内容失败" << endl;
            break;
        } else {
            file.write(buffer, size);
            remainingSize -= size;
        }
    }

    file.close();
    cout << "文件接收完成，保存为: received_" << filename << endl;

    close(sock_client);
    return 0;
}

