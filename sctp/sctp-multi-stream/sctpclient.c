/*************************************
 ***                               ***
 ***  Author: Nirav Shah           ***
 ***  Email: nsnirav12@gmail.com   ***
 ***  Date: 22nd Jan 2017          ***
 ***                               *** 
 *************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdlib.h>
#include <pthread.h>


#define HEARTBEAT_INTERVAL    3000
#define RTO_MIN               1000
#define RTO_INITIAL           1000
#define RTO_MAX               1000
#define PATH_MAX_RETRANSMIT   5
#define ASSOC_MAX_RETRANSMIT  5
#define MAX_IN_STREAM         5
#define MAX_OUT_STREAM        5
#define MAX_INIT_ATTEMPTS     10


#define BUFFER_SIZE 100
#define PORT 10000   

int sock, ret, flags;
int i, reuse = 1;
int addr_count = 0;
char buffer[BUFFER_SIZE];
char send_buffer[1000];

struct sockaddr_in addr;
struct sockaddr_in *laddr[10];
struct sockaddr_in *paddrs[10];
struct sigaction sig_handler;
struct sockaddr_in bindaddr[2];
struct sockaddr_in serveraddr[2];

int apply_sctp_params(int sock)
{
    struct sctp_paddrparams heartbeat;
    struct sctp_rtoinfo rtoinfo;
    struct sctp_initmsg initmsg;
    struct sctp_assocparams assoc_param;

    memset(&heartbeat,  0, sizeof(struct sctp_paddrparams));
    memset(&rtoinfo,    0, sizeof(struct sctp_rtoinfo));
    memset(&initmsg,    0, sizeof(struct sctp_initmsg));
    memset(&assoc_param,    0, sizeof(struct sctp_assocparams));
 
    heartbeat.spp_flags = SPP_HB_ENABLE;
    heartbeat.spp_hbinterval = HEARTBEAT_INTERVAL;
    heartbeat.spp_pathmaxrxt = PATH_MAX_RETRANSMIT;

    /*Set Heartbeats*/
    if (setsockopt(sock, SOL_SCTP, SCTP_PEER_ADDR_PARAMS,
                  &heartbeat, sizeof(heartbeat)) != 0) {
        perror("setsockopt");
        return -1;
    }

    rtoinfo.srto_initial = RTO_INITIAL;
    rtoinfo.srto_min = RTO_MIN;
    rtoinfo.srto_max = RTO_MAX;

    /* Set rto parameters */
    if (setsockopt(sock, SOL_SCTP, SCTP_RTOINFO,
                  &rtoinfo, sizeof(rtoinfo)) != 0){
        perror("setsockopt");
        return -1;
    }

    initmsg.sinit_num_ostreams = MAX_OUT_STREAM;
    initmsg.sinit_max_instreams = MAX_IN_STREAM;
    initmsg.sinit_max_attempts = MAX_INIT_ATTEMPTS;

    /* Set SCTP Init Message */
    if ((ret = setsockopt(sock, SOL_SCTP, SCTP_INITMSG,
                         &initmsg, sizeof(initmsg))) != 0) {
        perror("setsockopt");
        return -1;
    }

    assoc_param.sasoc_asocmaxrxt = ASSOC_MAX_RETRANSMIT;

    if ((ret = setsockopt(sock, SOL_SCTP, SCTP_ASSOCINFO,
                          &assoc_param, sizeof(assoc_param))) != 0) {
        perror("setsockopt");
        return -1;
    }
 
    /*Set the Reuse of Address*/
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0) {
        perror("setsockopt");
        return -1;
    }
}

void handle_signal(int signum)
{
    switch(signum)
    {
    case SIGINT:
        printf("close called\n");
        if (close(sock) != 0)
            perror("close");
        exit(0);
        break;  
    default:
        exit(0);
        break;
    }
}

int main(int argc, char **argv)
{
    char bind_ip1[20];
    char bind_ip2[20];
    char server_ip[20];
    int port = 0;

    if (argc != 5 || (inet_addr(argv[1]) == -1) ||
        (inet_addr(argv[2]) == -1) ||
        (inet_addr(argv[3]) == -1) ||
        (atoi(argv[4]) > 65535) || (atoi(argv[4]) < 0)) {

        puts("Usage: ./client bind_ip1 bind_ip2 server_ip port");        
        return -1;
    }

    strncpy(bind_ip1, argv[1], 20);
    strncpy(bind_ip2, argv[2], 20);
    strncpy(server_ip, argv[3], 20);
    port = atoi(argv[4]);

    printf("bind_ip1 is %s\n", bind_ip1);
    printf("bind_ip2 is %s\n", bind_ip2);
    printf("server_ip is %s\n", server_ip);
    printf("port is %d\n", port);

    memset(&buffer, 'n', sizeof(buffer));

    /* create SCTP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0) {
        perror("socket");
        return -1;
    }

    sig_handler.sa_handler = handle_signal;
    sig_handler.sa_flags = 0;

    /*Set Signal Handler*/
    if (sigaction(SIGINT, &sig_handler, NULL) == -1) {
        perror("sigaction");
        close(sock);
        return -1;
    }

    /* apply sctp parameters */
    if (apply_sctp_params(sock) < 0) {
        printf("apply_sctp_param failed");
        close(sock);
        return -1;
    }

    bindaddr[0].sin_family = AF_INET;
    bindaddr[0].sin_port = htons(port);
    bindaddr[0].sin_addr.s_addr = inet_addr(bind_ip1);

    bindaddr[1].sin_family = AF_INET;
    bindaddr[1].sin_port = htons(port);
    bindaddr[1].sin_addr.s_addr = inet_addr(bind_ip2);


    if (sctp_bindx(sock, (struct sockaddr*)bindaddr,
                  2, SCTP_BINDX_ADD_ADDR ) == -1) {
        perror("bind");
        close(sock);
        return -1;
    }

    serveraddr[0].sin_family = AF_INET;
    serveraddr[0].sin_port = htons(port);
    serveraddr[0].sin_addr.s_addr = inet_addr(server_ip);

    /*Connect to Host*/
    if (connect(sock, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) < 0)
    {
        perror("connect");
        close(sock);
        return -1;
    }

    /* Print Locally Binded Addresses */
    addr_count = sctp_getladdrs(sock, 0, (struct sockaddr**)laddr);
    printf("Addresses binded: %d\n", addr_count);
    for (i = 0; i < addr_count; i++)
    {
        printf("Address %d: %s:%d\n", i +1, inet_ntoa((*laddr)[i].sin_addr),
                                            (*laddr)[i].sin_port);
    }
    sctp_freeladdrs((struct sockaddr*)*laddr);

    for (i = 0; i < 100; i++)
    {
        printf("Sending data chunk #%d...\n", i+1);

        if ((ret = sctp_sendmsg(sock, buffer, BUFFER_SIZE, NULL, 0,
                         0, 0, 1, 0, 10)) < 0) {
            printf("error\n");
            perror("write");
        }
        printf("Sent %d bytes to peer on stream 1\n",ret);

        if ((ret = sctp_sendmsg(sock, buffer, BUFFER_SIZE, NULL, 0,
                         0, 0, 2, 0, 11)) < 0) {
            printf("error\n");
            perror("write");
        }
        printf("Sent %d bytes to peer on stream 2\n",ret);

        sleep(1);
    }

    if (close(sock) < 0)
        perror("close");
}   
