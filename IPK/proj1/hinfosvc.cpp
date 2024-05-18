#include <iostream>
#include <string>           // string
#include <cstring>          // strncmp
#include <fstream>          // ifstream
#include <sstream>          // istringstream
#include <sys/socket.h>     // socket, bind, accept, listen
#include <netinet/in.h>     // struct sockaddr_in
#include <unistd.h>         // read

using std::string;

#define ERR -1
#define SLEEP_TIME 1    // sleep time for cpu_load function

/**
 * @brief Convert port number from string format to integer
 * @param[in] str_port Port number in string format
 * @return Converted port number as integer
 * @exception invalid_argument when port number is float or not in range of uint16_t
 */
int get_port(char *str_port)
{
    int port;
    std::istringstream ss(str_port);
    if (!(ss >> port) || !ss.eof() || port > 65535 || port < 0) {
        throw std::invalid_argument("Incorrect port number - must be integer in range 0-65535");
    }

    return port;
}

/**
 * @brief Get hostname from /etc/hostname
 * @param[out] str String to store name into
 * @return 0 if successful, otherwise ERR
 */
int get_hostname(string &str)
{
    std::ifstream hostname;
    hostname.open("/etc/hostname", std::ifstream::in);
    if (!hostname)
        return ERR;

    getline(hostname, str);
    hostname.close();
    return 0;
}

/**
 * @brief Get CPU name from /proc/cpuinfo
 * @param[out] str String to store CPU name into
 * @return 0 if successful, otherwise ERR
 */
int get_cpuname(string &str)
{
    std::ifstream proc;
    proc.open("/proc/cpuinfo", std::ifstream::in);
    if (!proc)
        return ERR;

    string tmp;
    for (int i = 0; i < 5; i++) {
        getline(proc, str);
    }

    // remove "model name \t: " from string
    str.erase(0, str.find_first_of(':'));
    str.erase(0, 2);

    proc.close();
    return 0;
}

/**
 * @brief Calculate CPU load percentage from 2 arrays
 * @param[in] old_v First array of measured CPU values
 * @param[in] new_v Second array of measured CPU values
 * @return CPU load percentage truncated to int
 */
int cpu_percentage(unsigned long old_v[], unsigned long new_v[])
{
    //code taken from: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
    //user: https://stackoverflow.com/users/1275161/vangelis-tasoulas
    //date: 11.02.2022
    unsigned long PrevIdle = old_v[3] + old_v[4];
    unsigned long Idle = new_v[3] + new_v[4];

    unsigned long PrevNonIdle = old_v[0] + old_v[1] + old_v[2] + old_v[5] + old_v[6] + old_v[7];
    unsigned long NonIdle = new_v[0] + new_v[1] + new_v[2] + new_v[5] + new_v[6] + new_v[7];

    unsigned long PrevTotal = PrevIdle + PrevNonIdle;
    unsigned long Total = Idle + NonIdle;

    unsigned long totald = Total - PrevTotal;
    unsigned long idled = Idle - PrevIdle;

    return (float)(totald - idled)/totald*100;
}

/**
 * @brief Get numeric values from /proc/stat
 * @param[out] arr Array to store extracted values into
 * @return 0 if successful, otherwise ERR
 */
int cpu_info(unsigned long arr[])
{
    std::ifstream proc;
    proc.open("/proc/stat", std::ifstream::in);

    if (!proc) {
        return ERR;
    }

    string values;
    getline(proc, values);
    // remove "cpu" from line
    values.erase(0, values.find(' '));
    std::istringstream stream(values);

    // convert numbers from string into array
    unsigned long n;
    int counter = 0;
    while(stream >> n) {
        arr[counter++] = n;
    }

    proc.close();
    return 0;
}

/**
 * @brief Provide CPU usage percentage
 * @param[out] str String to store evaluated CPU usage
 * @param[in] sleep_time Time between CPU measurings
 * @return 0 if successful, otherwise return ERR
 */
int get_cpuload(string &str, int sleep_time)
{
    // get 2 sets of CPU time values, separated by sleep time
    unsigned long old_values[10], new_values[10];
    if (cpu_info(old_values)) {
        return ERR;
    }
    sleep(sleep_time);
    if (cpu_info(new_values)) {
        return ERR;
    }

    str = std::to_string(cpu_percentage(old_values, new_values));
    str.push_back('%');
    return 0;
}

/**
 * @brief Send answer to user request on given socket
 * @param[in] socket_fd socket descriptor
 * @param[in] s_addr socket address
 * @param[in] addr_size address size
 * @return 0 if successful, otherwise ERR
 */
int accept_request(int socket_fd, sockaddr_in s_addr, socklen_t addr_size)
{
    int receive_fd;
    if ((receive_fd = accept(socket_fd, (sockaddr *) &s_addr, &addr_size)) < 0)
        return ERR;

    string msg = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
    const string err_msg = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain;\
                            \r\n\r\nBad Request\n";

    const string hostname = "GET /hostname ";
    const string cpu_name = "GET /cpu-name ";
    const string load = "GET /load ";

    char request[2048] = {0};
    if (read(receive_fd, request, 2048) < 0) {
        close(receive_fd);
        return ERR;
    }

    // temporary string to store hostname, cpuname or cpuload and then append it to msg
    string http_content;

    // separate request types
    if (!strncmp(request, hostname.c_str(), hostname.length())) {
        if (get_hostname(http_content))
            return ERR;
    } else if (!strncmp(request, cpu_name.c_str(), cpu_name.length())) {
        if (get_cpuname(http_content))
            return ERR;
    } else if (!strncmp(request, load.c_str(), load.length())) {
        if (get_cpuload(http_content, SLEEP_TIME))
            return ERR;
    } else {
        if (send(receive_fd, err_msg.data(), err_msg.length(), 0) < 0)
            return ERR;
        close(receive_fd);
        return 0;
    }

    msg.append(http_content);
    msg.append("\n");

    if (send(receive_fd, msg.data(), msg.length(), 0) < 0)
        return ERR;

    close(receive_fd);
    return 0;
}


int main(int argc, char *argv[])
{
    // get port number from argv
    uint16_t port;
    try {
        if (argc != 2) {
            throw std::invalid_argument("USAGE: ./hinfosvc port_number");
        } else {
            port = get_port(argv[1]);
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return ERR;
    }


    // create socket, bind it to address and listen to it
    int socket_fd;
    sockaddr_in s_addr;
    socklen_t s_addr_size = sizeof(sockaddr_in);

    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = INADDR_ANY;
    s_addr.sin_port = htons(port);

    try {
        if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            throw std::runtime_error("could not create socket");

        int option = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
            throw std::runtime_error("setsockopt failed");

        if (bind(socket_fd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in)) < 0)
            throw std::runtime_error("failed to bind socket");

        if (listen(socket_fd, 1) < 0)
            throw std::runtime_error("server could not listen to requests");

    } catch(const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return ERR;
    }

    // comunicate with user and process requests until user ends program
    while (1) {
        if (accept_request(socket_fd, s_addr, s_addr_size)) {
            std::cerr << "Internal error occured" << std::endl;
            return ERR;
        }
    }

    return 0;
}
