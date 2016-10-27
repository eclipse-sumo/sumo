import os
import numpy as np
import wx
import urllib
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
#import timeit
#t = timeit.Timer()
#t_start = t.timer()

#from matplotlib import pyplot as plt

IS_MAPSUPPORT = True
try:
    from PIL import ImageFilter,Image,ImageChops,ImagePath,ImageDraw
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
        #print __doc__
        #raise


URL_GOOGLEMAP = "http://maps.googleapis.com/maps/api/staticmap?" 

def download_googlemap(filepath,bbox,proj,size = 640,filetype = 'gif',maptype = 'satellite'):
    # https://developers.google.com/maps/documentation/static-maps/intro#Paths
    x_sw, y_sw = bbox[0]
    x_ne, y_ne = bbox[1]
    
    # 01    11
    #
    #
    # 00    10
    lon00, lat00 = proj(x_sw, y_sw, inverse = True)
    lon10, lat10 = proj(x_ne, y_sw, inverse = True)
    lon11, lat11 = proj(x_ne, y_ne, inverse = True)    
    lon01, lat01 = proj(x_sw, y_ne, inverse = True) 
    
    size_x = size_y = size/2
    urllib.urlretrieve (URL_GOOGLEMAP+"size=%dx%d&visible=%.6f,%.6f|%.6f,%.6f&format=%s&maptype=%s&scale=2"\
        %(size_x,size_y,lat00,lon00, lat11,lon11,filetype.upper(),maptype), filepath)
    
def download_googlemap_bb(filepath,bbox,proj,size = 640,filetype = 'gif',maptype = 'satellite', color = "0xff0000ff"):
    # https://developers.google.com/maps/documentation/static-maps/intro#Paths
    x_sw, y_sw = bbox[0]
    x_ne, y_ne = bbox[1]
    
    # 01    11
    #
    #
    # 00    10
    lon00, lat00 = proj(x_sw, y_sw, inverse = True)
    lon10, lat10 = proj(x_ne, y_sw, inverse = True)
    lon11, lat11 = proj(x_ne, y_ne, inverse = True)    
    lon01, lat01 = proj(x_sw, y_ne, inverse = True) 
    
    size_x = size_y = size/2
    
    url = URL_GOOGLEMAP+"size=%dx%d&format=%s&maptype=%s&scale=2&path=color:%s|weight:1"%(size_x,size_y,filetype.upper(),maptype,color)
    url += "|%.6f,%.6f"%(lat00,lon00)
    url += "|%.6f,%.6f"%(lat10,lon10)
    url += "|%.6f,%.6f"%(lat11,lon11)
    url += "|%.6f,%.6f"%(lat01,lon01)
    url += "|%.6f,%.6f"%(lat00,lon00)
    
    #urllib.urlretrieve (URL_GOOGLEMAP+"size=%dx%d&format=%s&maptype=%s&scale=2&path=color:0xff0000ff|weight:1|%.5f,%.5f|%.5f,%.5f|%.5f,%.5f|%.5f,%.5f"\
    #    %(size_x,size_y,filetype,maptype,lat00,lon00, lat11,lon11, lat01,lon01, lat10,lon10), filepath)
    #print 'url=',url
    urllib.urlretrieve (url,filepath)


def estimate_angle( filepath,
                    rect = [(72,36),(1243,69),(1210,1244),(39,1211)],
                   ):
    
    im = Image.open(filepath).convert("RGB")
    print 'estimate_angle image',filepath,  "%dx%d" % im.size, im.mode, im.getbands() 
    imr, img, imb = im.split()
    
    # calculate width and height of bbox in pixel from measured rectangle             
    wr = int(np.sqrt((rect[1][0]-rect[0][0])**2+(rect[1][1]-rect[0][1])**2))
    #wr_check = int(np.sqrt((rect[2][0]-rect[3][0])**2+(rect[2][1]-rect[3][1])**2))
    hr = int(np.sqrt((rect[3][0]-rect[0][0])**2+(rect[3][1]-rect[0][1])**2))
    #h_check = int(np.sqrt((rect[2][0]-rect[1][0])**2+(rect[2][1]-rect[1][1])**2))
    
    xcb = im.size[0]/2
    ycb = im.size[1]/2
    
    bbox = [(xcb-wr/2,ycb-hr/2), (xcb+wr/2,ycb-hr/2), (xcb+wr/2,ycb+hr/2), (xcb-wr/2,ycb+hr/2),(xcb-wr/2,ycb-hr/2)]
    im_bbox = ImageChops.constant(im, 0)
    draw = ImageDraw.Draw(im_bbox)
    draw.line(bbox,fill=255)
    del draw
    
    angles = np.arange(-2.0,2.0,0.01)
    matches =  np.zeros(len(angles))
    for i in xrange(len(angles)):
        im_bbox_rot =im_bbox.rotate(angles[i])# gimp 1.62
        im_corr= ImageChops.multiply(imr,im_bbox_rot)
        #im_corr.show()
        im_corr_arr = np.asarray(im_corr)
        matches[i] = np.sum(im_corr_arr)/255
        #print ' angles[i],matches[i]',angles[i],matches[i]
    
    angle_opt = angles[np.argmax(matches)]
    print '  angle_opt',angle_opt
    return -angle_opt, bbox
    #im_box.show()


class MapsImporter(Process):
    def __init__(self,  maps, logger = None, **kwargs):
        print 'MapsImporter.__init__',maps,maps.parent.get_ident()
        self._init_common(  'mapsimporter', name = 'Background maps importer', 
                            logger = logger,
                            info ='Downloads and converts background maps.',
                            )
        self._maps = maps
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        #self.net = attrsman.add(   cm.ObjConf( network.Network(self) ) )
        #self.status = attrsman.add(cm.AttrConf( 
        #                            'status', 'preparation',
        #                            groupnames = ['_private','parameters'], 
        #                            perm='r', 
        #                            name = 'Status', 
        #                            info = 'Process status: preparation-> running -> success|error.' 
        #                            ))
                                    
        self.width_tile = attrsman.add(cm.AttrConf( 'width_tile',kwargs.get('width_tile',500.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Tile width', 
                            unit = 'm',
                            info = 'Tile width in meter of quadratic tile. This is the real width of one tile that will be downloaded.',
                            ))
    
    
        self.size_tile = attrsman.add(cm.AttrConf(  'size_tile',kwargs.get('size_tile',1280),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Tile size', 
                            info = 'Tile size in pixel. This is the size of one tile that will be downloaded. Maximum is 1280.',
                            ))
                            
                            
        #self.add_option(  'maptype',kwargs.get('maptype','satellite'),
        #                     choices = ['satellite',]
        #                     perm='rw', 
        #                     name = 'Map type', 
        #                     info = 'Type of map to be downloaded.',
        #                     ) 
        #self.add_option(  'filetype',kwargs.get('filetype','png'),
        #                     choices = ['png',]
        #                     perm='rw', 
        #                     name = 'File type', 
        #                     info = 'Image file format to be downloaded.',
        #                     ) 
                             
        #self.add_option(  'mapserver',kwargs.get('mapserver','google'),
        #                     choices = ['google',]
        #                     perm='rw', 
        #                     name = 'Map server', 
        #                     info = 'Map server from where to download. Some servers require username and password.',
        #                     ) 
       
        #self.add_option(  'username',kwargs.get('username',''),
        #                     perm='rw', 
        #                     name = 'User', 
        #                      info = 'User name of map server (if required).',
        #                     )
                            
        #self.add_option(  'password',kwargs.get('password',''),
        #                     perm='rw', 
        #                     name = 'User', 
        #                     info = 'User name of map server (if required).',
        #                     )

        self.is_remove_orig = attrsman.add(cm.AttrConf( 'is_remove_orig',kwargs.get('is_remove_orig',True),
                         groupnames = ['options'], 
                         perm='rw', 
                         name = 'Remove originals', 
                         info = 'Remove original files. Original, untransformed files are not necessary, but can be kept.',
                         ))
    
    def do(self):
        self.update_params()
        
        print 'MapsImporter.do'
        #self._maps.download(maptype = self.maptype, mapserver = self.mapserver, 
        #            filetype = 'png', rootfilepath = None,
        #            width_tile = self.width_tile,  size_tile = self.size_tile,
        #            is_remove_orig = True):
        self._maps.download(maptype = 'satellite', mapserver = 'google', 
                    filetype = 'png', rootfilepath = None,
                    width_tile = self.width_tile,  size_tile = self.size_tile,
                    is_remove_orig = self.is_remove_orig)
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        #self.run_cml(cml)
        #if self.status == 'success':
            
    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass
    
class Maps(am.ArrayObjman):
    def __init__(self, landuse, **kwargs):
        
        self._init_objman(  ident='maps', 
                            parent=landuse, 
                            name = 'Maps', 
                            info = 'Information on background maps.', 
                            **kwargs)
        self._init_attributes()              
    
    def _init_attributes(self):   
        #print 'maps._init_attributes'
        
        #self.add(cm.AttrConf(  'width_tile',500,
        #                    groupnames = ['state'], 
        #                    perm='r', 
        #                    name = 'Tile width', 
        #                    unit = 'm',
        #                    info = 'Tile width in meter of quadratic tile. This is the real wdith of one tile that will be downloaded.',
        #                    ))
        
        #self.add(cm.AttrConf(  'size_tile',1280,
        #                    groupnames = ['state'], 
        #                    perm='r', 
        #                    name = 'Tile size', 
        #                    info = 'Tile size in pixel. This is the size of one tile that will be downloaded.',
        #                    ))
        
        if  self.has_attrname('width_tile'):
            # no longer attributes
            self.delete('width_tile')
            self.delete('size_tile')  
            # put r/w permissione to older version 
            #self.get_config('width_tile').set_perm('rw')
            #self.get_config('size_tile').set_perm('rw')                                         
        
        self.add_col(am.ArrayConf( 'bboxes',  np.zeros((2,3), dtype=np.float32),
                                        groupnames = ['state'],
                                        perm='r', 
                                        name = 'BBox',
                                        unit = 'm',
                                        info = 'Bounding box of map in network coordinate system (lower left coord, upper right coord).',
                                        is_plugin = True,
                                        ))
                                        
        self.add_col(am.ArrayConf( 'filenames',  None, 
                                        dtype=np.object,
                                        groupnames = ['state'],
                                        perm='rw', 
                                        metatype = 'filepath',
                                        name = 'File',
                                        info = 'Image file name.',
                                        ))
                                        
    
    def write_decals(self, fd, indent = 4,  rootdir = None):
        #print 'write_decals'
        net = self.parent.get_net()
        if rootdir ==None:
            rootdir = os.path.dirname(net.parent.get_rootfilepath())
            
        #width_tile = self.width_tile.value 
        #size_tile = self.size_tile.value 
        for filename, bbox in zip(self.filenames.get_value(),self.bboxes.get_value()):
            xc,yc,zc = 0.5*(bbox[0]+bbox[1])
            
            width_tile = bbox[1,0] - bbox[0,0]
            #print '  width_tile',width_tile,bbox
            if filename == os.path.basename(filename):
                # filename does not contain path info
                filepath = filename#os.path.join(rootdir,filename)
            else:
                # filename contains path info (can happen if interactively inserted)
                filepath = filename
                
            calxml = '<decal filename="%s" centerX="%.2f" centerY="%.2f" centerZ="0.00" width="%.2f" height="%.2f" altitude="0.00" rotation="0.00" tilt="0.00" roll="0.00" layer="0.00"/>\n'%(filepath,xc,yc,width_tile,width_tile)
            fd.write(indent*' '+calxml)
        
    def clear_all(self):
        """
        Remove all map information.
        """
        self.clear_rows()
            
    def download(self, maptype = 'satellite', mapserver = 'google', 
                    filetype = 'png', rootfilepath = None,
                    width_tile = 1000,  size_tile = 1280,
                    is_remove_orig = True):
        
        
        
        
        self.clear_rows()
        net = self.parent.get_net()
        if rootfilepath == None:
            rootfilepath = net.parent.get_rootfilepath()
            
        bbox_sumo,bbox_lonlat =  net.get_boundaries()
        
        
        offset = net.get_offset()
        latlon_sw=np.array([bbox_lonlat[1],bbox_lonlat[0]],np.float32)
        latlon_ne=np.array([bbox_lonlat[3],bbox_lonlat[2]],np.float32)
        
        params_proj = net.get_projparams()
        
        x0 = bbox_sumo[0]
        y0 = bbox_sumo[1]
        width  = bbox_sumo[2]-x0
        height =  bbox_sumo[3]-y0
        
        
        
        print 'download to',rootfilepath
        print '  params_proj',params_proj,IS_MAPSUPPORT
        #            '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs'
        #params_proj="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
        
        proj = pyproj.Proj(str(params_proj))
        
        # these values are measured manually and are only valid for  size_tile = 256/640
        width_tile_eff= width_tile#*float(2*size_tile)/238.0#500m 1205.0*width_tile#m 1208 
        height_tile_eff = width_tile#*float(2*size_tile)/238.0# 500m1144.0*width_tile#m 1140
        
        nx = int(width/width_tile+0.5)
        ny = int(height/width_tile+0.5)
        print '  Will download %dx%d= %d maps'%(nx,ny,nx*ny)
        latlon_tile = np.array([(latlon_ne[0]-latlon_sw[0])/ny, (latlon_ne[1]-latlon_sw[1])/nx])
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
                
                x_tile = x0+ix*width_tile
                y_tile = y0+iy*width_tile
                xc = x_tile+0.5*width_tile
                yc = y_tile+0.5*width_tile
                
                #traces.pointsets.get(id_trace).project( traces._proj, traces.offset)
                #x,y = proj(self.cols.lon, self.cols.lat)
                #self.cols.coords = np.transpose(np.concatenate(([x],[y]),axis=0))+offset
                x_sw = x_tile-offset[0]
                y_sw = y_tile-offset[1]
                
                x_ne = x_sw+width_tile
                y_ne = y_sw+width_tile
                
                bbox_tile = [[x_sw,y_sw ],[x_ne,y_ne]]
                
                filepath = rootfilepath+'_map%04dx%04d.%s'%(ix,iy,filetype)
                #print '  filepath=',filepath
                #filepaths.append(filepath)
                #centers.append((xc,yc))
                
                if angle == None:
                    download_googlemap_bb(filepath, bbox_tile, proj, 
                                            size=size_tile,
                                            filetype = filetype,maptype=maptype)
                    
                    angle, bbox = estimate_angle(filepath)
                
                else:
                    download_googlemap_bb(filepath, bbox_tile, proj, 
                                            size=size_tile,filetype = filetype,
                                            maptype=maptype, color = "0x0000000f")
                    
                im = Image.open(filepath).convert("RGB")
                if 1:
                    print '    downloaded image',filepath,  "%dx%d" % im.size, im.mode, im.getbands() 
                    #print '    x_sw,y_sw',x_sw,y_sw
                    #print '    x_ne,y_ne',x_ne,y_ne
                
                print '  start rotation'
                im_rot =im.rotate(angle)# gimp 1.62
                #im_rot.show()
                region = im_rot.crop([bbox[0][0],bbox[0][1], bbox[2][0], bbox[2][1] ])
                regsize = region.size
                #print ' regsize',regsize
                im_crop=Image.new('RGB',(regsize[0],regsize[1]),(0,0,0))
                im_crop.paste(region,(0,0,regsize[0],regsize[1]))
                im_tile = im_crop.resize((1024,1024))
                #im_crop.show()
                outfilepath = rootfilepath+'_rot%04dx%04d.%s'%(ix,iy,filetype)
                
                #print 'save ',outfilepath,"%dx%d" % im_crop.size,im_crop.getbands() 
                im_tile.save(outfilepath,filetype.upper())
                bb = np.array([[x_tile,y_tile, 0.0],[x_tile+width_tile, y_tile+width_tile,0.0]],np.float32)
                #print '  bb',bb.shape,bb
                print '  outfilepath',outfilepath,os.path.basename(outfilepath)
                id_map = self.add_row(  filenames = os.path.basename(outfilepath), 
                                        bboxes = bb)
                ids_map.append(id_map)
                
                # remove original file
                os.remove(filepath) 
        
        return ids_map
        
        
                                                
    
if __name__ == '__main__':          
    ############################################################################
    ###
    rootfilepath = "/home/joerg/projects/sumopy/scenarios/test_map/sara_map"
    
    filetype = 'png'
    
    # origBoundary="11.311343,44.485335,11.348625,44.498801"
    bbox_lonlat = np.array([11.311343,44.485335,11.348625,44.498801],np.float32)
    bbox_orig = np.array([0.0,0.0,2979.82,1447.34],np.float32)
    offset = np.array([-683778.0,-4928460.0],np.float32)
    latlon_sw=np.array([bbox_lonlat[1],bbox_lonlat[0]],np.float32)
    latlon_ne=np.array([bbox_lonlat[3],bbox_lonlat[2]],np.float32)
    
    params_proj="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    width_tile = 500.0#m 500m
    size_tile = 1280#2*256#2*640
    
    
    maptype = 'satellite'
    #roadmap (default) specifies a standard roadmap image, as is normally shown on the Google Maps website. If no maptype value is specified, the Google Static Maps API serves roadmap tiles by default.
    #satellite specifies a satellite image.
    #terrain specifies a physical relief map image, showing terrain and vegetation.
    #hybrid specifies a hybrid of the satellite and roadmap image, showing a transparent layer of major streets and place names on the satellite image.

    ############
    x0 = bbox_orig[0]
    y0 = bbox_orig[1]
    width  = bbox_orig[2]
    height =  bbox_orig[3]
    
    
    proj = pyproj.Proj(params_proj)
    
    
    # these values are measured manually and are only valid for  size_tile = 256/640
    width_tile_eff= width_tile#*float(2*size_tile)/238.0#500m 1205.0*width_tile#m 1208 
    height_tile_eff = width_tile#*float(2*size_tile)/238.0# 500m1144.0*width_tile#m 1140
    
    nx = int(width/width_tile+0.5)
    ny = int(height/width_tile+0.5)
    #print 'Will download %dx%d= %d maps'%(nx,ny,nx*ny)
    latlon_tile = np.array([(latlon_ne[0]-latlon_sw[0])/ny, (latlon_ne[1]-latlon_sw[1])/nx])
    filepaths = []
    centers = []
    #
    #
    #  0    1
    #  3    2
    #
    #          0        1         2          3
    
    
    angle = None   
    bbox = None
    for ix in xrange(nx):
        for iy in xrange(ny):
            
            x_tile = x0+ix*width_tile
            y_tile = y0+iy*width_tile
            xc = x_tile+0.5*width_tile
            yc = y_tile+0.5*width_tile
            
            #traces.pointsets.get(id_trace).project( traces._proj, traces.offset)
            #x,y = proj(self.cols.lon, self.cols.lat)
            #self.cols.coords = np.transpose(np.concatenate(([x],[y]),axis=0))+offset
            x_sw = x_tile-offset[0]
            y_sw = y_tile-offset[1]
            
            x_ne = x_sw+width_tile
            y_ne = y_sw+width_tile
            
            bbox_tile = [[x_sw,y_sw ],[x_ne,y_ne]]
            
            filepath = rootfilepath+'_%04dx%04d.%s'%(ix,iy,filetype)
            filepaths.append(filepath)
            centers.append((xc,yc))
            
            if angle == None:
                download_googlemap_bb(filepath, bbox_tile, proj, size=size_tile,filetype = filetype,maptype=maptype)
                angle, bbox = estimate_angle(filepath)
            else:
                #download_googlemap_bb(filepath, bbox_tile, proj, size=size_tile,filetype = filetype,maptype=maptype, color = "0x0000000f")
                pass
            
            
            
            
            #print 'start rotation'
            im = Image.open(filepath).convert("RGB")
            if 0:
                print '  downloaded image',filepath,  "%dx%d" % im.size, im.mode, im.getbands() 
                print '  x_sw,y_sw',x_sw,y_sw
                print '  x_ne,y_ne',x_ne,y_ne
                
            im_rot =im.rotate(angle)# gimp 1.62
            #im_rot.show()
            region = im_rot.crop([bbox[0][0],bbox[0][1], bbox[2][0], bbox[2][1] ])
            regsize = region.size
            #print ' regsize',regsize
            im_crop=Image.new('RGB',(regsize[0],regsize[1]),(0,0,0))
            im_crop.paste(region,(0,0,regsize[0],regsize[1]))
            #im_crop.show()
            outfilepath = rootfilepath+'_rot_%04dx%04d.%s'%(ix,iy,filetype)
            
            #print 'save ',outfilepath,"%dx%d" % im_crop.size,im_crop.getbands() 
            im_crop.save(outfilepath,"PNG")
            print '<decal filename="%s" centerX="%.2f" centerY="%.2f" centerZ="0.00" width="%.2f" height="%.2f" altitude="0.00" rotation="0.00" tilt="0.00" roll="0.00" layer="0.00"/>'%(outfilepath,xc,yc,width_tile_eff,height_tile_eff)
            
            

            
            
            
            
            
        