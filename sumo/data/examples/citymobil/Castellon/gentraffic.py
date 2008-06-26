import sys, random

rutas = [(3, "ruta1NS", ("cruce4iN cruce4oS cruce4oS_",)),
         (4, "ruta1SN", ("cruce4iS cruce4oN cruce4oN_",)),
         (5, "ruta2NS", ("cruceA45iN cruceA45oS cruceA45oS_",)),
         (6, "ruta2SN", ("cruceA45iS cruceA45oN cruceA45oN_",)),
         (7, "ruta3NS", ("cruceB45iN cruceB45oS cruceB45oS_",)),
         (8, "ruta3SN", ("cruceB45iS cruceB45oN cruceB45oN_",)),
         (9, "ruta136", ("cruce13iS 135 56 cruce6oN cruce6oN_",)),
         (10, "ruta67", ("cruce6iN 67 cruce7oN cruce7oN_",)),
         (11, "ruta7x", ("cruce7iN 78 cruce8oN cruce8oN_", "cruce7iN 711 1112 cruce12oS cruce12oS_")),
         (12, "ruta89", ("cruce8iN 89 cruce9oS cruce9oS_",)),
         (13, "ruta912", ("cruce9iS 910 1011 1112 cruce12oS cruce12oS_",)),
         (14, "ruta1213", ("cruce12iS 1213 cruce13oS cruce13oS_",)),
         (15, "rutashared", ("input_shared_traffic_lane 4cruceA45 cruceA45cruceB45 cruceB455 56 cruce6oN cruce6oN_",)),
         (16, "rutashared2", ("cruce13iS_ cruce13iS 135 5cruceB45 cruceB45cruceA45 cruceA454 cruce4oN",))]

salida = open(sys.argv[1] + ".rou.xml", "w")
print >> salida, "<routes>"
time = 1;
while time < int(sys.argv[2]):
    for idx, routeName, routes in rutas:
        if random.randint(1, 60) < int(sys.argv[idx]):
            print >> salida, """\
    <vehicle id="%s_%s" type="CAR" depart="%s">
        <route>%s</route>
    </vehicle>""" % (routeName, time, time, random.choice(routes));
    time += 1
print >> salida, "</routes>"
salida.close()
