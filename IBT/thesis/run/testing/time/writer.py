import argparse, subprocess, os, shutil, re, math, threading, sys
from subprocess import PIPE

# Repetitions of single test
SINGLE_TEST_REP = 5

BASEPATH = "/home/current/xzvara01/"
CWD = os.getcwd()

## Parse arguments
parser = argparse.ArgumentParser(
            prog='writer.py',
            description='Write output of execution time tests for given configuration')

parser.add_argument('output', type=str, help='Path to store output files')
parser.add_argument('path', type=str, help='Path to input files')
parser.add_argument('-p', '--plain', action='store_true', help='Only run plain tests')
parser.add_argument('-a', '--app', action='store_true', help='Only run app tests')

parser.set_defaults(plain=False)
parser.set_defaults(app=False)

args = parser.parse_args()
if (args.path[-1] == "/"):
    args.path = args.path[:-1]

if (not args.plain and not args.app):
    args.plain = args.app = True

output = b""

def str2int(value):
    return int(value[:value.index("\\")])

class Test:
    _results = []
    files_path = ""
    plain_files = [f"/home/current/xzvara01/bp/run/flow_files/test_data/data-plain-{x}.ipfix" for x in [512, 1000, 1500, 2000, 2500, 3000, 5000, 10000]]
    app_files = [f"/home/current/xzvara01/bp/run/flow_files/test_data/data-app-{x}.ipfix" for x in [512, 1000, 1500, 2000, 2500, 3000, 5000, 10000]]

    def __init__(self, out, config, plain, app):
        self.plain = plain
        self.app = app
        self.out_file = out
        self.file = open(out, "w")
        self.config_file = config

    def __del__(self):
        self.file.close()

    def get_cnt(self, config):
        cnt = 1
        N = self.config2int(config)

        if ("asn" in self.out_file or "geoip" in self.out_file):
            if ("plain" in config):
                # PLAIN CNT
                if   N <= 1500: cnt = 2
                elif N <= 2500: cnt = 3
                elif N > 2500:  cnt = 3
                else:           cnt = 7
            else:
                # APP CNT
                if   N <= 512:   cnt = 1
                elif N <= 2500:  cnt = 2
                else:            cnt = 3
        # elif ():
        #     cnt = 1
        elif ("dummy" in self.out_file):
            if ("plain" in config):
                # PLAIN CNT
                if   N <= 1000:  cnt = 1
                elif N >= 10000: cnt = 3
                else:            cnt = 2
            else:
                # APP CNT
                if   N <= 2500:  cnt = 1
                else:            cnt = 2
        else:
            if ("plain" in config or config.count("dummy") == 2):
                # PLAIN CNT
                if   N <= 512:  cnt = 2
                elif N <= 1500: cnt = 5
                elif N <= 2500: cnt = 10
                elif N > 2500:  cnt = 15
                else:           cnt = 20
            else:
                # APP CNT
                if   N <= 512:   cnt = 1
                elif N <= 2000:  cnt = 3
                elif N <= 2500:  cnt = 5
                elif N <  10000: cnt = 7
                else:            cnt = 8
        return cnt

    def config2int(self, config):
        delim = config.rfind("-")
        return int(config[delim+1:config.rfind('.')])

    def output_reader(self, proc):
        # Read output from ipfixcol
        global output
        while True:
            byte = proc.stdout.read(1)
            if byte:
                output += byte
            else:
                break

    def parse_records_cnt(self, out: str):
        out = str(out)
        out = out.split()
        return int(out[3][:-3])

    def parse_exec_time(self, start: str, end: str):
        # Get start time
        start_time = str2int(start)
        # Get end time
        end_time = str2int(end)
        return (end_time - start_time) / 1_000_000

    # Run single list
    def run_single(self, config):
        global output
        all_times = []

        # Get number of repetitions for ipfixsend depending on file size
        cnt = self.get_cnt(config)
        if ("asn" in self.out_file):
            cmd = f"{BASEPATH}ipfixcol2/build/src/core/ipfixcol2 -c {BASEPATH}bp/run/testing/time/config/tcp_asn_dummy.xml -p /home/current/xzvara01/local/lib64/ipfixcol2/"
        elif ("geoip" in self.out_file):
            cmd = f"{BASEPATH}ipfixcol2/build/src/core/ipfixcol2 -c {BASEPATH}bp/run/testing/time/config/tcp_geoip_dummy.xml -p /home/current/xzvara01/local/lib64/ipfixcol2/"
        elif ("dummy" in self.out_file):
            cmd = f"{BASEPATH}ipfixcol2/build/src/core/ipfixcol2 -c {BASEPATH}bp/run/testing/time/config/tcp_dummy_dummy.xml -p /home/current/xzvara01/local/lib64/ipfixcol2/"
        else:
            cmd = f"{BASEPATH}ipfixcol2/build/src/core/ipfixcol2 -c {BASEPATH}bp/run/testing/time/config/tcp_anon_cryptopan.xml -p /home/current/xzvara01/local/lib64/ipfixcol2/"
        print(cmd)

        file = config
        cmd2 = f"{BASEPATH}ipfixcol2/build/src/tools/ipfixsend/ipfixsend2 -c -t TCP -p 8000 -n {cnt} -i {file}"
        print(cmd2)

        c = config[config.rfind("/")+1:]
        c = c[c.find("-")+1:]
        print(f"\t{c}, N: {cnt}")

        _ = 0

        # Run tests
        while _ != SINGLE_TEST_REP:

            # Create instance of IPFIXcol2
            ipficol = subprocess.Popen([cmd], stdout = PIPE, shell = True)

            # Create instance of ipfixsend
            ipfixsend = subprocess.Popen([cmd2], shell = True)

            # Start testing
            t1 = threading.Thread(target=self.output_reader, args=[ipficol])
            t1.start()
            ipfixsend_ret = ipfixsend.wait()
            while ipfixsend_ret != 0:
                # ipfixsend failed, try to repeat
                ipfixsend = subprocess.Popen([cmd2], shell = True)
                ipfixsend_ret = ipfixsend.wait()
            t1.join()

            # Parse output
            result = str(output).split()
            if (len(result) < 5):
                continue

            # Get execution time from output
            exec_time = self.parse_exec_time(result[4], result[8])
            all_times.append(exec_time)
            output = b""
            _ += 1

        # Get average of execution times
        exec_time = sum(all_times) / len(all_times)
        print(f"\t\t times: {all_times}, average: {exec_time}")
        # Get number of records from stdout
        record_cnt = str2int(result[11])
        # Get number of records per second
        records_per_sec = record_cnt / exec_time
        # Store results in array
        name = config[config.rfind("/")+1:-4]
        self._results.append([name, math.floor(records_per_sec)])

    def run_suite(self, type_str, files):
        print(f"Runnig {type_str} tests from: {self.config_file}")
        for file in files:
            self.run_single(file)

    # Run all tests based on program arguments
    def run(self):
        if (self.plain):
            self.run_suite("plain", self.plain_files)
        elif (self.app):
            self.run_suite("app", self.app_files)

    # Print results
    def print_results(self):
        for result in self._results:
            print(result[0], result[1])
        self._results.clear()

    # Store results
    def store_results(self):
        for result in self._results:
            name = str(result[0])
            self.file.write(name[name.rfind("-")+1:name.rfind(".")] + "\t")
            self.file.write(str(result[1]))
            self.file.write("\n")
        self._results.clear()
        self.file.flush()

    def __str__(self):
        return self.file.name

def get_last_dirs(path):
    return_dirs = []
    for subdir, dirs, _ in os.walk(path):
        if (dirs == []):
            return_dirs.append(subdir)
    return return_dirs

class Startups:
    def __init__(self, out_file, path, plain, app):
        # Create output directory
        try:
            os.mkdir(CWD+"/"+out_file)
        except Exception:
            pass
        if (os.path.isfile(path)):
            if ("asn" in path):
                prefix = "asn"
            elif ("geoip" in path):
                prefix = "geoip"
            elif (path.count("dummy") == 2):
                prefix = "dummy"
            else:
                prefix = "default"

            if plain:
                self.startups[prefix+"_plain"] = Test(out_file+"/"+prefix+"_plain", path, plain, False)
            if app:
                self.startups[prefix+"_app"] = Test(out_file+"/"+prefix+"_app", path, False, app)

    @property
    def startups(self):
        if not hasattr(self, "_startups"):
            self._startups = {}
        return self._startups

    @startups.setter
    def startups(self, key, value):
        self._startups[key] = value

if __name__ == "__main__":
    S = Startups(args.output, args.path, args.plain, args.app)

    for startup in S.startups.items():
        startup = startup[1]
        startup.run()
        startup.store_results()
