# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
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

# @file    maps.py
# @author  Joerg Schweizer
# @date   2012

# size limit at 1280x1280
# http://maps.googleapis.com/maps/api/staticmap?size=500x500&path=color:0x000000|weight:10|44.35789,11.3093|44.4378,11.3935&format=GIF&maptype=satellite&scale=2
# https://console.cloud.google.com/google/maps-apis
import os
import sys
import numpy as np
import wx
import urllib
from collections import OrderedDict
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm

import matplotlib.pyplot as plt
from coremodules.misc.matplottools import *

##from agilepy.lib_base.processes import Process
from coremodules. network import netconvert
from agilepy.lib_base.processes import Process, CmlMixin, P

import csv

from matplotlib import cm as cmp
from mpl_toolkits.mplot3d import Axes3D
try:
    from scipy import interpolate
    is_scipy = True
except:
    is_scipy = False

IS_MAPSUPPORT = True
try:
    from PIL import ImageFilter, Image, ImageChops, ImagePath, ImageDraw
except:
    print "WARNING: Maps requires PIL module."
    IS_MAPSUPPORT = False

try:
    import pyproj

except:
    try:
        from mpl_toolkits.basemap import pyproj

    except:
        print "WARNING: Maps requires pyproj module."
        IS_MAPSUPPORT = False
        # print __doc__
        # raise


URL_GOOGLEMAP = "https://maps.googleapis.com/maps/api/staticmap?"


def download_googlemap(filepath, bbox, proj, size=640, filetype='gif', maptype='satellite'):
    print 'download_googlemap', bbox
    # https://developers.google.com/maps/documentation/static-maps/intro#Paths
    x_sw, y_sw = bbox[0]
    x_ne, y_ne = bbox[1]

    # 01    11
    #
    #
    # 00    10
    lon00, lat00 = proj(x_sw, y_sw, inverse=True)
    lon10, lat10 = proj(x_ne, y_sw, inverse=True)
    lon11, lat11 = proj(x_ne, y_ne, inverse=True)
    lon01, lat01 = proj(x_sw, y_ne, inverse=True)

    size_x = size_y = size/2
    url = URL_GOOGLEMAP+"size=%dx%d&visible=%.6f,%.6f|%.6f,%.6f&format=%s&maptype=%s&scale=2"\
        % (size_x, size_y, lat00, lon00, lat11, lon11, filetype.upper(), maptype)
    print '  url=', url
    urllib.urlretrieve(url, filepath)

    bbox_lonlat = np.array([[lon00, lat00], [lon11, lat11]])
    return bbox_lonlat


def download_googlemap_bb(filepath, bbox, proj, apikey, size=640, filetype='gif', maptype='satellite', color="0xff0000ff"):
    print 'download_googlemap_bb', bbox
    # https://developers.google.com/maps/documentation/static-maps/intro#Paths
    x_sw, y_sw = bbox[0]
    x_ne, y_ne = bbox[1]

    # 01    11
    #
    #
    # 00    10
    lon00, lat00 = proj(x_sw, y_sw, inverse=True)
    lon10, lat10 = proj(x_ne, y_sw, inverse=True)
    lon11, lat11 = proj(x_ne, y_ne, inverse=True)
    lon01, lat01 = proj(x_sw, y_ne, inverse=True)

    size_x = size_y = size/2

    #    3
    #
    # 0     2
    #
    #   1
    dhx = (x_ne-x_sw)/2
    dhy = (y_ne-y_sw)/2
    lon0, lat0 = proj(x_sw, y_sw+dhy, inverse=True)
    lon1, lat1 = proj(x_sw+dhx, y_sw, inverse=True)
    lon2, lat2 = proj(x_ne, y_sw+dhy, inverse=True)
    lon3, lat3 = proj(x_sw+dhx, y_ne, inverse=True)

    url = URL_GOOGLEMAP+"key=%s&size=%dx%d&format=%s&maptype=%s&scale=2&path=color:%s|weight:3" % (
        apikey, size_x, size_y, filetype.upper(), maptype, color)
    url += "|%.6f,%.6f" % (lat0, lon0)
    url += "|%.6f,%.6f" % (lat2, lon2)
    url += "|%.6f,%.6f" % (lat10, lon10)
    url += "|%.6f,%.6f" % (lat1, lon1)
    url += "|%.6f,%.6f" % (lat3, lon3)
    url += "|%.6f,%.6f" % (lat01, lon01)
    url += "|%.6f,%.6f" % (lat00, lon00)
    url += "|%.6f,%.6f" % (lat10, lon10)
    url += "|%.6f,%.6f" % (lat11, lon11)
    url += "|%.6f,%.6f" % (lat3, lon3)

    #url = URL_GOOGLEMAP+"key=%s&size=%dx%d&format=%s&maptype=%s&scale=2&path=color:%s|weight:5"%(apikey,size_x,size_y,filetype.upper(),maptype,color)
    #url += "|%.6f,%.6f"%(lat00,lon00)
    #url += "|%.6f,%.6f"%(lat10,lon10)
    #url += "|%.6f,%.6f"%(lat11,lon11)
    #url += "|%.6f,%.6f"%(lat01,lon01)
    #url += "|%.6f,%.6f"%(lat00,lon00)

    # urllib.urlretrieve (URL_GOOGLEMAP+"size=%dx%d&format=%s&maptype=%s&scale=2&path=color:0xff0000ff|weight:1|%.5f,%.5f|%.5f,%.5f|%.5f,%.5f|%.5f,%.5f"\
    #    %(size_x,size_y,filetype,maptype,lat00,lon00, lat11,lon11, lat01,lon01, lat10,lon10), filepath)
    print '  url=', url
    urllib.urlretrieve(url, filepath)
    bbox_lonlat = np.array([[lon00, lat00], [lon11, lat11]])
    return bbox_lonlat


def estimate_angle(filepath):

    im = Image.open(filepath).convert("RGB")
    print 'estimate_angle image', filepath,  "%dx%d" % im.size, im.mode, im.getbands()
    imr, img, imb = im.split()

    # calculate width and height of bbox in pixel from measured rectangle
    #wr = int(np.sqrt((rect[1][0]-rect[0][0])**2+(rect[1][1]-rect[0][1])**2))
    #wr_check = int(np.sqrt((rect[2][0]-rect[3][0])**2+(rect[2][1]-rect[3][1])**2))
    #hr = int(np.sqrt((rect[3][0]-rect[0][0])**2+(rect[3][1]-rect[0][1])**2))
    #h_check = int(np.sqrt((rect[2][0]-rect[1][0])**2+(rect[2][1]-rect[1][1])**2))

    xcb = im.size[0]/2
    ycb = im.size[1]/2
    wx = im.size[0]
    wy = im.size[1]
    hx = wx/2
    hy = wy/2

    # create an image and draw a rectanle with coordinates from bbox
    #bbox = [(xcb-wr/2,ycb-hr/2), (xcb+wr/2,ycb-hr/2), (xcb+wr/2,ycb+hr/2), (xcb-wr/2,ycb+hr/2),(xcb-wr/2,ycb-hr/2)]
    im_bbox = ImageChops.constant(im, 0)
    draw = ImageDraw.Draw(im_bbox)
    draw.line([(0, hy), (wx, hy)], fill=255)
    draw.line([(hx, 0), (hx, wy)], fill=255)
    del draw

    # rotate im_bbox with different angles
    # and correlate with the red component of the image in filepath
    # which contains a geo-referenced red rectangle
    angles = np.arange(-3.0, 3.0, 0.01)
    matches = np.zeros(len(angles))
    for i in xrange(len(angles)):
        im_bbox_rot = im_bbox.rotate(angles[i])  # gimp 1.62
        im_corr = ImageChops.multiply(imr, im_bbox_rot)
        # im_corr.show()
        im_corr_arr = np.asarray(im_corr)
        matches[i] = np.sum(im_corr_arr)/255
        # print ' angles[i],matches[i]',angles[i],matches[i]

    angle_opt = angles[np.argmax(matches)]
    print '  angle_opt', angle_opt

    ys = np.arange(0, int(0.2*wy), 1)
    matches = np.zeros(len(ys))
    for y in xrange(len(ys)):
        im_bbox = ImageChops.constant(im, 0)
        draw = ImageDraw.Draw(im_bbox)
        draw.line([(0, y), (wx, y)], fill=255)
        draw.line([(0, wy-y), (wx, wy-y)], fill=255)
        del draw
        im_bbox_rot = im_bbox.rotate(angle_opt)
        im_corr = ImageChops.multiply(imr, im_bbox_rot)
        # im_corr.show()
        im_corr_arr = np.asarray(im_corr)
        matches[y] = np.sum(im_corr_arr)/255
        # print ' y,matches[y]',y,matches[y]

    y_opt = ys[np.argmax(matches)]
    print '  y_opt', y_opt

    if 0:
        im_bbox = ImageChops.constant(im, 0)
        draw = ImageDraw.Draw(im_bbox)
        draw.line([(0, y_opt), (wx, y_opt)], fill=255)
        draw.line([(0, wy-y_opt), (wx, wy-y_opt)], fill=255)
        im_bbox_rot = im_bbox.rotate(angle_opt)
        im_bbox_rot.show()
        im.show()

    # assuming rectangle:
    bbox = [(y_opt, y_opt), (wx-y_opt, y_opt), (wx-y_opt, wy-y_opt), (y_opt, wy-y_opt), (y_opt, y_opt)]
    print '  bbox', bbox
    return -angle_opt, bbox


class MapsImporter(Process):
    def __init__(self,  maps, logger=None, **kwargs):
        print 'MapsImporter.__init__', maps, maps.parent.get_ident()
        self._init_common('mapsimporter', name='Background maps importer',
                          logger=logger,
                          info='Downloads and converts background maps.',
                          )
        self._maps = maps

        attrsman = self.set_attrsman(cm.Attrsman(self))
        #self.net = attrsman.add(   cm.ObjConf( network.Network(self) ) )
        # self.status = attrsman.add(cm.AttrConf(
        #                            'status', 'preparation',
        #                            groupnames = ['_private','parameters'],
        #                            perm='r',
        #                            name = 'Status',
        #                            info = 'Process status: preparation-> running -> success|error.'
        #                            ))

        self.width_tile = attrsman.add(cm.AttrConf('width_tile', kwargs.get('width_tile', 500.0),
                                                   groupnames=['options'],
                                                   choices=OrderedDict([("500", 500.0),
                                                                        ("1000", 1000.0),
                                                                        ("2000", 2000.0),
                                                                        ("4000", 4000.0),
                                                                        ("8000", 8000.0),
                                                                        ]),
                                                   perm='rw',
                                                   name='Tile width',
                                                   unit='m',
                                                   info='Tile width in meter of quadratic tile. This is the real width of one tile that will be downloaded.',
                                                   ))

        self.size_tile = attrsman.add(cm.AttrConf('size_tile', kwargs.get('size_tile', 1280),
                                                  groupnames=['options'],
                                                  perm='r',
                                                  name='Tile size',
                                                  info='Tile size in pixel. This is the size of one tile that will be downloaded and determins the map resolution. Maximum is 1280.',
                                                  ))

        self.n_tiles = attrsman.add(cm.FuncConf('n_tiles', 'get_n_tiles', 0,
                                                groupnames=['options'],
                                                name='Number of tiles',
                                                #info = 'Delete a row.',
                                                ))

        # self.add_option(  'maptype',kwargs.get('maptype','satellite'),
        #                     choices = ['satellite',]
        #                     perm='rw',
        #                     name = 'Map type',
        #                     info = 'Type of map to be downloaded.',
        #                     )
        # self.add_option(  'filetype',kwargs.get('filetype','png'),
        #                     choices = ['png',]
        #                     perm='rw',
        #                     name = 'File type',
        #                     info = 'Image file format to be downloaded.',
        #                     )

        # self.add_option(  'mapserver',kwargs.get('mapserver','google'),
        #                     choices = ['google',]
        #                     perm='rw',
        #                     name = 'Map server',
        #                     info = 'Map server from where to download. Some servers require username and password.',
        #                     )

        # self.add_option(  'username',kwargs.get('username',''),
        #                     perm='rw',
        #                     name = 'User',
        #                      info = 'User name of map server (if required).',
        #                     )

        self.apikey = attrsman.add(cm.AttrConf('apikey', kwargs.get('apikey', ''),
                                               groupnames=['options'],
                                               perm='rw',
                                               name='API key',
                                               info='API key for google maps. API key can be obtained from Google at https://cloud.google.com/maps-platform/?refresh=1&pli=1#get-started.',
                                               ))

        # self.add_option(  'password',kwargs.get('password',''),
        #                     perm='rw',
        #                     name = 'User',
        #                     info = 'User name of map server (if required).',
        #                     )

        self.is_remove_orig = attrsman.add(cm.AttrConf('is_remove_orig', kwargs.get('is_remove_orig', True),
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Remove originals',
                                                       info='Remove original files. Original, untransformed files are not necessary, but can be kept.',
                                                       ))

    def get_n_tiles(self):
        """
        The number of tiles to be downloaded. Please do not download more han 300 tiles, otherwise map server is likely to be offended.
        """
        return self._maps.get_n_tiles(self.width_tile)

    def do(self):
        self.update_params()

        print 'MapsImporter.do'
        # self._maps.download(maptype = self.maptype, mapserver = self.mapserver,
        #            filetype = 'png', rootfilepath = None,
        #            width_tile = self.width_tile,  size_tile = self.size_tile,
        #            is_remove_orig = True):
        self._maps.download(maptype='satellite', mapserver='google',
                            filetype='png', rootfilepath=None, apikey=self.apikey,
                            width_tile=self.width_tile,  size_tile=self.size_tile,
                            is_remove_orig=self.is_remove_orig)
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        # self.run_cml(cml)
        # if self.status == 'success':
        return True

    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass


class Maps(am.ArrayObjman):
    def __init__(self, landuse, **kwargs):

        self._init_objman(ident='maps',
                          parent=landuse,
                          name='Maps',
                          info='Information on background maps.',
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):
        # print 'maps._init_attributes'

        # self.add(cm.AttrConf(  'width_tile',500,
        #                    groupnames = ['state'],
        #                    perm='r',
        #                    name = 'Tile width',
        #                    unit = 'm',
        #                    info = 'Tile width in meter of quadratic tile. This is the real wdith of one tile that will be downloaded.',
        #                    ))

        # self.add(cm.AttrConf(  'size_tile',1280,
        #                    groupnames = ['state'],
        #                    perm='r',
        #                    name = 'Tile size',
        #                    info = 'Tile size in pixel. This is the size of one tile that will be downloaded.',
        #                    ))

        if self.has_attrname('width_tile'):
            # no longer attributes
            self.delete('width_tile')
            self.delete('size_tile')
            # put r/w permissione to older version
            # self.get_config('width_tile').set_perm('rw')
            # self.get_config('size_tile').set_perm('rw')

        self.add_col(am.ArrayConf('bboxes',  np.zeros((2, 2), dtype=np.float32),
                                  groupnames=['state'],
                                  perm='r',
                                  name='BBox',
                                  unit='m',
                                  info='Bounding box of map in network coordinate system (lower left coord, upper right coord).',
                                  is_plugin=True,
                                  ))

        self.add_col(am.ArrayConf('filenames',  None,
                                  dtype=np.object,
                                  groupnames=['state'],
                                  perm='rw',
                                  metatype='filepath',
                                  name='File',
                                  info='Image file name.',
                                  ))

    def write_decals(self, fd, indent=4,  rootdir=None, delta=np.zeros(3, dtype=np.float32)):
        print 'write_decals', len(self)
        net = self.parent.get_net()
        if rootdir is None:
            rootdir = os.path.dirname(net.parent.get_rootfilepath())

        #proj = pyproj.Proj(str(net.get_projparams()))
        #offset = net.get_offset()
        #width_tile = self.width_tile.value
        #size_tile = self.size_tile.value

        for filename, bbox in zip(self.filenames.get_value(), self.bboxes.get_value()):
            #x0, y0 = proj(bbox_lonlat[0][0], bbox_lonlat[0][1])
            #x1, y1 = proj(bbox_lonlat[1][0],bbox_lonlat[1][1])
            #bbox = np.array([[x0, y0, 0.0],[x1, y1 ,0.0]],np.float32)

            #bbox_tile = [[x_sw,y_sw ],[x_ne,y_ne]]
            #x0,y0 = bbox_abs[0]+offset
            #x1,y1 = bbox_abs[1]+offset

            #bbox = np.array([[x0, y0, 0.0],[x1, y1 ,0.0]],np.float32)
            # print '  bbox decal',bbox
            xc, yc = 0.5*(bbox[0]+bbox[1])-delta[:2]
            zc = 0.0
            width_tile = bbox[1, 0] - bbox[0, 0]
            # print '  xc,yc',xc,yc
            # print '  width_tile',width_tile,bbox
            if filename == os.path.basename(filename):
                # filename does not contain path info
                filepath = filename  # os.path.join(rootdir,filename)
            else:
                # filename contains path info (can happen if interactively inserted)
                filepath = filename

            calxml = '<decal filename="%s" centerX="%.2f" centerY="%.2f" centerZ="0.00" width="%.2f" height="%.2f" altitude="0.00" rotation="0.00" tilt="0.00" roll="0.00" layer="0.00"/>\n' % (
                filepath, xc, yc, width_tile, width_tile)
            fd.write(indent*' '+calxml)

    def clear_all(self):
        """
        Remove all map information.
        """
        self.clear_rows()
        # here we could also delete files ??

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        bboxes = self.bboxes.get_value()
        bboxes[:, :, :2] = bboxes[:, :, :2] + deltaoffset

    def get_n_tiles(self, width_tile):
        """
        Estimates number of necessary tiles.
        """
        net = self.parent.get_net()
        bbox_sumo, bbox_lonlat = net.get_boundaries()
        x0 = bbox_sumo[0]  # -0.5*width_tile
        y0 = bbox_sumo[1]  # -0.5*width_tile
        width = bbox_sumo[2]-x0
        height = bbox_sumo[3]-y0
        nx = int(width/width_tile+0.5)
        ny = int(height/width_tile+0.5)
        return nx*ny

    def download(self, maptype='satellite', mapserver='google', apikey=None,
                 filetype='png', rootfilepath=None,
                 width_tile=1000.0,  size_tile=1280,
                 is_remove_orig=True):

        self.clear_rows()
        net = self.parent.get_net()
        if rootfilepath is None:
            rootfilepath = net.parent.get_rootfilepath()

        bbox_sumo, bbox_lonlat = net.get_boundaries()

        offset = net.get_offset()
        # latlon_sw=np.array([bbox_lonlat[1],bbox_lonlat[0]],np.float32)
        # latlon_ne=np.array([bbox_lonlat[3],bbox_lonlat[2]],np.float32)

        x0 = bbox_sumo[0]  # -0.5*width_tile
        y0 = bbox_sumo[1]  # -0.5*width_tile
        width = bbox_sumo[2]-x0
        height = bbox_sumo[3]-y0

        print 'download to', rootfilepath

        #            '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs'
        #params_proj="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
        params_proj = net.get_projparams()
        proj = pyproj.Proj(str(params_proj))
        # print '  params_proj',params_proj,IS_MAPSUPPORT
        # these values are measured manually and are only valid for  size_tile = 256/640
        # width_tile_eff= width_tile#*float(2*size_tile)/238.0#500m 1205.0*width_tile#m 1208
        # height_tile_eff = width_tile#*float(2*size_tile)/238.0# 500m1144.0*width_tile#m 1140

        nx = int(width/width_tile+0.5)
        ny = int(height/width_tile+0.5)
        print '  offset', offset
        print '  bbox_sumo', bbox_sumo
        print '  width_tile', width_tile, 'm'
        print '  Will download %dx%d= %d maps' % (nx, ny, nx*ny)
        #latlon_tile = np.array([(latlon_ne[0]-latlon_sw[0])/ny, (latlon_ne[1]-latlon_sw[1])/nx])
        #filepaths = []
        #centers = []
        #
        #
        #  0    1
        #  3    2
        #
        #          0        1         2          3

        angle = None
        bbox = None
        ids_map = []
        for ix in xrange(nx):
            for iy in xrange(ny):

                # tile in SUMO network coords. These are the saved coords
                x_tile = x0+ix*width_tile
                y_tile = y0+iy*width_tile
                print '  x_tile,y_tile', x_tile, y_tile
                bb = np.array([[x_tile, y_tile], [x_tile+width_tile, y_tile+width_tile]], np.float32)

                # tile in absolute coordinates. Coords used for download
                x_sw = x_tile-offset[0]
                y_sw = y_tile-offset[1]

                x_ne = x_sw+width_tile
                y_ne = y_sw+width_tile
                bbox_tile = [[x_sw, y_sw], [x_ne, y_ne]]

                filepath = rootfilepath+'_map%04dx%04d.%s' % (ix, iy, filetype)
                # print '  filepath=',filepath

                if angle is None:
                    download_googlemap_bb(filepath, bbox_tile, proj, apikey,
                                          size=size_tile,
                                          filetype=filetype, maptype=maptype)
                    if os.path.getsize(filepath) > 2000:  # download OK
                        angle, bbox = estimate_angle(filepath)
                        # sys.exit(0)
                    else:
                        print 'WARNING in download: no file downloaded from mapserver'
                        return ids_map

                bbox_tile_lonlat = download_googlemap_bb(filepath, bbox_tile, proj, apikey,
                                                         size=size_tile, filetype=filetype,
                                                         maptype=maptype, color="0x0000000f")

                if os.path.getsize(filepath) < 2000:  # download failed
                    print 'WARNING in download: no file downloaded from mapserver'
                    return ids_map

                print '  bbox_tile', bbox_tile
                print '  bbox_tile_lonlat', bbox_tile_lonlat

                im = Image.open(filepath).convert("RGB")
                if 1:
                    print '    downloaded image', filepath,  "%dx%d" % im.size, im.mode, im.getbands()
                    print '      x_sw,y_sw', x_sw, y_sw
                    print '      x_ne,y_ne', x_ne, y_ne

                # print '  start rotation'
                im_rot = im.rotate(angle)  # gimp 1.62
                # im_rot.show()
                region = im_rot.crop([bbox[0][0], bbox[0][1], bbox[2][0], bbox[2][1]])
                regsize = region.size
                print '      regsize', regsize
                im_crop = Image.new('RGB', (regsize[0], regsize[1]), (0, 0, 0))
                im_crop.paste(region, (0, 0, regsize[0], regsize[1]))
                im_tile = im_crop.resize((1024, 1024))
                # im_crop.show()
                outfilepath = rootfilepath+'_rot%04dx%04d.%s' % (ix, iy, filetype)

                # print 'save ',outfilepath,"%dx%d" % im_crop.size,im_crop.getbands()
                im_tile.save(outfilepath, filetype.upper())

                # print '  bb_orig=',bb
                print '      bb_orig', bb
                #lon0, lat0 = proj(x_tile-offset[0], y_tile-offset[1])
                #lon1, lat1 = proj(x_tile+width_tile-offset[0], y_tile+width_tile-offset[1])

                # print '  bb',bb.shape,bb
                # print '  outfilepath',outfilepath,os.path.basename(outfilepath)
                # print '  saved bbox',np.array([[x_tile-offset[0], y_tile-offset[1]],[x_tile+width_tile-offset[0], y_tile+width_tile-offset[1]]],np.float32)
                # print '  saved bbox',bbox_tile_lonlat
                id_map = self.add_row(filenames=os.path.basename(outfilepath),
                                      # bbox_tile,#bbox_tile_lonlat#np.array([[lon0, lat0],[lon1, lat1]],np.float32),
                                      bboxes=bb,
                                      )
                ids_map.append(id_map)

                if is_remove_orig:
                    # remove original file
                    os.remove(filepath)

        return ids_map


class CsvElevationsImport(PlotoptionsMixin, CmlMixin, Process):
    def __init__(self, landuse, rootname=None, rootdirpath=None, netfilepath=None,
                 is_clean_nodes=False, logger=None, **kwargs):

        self._init_common('csvelevationsimporter', name='Elevations csv import',
                          logger=logger,
                          info='Imports the elevations .csv file with 3 colums (Longitudes, Latitudes, Elevations), starting from the first lane',
                          )
        self._landuse = landuse

        self.init_cml('netconvert')

        attrsman = self.get_attrsman()
        self.add_option('elevationscsv_filepath', '/home/cristian/scenarios_cri/Elevation/bo_elevations2.csv',
                        groupnames=['options'],  # this will make it show up in the dialog
                        cml='--csv-elevation-file',
                        perm='rw',
                        name='Elevations file',
                        wildcards='Elevations CSV file(*.csv)|*.csv*',
                        metatype='filepath',
                        info='Elevations CSV file in',
                        )

        self.interpolation_radius = attrsman.add(cm.AttrConf('interpolation_radius', kwargs.get('interpolation_radius', 150.),
                                                             groupnames=['options'],
                                                             name='Interpolation radius',
                                                             info='In order to find the quote of an unknown ponit, will be interpolated the elevation points around him within this radius.\
                                        If it count zero, the point wil have the quote equal to the nearest elevation point. ',
                                                             ))

# self.method_interp = attrsman.add(cm.AttrConf(  'method_interp', kwargs.get('method_interp', 'cubic'),
##                                        groupnames = ['options'],
##                                        choices = ['linear', 'cubic', 'quintic'],
##                                        name = 'Interpolation method',
##                                        info = 'Elevation interpolation method.',
# ))

        self.x_offset = attrsman.add(cm.AttrConf('x_offset', kwargs.get('x_offset', -679244.),
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Longitude offset',
                                                 unit='m',
                                                 info='Longitude Offset for the importation phase.',
                                                 ))

        self.y_offset = attrsman.add(cm.AttrConf('y_offset', kwargs.get('y_offset', -4924610.),
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Latitude offset',
                                                 unit='m',
                                                 info='Latitude Offset for the importation phase.',
                                                 ))

        self.is_plot_elevations = attrsman.add(cm.AttrConf('is_plot_elevations', kwargs.get('is_plot_elevations', False),
                                                           groupnames=['options'],
                                                           perm='rw',
                                                           name='Plot Elevations',
                                                           info='Plot point elevations.',
                                                           ))

        # Match elevations

        self.is_match_to_nodes = attrsman.add(cm.AttrConf('is_match_to_nodes', kwargs.get('is_match_to_nodes', True),
                                                          groupnames=['options'],
                                                          perm='rw',
                                                          name='Match to node',
                                                          info='Match the elevations to the network nodes.',
                                                          ))

# self.is_match_to_nodes_shape = attrsman.add(cm.AttrConf( 'is_match_to_nodes_shape',kwargs.get('is_match_to_nodes_shape',True),
##                            groupnames = ['options'],
# perm='rw',
##                            name = 'Match to node shapes',
##                            info = 'Match the elevations to the network nodes shapes.',
# ))

# self.is_match_to_edge = attrsman.add(cm.AttrConf( 'is_match_to_node',kwargs.get('is_match_to_node',True),
##                            groupnames = ['options'],
# perm='rw',
##                            name = 'Match to node',
##                            info = 'Matche the elevations to the network nodes.',
# ))

        self.is_match_to_edges = attrsman.add(cm.AttrConf('is_match_to_edges', kwargs.get('is_match_to_edges', True),
                                                          groupnames=['options'],
                                                          perm='rw',
                                                          name='Match to edge ',
                                                          info='Match the elevations to the network edges.',
                                                          ))

        self.is_match_to_zones = attrsman.add(cm.AttrConf('is_match_to_zones', kwargs.get('is_match_to_zones', True),
                                                          groupnames=['options'],
                                                          perm='rw',
                                                          name='Match to zones',
                                                          info='Match the elevations to the assignment zones.',
                                                          ))

# self.is_match_to_zones_shape = attrsman.add(cm.AttrConf( 'is_match_to_zones_shape',kwargs.get('is_match_to_zones_shape',True),
##                            groupnames = ['options'],
# perm='rw',
##                            name = 'Match to node shapes',
##                            info = 'Match the elevations to the network nodes shapes.',
# ))

        self.is_match_to_facilities = attrsman.add(cm.AttrConf('is_match_to_facilities', kwargs.get('is_match_to_facilities', True),
                                                               groupnames=['options'],
                                                               perm='rw',
                                                               name='Match to facilities',
                                                               info='Match the elevations to the facilities.',
                                                               ))

# self.is_match_to_facilities_shape = attrsman.add(cm.AttrConf( 'is_match_to_facilities_shape',kwargs.get('is_match_to_facilities_shape',True),
##                            groupnames = ['options'],
# perm='rw',
##                            name = 'Match to facility shapes',
##                            info = 'Match the elevations to the facility shapes.',
# ))

        self.add_plotoptions(**kwargs)
        self.add_save_options(**kwargs)
        attrsman.delete('plottype')
        attrsman.delete('length_arrowhead')

    def do(self):

        net = self._landuse.parent.net

        with open(self.elevationscsv_filepath, 'r') as csvFile:
            reader = csv.reader(csvFile)
            i = 1
            longitudes = []
            latitudes = []
            elevations = []
            ids_point = []
            for row in reader:

                longitudes.append(float(row[0]))
                latitudes.append(float(row[1]))
                elevations.append(float(row[2]))
                ids_point.append(i)
                i += 1

            ids_point = np.array(ids_point)
            longitudes = np.array(longitudes) + self.x_offset
            latitudes = np.array(latitudes) + self.y_offset
            elevations = np.array(elevations)

            csvFile.close()
            #################################################################################
            quote = self.evaluate_quote(longitudes, latitudes, elevations, 2000., 10000.)
            quote = self.evaluate_quote(longitudes, latitudes, elevations, 4000., 7000.)
            quote = self.evaluate_quote(longitudes, latitudes, elevations, 7000., 11000.)
            quote = self.evaluate_quote(longitudes, latitudes, elevations, 10000., 3000.)
            ###################################################

        if self.is_match_to_nodes:
            nodes = net.nodes
            ids_node = nodes.get_ids()
            coords = nodes.coords[ids_node]
            for coord, id_node in zip(coords, ids_node):

                quote = self.evaluate_quote(longitudes, latitudes, elevations, coord[0], coord[1])

                nodes.coords[id_node] = [coord[0], coord[1], quote]
# print 'node_coord', nodes.coords[id_node]

            shapes = nodes.shapes[ids_node]
            for shape, id_node in zip(shapes, ids_node):
                for coord, i in zip(shape, range(len(shape))):
                    # print coord
                    # print np.stack((longitudes, latitudes), axis = -1)
                    quote = self.evaluate_quote(longitudes, latitudes, elevations, coord[0], coord[1])
                    nodes.shapes[id_node][i] = [coord[0], coord[1], quote]
# print 'node_shape', nodes.shapes[id_node]

# coord[0]
# coord[1]

        if self.is_match_to_edges:
            edges = net.edges
            ids_edge = edges.get_ids()
            shapes = edges.shapes[ids_edge]
            for shape, id_edge in zip(shapes, ids_edge):
                positive_climb = 0.
                negative_climb = 0.
                for coord, i in zip(shape, range(len(shape))):
                    # print coord
                    # print np.stack((longitudes, latitudes), axis = -1)
                    quote = self.evaluate_quote(longitudes, latitudes, elevations, coord[0], coord[1])

                    edges.shapes[id_edge][i] = [coord[0], coord[1], quote]
                    print edges.shapes[id_edge][i]
                    if i > 0 and (quote-quote_pre) > 0:
                        positive_climb += (quote-quote_pre)
                    elif i > 0 and (quote-quote_pre) < 0:
                        negative_climb += (-quote + quote_pre)
                    quote_pre = quote
# print 'edge_shape', edges.shapes[id_edge][i]
                slope = (edges.shapes[id_edge][-1][2]-edges.shapes[id_edge][0][2])/edges.lengths[id_edge]
                edges.average_slopes[id_edge] = slope
                edges.positive_climbs[id_edge] = positive_climb
                edges.negative_climbs[id_edge] = negative_climb
# print 'slope', slope

# if self.is_match_to_zones:
##
# if self.is_match_to_facilities:

        #######################
        if self.is_plot_elevations:
            fig = plt.figure()
            ax = fig.add_subplot(111)
##            import matplotlib
##            import matplotlib.pyplot as plt
##            cm = matplotlib.cm.get_cmap('RdYlBu')
# plt.subplot(111)
##            plt.scatter(longitudes, latitudes, c=elevations, s=20, vmax = 70)
# plt.colorbar()
# plt.show()
            plot_point_results_on_map(self, net, ax, longitudes, latitudes, values=elevations,
                                      title='Elevations', valuelabel='[m]',
                                      )
            plt.show()
        ############################################

        return True

    def evaluate_quote(self, longitudes, latitudes, elevations, x_point, y_point):

        dists = np.sqrt(np.sum((np.stack((longitudes, latitudes), axis=-1) - [x_point, y_point])**2, 1))

        if is_scipy:
            print 'use scipy to interpolate'
            #tck = interpolate.splrep(x, y, s=0)
            #xnew = np.linspace(np.min(x), np.max(x), 200)
            #ynew = interpolate.splev(xnew, tck, der=0)
            # if 1:

            nearest_longitudes = longitudes[(dists < self.interpolation_radius)]
            nearest_latitudes = latitudes[(dists < self.interpolation_radius)]
            nearest_elevations = elevations[(dists < self.interpolation_radius)]
##            nearest_longitudes = longitudes[(longitudes < x_point + self.interpolation_radius)&(longitudes > x_point - self.interpolation_radius)&(latitudes < y_point + self.interpolation_radius)&(latitudes > y_point - self.interpolation_radius)]
##            nearest_latitudes = latitudes[(longitudes < x_point + self.interpolation_radius)&(longitudes > x_point - self.interpolation_radius)&(latitudes < y_point + self.interpolation_radius)&(latitudes > y_point - self.interpolation_radius)]
##            nearest_elevations = elevations[(longitudes < x_point + self.interpolation_radius)&(longitudes > x_point - self.interpolation_radius)&(latitudes < y_point + self.interpolation_radius)&(latitudes > y_point - self.interpolation_radius)]
            print[x_point, y_point], nearest_longitudes, nearest_latitudes, nearest_elevations
            if len(nearest_longitudes) > 15:

                f_inter = interpolate.SmoothBivariateSpline(
                    nearest_longitudes, nearest_latitudes, nearest_elevations,)  # kind = self.method_interp )
##    #############################################
##                xnew = np.linspace(x_point-self.interpolation_radius/2, x_point+self.interpolation_radius/2,200)
##                ynew = np.linspace(y_point-self.interpolation_radius/2, y_point+self.interpolation_radius/2,200)
##                X, Y = np.meshgrid(xnew, ynew)
##                Z = f_inter(xnew, ynew)
##
##
##                fig = plt.figure()
##                ax = fig.gca(projection='3d')
##
##
# Plot the surface.
# surf = ax.plot_surface(X, Y, Z, cmap=cmp.coolwarm,
# linewidth=0, antialiased=False)
##
##                fig.colorbar(surf, shrink=0.5, aspect=5)
# plt.savefig('/home/cristian/scenarios_cri/Elevation/interpolation_%d'%(xnew[0]))
# plt.show()
    #############################################
                quote = f_inter(x_point, y_point)
            else:
                quote = elevations[np.argmin(dists)]
                print 'nearest quote'
        else:

            nearest_quotes = elevations[(dists < 100)]
            nearest_dists = dists[(dists < 100)]
            numerator = 0.0
            denominator = 0.0
            for near_quote, near_dist in zip(nearest_quotes, nearest_dists):
                numerator += near_quote/(10**(near_dist/10))
                denominator += 1/(10**(near_dist/10))
    # print numerator, denominator
            if denominator != 0:
                quote = numerator/denominator
            else:
                quote = elevations[np.argmin(dists)]
                print 'nearest quote'

        return quote


if __name__ == '__main__':
    ############################################################################
    ###
    pass
