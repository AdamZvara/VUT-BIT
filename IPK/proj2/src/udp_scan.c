/**
 * @brief 	Implementation of UDP scanning functions
 * @author  xzvara01(@vutbr.cz)
 * @file    udp_scan.c
 * @date    22.04.2022
 */

#include <netinet/udp.h>	    // struct udphdr
#include <netinet/ip_icmp.h>	// struct icmphdr
#include <netinet/icmp6.h>      // struct icmp6 
#include <netinet/ip6.h>        // struct ip6_hdr
#include <arpa/inet.h>          // inet_pton() and inet_ntop()
#include <linux/if_ether.h>     // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>    // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include "udp_scan.h"

#define UDP_HDRLEN  8

/**
 * @brief Send UDP packet
 * 
 * @param[in] socket Socket descriptor
 * @param[in] domain Domain (IP address) to send packet to
 * @param[in] interface Name of interface to send packet from
 * @param[in] port Port number to send packet to 
 */
void ipv4_udp(int socket, char *domain, char *interface, int port)
{
    /* Bind socket to interface given by user */
    if ((setsockopt(socket, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface))) == -1) {
        perror("Unable to bind socket to given interface");
        exit(ERR);
    }

    // set destination port and address for connection
    struct sockaddr_in dst_address;
    dst_address.sin_family = AF_INET;
    dst_address.sin_port = htons(port);
    inet_pton(AF_INET, domain, &(dst_address.sin_addr.s_addr));  // convert string IP address

    /* Create UDP header */
    struct udphdr *udph = malloc(sizeof(struct udphdr));
    memset(udph, 0, sizeof(struct udphdr)); // initialize udp header

    udph->source = htons(SENDER_PORT);
    udph->dest = htons(port);
    udph->len = htons(sizeof(struct udphdr));

    /* Connect to given interface on given port and send packet*/
    if (connect(socket, (struct sockaddr*)&dst_address, sizeof(dst_address)) == -1) {
        perror("Unable to connect to address");
        free(udph);
        exit(ERR);
    }

    sendto(socket, udph, sizeof(struct udphdr), 0, (struct sockaddr*)&dst_address, sizeof(dst_address));
    free(udph);
}

p_status udp_ipv4_scan(struct arguments uargs, int port)
{
     /* Create raw socket which will be using UDP protocol */
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (socket_fd == -1) {
        perror("Unable to create socket");
        exit(ERR);
    }

    /* Set pcap filter string */
    struct bpf_program fp;
    bpf_u_int32 netp, maskp;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_lookupnet(uargs.interface, &netp, &maskp, errbuf);
    char *filter_string = set_filter_string(uargs, port, "icmp");

    if (pcap_compile(handle, &fp, filter_string, 0, netp) == -1) {
        perror("Error calling pcap_compile");
        exit(ERR);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        perror("Error setting filter");
        exit(ERR);
    }

    /* Send UDP packet */
    ipv4_udp(socket_fd, uargs.domain, uargs.interface, port);

    /* Prepare alarm signal to interrupt pcap_next
     *  
     * source: https://stackoverflow.com/a/13749514
     * author: lemonsqueeze
     */
    if (signal(SIGALRM, breakloop) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        exit(ERR);
    }

    struct itimerval time;
    time.it_interval.tv_sec = time.it_interval.tv_usec = 0;
    time.it_value.tv_sec = uargs.timeout / 1000;
    time.it_value.tv_usec = (uargs.timeout % 1000) * 1000;

    if (setitimer(ITIMER_REAL, &time, NULL) == -1) {
        perror("Unable to send SIGALRM");
        exit(ERR);
    }

    /* Catch response */
    struct pcap_pkthdr hdr;
    const u_char *packet;
    packet = pcap_next(handle, &hdr);
    if (packet == NULL) {
        pcap_freecode(&fp);
        close(socket_fd);
        return OPENED;
    }
    
    /* Get to ICMP header from incoming packet and check type */
    struct iphdr *ip = (struct iphdr *)(packet + ETH_HDRLEN);
    int ip_header_length = ((ip->ihl) & 0xf) * 4;

    struct icmphdr* icmp_header = (struct icmphdr*) (packet + ETH_HDRLEN + ip_header_length);
    if (icmp_header->type == 3) {
        pcap_freecode(&fp);
        close(socket_fd);
        return CLOSED;
    }
 
    pcap_freecode(&fp);
    close(socket_fd);

    return NONE;
}

/**
 * @brief Build IPv6 UDP pseudo-header and call checksum function
 * 
 * author: P.D. Buchan
 * source: https://www.pdbuchan.com/rawsock/udp6_ll.c
 */
uint16_t udp6_checksum(struct ip6_hdr iphdr, struct udphdr udphdr) 
{
    char buf[IP_MAXPACKET];
    char *ptr;
    int chksumlen = 0;

    ptr = &buf[0];  // ptr points to beginning of buffer buf

    // Copy source IP address into buf (128 bits)
    memcpy(ptr, &iphdr.ip6_src.s6_addr, sizeof(iphdr.ip6_src.s6_addr));
    ptr += sizeof(iphdr.ip6_src.s6_addr);
    chksumlen += sizeof(iphdr.ip6_src.s6_addr);

    // Copy destination IP address into buf (128 bits)
    memcpy(ptr, &iphdr.ip6_dst.s6_addr, sizeof(iphdr.ip6_dst.s6_addr));
    ptr += sizeof(iphdr.ip6_dst.s6_addr);
    chksumlen += sizeof(iphdr.ip6_dst.s6_addr);

    // Copy UDP length into buf (32 bits)
    memcpy(ptr, &udphdr.len, sizeof(udphdr.len));
    ptr += sizeof(udphdr.len);
    chksumlen += sizeof(udphdr.len);

    // Copy zero field to buf (24 bits)
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 3;

    // Copy next header field to buf (8 bits)
    memcpy(ptr, &iphdr.ip6_nxt, sizeof(iphdr.ip6_nxt));
    ptr += sizeof(iphdr.ip6_nxt);
    chksumlen += sizeof(iphdr.ip6_nxt);

    // Copy UDP source port to buf (16 bits)
    memcpy(ptr, &udphdr.source, sizeof(udphdr.source));
    ptr += sizeof(udphdr.source);
    chksumlen += sizeof(udphdr.source);

    // Copy UDP destination port to buf (16 bits)
    memcpy(ptr, &udphdr.dest, sizeof(udphdr.dest));
    ptr += sizeof(udphdr.dest);
    chksumlen += sizeof(udphdr.dest);

    // Copy UDP length again to buf (16 bits)
    memcpy(ptr, &udphdr.len, sizeof(udphdr.len));
    ptr += sizeof(udphdr.len);
    chksumlen += sizeof(udphdr.len);

    // Copy UDP checksum to buf (16 bits)
    // Zero, since we don't know it yet
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 2;

    return csum((uint16_t *) buf, chksumlen);
}

/**
 * @brief Send IPv6 UDP packet
 * 
 * author: P.D. Buchan
 * source: https://www.pdbuchan.com/rawsock/udp6_ll.c (modified)
 */
void ipv6_udp(int socket_fd, char *domain, char *interface, int port)
{
    int status;

	uint8_t *ether_frame = malloc(IP_MAXPACKET * sizeof(uint8_t));
	if (ether_frame == NULL)
		error_internal();

    /* Submit request for a socket descriptor to look up interface */
	int sd;
    if ((sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
      perror("socket() failed to get socket descriptor for using ioctl(): ");
      exit(ERR);
    }

    /* Use ioctl() to look up interface name and get its MAC address */
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);
    if (ioctl(sd, SIOCGIFHWADDR, &ifr) < 0) {
      perror("ioctl() failed to get source MAC address:");
      exit(ERR);
    }
    close (sd);

    /* Get source MAC and IPv6 address */
    char src_ip[INET6_ADDRSTRLEN];
	uint8_t src_mac[8];
    memcpy(src_mac, ifr.ifr_hwaddr.sa_data, 6 * sizeof(uint8_t));
    memcpy(src_ip, get_interface_ipv6(interface), sizeof(src_ip));
	

    /* Find interface index from interface name and store index in
       struct sockaddr_ll device, which will be used as an argument of sendto() */
    struct sockaddr_ll device;
    memset(&device, 0, sizeof(device));
    if ((device.sll_ifindex = if_nametoindex(interface)) == 0) {
      perror("if_nametoindex() failed to obtain interface index: ");
      exit(ERR);
    }

    /* Set destination MAC address
	! Destination MAC address is normally address to the nearest transit node, which is found using
	neighbour discovery, in this example it will only work for loopback interface (localhost) ! */
    uint8_t dst_mac[8];
	for (int j = 0; j <= 5; j++) {
    	dst_mac[j] = 0x00;
	}

    /* Fill out hints for getaddrinfo() */
	struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = hints.ai_flags | AI_CANONNAME;

    /* Resolve target using getaddrinfo() */
	char dst_ip[INET6_ADDRSTRLEN];
    struct sockaddr_in6 *ipv6;
    void *tmp;
    if ((status = getaddrinfo(domain, NULL, &hints, &res)) != 0) {
      fprintf(stderr, "getaddrinfo() failed for target: %s\n", gai_strerror(status));
      exit(ERR);
    }

    ipv6 = (struct sockaddr_in6 *) res->ai_addr;
    tmp = &(ipv6->sin6_addr);

    if (inet_ntop(AF_INET6, tmp, dst_ip, INET6_ADDRSTRLEN) == NULL) {
      status = errno;
      fprintf(stderr, "inet_ntop() failed for target.\nError message: %s", strerror(status));
      exit(ERR);
    }
    freeaddrinfo (res);

    /* Fill out sockaddr_ll */
    device.sll_family = AF_PACKET;
    memcpy(device.sll_addr, src_mac, 6 * sizeof(uint8_t));
    device.sll_halen = 6;

    /* IPv6 header */
    struct ip6_hdr iphdr;
    // IPv6 version (4 bits), Traffic class (8 bits), Flow label (20 bits)
    iphdr.ip6_flow = htonl((6 << 28) | (0 << 20) | 0);
    iphdr.ip6_plen = htons(UDP_HDRLEN);
    iphdr.ip6_nxt = IPPROTO_UDP;
    iphdr.ip6_hops = 255;
    // Source IPv6 address (128 bits)
    if ((status = inet_pton (AF_INET6, src_ip, &(iphdr.ip6_src))) != 1) {
      fprintf(stderr, "inet_pton() failed for source address.\nError message: %s", strerror(status));
      exit(ERR);
    }
    // Destination IPv6 address (128 bits)
    if ((status = inet_pton (AF_INET6, dst_ip, &(iphdr.ip6_dst))) != 1) {
      fprintf(stderr, "inet_pton() failed for destination address.\nError message: %s", strerror(status));
      exit(ERR);
    }

    /* UDP header */
    struct udphdr udphdr;
	memset(&udphdr, 0, sizeof(struct udphdr));
    udphdr.source = htons(SENDER_PORT);
    udphdr.dest = htons(port);
    udphdr.len = htons(UDP_HDRLEN);
    udphdr.check = udp6_checksum (iphdr, udphdr);

    /* Fill out ethernet frame header */
    // ethernet header (2*MAC(6) + ethernet type(2)) + ethernet data (IP header + UDP header)
    int frame_length = 14 + IP6_HDRLEN + UDP_HDRLEN;
    // Destination and Source MAC addresses
    memcpy(ether_frame, dst_mac, 6 * sizeof(uint8_t));	// dst MAC
    memcpy(ether_frame + 6, src_mac, 6 * sizeof(uint8_t)); // src MAC
    ether_frame[12] = ETH_P_IPV6 / 256; // ethernet type code
    ether_frame[13] = ETH_P_IPV6 % 256;
    memcpy(ether_frame + ETH_HDRLEN, &iphdr, IP6_HDRLEN * sizeof(uint8_t)); // IPv6 header
    memcpy(ether_frame + ETH_HDRLEN + IP6_HDRLEN, &udphdr, UDP_HDRLEN * sizeof(uint8_t)); // UDP header

    /* Send ethernet frame to socket */
    if ((sendto (socket_fd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof(device))) <= 0) {
      perror("sendto() failed: ");
      exit(ERR);
    }

    free(ether_frame);
}

p_status udp_ipv6_scan(struct arguments uargs, int port)
{
	/* Get raw socket */
    int socket_fd;
    if ((socket_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
      perror("socket() failed: ");
      exit(ERR);
    }

    /* Set pcap filter string */
    struct bpf_program fp;
    bpf_u_int32 netp, maskp;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_lookupnet(uargs.interface, &netp, &maskp, errbuf);
    char *filter_string = set_filter_string(uargs, port, "icmp6");

    if (pcap_compile(handle, &fp, filter_string, 0, netp) == -1) {
        perror("Error calling pcap_compile");
        exit(1);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        perror("Error setting filter");
        exit(1);
    }

    /* Send UDP packet */
    ipv6_udp(socket_fd, uargs.domain, uargs.interface, port);

    /* Prepare alarm signal to interrupt pcap_next
     *  
     * source: https://stackoverflow.com/a/13749514
     * author: lemonsqueeze
     */
    if (signal(SIGALRM, breakloop) == SIG_ERR) {
        perror("Unable to catch SIGALRM");
        exit(1);
    }

    struct itimerval time;
    time.it_interval.tv_sec = time.it_interval.tv_usec = 0;
    time.it_value.tv_sec = uargs.timeout / 1000;
    time.it_value.tv_usec = (uargs.timeout % 1000) * 1000;

    if (setitimer(ITIMER_REAL, &time, NULL) == -1) {
        perror("Unable to send SIGALRM");
        exit(1);
    }

    /* Catch response */
    struct pcap_pkthdr hdr;
    const u_char *packet;
    packet = pcap_next(handle, &hdr);
    if (packet == NULL) {
        pcap_freecode(&fp);
        close(socket_fd);
        return OPENED;
    }
    
    /* Get to ICMP header from incoming packet and check type */
    struct icmp6_hdr* icmp_header = (struct icmp6_hdr *) (packet + ETH_HDRLEN + IP6_HDRLEN);
    if (icmp_header->icmp6_type == 1 && icmp_header->icmp6_code == 4) {
        pcap_freecode(&fp);
        close(socket_fd);
        return CLOSED;
    }
 
    pcap_freecode(&fp);
    close(socket_fd);

    return NONE;
}