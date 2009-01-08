import os

box = "11.27053,44.46526,11.41250,44.53737"
prefix = "bologna"
num = 10

boxV = box.split(",")
bb = float(boxV[0])
ee = float(boxV[2])
b = bb
for i in range(0,num):
	e = b + (ee-bb) / float(num)
	cbox = str(b) + "," + boxV[1] + "," + str(e) + "," + boxV[3]
	call = "d:\\tools\\wget.exe http://api.openstreetmap.org/api/0.5/map?bbox=" + cbox + " -O " + prefix + str(i) + "_" + str(num) + ".osm.xml"
	print call
	os.system(call)
	b = e

	