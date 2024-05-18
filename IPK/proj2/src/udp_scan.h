/**
 * @brief 	Header file for UDP scanning functions
 * @author  xzvara01(@vutbr.cz)
 * @file    udp_scan.h
 * @date    22.04.2022
 */

#ifndef _UDP_SCAN_H
#define _UDP_SCAN_H 1

#include "common.h"

/**
 * @brief Scan single UDP port using IPv4
 * 
 * @param[in] uargs Program options
 * @param[in] port 	Port number to scan
 *
 * @return Value from enum port_status which represents state of scanned port
 */
p_status udp_ipv4_scan(struct arguments uargs, int port);

/**
 * @brief Scan single UDP port using IPv6
 * 
 * @param[in] uargs Program options
 * @param[in] port 	Port number to scan
 *
 * @return Value from enum port_status which represents state of scanned port
 */
p_status udp_ipv6_scan(struct arguments uargs, int port);

#endif