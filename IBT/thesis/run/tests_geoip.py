import sys, json
from ip2geotools.databases.noncommercial import MaxMindGeoLite2City

ip_src_prefix = "iana:sourceIP"
ip_dst_prefix = "iana:destinationIP"
ip_src = [ip_src_prefix+"v4Address", ip_src_prefix+"v6Address"]
ip_dst = [ip_dst_prefix+"v4Address", ip_dst_prefix+"v6Address"]

geo_country = [f"cesnet:geoip{x}" for x in ["SrcCountry", "DstCountry"]]
geo_latitude = [f"cesnet:geoip{x}" for x in ["SrcLatitude", "DstLatitude"]]
geo_longitude = [f"cesnet:geoip{x}" for x in ["SrcLongitude", "DstLongitude"]]

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
geo_vals_parsed = 0
geo_vals_skipped = 0
failed = False

def check_country(country, flow, direction):
    global failed, geo_vals_parsed, geo_vals_skipped
    used_geo_keys = [x for x in geo_country if direction in x]
    for key in used_geo_keys:
        if (country == None):
            geo_vals_skipped += 1
            if (flow[key] != ""):
                print(f"{bcolors.FAIL + bcolors.UNDERLINE + key + bcolors.ENDC + bcolors.FAIL} with value {flow[key]} does not match database value {country} ({flow_cnt}){bcolors.ENDC}")
            continue
        if (flow[key] != country):
            print(f"{bcolors.FAIL + bcolors.UNDERLINE + key + bcolors.ENDC + bcolors.FAIL} with value {flow[key]} does not match database value {country} ({flow_cnt}){bcolors.ENDC}")
            return 1
        else:
            geo_vals_parsed += 1
            return 0

def check_latitude(latitude, flow, direction):
    global failed, geo_vals_parsed, geo_vals_skipped
    used_geo_keys = [x for x in geo_latitude if direction in x]
    for key in used_geo_keys:
        if (latitude == None):
            geo_vals_skipped += 1
            if (flow[key] != 0):
                print(f"{bcolors.FAIL + bcolors.UNDERLINE + key + bcolors.ENDC + bcolors.FAIL} with value {flow[key]} does not match database value {latitude} ({flow_cnt}){bcolors.ENDC}")
            continue
        if (flow[key] != latitude):
            print(f"{bcolors.FAIL + bcolors.UNDERLINE + key + bcolors.ENDC + bcolors.FAIL} with value {flow[key]} does not match database value {latitude} ({flow_cnt}){bcolors.ENDC}")
            return 1
        else:
            geo_vals_parsed += 1
            return 0

def check_longitude(longitude, flow, direction):
    global failed, geo_vals_parsed, geo_vals_skipped
    used_geo_keys = [x for x in geo_longitude if direction in x]
    for key in used_geo_keys:
        if (longitude == None):
            geo_vals_skipped += 1
            if (flow[key] != 0):
                print(f"{bcolors.FAIL + bcolors.UNDERLINE + key + bcolors.ENDC + bcolors.FAIL} with value {flow[key]} does not match database value {longitude} ({flow_cnt}){bcolors.ENDC}")
            continue
        if (flow[key] != longitude):
            print(f"{bcolors.FAIL + bcolors.UNDERLINE + key + bcolors.ENDC + bcolors.FAIL} with value {flow[key]} does not match database value {longitude} ({flow_cnt}){bcolors.ENDC}")
            return 1
        else:
            geo_vals_parsed += 1
            return 0

def check_geo(flow, ip_list, direction):
    global failed, geo_vals_skipped

    # Go through IP addresses list (v4 or v6)
    for ip_type in ip_list:
        if (ip_type in flow.keys()): # It contains IP address
            try:
                ipinfo = MaxMindGeoLite2City.get(flow[ip_type], api_key='free', db_path='/home/adam/bp/GeoLite2-City.mmdb')
                err1 = check_country(ipinfo.country, flow, direction)
                err2 = check_longitude(ipinfo.longitude, flow, direction)
                err3 = check_latitude(ipinfo.latitude, flow, direction)
                if (err1 or err2 or err3):
                    print(flow)
            except Exception:
                # IP not found in database
                geo_vals_skipped += 1
                continue
    return 0

def parse_flow(flow):
    global flow_cnt
    flow_cnt += 1
    changed = 0
    changed += check_geo(flow, ip_src, "Src")
    changed += check_geo(flow, ip_dst, "Dst")
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


file.close()

print(f"{bcolors.OKCYAN}Found {geo_vals_parsed} geoip values {bcolors.ENDC}")
print(f"{bcolors.OKCYAN}Skipped {geo_vals_skipped} geoip values {bcolors.ENDC}")
print(f"{bcolors.OKBLUE}Parsed {parsed_flows} flows {bcolors.ENDC}")

if (failed):
    exit(1)
else:
    exit(0)