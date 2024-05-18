/**
 * @brief   Simple TCP/UDP port scanner 
 * @author  xzvara01(@vutbr.cz)
 * @file    ipk-l4-scan.c
 * @date    22.04.2022
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcap.h>
#include "src/common.h"
#include "src/parse_args.h"
#include "src/udp_scan.h"
#include "src/tcp_scan.h"

/* Global variables */
pcap_t *handle;

/**
 * @brief Print status of single scanned port
 * 
 * @param[in] portnum   Port number
 * @param[in] status    Status of port
 * @param[in] protocol  Protocol type to be printed
 */
void print_status(int portnum, enum port_status status, char *protocol);

/**
 * @brief Print status of every opened scanned port (exclude closed and filtered ports)
 * 
 * @see print_status_all
 */
void print_status_opened(int portnum, enum port_status status, char *protocol);


int main(int argc, char const *argv[])
{
    struct arguments *user_args;
    user_args = parse_args(argc, (char **)argv);
    // print_args(*user_args);

    /* Prepare pcap handle for capturing responses from server */
    char error_buffer[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live(user_args->interface, BUFSIZ, 0, 100, error_buffer);
    if (handle == NULL) {
        fprintf(stderr, "pcap_open_live(): %s\n", error_buffer);
        return ERR;
    }

    /* Make sure we are capturing on Ethernet */
    if(pcap_datalink(handle) != DLT_EN10MB){
        fprintf(stderr, "Not capturing on ethernet\n");
        exit(ERR);
    }

    printf("Interesting ports on %s:\n", user_args->domain);
    printf("PORT\tSTATE\n");

    enum port_format pformat;
    enum port_status pstatus;
    
    /* TCP scanning */
    if ((pformat = user_args->tcp_type) != 0) { // check if user asked for any TCP ports
        if (strstr(user_args->domain, ".") && !(strstr(user_args->domain, ":"))) { // ipv4 address
            if (pformat == CONT) {
                for (int i = user_args->tcp.start; i <= user_args->tcp.end; i++) {
                    pstatus = tcp_ipv4_scan(*user_args, i);
                    print_status(i, pstatus, "tcp");
                }
            } else {
                for (int i = 0; i < user_args->tcp.array_length; i++) {
                    pstatus = tcp_ipv4_scan(*user_args, user_args->tcp.array[i]);
                    print_status(user_args->tcp.array[i], pstatus, "tcp");
                }   
            }
        } else if (strstr(user_args->domain, ":") && !(strstr(user_args->domain, "."))) { // ipv6 address
            if (pformat == CONT) {
                for (int i = user_args->tcp.start; i <= user_args->tcp.end; i++) {
                    pstatus = tcp_ipv6_scan(*user_args, i);
                    print_status(i, pstatus, "tcp");
                }
            } else {
                for (int i = 0; i < user_args->tcp.array_length; i++) {
                    pstatus = tcp_ipv6_scan(*user_args, user_args->tcp.array[i]);
                    print_status(user_args->tcp.array[i], pstatus, "tcp");
                }   
            }
        } else {
            fprintf(stderr, "Unsuported domain format\n");
            exit(ERR);
        }
    }

    /* UDP scanning */
    if ((pformat = user_args->udp_type) != 0) {    // check if user asked for any UDP ports
        if (strstr(user_args->domain, ".") && !(strstr(user_args->domain, ":"))) { // ipv4 address
            if (pformat == CONT) {
                for (int i = user_args->udp.start; i <= user_args->udp.end; i++) {
                    pstatus = udp_ipv4_scan(*user_args, i);
                    print_status(i, pstatus, "udp");
                }
            } else {
                for (int i = 0; i < user_args->udp.array_length; i++) {
                    pstatus = udp_ipv4_scan(*user_args, user_args->udp.array[i]);
                    print_status(user_args->udp.array[i], pstatus, "udp");
                }
            }
        } else if (strstr(user_args->domain, ":") && !(strstr(user_args->domain, "."))) { // ipv6 address
            if (pformat == CONT) {
                for (int i = user_args->udp.start; i <= user_args->udp.end; i++) {
                    pstatus = udp_ipv6_scan(*user_args, i);
                    print_status(i, pstatus, "udp");
                }
            } else {
                for (int i = 0; i < user_args->udp.array_length; i++) {
                    pstatus = udp_ipv6_scan(*user_args, user_args->udp.array[i]);
                    print_status(user_args->udp.array[i], pstatus, "udp");
                }
            }
        } else {
            fprintf(stderr, "Unsuported domain format\n");
            exit(ERR);
        }
    }

    free_args(user_args);
    pcap_close(handle);
    
    return 0;
}

void print_status(int portnum, enum port_status status, char *protocol)
{
    printf("%d/%s\t", portnum, protocol);
    switch (status) {
        case OPENED:
            printf("opened\n");
            break;

        case CLOSED:
            printf("closed\n");
            break;

        case FILTERED:
            printf("filtered\n");
            break;
        
        default:
            printf("error occured\n");
            break;
    }
}

void print_status_opened(int portnum, enum port_status status, char *protocol)
{
    if (status == OPENED) {
        printf("%d/%s\t", portnum, protocol);
        printf("opened\n");
    }
}