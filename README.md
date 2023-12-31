# 基于TCP协议Linux下的IPv6简单socket网络通信传输文件实现

### 服务端代码

1. 创建一个IPv6的TCP套接字。
2. 绑定套接字到指定的IP地址和端口号。
3. 循环监听客户端的连接请求和数据交互。
4. 接收客户端的连接请求后，发送连接成功消息。
5. 接收客户端请求的文件名，打开文件，获取文件大小。
6. 发送文件大小给客户端。
7. 逐块读取文件内容，发送给客户端。

### 客户端代码

1. 创建一个IPv6的TCP套接字。
2. 设置服务器的IP地址和端口号。
3. 发送连接请求给服务器，并接收连接成功消息。
4. 输入需要请求的文件名，发送文件名给服务器。
5. 接收服务器发送的文件大小。
6. 创建一个新文件，逐块接收文件内容，写入新文件。

### 注意事项

1. 这份代码中使用的是IPv6的套接字地址结构 `sockaddr_in6`，在实际使用时，需要确认服务器和客户端都支持IPv6，并根据实际情况修改IP地址和端口号。

   