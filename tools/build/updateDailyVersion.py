import os
import sys
import zipfile
import shutil

# first download sumo
print ("Downloading daily SUMO...")
os.system('curl https://sumo.dlr.de/daily/sumo-win64extra-git.zip --output sumo.zip')

# now check if sumo.zip exist
if os.path.exists("sumo.zip"):
    print ("Download sucesfully. Unzipping...")
else:
    print ("Error downloading SUMO")
    sys.exit()

# unzip SUMO
with zipfile.ZipFile("sumo.zip", 'r') as zip_ref:
    zip_ref.extractall(".")
 
# check if sumo-git exists 
if os.path.exists("sumo-git"):
    print ("Unzip sucesfully. Updating SUMO folder...")
else:
    print ("Error unzipping SUMO")
    sys.exit()
    
# Copy all files
for src_dir, dirs, files in os.walk('./sumo-git'):
    dst_dir = src_dir.replace('./sumo-git', '../../', 1)
    if not os.path.exists(dst_dir):
        os.makedirs(dst_dir)
    for file_ in files:
        src_file = os.path.join(src_dir, file_)
        dst_file = os.path.join(dst_dir, file_)
        if os.path.exists(dst_file):
            # in case of the src and dst are the same file
            if os.path.samefile(src_file, dst_file):
                continue
            os.remove(dst_file)
        shutil.move(src_file, dst_dir)

# write info
print ("All files copied. Cleaning...")

# remove temporal files
os.remove("sumo.zip")
shutil.rmtree("sumo-git")

# finished
print ("Done.")
