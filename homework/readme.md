网络编程实战

期中复习周/期中大作业

## 题干
请你分别写一个客户端程序和服务器程序，客户端程序连接上服务器之后，通过敲命令和服务器进行交互，支持的交互命令包括：

pwd：显示服务器应用程序启动时的当前路径。
cd：改变服务器应用程序的当前路径。
ls：显示服务器应用程序当前路径下的文件列表。
quit：客户端进程退出，但是服务器端不能退出，第二个客户可以再次连接上服务器端。


### 客户端程序要求
- 可以指定待连接的服务器端 IP 地址和端口。
- 在输入一个命令之后，回车结束，之后等待服务器端将执行结果返回，客户端程序需要将结果显示在屏幕上。

样例输出如下所示。
```c
第一次连接服务器
$./telnet-client 127.0.0.1 43211
pwd
/home/vagrant/shared/Code/network/yolanda/build/bin
cd ..
pwd
/home/vagrant/shared/Code/network/yolanda/build
cd ..
pwd
/home/vagrant/shared/Code/network/yolanda
ls
build
chap-11
chap-12
chap-13
chap-14
chap-15
chap-16
chap-17
chap-18
chap-20
chap-21
chap-22
chap-23
chap-25
chap-26
chap-27
chap-28
chap-4
chap-5
chap-6
chap-7
clean.sh
cmake-build-debug
CMakeLists.txt
lib
mid-homework
README.md
 
 
cd -
pwd
/home/vagrant/shared/Code/network/yolanda
cd /home
pwd
/home
ls
ubuntu
vagrant
quit
 
// 再次连接服务器
$./telnet-client 127.0.0.1 43211
pwd
/home/vagrant/shared/Code/network/yolanda/build
ls
bin
chap-11
chap-12
chap-13
chap-15
chap-16
chap-17
chap-18
chap-20
chap-21
chap-22
chap-23
chap-25
chap-26
chap-28
chap-4
chap-5
chap-6
chap-7
CMakeCache.txt
CMakeFiles
cmake_install.cmake
lib
Makefile
mid-homework
 
quit
```


### 服务器程序要求
- 暂时不需要考虑多个客户并发连接的情形，只考虑每次服务一个客户连接。
- 要把命令执行的结果返回给已连接的客户端。
- 服务器端不能因为客户端退出就直接退出。