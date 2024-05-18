RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

path='.././ipk-l4-scan'
ports=(1111 1234 1329 1395 1500 17897 18345 26324 40256 65535)
udpProcesses=()
tcpProcesses=()

for port in ${ports[@]}; do
    # TCP ipv4
    nc -l $port &
    tcpProcesses+=("$!")
    # TCP ipv6
    nc -6 -l $port &
    tcpProcesses+=("$!")
    # UDP ipv4
    nc -k -u -l $port &
    udpProcesses+=("$!")
    # UDP ipv6
    nc -k -6 -u -l $port &
    udpProcesses+=("$!")
done

cleanup()
{
    allProcesses=( "${tcpProcesses[@]}" "${udpProcesses[@]}" )
    rm -rf .tmp
    for process in ${allProcesses[@]}; do
        sudo kill $process 2> /dev/null
    done
}

# arg1 = actual value
# arg2 = expected value
# arg3 = name of the test
compare_result() {
    if [ $1 -eq $2 ]; then
        echo -e "\t$3: ${GREEN}passed${NC}"
    else
        echo -e "\t$3: ${RED}failed${NC}"
    fi
}

trap 'cleanup;exit 0' 2 # in case of SIGINT cleanup

# TCP TESTS - localhost IPv4
echo "Testing TCP ports (IPv4)"
actual=$(sudo ${path} -i lo -t 1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 localhost -w 200 | grep opened | wc -l)
expected=$(sudo nmap -p1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 localhost | grep open | wc -l)
compare_result $actual $expected "All opened"

actual=$(sudo ${path} -i lo -t 10,60,100,1368,1600,17837,16345,26624,40156,65534 localhost -w 200 | grep closed | wc -l)
expected=$(sudo nmap -p10,60,100,1368,1600,17837,16345,26624,40156,65534 localhost | grep closed | wc -l)
compare_result $actual $expected "All closed"

# TCP TESTS - localhost IPv6
echo "Testing TCP ports (IPv6)"
actual=$(sudo ${path} -i lo -t 1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 ::1 -w 200 | grep opened | wc -l)
expected=$(sudo nmap -6 -p1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 ::1 | grep open | wc -l)
compare_result $actual $expected "All opened"

actual=$(sudo ${path} -i lo -t 10,60,100,1368,1600,17837,16345,26624,40156,65534 ::1 -w 200 | grep closed | wc -l)
expected=$(sudo nmap -6 -p10,60,100,1368,1600,17837,16345,26624,40156,65534 ::1 | grep closed | wc -l)
compare_result $actual $expected "All closed"

# UDP Tests - localhost IPv4
echo "Testing UDP ports (IPv4)"
actual=$(sudo ${path} -i lo -u 1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 localhost -w 200 | grep opened | wc -l)
expected=$(sudo nmap -sU -p1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 localhost | grep open | wc -l)
compare_result $actual $expected "All opened"

actual=$(sudo ${path} -i lo -u 10,60,100,1368,1600,17837,16345,26624,40156,65534 localhost -w 200 | grep closed | wc -l)
expected=$(sudo nmap -sU -p10,60,100,1368,1600,17837,16345,26624,40156,65534 localhost | grep closed | wc -l)
compare_result $actual $expected "All closed"

# UDP TESTS - localhost IPv6
echo "Testing UDP ports (IPv6)"
actual=$(sudo ${path} -i lo -u 1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 ::1 -w 200 | grep opened | wc -l)
expected=$(sudo nmap -sU -6 -p1111,1234,1329,1395,1500,17897,18345,26324,40256,65535 ::1 | grep open | wc -l)
compare_result $actual $expected "All opened"

actual=$(sudo ${path} -i lo -u 10,60,100,1368,1600,17837,16345,26624,40156,65534 ::1 -w 200 | grep closed | wc -l)
expected=$(sudo nmap -sU -6 -p10,60,100,1368,1600,17837,16345,26624,40156,65534 ::1 | grep closed | wc -l)
compare_result $actual $expected "All closed"

# long running tests
echo -e "Range syntax tests (might take a bit longer):"
# TCPv4
sudo ${path} -i lo --pt 1111-1395 localhost -w 150 > .tmp
opened_cnt=$(cat .tmp | grep opened | wc -l)
closed_cnt=$(cat .tmp | grep closed | wc -l)
expected=$(sudo nmap -p1111-1395 localhost | grep open | wc -l)
if [ $closed_cnt -eq 281 ]; then
    compare_result $opened_cnt $expected "Mixed TCPv4 ports"
else
    echo -e "\tMixed TCPv4 ports: ${RED}failed${NC}"
fi
# UDPv4
sudo ${path} -i lo --pu 1111-1395 localhost -w 150 > .tmp
opened_cnt=$(cat .tmp | grep opened | wc -l)
closed_cnt=$(cat .tmp | grep closed | wc -l)
expected=$(sudo nmap -sU -p1111-1395 localhost | grep open | wc -l)
if [ $closed_cnt -eq 281 ]; then
    compare_result $opened_cnt $expected "Mixed UDPv4 ports"
else
    echo -e "\tMixed UDPv4 ports: ${RED}failed${NC}"
fi
# TCPv6
sudo ${path} -i lo --pt 1111-1395 ::1 -w 150 > .tmp
opened_cnt=$(cat .tmp | grep opened | wc -l)
closed_cnt=$(cat .tmp | grep closed | wc -l)
expected=$(sudo nmap -6 -p1111-1395 ::1 | grep open | wc -l)
if [ $closed_cnt -eq 281 ]; then
    compare_result $opened_cnt $expected "Mixed TCPv6 ports"
else
    echo -e "\tMixed TCPv6 ports: ${RED}failed${NC}"
fi
# UDPv6
sudo ${path} -i lo --pu 1111-1395 ::1 -w 150 > .tmp
opened_cnt=$(cat .tmp | grep opened | wc -l)
closed_cnt=$(cat .tmp | grep closed | wc -l)
expected=$(sudo nmap -sU -6 -p1111-1395 ::1 | grep open | wc -l)
if [ $closed_cnt -eq 281 ]; then
    compare_result $opened_cnt $expected "Mixed UDPv4 ports"
else
    echo -e "\tMixed UDPv4 ports: ${RED}failed${NC}"
fi

cleanup