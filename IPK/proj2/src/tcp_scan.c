/**
 * @brief 	Implementation of TCP scanning functions
 * @author  xzvara01(@vutbr.cz)
 * @file    tcp_scan.c
 * @date    22.04.2022
 */

#include <netinet/tcp.h>	    // struct tcphdr
#include <netinet/ip.h>	        // struct iphdr
#include <netinet/ip6.h>        // struct ip6_hdr
#include <arpa/inet.h>          // inet_pton and inet_ntop
#include <linux/if_ether.h>     // ETH_P_IP, ETH_P_IPV6
#include <linux/if_packet.h>    // struct sockaddr_ll
#include <net/ethernet.h>
#include "tcp_scan.h"

#define TCP_HDRLEN 20  // TCP header length, excludes options data
#define ICMP_HDRLEN 8         // ICMP header length for echo request, excludes data

/**
 * @brief Send TCP packet with SYN flag set
 * 
 * @param[in] socket Socket descriptor
 * @param[in] domain Domain (IP address) to send packet to
 * @param[in] interface Name of interface to send packet from
 * @param[in] port Port number to send packet to 
 */
void ipv4_syn(int socket, char *domain, char *interface, int port)
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

    /* Create TCP header
     * source: https://www.binarytides.com/raw-sockets-c-code-linux/
     * author: Silver Moon */
    struct tcphdr *tcph = malloc(sizeof(struct tcphdr));
    memset(tcph, 0, sizeof(struct tcphdr)); // initialize tcp header
    tcph->source = htons(SENDER_PORT);
    tcph->dest = htons(port);
    tcph->doff = 5;	                        // tcp header size
    tcph->syn = 1;                          // SYN flag
    tcph->window = htons(5840);	            // maximum allowed window size

    // fill IP pseudo header 
	  struct pseudo_header psh;
    psh.source_address = get_interface_ipv4(interface);
    psh.dest_address = dst_address.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
    char *pseudogram = malloc(psize);

    memcpy(pseudogram , (char*) &psh , sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr));

      // calculate TCP header checksum
    tcph->check = csum( (unsigned short*) pseudogram , psize);

    /* Connect to given interface on given port and send packet*/
    if (connect(socket, (struct sockaddr*)&dst_address, sizeof(dst_address)) == -1) {
        perror("Unable to connect to address");
        exit(ERR);
    }

    sendto(socket, tcph, sizeof(struct tcphdr), 0, (struct sockaddr*)&dst_address, sizeof(dst_address));

    free(tcph);
    free(pseudogram);
}

p_status tcp_ipv4_scan(struct arguments uargs, int port)
{
    /* Create raw socket which will be using TCP protocol */
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (socket_fd == -1) {
        perror("Unable to create socket");
        exit(1);
    }

    /* Set pcap filter string */
    struct bpf_program fp;
    bpf_u_int32 netp, maskp;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_lookupnet(uargs.interface, &netp, &maskp, errbuf);
    char *filter_string = set_filter_string(uargs, port, "tcp");

    if (pcap_compile(handle, &fp, filter_string, 0, netp) == -1) {
        perror("Error calling pcap_compile");
        exit(1);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        perror("Error setting filter");
        exit(1);
    }

    /* Send SYN packet */
    ipv4_syn(socket_fd, uargs.domain, uargs.interface, port);

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
        // no packet captured, resend packet with SYN flag
        ipv4_syn(socket_fd, uargs.domain, uargs.interface, port);

        if (setitimer(ITIMER_REAL, &time, NULL) == -1) {
            perror("Unable to send SIGALRM");
            exit(1);
        }

        packet = pcap_next(handle, &hdr);
        if (packet == NULL) {
            pcap_freecode(&fp);
            close(socket_fd);
            return FILTERED;
        }
    }

    /* Get to TCP header from incoming packet and check flags */
    struct iphdr *ip = (struct iphdr *)(packet + ETH_HDRLEN);
    int ip_header_length = ((ip->ihl) & 0xf) * 4;

    struct tcphdr* tcp_header = (struct tcphdr*) (packet + ETH_HDRLEN + ip_header_length);
    if(tcp_header->rst == 1 && tcp_header->ack == 1){
        pcap_freecode(&fp);
        close(socket_fd);
        return CLOSED;
    }
    else if(tcp_header->rst == 0 && tcp_header->ack == 1){
        pcap_freecode(&fp);
        close(socket_fd);
        return OPENED;
    }

    pcap_freecode(&fp);
    close(socket_fd);

    return NONE;
}

/**
 * @brief Build IPv6 TCP pseudo-header and call checksum function (Section 8.1 of RFC 2460)
 *
 * author: P.D. Buchan
 * source: https://www.pdbuchan.com/rawsock/tcp6_ll.c
 */
uint16_t tcp6_checksum(struct ip6_hdr iphdr, struct tcphdr tcphdr) 
{
    uint32_t lvalue;
    char buf[IP_MAXPACKET], cvalue;
    char *ptr;
    int chksumlen = 0;

    ptr = &buf[0];  // ptr points to beginning of buffer buf

    // Copy source IP address into buf (128 bits)
    memcpy(ptr, &iphdr.ip6_src, sizeof(iphdr.ip6_src));
    ptr += sizeof(iphdr.ip6_src);
    chksumlen += sizeof(iphdr.ip6_src);

    // Copy destination IP address into buf (128 bits)
    memcpy(ptr, &iphdr.ip6_dst, sizeof(iphdr.ip6_dst));
    ptr += sizeof(iphdr.ip6_dst);
    chksumlen += sizeof(iphdr.ip6_dst);

    // Copy TCP length to buf (32 bits)
    lvalue = htonl(sizeof(tcphdr));
    memcpy(ptr, &lvalue, sizeof(lvalue));
    ptr += sizeof(lvalue);
    chksumlen += sizeof(lvalue);

    // Copy zero field to buf (24 bits)
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 3;

    // Copy next header field to buf (8 bits)
    memcpy(ptr, &iphdr.ip6_nxt, sizeof(iphdr.ip6_nxt));
    ptr += sizeof(iphdr.ip6_nxt);
    chksumlen += sizeof(iphdr.ip6_nxt);

    // Copy TCP source port to buf (16 bits)
    memcpy(ptr, &tcphdr.th_sport, sizeof(tcphdr.th_sport));
    ptr += sizeof(tcphdr.th_sport);
    chksumlen += sizeof(tcphdr.th_sport);

    // Copy TCP destination port to buf (16 bits)
    memcpy(ptr, &tcphdr.th_dport, sizeof(tcphdr.th_dport));
    ptr += sizeof(tcphdr.th_dport);
    chksumlen += sizeof(tcphdr.th_dport);

    // Copy sequence number to buf (32 bits)
    memcpy(ptr, &tcphdr.th_seq, sizeof(tcphdr.th_seq));
    ptr += sizeof(tcphdr.th_seq);
    chksumlen += sizeof(tcphdr.th_seq);

    // Copy acknowledgement number to buf (32 bits)
    memcpy(ptr, &tcphdr.th_ack, sizeof(tcphdr.th_ack));
    ptr += sizeof(tcphdr.th_ack);
    chksumlen += sizeof(tcphdr.th_ack);

    // Copy data offset to buf (4 bits) and
    // copy reserved bits to buf (4 bits)
    cvalue = (tcphdr.th_off << 4) + tcphdr.th_x2;
    memcpy(ptr, &cvalue, sizeof(cvalue));
    ptr += sizeof(cvalue);
    chksumlen += sizeof(cvalue);

    // Copy TCP flags to buf (8 bits)
    memcpy(ptr, &tcphdr.th_flags, sizeof(tcphdr.th_flags));
    ptr += sizeof(tcphdr.th_flags);
    chksumlen += sizeof(tcphdr.th_flags);

    // Copy TCP window size to buf (16 bits)
    memcpy(ptr, &tcphdr.th_win, sizeof(tcphdr.th_win));
    ptr += sizeof(tcphdr.th_win);
    chksumlen += sizeof(tcphdr.th_win);

    // Copy TCP checksum to buf (16 bits)
    // Zero, since we don't know it yet
    *ptr = 0; ptr++;
    *ptr = 0; ptr++;
    chksumlen += 2;

    // Copy urgent pointer to buf (16 bits)
    memcpy(ptr, &tcphdr.th_urp, sizeof(tcphdr.th_urp));
    ptr += sizeof(tcphdr.th_urp);
    chksumlen += sizeof(tcphdr.th_urp);

    return csum((uint16_t *) buf, chksumlen);
}

/**
 * @brief Send TCP packet using IPv6 with SYN flag set
 * 
 * @param[in] socket_fd Socket descriptor
 * @param[in] domain Domain (IP address) to send packet to
 * @param[in] interface Name of interface to send packet from
 * @param[in] port Port number to send packet to 
 * 
 * author: P.D. Buchan
 * source: https://www.pdbuchan.com/rawsock/tcp6_ll.c (modified)
 */
void ipv6_syn(int socket_fd, char *domain, char *interface, int port)
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
    iphdr.ip6_plen = htons(TCP_HDRLEN);
    iphdr.ip6_nxt = IPPROTO_TCP;
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

    /* TCP header */
    struct tcphdr tcphdr;
	memset(&tcphdr, 0, sizeof(struct tcphdr));
    tcphdr.th_sport = htons(SENDER_PORT);
    tcphdr.th_dport = htons(port);
    // Data offset (4 bits): size of TCP header in 32-bit words
    tcphdr.th_off = TCP_HDRLEN / 4;
    // SYN Flag (1 bit on 1st position)
	tcphdr.th_flags += (1 << 1);
    tcphdr.th_win = htons(65535);
    tcphdr.th_sum = tcp6_checksum(iphdr, tcphdr);

    /* Fill out ethernet frame header */
    // ethernet header (2*MAC(6) + ethernet type(2)) + ethernet data (IP header + TCP header)
    int frame_length = 14 + IP6_HDRLEN + TCP_HDRLEN;
    // Destination and Source MAC addresses
    memcpy(ether_frame, dst_mac, 6 * sizeof(uint8_t));	// dst MAC
    memcpy(ether_frame + 6, src_mac, 6 * sizeof(uint8_t)); // src MAC
    ether_frame[12] = ETH_P_IPV6 / 256; // ethernet type code
    ether_frame[13] = ETH_P_IPV6 % 256;
    memcpy(ether_frame + ETH_HDRLEN, &iphdr, IP6_HDRLEN * sizeof(uint8_t)); // IPv6 header
    memcpy(ether_frame + ETH_HDRLEN + IP6_HDRLEN, &tcphdr, TCP_HDRLEN * sizeof(uint8_t)); // TCP header

    /* Send ethernet frame to socket */
    if ((sendto (socket_fd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof(device))) <= 0) {
      perror("sendto() failed: ");
      exit(ERR);
    }

    free(ether_frame);
}

p_status tcp_ipv6_scan(struct arguments uargs, int port)
{
	int socket_fd;

	/* Get raw socket */
    if ((socket_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
      perror("socket() failed: ");
      exit(EXIT_FAILURE);
    }

	/* Set pcap filter string */
    struct bpf_program fp;
    bpf_u_int32 netp, maskp;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_lookupnet(uargs.interface, &netp, &maskp, errbuf);
    char *filter_string = set_filter_string(uargs, port, "tcp");

    if (pcap_compile(handle, &fp, filter_string, 0, netp) == -1) {
        perror("Error calling pcap_compile");
        exit(1);
    }

    if (pcap_setfilter(handle, &fp) == -1) {
        perror("Error setting filter");
        exit(1);
    }
    /* Send SYN packet */
    ipv6_syn(socket_fd, uargs.domain, uargs.interface, port);

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
        // no packet captured, resend packet with SYN flag
        ipv6_syn(socket_fd, uargs.domain, uargs.interface, port);

        if (setitimer(ITIMER_REAL, &time, NULL) == -1) {
            perror("Unable to send SIGALRM");
            exit(1);
        }

        packet = pcap_next(handle, &hdr);
        if (packet == NULL) {
            pcap_freecode(&fp);
            close(socket_fd);
            return FILTERED;
        }
    }

    /* Get to TCP header from incoming packet and check flags */
    struct tcphdr* tcp_header = (struct tcphdr*) (packet + ETH_HDRLEN + IP6_HDRLEN);
    if(tcp_header->rst == 1 && tcp_header->ack == 1){
        pcap_freecode(&fp);
        close(socket_fd);
        return CLOSED;
    }
    else if(tcp_header->rst == 0 && tcp_header->ack == 1){
        pcap_freecode(&fp);
        close(socket_fd);
        return OPENED;
    }

    pcap_freecode(&fp);
    close(socket_fd);

    return NONE;
}