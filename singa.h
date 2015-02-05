#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define PORTNUM 8081
#define error(mess) { perror(mess);exit(1); }
#define MAXLINE 256
#define NAMELEN 30
#define METHODLEN 10
#define PROJPATH "/home/charson/文档/singa/"
#define URLLEN 128
#define DATALEN 256

struct ROUTE {
    char url[URLLEN];
    void (* callback)();
};

// 所有申明
extern struct ROUTE routes[];
void load_static_file(int sock , char path[BUFSIZ] , char fileType[10]);
void get_content_type(char fileType[NAMELEN] , char * contentType);
void parse_file_type(char path[BUFSIZ] , char fileType[10]);
void get_request(int sock_fd_accept);
void singa_init();
// 解析文件类型
void parse_file_type(char path[BUFSIZ] , char fileType[10]) {
    char * file_types[] = { ".html" , ".js" , ".css" , ".jpg" , ".png" , ".gif" , ".ico" };
    int i = 0;
    for (; i < 7 ; i++) {
        if (strstr(path , file_types[i]) != NULL) {
            strcpy(fileType , ++file_types[i]);
            return;
        }
    }
    fileType = "";
}
// 根据文件类型获取对应的content-type
void get_content_type(char fileType[NAMELEN] , char * contentType) {
    if (strcmp(fileType , "html") == 0) {
        contentType = "Content-Type: text/html\r\n";
    } else if (strcmp(fileType , "js") == 0) {
        contentType = "Content-Type: text/javascript\r\n";
    } else if (strcmp(fileType , "css") == 0) {
        contentType = "Content-Type: text/css\r\n";
    } else if (strcmp(fileType , "jpg") == 0) {
        contentType = "Content-Type: image/jpeg\r\n";
    } else if (strcmp(fileType , "png") == 0) {
        contentType = "Content-Type: image/png\r\n";
    } else if (strcmp(fileType , "gif") == 0) {
        contentType = "Content-Type: image/gif\r\n";
    } else if (strcmp(fileType , "ico") == 0) {
        contentType = "Content-Type: image/jpeg\r\n";
    }
}
// 根据路径名加载对应的文件,逐字节读取
void load_static_file(int sock , char path[BUFSIZ] , char fileType[10]) {
    char fullPath[BUFSIZ] = "";
    strcat(fullPath , PROJPATH);
    strcat(fullPath , path);
    FILE * fp_resp = fdopen(sock , "w");
    FILE * fp_file = fopen(fullPath , "r");
    if (!fp_file) {
        fprintf(fp_resp , "HTTP/1.1 404 error\r\n");
    } else {
        int ch;
        char * contentType = "";
        get_content_type(fileType , contentType);
        fprintf(fp_resp , "HTTP/1.1 200 ok\r\n");
        fprintf(fp_resp , "%s" , contentType);
        fprintf(fp_resp , "\r\n");
        while ((ch = fgetc(fp_file)) != EOF) {
            fputc(ch , fp_resp);
        }
        fclose(fp_file);
    }
    fclose(fp_resp);
}
// 分析请求,根据路径分为加载静态文件和返回json
void get_request(int sock_fd_accept) {
    FILE * fp_requ = fdopen(sock_fd_accept , "r");
    char buf[BUFSIZ] = "";
    char method[5] = "";
    char path[BUFSIZ] = "";
    char search[BUFSIZ] = "";
    char fileType[10] = "";
    while (fgets(buf , BUFSIZ , fp_requ) != NULL && strcmp(buf , "\r\n") != 0) {
        if (strstr(buf , "HTTP") != NULL) {
            printf("%s\n" , buf);
            sscanf(buf , "%s /" , method);
            sscanf(buf , "%*s %[^?' '] HTTP" , path);
            sscanf(buf , "%*s %*[^?]?%s HTTP" , search);
            parse_file_type(path , fileType);
        }
    }
    if (strcmp(fileType , "") != 0) {
        load_static_file(sock_fd_accept , path , fileType);
    } else {
        FILE * fp_resp = fdopen(sock_fd_accept , "w");
        int i = 0;
        for (; i < 100 ; i++) {
            if (strcmp(routes[i] . url , path) == 0) {
                routes[i] . callback(fp_resp);
                break;
            }
        }
    }
}
void singa_init() {
    struct sockaddr_in cliaddr;
    cliaddr . sin_family = AF_INET;
    cliaddr . sin_port = htons(PORTNUM);
    cliaddr . sin_addr . s_addr = INADDR_ANY;
    int sock_fd = socket(PF_INET , SOCK_STREAM , 0);
    if (sock_fd == -1) {
        error("socker");
    }
    if (bind(sock_fd , (struct sockaddr *) & cliaddr , sizeof(cliaddr)) != 0) {
        error("bind");
    }
    if (listen(sock_fd , 5) != 0) {
        return;
    }
    int sock_fd_accept;
    while (1) {
        sock_fd_accept = accept(sock_fd , NULL , NULL);
        get_request(sock_fd_accept);
    }
}