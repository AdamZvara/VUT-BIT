/**
 * @file flow.cpp
 * @author xzvara01 (xzvara01@stud.fit.vutbr.cz)
 * @brief Main program for exporting captured data to collector
 * @date 2022-10-16
 *
 */

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>         // close

#include "flow.hpp"
#include "src/capture.hpp"
#include "src/debug.hpp"
#include "src/store.hpp"

#define ERR 1;

int main(int argc, char **argv)
{
    arguments args {};  // argument structure
    Capture cap {};     // pcap structure for capturing packets
    Store storage;      // storage of created netflow flows

    const char *filter_expr = "ip proto 1 or ip proto 6 or ip proto 17"; // pcap filter expression

    try {
        parse_arguments(argc, argv, args);
        cap.open(args.pcapfile);
        cap.apply_filter(filter_expr);
    } catch(const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << '\n';
        if (args.pcapfile != NULL) {
            fclose(args.pcapfile);
        }
        return ERR;
    }

    int ret;

    // capture packet
    while ((ret = cap.next_packet()) != PCAP_ERROR_BREAK) {
        // first check for my own defined errors (incomplete header or not ethernet header)
        if (ret < 0) {
            if (ret == ERR_INCOMPLETE || ret == ERR_NONETH) {
                // skip these packets and continue
                std::cerr << (ret == ERR_NONETH ?
                    "Non Ethernet packed received ... skipping\n" :
                    "Incomplete or malformed packet received\n");
                continue;
            } else {
                std::cerr << "Reading packet failed\n";
                break;
            }
        }

        // catching memory allocation exceptions
        try {
            storage.fexport(args);      // export flows from flow cache to collector
            storage.insert(cap, args);  // insert flow to flow cache
        } catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            return ERR;
        }

        if (cap.tcp_rstfin()) {
            storage.export_single(cap, args);
        }
    }

    dfcprintf(); // print out flow cache for debugging purposes

    try {
        storage.clear(args);
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return ERR;
    }

    return 0;
}