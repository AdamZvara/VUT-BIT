import argparse

DB_PATH="/home/current/xzvara01/bp/"
DATA_PATH="/home/current/xzvara01/bp/run/flow_files/test_data/"

"""
Common uses:
    python startup_generator.py ~/bp/run/testing/time/config/default/app/ app
    python startup_generator.py ~/bp/run/testing/time/config/default/plain/ plain

    python startup_generator.py ~/bp/run/testing/time/config/asn/app/ app -m asn
    python startup_generator.py ~/bp/run/testing/time/config/asn/plain/ plain -m asn

    python startup_generator.py ~/bp/run/testing/time/config/geoip/app/ app -m geoip
    python startup_generator.py ~/bp/run/testing/time/config/geoip/plain/ plain -m geoip
"""

## Parse arguments
parser = argparse.ArgumentParser(
            prog='startup_generator.py',
            description='Generate startup files for given configuration')

parser.add_argument('path', type=str, help='Path to location where startup files will be generated')
parser.add_argument('type', type=str, help='Type of input data (app or plain)', choices=['app', 'plain'])
parser.add_argument('-m', '--module', type=str, help='Optional intermediate module', choices=['asn', 'geoip'])

args = parser.parse_args()

ranges = [512, 1000, 1500, 2000, 2500, 3000, 5000, 10000]

## General plugin class
class Plugin:
    start = ""
    end = ""
    data = ""
    type = ""

    ranges_idx = 0

    def __init__(self, data, type):
        self.data = data
        self.type = type

    def print(self, filename):
        global ranges
        if (self.ranges_idx >= len(ranges)):
            return
        filename.write(self.start)
        if (self.data != "" and type != ""):
            filename.write(f"\t\t\t\t<path>{self.data}data-{self.type}-{ranges[self.ranges_idx]}.ipfix</path>")
        filename.write(self.end)
        self.ranges_idx += 1

## Input plugin class
class Input_plugin(Plugin):
    start = \
"""
  <inputPlugins>
    <input>
      <name>IPFIX File</name>
      <plugin>ipfix</plugin>
      <params>
"""
    end = \
"""
      </params>
    </input>
  </inputPlugins>
"""
    def __init__(self, type):
        super().__init__(DATA_PATH, type)

## Output plugin class
class Output_plugin(Plugin):
    start = \
"""
  <outputPlugins>
    <output>
      <name>Dummy output</name>
      <plugin>dummy</plugin>
      <params>
        <delay>0</delay>
        <stats>true</stats>
      </params>
    </output>
  </outputPlugins>
"""
    def __init__(self):
        super().__init__("", "")

## Intermediate ASN plugin class
class asn_plugin(Plugin):
    start = \
f"""
  <intermediatePlugins>
    <intermediate>
      <name>Int plugin</name>
      <plugin>asn</plugin>
        <params>
          <path>{DB_PATH}GeoLite2-ASN.mmdb</path>"""
    end = \
"""
        </params>
      </intermediate>
    </intermediatePlugins>
"""
    def __init__(self):
        super().__init__("", "")

## Intermediate ASN plugin class
class geoip_plugin(Plugin):
    start = \
f"""
  <intermediatePlugins>
    <intermediate>
      <name>Int plugin</name>
      <plugin>geoip</plugin>
        <params>
          <path>{DB_PATH}GeoLite2-City.mmdb</path>
          <fields>
            <field>country</field>
            <field>city</field>
          </fields>"""
    end = \
"""
        </params>
      </intermediate>
    </intermediatePlugins>
"""
    def __init__(self):
        super().__init__("", "")

a = Input_plugin(args.type)
b = Output_plugin()
c = geoip_plugin()
d = asn_plugin()

for i in range(len(ranges)):
    with open(f"{args.path}{ranges[i]}.xml", "w") as f:
        f.write("<ipfixcol2>")
        a.print(f)
        if (args.module == "asn"):
            d.print(f)
        elif (args.module == "geoip"):
            c.print(f)
        b.print(f)
        f.write("</ipfixcol2>")
