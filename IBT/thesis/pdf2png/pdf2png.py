import os, sys, re

# need to install pdf2image package and poppler-utils
from pdf2image import convert_from_path

suffix="_png"

if len(sys.argv) == 2:
    print("python pdf2png PATH_TO_IMAGES ORIGINAL_TEX")
    exit(1)

path=sys.argv[1]
if (path[-1] == "/"):
    path = path[:-1]

def create_new_dir(path):
    try:
        os.mkdir(path)
    except FileExistsError:
        return

# make copy of original path with suffix
create_new_dir(path+suffix)

for subdir, dirs, files in os.walk(path):
    # keep directory structure the same
    for dir in dirs:
        new_subdir = subdir.replace(path, path+suffix) + "/" + dir
        create_new_dir(new_subdir)

    pdfs = [file for file in files if "pdf" in file] # all PDF files
    for pdf_file in pdfs:
        pdf_file_path = subdir+"/"+pdf_file # original file name
        print(pdf_file_path)
        converted = convert_from_path(pdf_file_path)
        new_subdir = subdir.replace(path, path+suffix) # updated subdir path with suffix
        converted[0].save((new_subdir+"/"+pdf_file).replace(".pdf", ".png"))

# replace all occurences of pds images in tex source
tex_src=sys.argv[2]
original = open(tex_src).read()

def replace_group(string):
    return (str(string).replace(".pdf", ".png"))

modified_text = re.sub(r"\\includegraphics(\[.*\])?{(.*)}", lambda m: replace_group(m.group()), original)
new_file = open(tex_src.replace(".tex", suffix+".tex"), "w").write(modified_text)