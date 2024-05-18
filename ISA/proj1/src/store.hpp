/**
 * @file store.hpp
 * @author xzvara01 (xzvara01@stud.fit.vutbr.cz)
 * @brief Header file for storing flows created by the program
 * @date 2022-10-16
 *
 */

#ifndef _STORE_HPP
#define _STORE_HPP

#include <vector>
#include <map>
#include <tuple>
#include <cstdint>          // uintX types

#include "../flow.hpp"      // netflow structures
#include "capture.hpp"      // capture structure

#define ETH_HLEN 14
#define EXPORT_TIMER 1000

/**                             Flow cache key                                      */
//                  SRCIP      DSTIP   SRC_PORT  DST_PORT   PROTO     TOS
typedef std::tuple<uint32_t, uint32_t, uint16_t, uint16_t, uint8_t, uint8_t> netkey_t;

/** Flow_cache          */
typedef std::map<netkey_t, netflowV5R> flowc_t;

extern uint64_t SysUpTime;
extern timeval LastChecked;

class Store
{
private:
    /** Storage of created flows */
    flowc_t flow_cache;

    /**
     * @brief Create new flow to be inserted to flow cache
     *
     * @param[in]  cap    Captured packet
     * @param[out] flow   Allocated flow
     * @param[in]  Sportn Source port number
     * @param[in]  Dportn Destination port number
     */
    void create_flow(const Capture &cap, netflowV5R& flow, const uint16_t Sportn, const uint16_t Dportn);

    /**
     * @brief Update flow which is already stored in flow cache
     *
     * @param[in]  cap  Captured packet
     * @param[out] flow Flow to be updated
     */
    void update_flow(const Capture &cap, netflowV5R& flow);

    /**
     * @brief Find flow in flow cache and return in it argument called flow
     * @note If no such element exists in map, flow will be nullpointer
     *
     * @param[in] cap   Captured packet
     * @param[in] flow  Flow to be found
     *
     * @return 0 if flow was found, otherwise -1
     */
    int find_flow(const Capture& cap, netflowV5R *flow);

    /**
     * @brief Export oldest flow
     *
     * @param[in] args Argument structure
     */
    void export_oldest(arguments args);

public:
    /**
     * @brief Insert (or update) captured packet into flow cache
     *
     * @param[in]  cap        Captured packet
     * @param[in]  flowsize   Maximum amount of flow cache items
     */
    int insert(const Capture& cap, arguments args);

    /**
     * @brief Export all flows from flow cache based on active and inactive timers
     *
     * @param[in] args Argument structure containing valid active and inactive timer
     */
    void fexport(arguments args);

    /**
     * @brief Export single flow record from cache
     *
     * @param[in] cap   Captured packet
     * @param[in] args  Program arguments
     */
    void export_single(const Capture& cap, arguments args);

    /**
     * @brief Clear flow_cache and export all flows stored in it
     *
     * @param[in] args Program arguments
     */
    void clear(arguments args);
};

/**
 * @brief Create netflow header for given flows, store them all in char * sequence ready for UDP export
 *
 * @param[in]  flows     Flows to be exported
 * @param[out] buff_size Size of output buffer
 *
 * @return char* Converted flows with added header ready to be exported
 */
char *prepare_export(std::vector<netflowV5R> flows, size_t& buff_size);

/**
 * @brief Fill netflow v5 header with appropriate values
 *
 * @param[in] header Netflow v5 header
 * @param[in] count  Amount of netflow records to be exported with this header
 */
void fill_header(netflowV5H& header, const int count);

/**
 * @brief Send data over UDP
 *
 * @param[in] flows     Ports to be exported
 * @param[in] address   Destination IP address
 * @param[in] port      Destination port number
 */
void send_udp(std::vector<netflowV5R> flows, sockaddr_storage& address, const uint16_t port);

#endif