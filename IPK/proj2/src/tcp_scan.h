/**
 * @brief 	Header file for TCP scanning functions
 * @author  xzvara01(@vutbr.cz)
 * @file    tcp_scan.h
 * @date    22.04.2022
 */

#ifndef _TCP_SCAN_H
#define _TCP_SCAN_H 1

#include "common.h"
#include "parse_args.h"

/**
 * @struct pseudo_header
 * 
 * @brief Pseudo TCP header used for checksum
 * 
 * source: https://www.binarytides.com/raw-sockets-c-code-linux/
 * author: Silver Moon
 */
struct pseudo_header
{
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t  placeholder;
	u_int8_t  protocol;
	u_int16_t tcp_length;
};

/**
 * @brief Scan single TCP port using IPv4
 * 
 * @param[in] uargs Program options
 * @param[in] port 	Port number to scan
 *
 * @return Value from enum port_status which represents state of scanned port
 */
p_status tcp_ipv4_scan(struct arguments uargs, int port);

/**
 * @brief Scan single TCP port using IPv6
 * 
 * @param[in] uargs Program options
 * @param[in] port 	Port number to scan
 *
 * @return Value from enum port_status which represents state of scanned port
 */
p_status tcp_ipv6_scan(struct arguments uargs, int port);

#endif