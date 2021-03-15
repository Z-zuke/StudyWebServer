#ifndef HTTPCONN_H
#define HTTPCONN_H

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "locker.h"
#include <sys/uio.h>
#include <fcntl.h>


class http_conn {
public:
    http_conn() {}
    ~http_conn() {}

    void init(int sockfd, const sockaddr_in& addr);     // 初始化新接受的连接
    void close_conn();                                  // 关闭连接
    void process();                                     // 处理客户端请求
    bool read();                                        // 非阻塞读
    bool write();                                       // 非阻塞写

public:
    static int m_epollfd;                       // 注册所有socket 上的事件，设为静态变量
    static int m_user_count;                    // 统计用户数量

    static const int FILENAME_LEN = 1024;       // 文件名的最大长度
    static const int READ_BUFFER_SIZE = 2048;   // 度缓冲的大小
    static const int WRITE_BUFFER_SIZE = 1024;  // 写缓冲区的大小

    /* HTTP 请求方法 */
    // TODO: 这里仅支 GET 
    enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTION, CONNECT};

    /*
        解析客户端请求时，主状态机的状态
        CHECK_STATE_REQUESTLINE: 当前正在解析请求行
        CHECK_STATE_HEADER: 当前正在解析请求头
        CHECK_STATE_CONTENT: 当前正在解析请求体
    */
    enum CHECK_STATE {
        CHECK_STATE_REQUESTLINE = 0,             // 当前正在解析请求行
        CHECK_STATE_HEADER,                      // 当前正在解析请求头
        CHECK_STATE_CONTENT                      // 当前正在解析请求体
    };

   /*
        服务器处理 HTTP 请求的可能结果，报文解析的结果
        NO_REQUEST:         请求不完整，需要继续读取客户数据
        GET_REQUEST:        表示获得了一个完整的客户请求
        BAD_REQUEST:        表示客户请求语法错误
        NO_RESOURCE:        表示服务器没有资源
        FORBIDEN_REQUEST:   表示客户对资源没有足够的访问权限
        FILE_REQUEST:       文件请求，获取文件成功
        INTERNAL_ERROR:     表示服务器内部错误
        CLOSED_CONNECTION: 表示客户端已经关闭连接了
   */
    enum HTTP_CODE {
        NO_REQUEST,                             // 请求不完整，需要继续读取客户数据
        GET_REQUEST,                            // 表示获得了一个完整的客户请求
        BAD_REQUEST,                            // 表示客户请求语法错误
        NO_RESOURCE,                            // 表示服务器没有资源
        FORBIDEN_REQUEST,                       // 表示客户对资源没有足够的访问权限
        FILE_REQUEST,                           // 文件请求，获取文件成功
        INTERNAL_ERROR,                         // 表示服务器内部错误
        CLOSED_CONNECTION                       // 表示客户端已经关闭连接了
    };

    /*
        从状态机的三种可能状态（行的读取状态）
        1. 读取到一个完整的行
        2. 行出错
        3. 行数据尚且不完整
    */
   enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };


private:
    void init();                                        // 初始化连接
    HTTP_CODE process_read();                           // 解析 HTTP 请求
    bool process_write(HTTP_CODE ret);                  // 填充 HTTP 应答

    /* 这组函数被 process_read 调用以分析 HTTP 请求 */
    HTTP_CODE parse_request_line(char* text);
    HTTP_CODE parse_headers(char* text);
    HTTP_CODE parse_content(char* text);
    HTTP_CODE do_request();
    char* get_line() { return m_read_buf + m_start_line; }
    LINE_STATUS parse_line();

    /* 这组函数被 process_write 调用以填充 HTTP 应答*/
    void unmap();
    bool add_response(const char* format, ...);
    bool add_content(const char* content);
    bool add_content_type();
    bool add_status_line(int status, const char* title);
    bool add_headers(int content_length); 
    bool add_content_length(int content_length);
    bool add_linger();  // keep alive / close
    bool add_blank_line();

private:
    int m_sockfd;                               // 该 HTTP 连接的 socket
    sockaddr_in m_addr;                         // 对方的 socket 地址

    char m_read_buf[READ_BUFFER_SIZE];          // 读缓冲区
    int m_read_idx;                             // 标识读缓冲区中已读入数据的下一个位置
    int m_checked_idx;                          // 当前正在分析的字符在读缓冲区中的位置
    int m_start_line;                           // 当前正在解析的行的起始位置

    CHECK_STATE m_check_state;                  // 主状态机当前所处的状态
    METHOD m_method;                            // 请求方法

    char m_real_file[FILENAME_LEN];             // 客户端请求的目标文件的完整路径
                                                // 内容等于 doc_root + m_url, doc_root 为网站根目录
    char* m_url;                                // 客户请求的目标文件的文件名
    char* m_version;                            // HTTP 协议版本号，这里仅支持 HTTP1.1
    char* m_host;                               // 主机名
    int m_content_length;                       // HTTP 请求的消息总长度
    bool m_linger;                              // HTTP 请求是否要求保持连接

    char m_write_buf[WRITE_BUFFER_SIZE];        // 写缓冲区
    int m_write_idx;                            // 写缓冲区中待发送的字节数
    char* m_file_address;                       // 客户请求的目标文件被 mmap 到内存中的起始位置
    struct stat m_file_stat;                    // 目标文件的状态，通过它我们可以判断
                                                // 文件是否存在、是否为目录、是否可读，并获取文件大小等信息
    struct iovec m_iv[2];                       // 采用 writev 来执行写操作
                                                // 定义两个成员，其中 m_iv_count 表示被写内存块的数量
    int m_iv_count;
};


#endif