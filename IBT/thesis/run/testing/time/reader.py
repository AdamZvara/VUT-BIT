import argparse, os, sys
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

args = None
file_list = None

## Parse arguments
parser = argparse.ArgumentParser(
        prog='reader.py',
        description='Create graph from multiple imput files genereted by writer.py')

parser.add_argument('-f', '--files', type=str, help='Path to input files')
parser.add_argument('-o', '--output', type=str, help='Name of output graph')
parser.add_argument('file', type=str, nargs='*', help='Files to process')

args = parser.parse_args()

def get_file_list(path):
    if (path[-1] == "/"):
        path = path[:-1]
    return [path + "/" + file for file in os.listdir(path)]

if (args.file):
    print("Specified files:", args.file)
    file_list = args.file
else:
    print("Specified path:", args.files)
    file_list = get_file_list(args.files)

# Read all files
all_data = []
mark_flag = False
for file in file_list:
    if ("asn" in file or "geoip" in file):
        mark_flag = True
    with open(file, "r") as f:
        for line in f:
            line = line.rstrip().split()
            all_data.append([file, int(line[0]), int(line[1])])

# Generate dataframe
df = pd.DataFrame(all_data)

y_name = "Rýchlosť prenosu"
x_name = "Veľkosť záznamov"
z_name = "Typ súboru"

df.rename(columns={0: z_name, 1: x_name, 2: y_name}, inplace=True)

# Plot dataframe
ax = sns.barplot(data=df, x = x_name, y = y_name, hue = z_name)

## y axis formatting
plt.ticklabel_format(style='plain', axis='y')
mark_flag = True
if (mark_flag):
    M = 3
    unit = "k"
else:
    M = 6
    unit = "M"
ax.get_yaxis().set_major_formatter(plt.FuncFormatter(lambda x, loc: str(int(x)/(10**M))[:-2] + unit))

## grid
ax.grid(linestyle='dotted')
ax.xaxis.grid(False)
ax.set_axisbelow(True)

# Plot graph
if (args.output):
    plt.savefig(args.output)
else:
    plt.savefig("graph.png")