/**
 * @file capture.hpp
 * @author xzvara01 (xzvara01@stud.fit.vutbr.cz)
 * @brief Header file for capturing incoming traffic
 * @date 2022-10-16
 *
 */

#ifndef _CAPTURE_HPP
#define _CAPTURE_HPP

#include <pcap.h>
#define __FAVOR_BSD
#include <netinet/tcp.h>     // struct tcphdr
#include <netinet/udp.h>     // struct udphdr
#include <netinet/ip_icmp.h> // struct ip_icmp
#include <netinet/ip.h>         // struct iphdr
#include <netinet/if_ether.h>   // struct ether_header

#define ERR_INCOMPLETE -10
#define ERR_NONETH     -11

class Capture
{
private:
    pcap_t *handle;                     // handler to opened pcap file
    struct bpf_program fp;              // filter program
    char errbuf[PCAP_ERRBUF_SIZE];      // error buffer

public:
    const unsigned char *raw_pkt;       // captured raw packet data
    pcap_pkthdr *header;                // captured packet header
    ether_header *eth_header;           // ethernet header
    iphdr *ip_header;                   // ip header
    void *transport_header;             // trasnport header (later casted to the correct type)

    /** @brief Destroy the Capture object and free allocated memory */
    ~Capture();

    /**
     * @brief Initialize pcap handler to open given file
     *
     * @param[in] filename Name of the file
     *
     * @throw runtime_error Pcap handler could not be initialized
     */
    void open(FILE* file);

    /**
     * @brief Apply filter_expr to pcap handler
     *
     * @param[in] filter_expr Expression to be applied
     */
    void apply_filter(const char* filter_expr);

    /**
     * @brief Get next packet from savefile and parse it to public structures (header, iphdr, eth_header ...)
     *
     * @return ERR_INCOMPLETE Captured packet (one of the headers) was incomplete
     * @return ERR_NONETH Captured packet does not have ETHERNET link layer header
     * @return otherwise returns value from pcap function pcap_next()
     *
     */
    int next_packet();

    /**
     * @brief Calculate time, when packet has been capture (relative to SysUpTime)
     *
     * @return Calculated time
     */
    int get_packet_timestamp() const;

    /**
     * @brief Retrieve port numbers from captured packet
     *
     * @param[out] Sportn source port number
     * @param[out] Dportn destination port number
     */
    void get_ports(uint16_t& Sportn, uint16_t& Dportn) const;

    /**
     * @brief Return whether current packet is TCP with FIN or RST flag set
     */
    bool tcp_rstfin() const;
};

#endif