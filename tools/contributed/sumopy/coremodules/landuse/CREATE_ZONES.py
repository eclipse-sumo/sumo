# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    CREATE_ZONES.py
# @author  Joerg Schweizer
# @date   2012


# CREATE GRID ZONES


def create_zones(n_zones, x_min, x_max, y_min, y_max):
    zone_binx = np.int(np.sqrt(n_zones))
    zone_biny = np.int(np.sqrt(n_zones))
    zones = myscenario.landuse.zones
    zones.clear()

    x_bin = (x_max-x_min)/(zone_binx)
    y_bin = (y_max-y_min)/(zone_biny)
    zone_name = 1
    for i in range(zone_binx):
        for j in range(zone_biny):
            shape = [[x_min+i*x_bin, y_min+j*y_bin, 0.], [x_min+i*x_bin, y_min+(j+1)*y_bin, 0.], [x_min+(i+1)*x_bin, y_min+(
                j+1)*y_bin, 0.], [x_min+(i+1)*x_bin, y_min+j*y_bin, 0.], [x_min+i*x_bin, y_min+j*y_bin, 0.]]
            zones.make(zonename=zone_name, coord=np.zeros(3, dtype=np.float32), shape=shape, id_landusetype=6)
            zone_name += 1
    ids_zone = zones.get_ids()
    for id_zone in ids_zone:
        zones.identify_zonearea(id_zone)
        # ~ if zones.areas[id_zone] == 0:
        #~ zones.del_element(id_zone)
    ids_zone = zones.get_ids()
    return ids_zone

# CREATE ZONES by CLUSTER OF POINTS

    print 'k-value cluster'
    # k-value cluster
    # https://docs.scipy.org/doc/scipy/reference/generated/scipy.cluster.vq.kmeans.html

    features = np.array(origin_dest_points)
    print 'features', features
    # Whiten data
    st_dev_x = np.std(features[:][0])
    st_dev_y = np.std(features[:][1])
    print 'st_dev_x', st_dev_x
    print 'st_dev_y', st_dev_y

    whitened = whiten(features)
    # Find 2 clusters in the data
    codebook, distortion = kmeans(features, n_zones)
    print 'codebook', codebook

    features[:, 0] = features[:, 0]*st_dev_x
    features[:, 1] = features[:, 1]*st_dev_y
    codebook[:, 0] = codebook[:, 0]*st_dev_x
    codebook[:, 1] = codebook[:, 1]*st_dev_y

    # Plot whitened data and cluster centers in red
    plt.scatter(features[:, 0], features[:, 1])
    plt.scatter(codebook[:, 0], codebook[:, 1], c='r')
    # Create and plot zones Voronoy
    vor = Voronoi(codebook)
    fig = voronoi_plot_2d(vor)
    plt.show()

    # Create zones ON SUMOPY
    # myscenario.landuse.zones.clear()
    vertices = vor.vertices
    regions = vor.regions
    print vertices
    print regions
    zone_name = 1
    for region in regions:
        if region != []:
            shape = []
            for vertice in region:
                shape.append([vertices[vertice][0], vertices[vertice][1], 0.])
                ids_zone = myscenario.landuse.zones.get_ids()
                print 'n_zones =', len(ids_zone)
                print shape
            zone_name += 1
            # np.concatenate(shape).astype(None)
            print shape
            myscenario.landuse.zones.make(zonename='zone_name', coord=np.zeros(
                3, dtype=np.float32), shape=shape, id_landusetype=6)
    ids_zone = myscenario.landuse.zones.get_ids()
    zones_shape = myscenario.landuse.zones.shapes[ids_zone]
    # ~ #plt.scatter(zones_shape[0], zones_shape[1])
