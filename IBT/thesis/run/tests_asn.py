import sys, maxminddb, json

db = maxminddb.open_database("/home/adam/bp/GeoLite2-ASN.mmdb")
asn_label = "autonomous_system_number"

ip_src_prefix = "iana:sourceIP"
ip_dst_prefix = "iana:destinationIP"
ip_src = [ip_src_prefix+"v4Address", ip_src_prefix+"v6Address"]
ip_dst = [ip_dst_prefix+"v4Address", ip_dst_prefix+"v6Address"]

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

flow_cnt = 0
failed = False

def check_asn(flow, ip_list, type):
    global failed
    if (type == "src"):
        ASlabel = "iana:bgpSourceAsNumber"
    else:
        ASlabel = "iana:bgpDestinationAsNumber"
    for ip_type in ip_list:
        if (ip_type in flow.keys()):
            original_AS = db.get(flow[ip_type])
            if (original_AS) == None:
                if (ASlabel in flow.keys() and flow[ASlabel] != 0):
                    # AS from database does not exist, but it is in flow
                    print(f"{bcolors.FAIL + bcolors.UNDERLINE + ASlabel + bcolors.ENDC + bcolors.FAIL} with value {flow[ASlabel]} should not be in record ({flow_cnt}){bcolors.ENDC}")
                    print(f"{flow}")
                    failed = True
                    return 1
            else:
                # AS from database exists
                if (ASlabel not in flow.keys()):
                    # but it is not in flow
                    print(f"{bcolors.FAIL + bcolors.UNDERLINE + ASlabel + bcolors.ENDC + bcolors.FAIL} with value {original_AS[asn_label]} is not in record ({flow_cnt}){bcolors.ENDC}")
                    print(f"{flow}")
                    failed = True
                    return 1
                elif (original_AS[asn_label] != flow[ASlabel]):
                    # but numbers dont match
                    print(f"{bcolors.FAIL} Numbers for {bcolors.UNDERLINE + ASlabel + bcolors.ENDC + bcolors.FAIL} don't match ({flow_cnt}): expected {original_AS[asn_label]}, got {flow[ASlabel]}{bcolors.ENDC}")
                    print(f"{flow}")
                    failed = True
                    return 1
    return 0

def parse_flow(flow):
    global flow_cnt
    flow_cnt += 1
    changed = 0
    changed += check_asn(flow, ip_src, "src")
    changed += check_asn(flow, ip_dst, "dst")
    if (changed):
        print("")

file = sys.argv[1]
file = open(file)
parsed_flows = 0
for line in file:
    try:
        flow = json.loads(line.rstrip())
    except Exception:
        continue
    parse_flow(flow)
    parsed_flows += 1


db.close()
file.close()

print(f"{bcolors.OKCYAN}Parsed {parsed_flows} flows {bcolors.ENDC}")

if (failed):
    exit(1)
else:
    exit(0)