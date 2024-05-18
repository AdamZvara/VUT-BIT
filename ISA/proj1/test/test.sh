#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color
BOLD=$(tput bold)

declare -i PASSEDCNT=0
declare -i FAILEDCNT=0
declare -i ALLCNT=0

run_test() {
    rm -rf out/nfcapd*

    for FILE in $1; do
        ALLCNT+=1
        echo -e -n "${FILE}: "
        nfcapd -n "$FILE,127.0.0.1,out" 2> /dev/null &
        PID=$!
        sleep 0.3

        .././flow -f "src/$FILE" $2

        kill $PID

        nfdump -r `ls out/nfcapd*` -q -o 'fmt:%sa:%sp %da:%dp %pr %pkt %byt %tos' | sort > "out/${FILE}"$3
        if [ $? -ne 0 ]; then
            # sometimes tests fail here, need to rerun them
            echo -e "${RED}${BOLD}TESTS FAILED, RERUN"
            exit 1
        fi

        diff -w  --changed-group-format='%<' --unchanged-group-format='' out/${FILE}$3 out/${FILE%.*}$4
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}Passed${NC}"
            PASSEDCNT+=1
        else
            echo -e "${RED}Failed${NC}"
            FAILEDCNT+=1
        fi

    rm -rf out/nfcapd*
    done
}

echo -e "${BLUE}Running options tests${NC}"
while read -r CMD; do
    ALLCNT+=1
    read -r RESULT
    ERROR=$($CMD 2>&1 >/dev/null)
    echo -n "$CMD "
    if [ "$ERROR" = "$RESULT" ]; then
        echo -e "${GREEN}Passed${NC}"
        PASSEDCNT+=1
    else
        echo -e "${RED}Failed${NC}"
        FAILEDCNT+=1
    fi
done < cases

echo -e "\n${BLUE}Running basic tests${NC}"
run_test "`ls src`" "-c 127.0.0.1:9995" '_out' ''

echo -e "\n${BLUE}Running flow cache capacity tests${NC}"
run_test "icmp.pcap udp.pcap" "-c 127.0.0.1:9995 -m 1" '_fcache' '_fcache'

echo -e "\n${BLUE}Running inactive timer tests${NC}"
run_test "icmp.pcap udp.pcap" "-c 127.0.0.1:9995 -i 1" '_inactive' '_inactive'


echo -e "\nPassed ${GREEN}${BOLD}$PASSEDCNT${NC} out of ${BLUE}${BOLD}$ALLCNT${NC} tests"