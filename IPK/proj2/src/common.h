/**
 * @brief   Header file for common functions used by whole program
 * @author  xzvara01(@vutbr.cz)
 * @file    common.h
 * @date    23.04.2022
 */

#ifndef _COMMON_H
#define _COMMON_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>    
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>              // errno, perror()
#include <pcap.h>               // pcap_breakloop
#include <sys/types.h>
#include <sys/socket.h>         // needed for socket
#include <sys/ioctl.h>          // ioctl
#include <netinet/in.h>         // sockaddr_in, in_addr
#include <net/if.h>             // if_nameindex
#include <netdb.h>              // getaddrinfo, struct addrinfo
#include <ifaddrs.h>            // struct ifaddrs

#include "parse_args.h"

#define SENDER_PORT 46300

#define FILTER_STR_LEN 1024
#define ETH_HDRLEN 14  // Ethernet header length
#define IP6_HDRLEN 40  // IPv6 header length

#define ERR 1
#define error_internal() {fprintf(stderr, "Internal error occured\n"); exit(ERR);}

extern pcap_t *handle;		// global pcap handle from main file

/**
 * @enum port_status
 * 
 * @brief Values indicating state of scanned port
 */
typedef enum port_status {NONE, OPENED, FILTERED, CLOSED} p_status;

/**
 * @brief Check if given address is in valid IP(v4/v6) format
 *
 * @return True if address is in correct format, otherwise False
 */
bool valid_address(char *address);

/**
 * @brief Convert domain name to IP address (primarily IPv4)
 *
 * @param[in,out] domain Domain name which will be converted to IP address and stored back
 */
void domain_to_IP(char *domain);

/**
 * @brief Print all existing network interfaces
 * 
 * author: brm
 * source: https://stackoverflow.com/a/19228190
 */
void print_interfaces();

/**
 * @brief Get IPv4 address of used interface for pseudo header
 *
 * @param[in] interface String name of interface 
 *
 * @return 32bit IP address of interface
 *
 * source: https://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
 * author: Akimichi Ogawa
 */
uint32_t get_interface_ipv4(char *interface);

/**
 * @brief Get IPv6 address of used interface
 *
 * @param[in] interface String name of interface 
 *
 * @return String IP address of interface
 */
char *get_interface_ipv6(char *interface);

/**
 * @brief Create string to filter out unwanted traffic, dst ip address can be ipv4 or ipv6 depending on domain address
 * @details Example output string: tcp and src port 1234 and dst port 1234 and src host 127.0.0.1 and dst host 127.0.0.1
 * 
 * @param[in] uargs Program arguments
 * @param[in] port  Destination port number 
 * @param[in] socket_fd Socket descriptor
 * @param[in] protocol  Type of protocol
 * 
 * @return Pointer to static char array representing filter string
 */
char *set_filter_string(struct arguments uargs, int port, char *protocol);

/**
 * @brief Event handler for SIGALRM used for stopping pcap_next
 * 
 * @param[in] signum Signal number
 */
void breakloop(int signum);

/**
 * @brief Calculate generic checksum 
 * 
 * @return Calculated checksum
 *
 * source: https://www.binarytides.com/raw-sockets-c-code-linux/
 * author: Silver Moon
 */
unsigned short csum(unsigned short *ptr, int nbytes);

#endif