/* torbridge.c */
#include "torbridge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    90: request granted
	91: request rejected or failed
	92: request rejected because SOCKS server cannot connect to identd on the client
	93: request rejected because the client program and identd report different user-ids
 */

Req *request(const char *dstip, const int dstport) {
    Req *req;

    req = malloc(reqsize);

    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);
    snprintf((char *)req->userid, sizeof(req->userid), "%s", USERNAME);

    return req;
}

int main(int argc, char *argv[]) {
    char *host;
    int port, s;
    struct sockaddr_in sock;
    Req *req;
    Res *res;
    char buf[ressize];
    int success;
    char tmp[512];

    /* predicate - property*/
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    host = argv[1];
    port = atoi(argv[2]);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXYPORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (connect(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("connect");
        return EXIT_FAILURE;
    }

    printf("[+] Connected to proxy...\n");
    req = request(host, port);
    write(s, req, reqsize);

    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1) {
        perror("read");
        free(req);
        close(s);

        return EXIT_FAILURE;
    }

    res = (Res *)buf;
    success = (res->cd == 90);
    if(!success) {
        fprintf(stderr, "[x] Unable to traverse"
            "the proxy, error code: %d\n",
            res->cd);

            close(s);
            free(req);

            return EXIT_FAILURE;
    }

    printf("[+] Successfully connected through the proxy to"
        " %s:%d\n",host, port);

    memset(tmp, 0, 512);
    snprintf(tmp, 511,
        "HEAD / HTTP/1.0\r\n"
        "Host: www.networktechnology.org\r\n"
        "\r\n"
        );
    write(s, tmp, strlen(tmp));

    memset(tmp, 0, 512);
    read(s, tmp, 511);
    printf("'%s'\n", tmp);
    close(s);
    free(req);

    return  EXIT_SUCCESS;
}
