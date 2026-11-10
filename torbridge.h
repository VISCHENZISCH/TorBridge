/* torbridge.h  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


    /*
        9050
        10101100 10101100
        socket()
        connect()
        close()
        htons()
        inet_addr()
     */


#define PROXY       "127.0.0.1"
#define PROXYPORT   9050
#define USERNAME    "toraliz"
#define reqsize sizeof(struct proxy_request)
#define ressize sizeof(struct proxy_reponse)



/*

    +----+----+----+----+----+----+----+----+----+----+....+----+
	| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
	+----+----+----+----+----+----+----+----+----+----+....+----+
      1    1      2              4           variable       1

 */

struct proxy_request {
    int8_t vn;
    int8_t cd;
    int16_t dstport;
    int32_t dstip;
    unsigned char userid[8];
};

typedef struct proxy_request Req;

/*

    +----+----+----+----+----+----+----+----+
	| VN | CD | DSTPORT |      DSTIP        |
	+----+----+----+----+----+----+----+----+
        1    1      2              4
 */

struct proxy_reponse {
   int8_t vn;
   int8_t cd;
   int16_t dstport;
   int32_t dstip;
};

typedef struct proxy_reponse Res;

Req *request(const char*, const int);
int main(int, char**);
