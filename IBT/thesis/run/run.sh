#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

BASEDIR=$(pwd)
BASEDIR=${BASEDIR%/*/*}

IPXCOL=$BASEDIR/ipfixcol2/build/src/core/ipfixcol2
IPFIXSEND=$BASEDIR/ipfixcol2/build/src/tools/ipfixsend/ipfixsend2
STARTUPS=$BASEDIR/bp/run/startups
ASN_MODULE=$BASEDIR/ipfixcol2/build/src/plugins/intermediate/asn/libasn-intermediate.so
GEO_MODULE=$BASEDIR/ipfixcol2/build/src/plugins/intermediate/geoip/libgeoip-intermediate.so

usage() {
    echo -e "Simple tester for IPFIXcol2 collector"
    echo -e "Usage: ./run MODULE [OPTIONS]\n"
    echo -e "Modules supported: asn, geo"
    echo -e "OPTIONS:\n\t -d IPFIX input to dummy output using given plugin"
    echo -e "\t -j IPFIX input to printed output in JSON using given plugin"
    echo -e "\t -u UDP input to printed output in JSON using given plugin"
    echo -e "\t -t Testing output flow values in python script"
    exit 1
}

# Arguments: config file, module, verbosity
function run_collector {
    # Check module
    if [ "$2" == "asn" ]; then
        MODULE=$ASN_MODULE
    elif [ "$2" == "geo" ]; then
        MODULE=$GEO_MODULE
    else
        usage
    fi
    echo "$IPXCOL -c $STARTUPS/$1 -p $MODULE -$3"
    $IPXCOL -c $STARTUPS/$1 -p $MODULE -$3
}

echo -e "Building IPFIXcol2"

cd ~/ipfixcol2/build
make > /dev/null
if [ $? != 0 ]; then
    echo -e "${RED}Failed to build IPFIXcol2${NC}"
    exit 1
fi
echo -e "${GREEN}Build successful${NC}"

# Running specific configuration of IPFIXcol2
cd ~/bp/run

function failed {
    echo -e "${RED}Test failed${NC}"
    exit 1
}

function success {
    echo -e $1 "${GREEN}Test successful${NC}"
}

# Arguments: path to file, type, send x times, ODID
function ipfixsend {
    if [ "$2" == "UDP" ]; then
        port=7000
    else
        port=8000
    fi
    $IPFIXSEND -i $1 -d 127.0.0.1 -p $port -t $2 -n $3 -O $4
}

# Arguments: name of test, config file, module
function test {
    echo -e "Running test: " $1
    run_collector $2 $3 > tmp
    if [ $? != 0 ]; then
        echo -e "${RED}Failed to create temporary file${NC}"
        failed
    fi
    if [ $3 == "asn" ]; then
        python3.10 tests_asn.py tmp
    elif [ $3 == "geo" ]; then
        python3.10 tests_geoip.py tmp
    fi
    return $?
}

function test_multiple {
    rm -rf tmp
    echo -e "Running test: Multiple sources"
    mkdir tmp
    echo -e "Starting collector"
    if [ "$1" == "asn" ]; then
        run_collector multi_asn_json.xml asn > tmp/tmp &
    else
        run_collector multi_geo_json.xml geo > tmp/tmp &
    fi
    sleep 2
    echo -e "Sending short flows to TCP module"
    ipfixsend flow_files/short_flows.ipfix TCP 5 40
    echo -e "Sending short flows to UDP module"
    ipfixsend flow_files/short_flows.ipfix UDP 5 50
    echo -e "Sending long flows to UDP module"
    ipfixsend flow_files/3k_flows.ipfix UDP 10 10
    echo -e "Sending long flows to TCP module"
    ipfixsend flow_files/3k_flows.ipfix TCP 10 30
    sleep 20
    if [ $? != 0 ]; then
        echo -e "${RED}Failed to create temporary file${NC}"
        failed
    fi
    echo "Finished creating output files, running python"
    cd tmp/ && split -l 5000 tmp
    rm -rf tmp
    echo "Splitting files"
    for FILE in *; do
        echo -e -n "checking $FILE ";
        if [ "$1" == "asn" ]; then
            python3.10 ../tests_asn.py $FILE
        else
            python3.10 ../tests_geoip.py $FILE
        fi
        if [ $? != 0 ]; then
            return $?
        fi
    done
    cd ..
    rm -rf tmp/
    pkill ipfixcol2
}

function check_failed {
    FAILED=$?
    if [ $FAILED != 0 ]; then
        failed
    else
        success
    fi
}

if [ "$2" == "-d" ]; then
    # Dummy output
    if [ "$1" == "asn" ]; then
        echo -e "${RED}Running ASN module with dummy output${NC}"
        run_collector ipfix_asn_dummy.xml $1 $3
    elif [ "$1" == "geo" ]; then
        echo -e "${RED}Running GEO module with dummy output${NC}"
        run_collector ipfix_geoip_dummy.xml $1 $3
    else
        usage
    fi
elif [ "$2" == "-j" ]; then
    # JSON output
    if [ "$1" == "asn" ]; then
        echo -e "${RED}Running ASN module with json output${NC}"
        run_collector ipfix_asn_json.xml $1 $3
    elif [ "$1" == "geo" ]; then
        echo -e "${RED}Running GEO module with json output${NC}"
        run_collector ipfix_geoip_json.xml $1 $3
    else
        usage
    fi
elif [ "$2" == "-t" ]; then
    if [ "$1" == "asn" ]; then
        # Overall tests for ASN
        rm -rf tmp
        test "Short IPFIX file" ipfix_asn_json.xml asn
        check_failed
        test "Long IPFIX file" long_ipfix_asn_json.xml asn
        check_failed
        test_multiple asn
        check_failed
    elif [ "$1" == "geo" ]; then
        # Overall tests for GEO
        rm -rf tmp
        test "Short IPFIX file" ipfix_geoip_json.xml geo
        check_failed
        test "Long IPFIX file" long_ipfix_geoip_json.xml geo
        check_failed
        test_multiple geo
        check_failed
    else
        usage
    fi
elif [ "$2" == "-o" ]; then
    if [ "$1" == "asn" ]; then
        echo -e "${RED}Running ASN module with option flow${NC}"
        run_collector option/ipfix_asn_json.xml $1 $3
    elif [ "$1" == "geo" ]; then
        echo -e "${RED}Running GEO module with option flow ${NC}"
        run_collector option/ipfix_geoip_json.xml $1 $3
    else
        usage
    fi
else
    usage
fi
