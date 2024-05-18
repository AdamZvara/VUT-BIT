/**
 * @file parse.cpp
 * @author xzvara01 (xzvara01@stud.fit.vutbr.cz)
 * @brief Main file for parsing command line arguments
 * @date 2022-10-06
 *
 */

#include <iostream>
#include <arpa/inet.h>          // inet_pton
#include <unistd.h>             // uintX types
#include <netinet/in.h>         // sockaddr_in, in_addr

#include "parse.hpp"
#include "debug.hpp"

#define OPTIONS "hf:c:a:i:m:"   // getopt option string

void print_help()
{
    std::cerr << "Usage: ./flow [OPTIONS ...]\n"
            << "Generate netflow records from pcap file which are sent to collector over UDP\n\n"
            << "OPTIONS\n"
                << "  -f=FILENAME\t\tInput pcap file (default STDIN)\n"
                << "  -c=IP/HOSTNAME[:PORT]\tIP address or HOSTNAME of collector with optional UDP port (default 127.0.0.1:2055)\n"
                << "  -a=SECONDS\t\tInterval in seconds, after which active records are exported to collector (default 60 seconds)\n"
                << "  -i=SECONDS\t\tInterval in seconds, after which inactive records are exported to collector (default 10 seconds)\n"
                << "  -m=COUNT\t\tSize of flow cache (default 1024)\n";
}

void debug_print_options(const arguments& args)
{
    (void) args; // compiler does not produce warning when not debugging
    dpprintf("[parse.cpp] active\t%d\n", args.active);
    dpprintf("[parse.cpp] inactive\t%d\n", args.inactive);
    dpprintf("[parse.cpp] port\t%d\n", args.port);
    dpprintf("[parse.cpp] cache size\t%d\n", args.cache_size);
    dpprintf("[parse.cpp] collector\t%s\n", args.collector.c_str());
}

int arg_to_number(const char *str_number)
{
    int converted_num;
    char* end; // check if numerical values are converted correctly

    converted_num = strtol(str_number, &end, 10);
    if (strlen(end) != 0) {
        throw std::invalid_argument("Argument followed by an invalid number");
    } else if (converted_num < 0) {
        throw std::invalid_argument("Argument must be followed by a positive number");
    }
    return converted_num;
}

void parse_hostname(const char *original, std::string& parsed_hostname, uint16_t& parsed_port)
{
    size_t pos = 0;
    std::string hostname = original;
    std::string delim = ":";
    std::string str_port;

    // if unchanged original hostname is valid IPv6 address, end function
    if (is_valid_ipv6(original)) {
        parsed_hostname = hostname;
        return;
    }

    // try to find port number by searching for last colon in original hostname
    if ((pos = hostname.find_last_of(delim)) != std::string::npos) {
        str_port = hostname.substr(pos+1); // port number starts at pos+1 because of the colon
        hostname.erase(pos);
        parsed_port = arg_to_number(str_port.c_str());
    }

    // IPv6 notation with port number is [IPv6]:port, check if it matches
    if (hostname.rfind('[', 0) == 0) {
        std::string substr = hostname.substr(1, hostname.length() - 2);
        if (is_valid_ipv6(substr.c_str())) {
            parsed_hostname = substr;
            return;
        }
    }

    parsed_hostname = hostname;
}

void parse_arguments(int argc, char **argv, arguments& args)
{
    int c;

    while ((c = getopt(argc, argv, OPTIONS)) != -1) {
        switch (c)
        {
        case 'h':
            print_help();
            exit(1);
            break;

        case 'a':
            args.active = arg_to_number(optarg);
            break;

        case 'i':
            args.inactive = arg_to_number(optarg);
            break;

        case 'm':
            args.cache_size = arg_to_number(optarg);
            break;

        case 'f':
            if ((args.pcapfile = fopen(optarg, "r")) == NULL) {
                throw std::runtime_error("could not open given file");
            }
            break;

        case 'c':
            parse_hostname(optarg, args.collector, args.port);
            convert_hostname(args.collector, &(args.address));
            break;

        default:
            break;
        }
    }
    debug_print_options(args);
}

void convert_hostname(std::string hostname, sockaddr_storage* out_address)
{
    int sockfd;
    addrinfo hints = {}, *address_list;
    bool found = false;

    hints.ai_family = AF_UNSPEC;    // get IPv4 and IPv6 addresses
    hints.ai_socktype = SOCK_DGRAM; // prefered UDP socket type
    hints.ai_protocol = IPPROTO_UDP; // prefered UDP protocol

    // getaddrinfo returns linked list with available IP addresses
    if (getaddrinfo(hostname.c_str(), NULL, &hints, &address_list) != 0) {
        throw std::runtime_error("getaddrinfo failed: Could not resolve hostname");
    }

    // iterate through linked list of addresses and try to connect to one
    for (addrinfo *address = address_list; address != NULL; address = address->ai_next) {
        // create new socket to connect to address
        if ((sockfd = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) == -1) {
            // socket could not be created, continue to another address
            continue;
        }

        // try to connect to address
        if (connect(sockfd, address->ai_addr, address->ai_addrlen) == 0) {
            #if DEBUG_PARSE == 1 // debugging print
                char host[NI_MAXHOST] = {};
                getnameinfo(address->ai_addr, address->ai_addrlen, host, sizeof(host), NULL, 0, NI_NUMERICHOST);
                printf("[common.cpp] real IP\t%s\n", host);
            #endif

            // valid address was found
            memcpy(out_address, address->ai_addr, address->ai_addrlen);
            found = true;
            close(sockfd);
            break;
        }
        close(sockfd);
    }

    freeaddrinfo(address_list);

    // no valid address was found
    if (!found) {
        throw std::runtime_error("no collector IP address could be resolved");
    }

}

bool is_valid_ipv6(const char* address)
{
    sockaddr_in6 result = {};
    return inet_pton(AF_INET6, address, &result) == 1 ? true : false;
}