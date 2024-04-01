target: tcpepoll client

# 将生成的文件放到bin目录下
tcpepoll: tcpepoll.cpp InetAddress.cpp
	g++ -o bin/tcpepoll tcpepoll.cpp InetAddress.cpp

client: client.cpp
	g++ -o bin/client client.cpp

clean:
	rm -f bin/tcpepoll bin/client

