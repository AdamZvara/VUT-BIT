/**
 * @file capture.cpp
 * @author xzvara01 (xzvara01@stud.fit.vutbr.cz)
 * @brief Capturing incoming traffic with libpcap
 * @date 2022-10-16
 *
 */

#include <iostream>
#include <string>

#include "capture.hpp"

#define IP_MINLEN  20
#define TCP_MINLEN 20
#define UDP_LEN     8
#define ICMP_LEN    8

uint64_t SysUpTime;     // save system uptime from first captured packet (in miliseconds)
timeval LastChecked;    // store last time we checked flow_cache for records to export (in miliseconds)

Capture::~Capture()
{
    if (handle != NULL) {
        pcap_freecode(&fp);
        pcap_close(handle);
    }
}

void Capture::open(FILE* file)
{
    // open pcap file (or read from stdin)
    handle = pcap_fopen_offline(file, errbuf);
    if (handle == NULL) {
        throw std::runtime_error(errbuf);
    }
}

void Capture::apply_filter(const char* filter_expr)
{
	if (pcap_compile(handle, &fp, filter_expr, 0, 0) == -1) {
        // print out nice error message
        std::string msg("Couldn't parse filter \"");
        msg.append(filter_expr);
        msg.append("\"");
        throw std::invalid_argument(msg);
	}

	if (pcap_setfilter(handle, &fp) == -1) {
        // print out nice error message
        std::string msg("Couldn't install filter \"");
        msg.append(filter_expr);
        msg.append("\"");
        throw std::runtime_error(msg);
	}
}

int Capture::next_packet()
{
    int err;

    // reading packet
    if ((err = pcap_next_ex(handle, &header, &raw_pkt)) < 0) {
        return err;
    }

    // check if we are capturing ethernet
    if (pcap_datalink(handle) != DLT_EN10MB) {
        return ERR_NONETH;
    }

    /* parsing ethernet header */
    // check if eth header is long enough so that we dont access invalid memory
    if (header->len < ETH_HLEN) {
        return ERR_INCOMPLETE;
    }

    /* parsing ip header */
    // check if ip header is long enough so that we dont access invalid memory
    ip_header = (iphdr *)(raw_pkt + ETH_HLEN);
    if (header->len - ETH_HLEN < IP_MINLEN) {
        return ERR_INCOMPLETE;
    }

    // remaining of the header is transport layer protocol - check for each protocol if they are complete
    int ip_hlen = (ip_header->ihl & 0xf) * 4;
    if (ip_header->protocol == IPPROTO_TCP) {
        if (header->len - ETH_HLEN - ip_hlen < TCP_MINLEN) {
            return ERR_INCOMPLETE;
        }
    } else if (ip_header->protocol == IPPROTO_UDP) {
        if (header->len - ETH_HLEN - ip_hlen < UDP_LEN) {
            return ERR_INCOMPLETE;
        }
    } else { // ICMP header
        if (header->len - ETH_HLEN - ip_hlen < ICMP_LEN) {
            return ERR_INCOMPLETE;
        }
    }

    transport_header = (void *)(raw_pkt + ETH_HLEN + ip_hlen);

    /** Update LastChecked value with last packet */
    LastChecked.tv_sec = header->ts.tv_sec;
    LastChecked.tv_usec = header->ts.tv_usec;

    return 0;
}

int Capture::get_packet_timestamp() const
{
    return (header->ts.tv_sec * 1000) + (header->ts.tv_usec / 1000)  - SysUpTime;
}

void Capture::get_ports(uint16_t& Sportn, uint16_t& Dportn) const
{
    uint16_t proto = ip_header->protocol;
    if (proto == IPPROTO_TCP) {
        tcphdr *hdr = (tcphdr *)transport_header;
        Sportn = hdr->th_sport;
        Dportn = hdr->th_dport;
    } else if (proto == IPPROTO_UDP) {
        udphdr *hdr = (udphdr *)transport_header;
        Sportn = hdr->uh_sport;
        Dportn = hdr->uh_dport;
    } else {
        icmphdr *hdr = (icmphdr *)transport_header;
        Sportn = 0;
        Dportn = (hdr->type << 8) + hdr->code;
    }
}

bool Capture::tcp_rstfin() const
{
    tcphdr *hdr;
    if (ip_header->protocol == IPPROTO_TCP) {
        hdr = (tcphdr *)transport_header;
        if (hdr->th_flags & TH_FIN || hdr->th_flags == TH_RST) {
            return true;
        }
    }
    return false;
}
