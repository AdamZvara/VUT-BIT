/**
 * @brief   Implementation of common functions used by whole program
 * @author  xzvara01(@vutbr.cz)
 * @file    common.c
 * @date    23.04.2022
 */

#include "common.h"

bool valid_address(char *address)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, address, &(sa.sin_addr));
    if (result == 0) {
        result = inet_pton(AF_INET6, address, &(sa.sin_addr));
    }
    return result != 0;
}

void domain_to_IP(char *domain)
{
    struct addrinfo hints, *res;
    int status;
    static char ip[INET6_ADDRSTRLEN];
    memset(ip, 0, INET6_ADDRSTRLEN);

    /* First try to get IPv4 address */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    if((status = getaddrinfo(domain, NULL, &hints, &res)) != 0){
        fprintf(stderr, "Failed to get IP address\n");
        exit(ERR);
    }

    if (res != NULL) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(res->ai_family, &(ipv4->sin_addr), ip, sizeof(ip));
        strncpy(domain, ip, INET6_ADDRSTRLEN);
        freeaddrinfo(res);
        return;
    }

    /* IPv4 address was not found, try ipv6 */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;

    // get the address information
    if((status = getaddrinfo(domain, NULL, &hints, &res)) != 0){
        fprintf(stderr, "Failed to get IP address\n");
        exit(ERR);
    }

    if (res != NULL) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
        inet_ntop(res->ai_family, &(ipv6->sin6_addr), ip, sizeof(ip));
        strncpy(domain, ip, INET6_ADDRSTRLEN);
        freeaddrinfo(res);
        return;
    }

    error_internal();
}

void print_interfaces()
{
    struct if_nameindex *name_index, *intf;

    name_index = if_nameindex();
    if (name_index != NULL) {
        for (intf = name_index; intf->if_index != 0 || intf->if_name != NULL; intf++) {
            printf("%s\n", intf->if_name);
        }
        if_freenameindex(name_index);
    }
}

uint32_t get_interface_ipv4(char *interface)
{
    int socket_fd;
    struct ifreq ifr;

    if ((socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
        error_internal();
    
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
    ioctl(socket_fd, SIOCGIFADDR, &ifr);
    return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}

char *get_interface_ipv6(char *interface)
{
    static char ip[INET6_ADDRSTRLEN];
    struct ifaddrs *ifa, *ifa_iter;
    if (getifaddrs(&ifa) == -1) {
        perror("getifaddrs() failed to get interfaces: ");
        exit(ERR);
    }

    ifa_iter = ifa;
	// iterate through getifaddrs linked list and find interface with IPv6 address and matching interface name
    for (ifa_iter = ifa; ifa_iter != NULL; ifa_iter = ifa_iter->ifa_next) {
        if ((ifa_iter->ifa_addr) && (ifa_iter->ifa_addr->sa_family == AF_INET6) && !strcmp(ifa_iter->ifa_name, interface)) {
            struct sockaddr_in6 *in6 = (struct sockaddr_in6*) ifa_iter->ifa_addr;
            inet_ntop(AF_INET6, &in6->sin6_addr, ip, INET6_ADDRSTRLEN);
        }
    }
    freeifaddrs(ifa);

    return ip;
}

char *set_filter_string(struct arguments uargs, int port, char *protocol)
{
    static char filter_string[FILTER_STR_LEN];
    memset(filter_string, 0, FILTER_STR_LEN);

    strcat(filter_string, protocol);
    if (!strcmp(protocol, "tcp")) {
        strcat(filter_string, " and src port ");
        char port_str[10] = "";
        snprintf(port_str, 10, "%d", port);
        strcat(filter_string, port_str);
        strcat(filter_string, " and dst port ");
        snprintf(port_str, 10, "%d", SENDER_PORT);
        strcat(filter_string, port_str);
    }
    strcat(filter_string, " and src host ");
    strcat(filter_string, uargs.domain);
    strcat(filter_string, " and dst host ");
    if (strstr(uargs.domain, ".")) {
        int32_t IP_int = get_interface_ipv4(uargs.interface);
        char IP_str[INET6_ADDRSTRLEN];
        strcat(filter_string, inet_ntop(AF_INET, &IP_int, IP_str, INET6_ADDRSTRLEN));
    } else {
        strcat(filter_string, get_interface_ipv6(uargs.interface));
    }

    return filter_string;
}

void breakloop(int signum)
{
    (void) signum;
    pcap_breakloop(handle);
}

unsigned short csum(unsigned short *ptr, int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum=0;
    while(nbytes>1) {
      sum+=*ptr++;
      nbytes-=2;
    }
    if(nbytes==1) {
      oddbyte=0;
      *((u_char*)&oddbyte)=*(u_char*)ptr;
      sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}