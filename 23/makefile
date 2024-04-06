target: tcpepoll client

# 将生成的文件放到bin目录下
tcpepoll: tcpepoll.cpp InetAddress.cpp
	g++ -o bin/tcpepoll tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp

client: client.cpp
	g++ -o bin/client client.cpp

clean:
	rm -f bin/tcpepoll bin/client

