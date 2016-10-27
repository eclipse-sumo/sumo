# chart.py
# wx widget for showing a chart of objects
#   (depends: python-opengl, pyftgl)

# disable OpenGL error checking
import OpenGL
OpenGL.ERROR_CHECKING = False

import FTGL
import math
import numpy as np
from OpenGL.arrays.vbo import *
from OpenGL.GL import *
import sys
import time
import wx
from wx import glcanvas

import circle
import ngcic
import planets


class Chart (glcanvas.GLCanvas):

    def __init__ (self, parent, fov_ctrl, converter, planets):
        glcanvas.GLCanvas.__init__(self, parent, -1)
        self.context = glcanvas.GLContext(self)
        self.converter = converter
        self.planets = planets

        # fetch list of all NGC/IC objects and their positions
        self.ngcic = ngcic.pos_list("data/ngcic.csv")
        self.ngcic_rad = []
        for obj in self.ngcic:
            self.ngcic_rad.append([obj[0],
                [math.radians(obj[1][0]), math.radians(obj[1][1])]])

        # load font
        self.font = FTGL.BitmapFont("fonts/DejaVuSans.ttf")
        self.font.FaceSize(12)

        # field of view indicator widget
        self.fov_ctrl = fov_ctrl

        # drawing settings
        self.path = [] # list of points [crd_a, crd_b]
        self.given_equ = False # whether the path points are equatorial
        self.scan_center = [0, 0] # center of scan
        self.curpos_h = [0, 0] # current pointing direction in horiz coordinates
        self.h_fov = 100.0 # horizontal field of view
        self.show_equ = False # whether to show in equatorial coordinates
        self.cen_curscan = False # center the current scan
                                 #   (otherwise, center current position)

        self.adj_center = [0, 0] # center of screen in display coordinates
        self.equ_center = [0, 0] # center of screen in equatorial coordinates

        # event handlers
        self.Bind(wx.EVT_SIZE, self.on_resize)
        self.Bind(wx.EVT_PAINT, self.on_paint)
        self.Bind(wx.EVT_MOUSEWHEEL, self.scroll_fov)

        self.initialized = False


    # initialize OpenGL
    def gl_init (self):
        self.initialized = True
        self.resize(self.width, self.height)

        glDisable(GL_DEPTH_TEST) # using 2D drawing, so no depth
        glClearColor(0, 0, 0, 1) # black background

        # enable antialiasing on lines and points
        glEnable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glEnable(GL_LINE_SMOOTH)
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)

        glEnable(GL_POINT_SMOOTH)
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)

        # enable drawing from vertex buffer objects
        glEnableClientState(GL_VERTEX_ARRAY)

    # event handlers
    def on_resize (self, event):
        self.SetCurrent(self.context)
        size = self.GetClientSize()
        self.resize(size.width, size.height)

        self.Refresh()
        event.Skip()

    def on_paint (self, event):
        wx.PaintDC(self)
        self.SetCurrent(self.context)
        self.draw()
        self.SwapBuffers()

        event.Skip()

    # scrolling to change field of view directly on the sky chart
    def scroll_fov (self, event):
        if self.h_fov < 20 or \
                int(self.h_fov) == 20 and event.GetWheelRotation() > 0:
            self.h_fov += -1 * float(event.GetWheelRotation()) / 120

        elif self.h_fov < 60 or \
                int(self.h_fov) == 60 and event.GetWheelRotation() > 0:
            self.h_fov += -2 * float(event.GetWheelRotation()) / 120

        elif self.h_fov < 100 or \
                int(self.h_fov) == 100 and event.GetWheelRotation() > 0:
            self.h_fov += -4 * float(event.GetWheelRotation()) / 120

        elif self.h_fov < 180 or \
                int(self.h_fov) == 180 and event.GetWheelRotation() > 0:
            self.h_fov += -8 * float(event.GetWheelRotation()) / 120

        else:
            self.h_fov += -20 * float(event.GetWheelRotation()) / 120

        # constrain to range [1, 340]
        if self.h_fov > 340:
            self.h_fov = 340.0
        elif self.h_fov < 1:
            self.h_fov = 1.0

        # show new value on field of view indicator
        self.fov_ctrl.SetValue(self.h_fov)

        self.Refresh()
        event.Skip()


    # resize: update 2D OpenGL display with new size
    #   width, height: new chart size (pixels)
    def resize (self, width, height):
        self.width = width
        self.height = height
        glViewport(0, 0, width, height)

        # set up view so top-left is (0, 0)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        glOrtho(0, width, height, 0, 0, 1)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()


    # project: convert from sky coordinates to screen coordinates
    #          using equirectangular projection
    #   sky_coord -> [crd_a, crd_b]: position of object in the sky
    #   sky_center -> [crd_a, crd_b]: position to be centered on
    def project (self, sky_coord, sky_center):

        # find displacement of sky coordinate from center of screen
        displace = [(sky_coord[0] - sky_center[0]) % 360.0,
                     sky_coord[1] - sky_center[1]]

        # transform [0, 360) -> (-180, 180]
        if displace[0] > 180:
            displace[0] -= 360

        # compute pixels per degree from horizontal field of view
        pix_per_deg = self.width / self.h_fov

        # convert displacement from center in sky coordinates into displacement
        # in screen coordinates and compute final screen position
        if self.show_equ: # -> right ascension increases to the left
            return 0.5 * self.width  - displace[0] * pix_per_deg, \
                   0.5 * self.height - displace[1] * pix_per_deg
        else: # show horizontal system -> azimuth increases to the right
            return 0.5 * self.width  + displace[0] * pix_per_deg, \
                   0.5 * self.height - displace[1] * pix_per_deg

    # project_point: convert to display sky coordinates, then project
    #
    #   point -> [crd_a, crd_b]: position in given coordinate system
    def project_point (self, point):

        # convert to proper coordinate system
        if self.given_equ and not self.show_equ: # equatorial -> horizontal
            az, el = self.converter.radec_to_azel(
                math.radians(point[0]), math.radians(point[1]))
            sky_coord = [math.degrees(az), math.degrees(el)]

        elif not self.given_equ and self.show_equ: # horizontal -> equatorial
            ra, de = self.converter.azel_to_radec(
                math.radians(point[0]), math.radians(point[1]))
            sky_coord = [math.degrees(ra), math.degrees(de)]

        else: # already in correct coordinates
            sky_coord = point[:]

        return self.project (sky_coord, self.adj_center)


    # center_display: get center of screen in display sky coordinates
    def center_display (self):

        # center on current position
        if not self.cen_curscan:
            # convert to equatorial
            cur_ra, cur_de = \
                self.converter.azel_to_radec(
                    math.radians(self.curpos_h[0]),
                    math.radians(self.curpos_h[1]))
            self.equ_center = [math.degrees(cur_ra), math.degrees(cur_de)]


            if self.show_equ:
                self.adj_center = self.equ_center[:]
            else: # use original, in horizontal coordinates
                self.adj_center = [self.curpos_h[0], self.curpos_h[1]]

        # find scan center in proper coordinate system
        elif self.given_equ and not self.show_equ: # equatorial -> horizontal
            self.equ_center = self.scan_center[:]
            cen_az, cen_el = \
                self.converter.radec_to_azel(
                    math.radians(self.scan_center[0]),
                    math.radians(self.scan_center[1]))
            self.adj_center = [math.degrees(cen_az), math.degrees(cen_el)]

        elif not self.given_equ and self.show_equ: # horizontal -> equatorial
            cen_ra, cen_de = \
                self.converter.azel_to_radec(
                    math.radians(self.scan_center[0]),
                    math.radians(self.scan_center[1]))
            self.equ_center = [math.degrees(cen_ra), math.degrees(cen_de)]
            self.adj_center = self.equ_center[:]

        else: # already in correct coordinates
            self.adj_center = self.scan_center[:]

            if not self.show_equ: # compute equatorial center
                cen_ra, cen_de = \
                    self.converter.azel_to_radec(
                        math.radians(self.scan_center[0]),
                        math.radians(self.scan_center[1]))
                self.equ_center = [math.degrees(cen_ra), math.degrees(cen_de)]


    # draw: draw all objects onto the screen
    def draw (self):
        if not self.initialized:
            self.gl_init()

        glClear(GL_COLOR_BUFFER_BIT) # clear previous drawing

        # compute position of the center of screen
        self.center_display()

        ##
        # draw NGC/IC objects
        ##

        if self.h_fov <= 10.0: # only show when zoomed in
            glLineWidth(2)
            level = min((10.0 - self.h_fov) * 0.2, 1.0)
            glColor(0.8 * level, 0.9 * level, level)

            max_dist = math.radians(math.sqrt(
                self.h_fov**2 * (1.0 + (self.height/self.width)**2)))
            equ_center_rad = [math.radians(self.equ_center[0]),
                              math.radians(self.equ_center[1])]

            # create list of NGC/IC objects to draw
            ngcic_list = []
            ngcic_num = 0 # number of objects being drawn
            for obj in self.ngcic_rad:
                # ignore objects nowhere near the field of view
                if circle.distance_rad(equ_center_rad, obj[1]) > max_dist:
                     continue

                if self.show_equ:
                    # -> [ra, de]
                    pos = [math.degrees(obj[1][0]), math.degrees(obj[1][1])]
                else: # convert to horizontal if display is in horizontal
                    az, el = self.converter.radec_to_azel(
                        obj[1][0], obj[1][1])
                    pos = [math.degrees(az), math.degrees(el)]

                # get screen position
                point = self.project(pos, self.adj_center)

                if 0 < point[0] <= self.width and \
                   0 < point[1] <= self.height:

                    # add square diamond shape
                    ngcic_list.extend(
                        (point[0], point[1] - 5,
                         point[0] - 5, point[1],
                         point[0], point[1] + 5,
                         point[0] + 5, point[1])
                    )
                    ngcic_num += 1

                    # draw label
                    glRasterPos(point[0] + 10, point[1] + 4)
                    self.font.Render(obj[0]) # obj[0] -> object name

                ngcic_vbo = VBO(np.array(ngcic_list, dtype=np.float32))
                ngcic_vbo.bind()
                glVertexPointer(2, GL_FLOAT, 0, ngcic_vbo)
                for i in range(0, ngcic_num):
                    glDrawArrays(GL_LINE_LOOP, 4 * i, 4)

        ##
        # draw solar system objects
        ##

        # compute positions
        if not hasattr(self, "sso_list"):
            self.sso_list = {}
        pos_func = self.show_equ and self.planets.equ_pos \
                                  or self.planets.hor_pos
        for obj_name in planets.objects:
            self.sso_list[obj_name] = pos_func(self.planets.get_obj(obj_name))

        # draw as points with labels on the right
        glColor(0.8, 0.7, 0.5) # orange tint
        glPointSize(5)

        for name, pos in self.sso_list.items():
            point = self.project(pos, self.adj_center)

            if 0 < point[0] <= self.width and 0 < point[1] <= self.height:
                # draw point
                pt = np.array(point, dtype=np.float32)
                pt_vbo = VBO(pt)
                pt_vbo.bind()
                glVertexPointer(2, GL_FLOAT, 0, pt_vbo)
                glDrawArrays(GL_POINTS, 0, 1)

                # draw label
                glRasterPos(point[0] + 10, point[1] + 4)
                self.font.Render(name)

        ##
        # draw grid
        ##

        glLineWidth(2.5)          # width of 2.5px
        glColor(0.5, 0.5, 0.5)    # gray
        glLineStipple(2, 0xAAAA)  # dashed lines
        glEnable(GL_LINE_STIPPLE)

        # mark frequency
        if self.h_fov >= 150:
            mark = 30
        elif self.h_fov >= 50:
            mark = 10
        elif self.h_fov >= 25:
            mark = 5
        elif self.h_fov >= 10:
            mark = 2
        else: # self.h_fov < 10
            mark = 1

        # vertical lines
        for azi in range(0, 360/mark):
            point = self.project([azi * mark, 0], self.adj_center)
            if -1 < point[0] < self.width - 30:

                # draw line
                line = np.array([
                    point[0], 0,
                    point[0], self.height - self.font.line_height
                ], dtype=np.float32)
                line_vbo = VBO(line)
                line_vbo.bind()
                glVertexPointer(2, GL_FLOAT, 0, line_vbo)
                glDrawArrays(GL_LINES, 0, 2)

                # draw label
                text = str(azi * mark)
                glRasterPos(point[0] - 0.5 * self.font.Advance(text),
                            self.height - 5)
                self.font.Render(text)

        # horizontal lines
        for alt in range(-90/mark, 90/mark + 1):
            point = self.project([0, alt * mark], self.adj_center)
            if -1 < point[1] < self.height - self.font.line_height:

                # draw line
                line = np.array([
                    0, point[1],
                    self.width - 30, point[1]
                ], dtype=np.float32)
                line_vbo = VBO(line)
                line_vbo.bind()
                glVertexPointer(2, GL_FLOAT, 0, line_vbo)
                glDrawArrays(GL_LINES, 0, 2)

                # draw label
                text = str(alt * mark)
                glRasterPos(self.width - self.font.Advance(text) - 5,
                            point[1] + 3)
                self.font.Render(text)

        glDisable(GL_LINE_STIPPLE)

        ##
        # draw the path
        ##

        glLineWidth(3)         # width of 3px
        glColor(0.8, 0.8, 0.8) # light gray
        line = []

        def break_line ():
            line_vbo = VBO(np.array(line, dtype=np.float32))
            line_vbo.bind()
            glVertexPointer(2, GL_FLOAT, 0, line_vbo)
            glDrawArrays(GL_LINE_STRIP, 0, len(line)/2)
            line[:] = []

        prev_pt = False # previous actual point specified in path
        prev_x, prev_y = 0, 0 # previous point including intermediate points
        for next_pt in self.path:

            # find horizontal coordinates of next point
            if self.given_equ:
                az, el = self.converter.radec_to_azel(
                    math.radians(next_pt[0]), math.radians(next_pt[1]))
                next_hor = [math.degrees(az), math.degrees(el)]

            else: # already in horizontal coordinates
                next_hor = next_pt

            # intermediate points
            if prev_pt:

                x, y = self.project_point(prev_pt)

                # check whether we need to break the list for wrap-around
                if x < 0 and prev_x > self.width or \
                   x > self.width and prev_x < 0 or \
                   y < 0 and prev_y > self.height or \
                   y > self.height and prev_y < 0:
                    break_line()
                else:
                    line.append(x)
                    line.append(y)

                prev_x, prev_y = x, y

                # change in azimuth and altitude to next point
                d_az = (next_hor[0] - prev_hor[0]) % 360.0
                d_el = next_hor[1] - prev_hor[1]
                d_tot = d_az + d_el

                # adjust for wrap-around
                if d_az > 180.0:
                    d_az -= 360.0

                # generate list of intermediate points to slew to
                # (linear on each horizontal axis)
                num_int = int(d_tot) # one intermediate point per degree
                                     # on each axis

                for i in range(1, num_int + 1):
                    int_pt_hor = [prev_hor[0] + d_az * i / num_int,
                                  prev_hor[1] + d_el * i / num_int]

                    if self.given_equ: # convert to equatorial
                        ra, de = self.converter.azel_to_radec(
                            math.radians(int_pt_hor[0]),
                            math.radians(int_pt_hor[1]))
                        int_pt = [math.degrees(ra), math.degrees(de)]

                    else: # keep horizontal
                        int_pt = int_pt_hor

                    x, y = self.project_point(int_pt)

                    # check whether we need to break the list for wrap-around
                    if x < 0 and prev_x > self.width or \
                       x > self.width and prev_x < 0 or \
                       y < 0 and prev_y > self.height or \
                       y > self.height and prev_y < 0:
                        break_line()
                    else:
                        line.append(x)
                        line.append(y)

                    prev_x, prev_y = x, y

            prev_pt = next_pt
            prev_hor = next_hor

        # add in last point
        if prev_pt:
            x, y = self.project_point(prev_pt)
            if not (x < 0 and prev_x > self.width or \
                    x > self.width and prev_x < 0 or \
                    y < 0 and prev_y > self.height or \
                    y > self.height and prev_y < 0):
                line.append(x)
                line.append(y)
        break_line()

        ##
        # show a cross-hair at the current position
        ##

        glColor(1, 1, 1)
        glLineWidth(4)

        # convert current position to be same as path coordinates
        if self.given_equ:
            cur_ra, cur_de = \
                self.converter.azel_to_radec(
                    math.radians(self.curpos_h[0]), math.radians(self.curpos_h[1]))
            curpos = [math.degrees(cur_ra), math.degrees(cur_de)]
        else: # already in horizontal coordinates
            curpos = self.curpos_h[:]

        x, y = self.project_point(curpos)

        # draw cross hair
        crosshair = np.array([
            # top
            x, y - 10,
            x, y - 3,
            # left
            x - 10, y,
            x - 3, y,
            # bottom
            x, y + 10,
            x, y + 3,
            # right
            x + 10, y,
            x + 3, y], dtype=np.float32)
        crosshair_vbo = VBO(crosshair)
        crosshair_vbo.bind()
        glVertexPointer(2, GL_FLOAT, 0, crosshair_vbo)
        glDrawArrays(GL_LINES, 0, 8)

        glFlush()
