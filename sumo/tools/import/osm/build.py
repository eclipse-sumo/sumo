import os

prefix = "bologna"
num = 10

call = ""
for i in range(0,num):
	if i!=0:
		call = call + ","
	call = call + prefix + str(i) + "_" + str(num) + ".osm.xml"
call = "%SUMO%\\netconvert --osm " + call + " --use-projection -o all_plain.net.xml -v"
print call
os.system(call)

	