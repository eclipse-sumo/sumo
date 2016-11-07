
import numpy as np
from collections import OrderedDict
import matplotlib as mpl
from matplotlib.patches import Arrow, Circle, Wedge, Polygon, FancyArrow
from matplotlib.collections import PatchCollection
import matplotlib.pyplot as plt

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process


def init_plot():
    plt.close("all")
    fig = plt.figure()
    ax = fig.add_subplot(111)
    return ax


def plot_net(ax, net, edgecolor="gray", edgewidth=2, color_node=None,
             alpha=0.5):
    for shape in net.edges.shapes.get_value():
        x_vec = np.array(shape)[:, 0]
        y_vec = np.array(shape)[:, 1]
        ax.plot(x_vec, y_vec, color=edgecolor, lw=edgewidth, alpha=alpha)

    # do nodes
    if color_node is None:
        is_nodefill = False
        color_node = 'none'
    else:
        is_nodefill = True

    #ax.scatter(x_vec[0], y_vec[0], s=np.pi * (10.0)**2, c=colors, alpha=0.5)
    coords = net.nodes.coords.get_value()
    radii = net.nodes.radii.get_value()
    #ax.scatter(coords[:,0], coords[:,1], s=np.pi * (radii)**2, alpha=0.5)
    #patches = []
    for coord, radius in zip(coords, radii):
        ax.add_patch(Circle(coord, radius=radius,
                            linewidth=edgewidth,
                            edgecolor=edgecolor,
                            facecolor=color_node,
                            fill=is_nodefill,
                            alpha=alpha))


def plot_edgevalues(ax,  ids_result, config_ids_edge, config_values, width_max=10.0):
    edges = config_ids_edge.get_linktab()
    ids_edges = config_ids_edge[ids_result]
    values = config_values[ids_result]
    values_norm = np.array(values, dtype=np.float32) / np.max(values)
    colors = mpl.cm.cool_r(values_norm, 1)
    for shape, val_norm, val, color, _id in zip(edges.shapes[ids_edges], values_norm, values, colors, ids_result):
        x_vec = np.array(shape)[:, 0]
        y_vec = np.array(shape)[:, 1]
        #ax.text(x_vec[-1]+0.5*width_max,y_vec[-1]+0.5*width_max, config_values.format_value(_id) )
        ax.text(x_vec[-1] + 0.5 * width_max, y_vec[-1] +
                0.5 * width_max, "%.1f" % config_values[_id])
        ax.plot(x_vec, y_vec, color=color, linewidth=width_max * val_norm)
    # plt.colorbar() # no


def plot_edgevalues_patches(ax, ids_result, config_ids_edge, config_values,
                            width_max=10.0, alpha=0.8, printformat='%.2f',
                            color_outline=None, color_fill=None, is_antialiased=True,
                            is_fill=True, is_widthvalue=True,
                            arrowshape='left', length_arrowhead=10.0,
                            headwidthstretch=1.3):

    head_width = headwidthstretch * width_max

    edges = config_ids_edge.get_linktab()
    ids_edges = config_ids_edge[ids_result]
    values = config_values[ids_result]
    values_norm = np.array(values, dtype=np.float32) / np.max(values)

    patches = []
    displacement = float(width_max) / 4.0
    if is_widthvalue:
        linewidths = width_max * values_norm
    else:
        linewidths = width_max * np.ones(len(values_norm), np.float32)

    deltaangle_text = -np.pi / 2.0
    displacement_text = displacement + width_max
    for id_edge, value, value_norm, linewidth in zip(ids_edges, values, values_norm, linewidths):
        shape, angles_perb = get_resultshape(edges, id_edge, displacement)
        x_vec = np.array(shape)[:, 0]
        y_vec = np.array(shape)[:, 1]
        deltax = x_vec[-1] - x_vec[0]
        deltay = y_vec[-1] - y_vec[0]

        if printformat is not '':
            angles_text = np.arctan2(deltay, deltax)

            x_label = x_vec[0] + 0.5 * deltax + displacement_text * \
                np.cos(angles_text + deltaangle_text)
            y_label = y_vec[0] + 0.5 * deltay + displacement_text * \
                np.sin(angles_text + deltaangle_text)

            ax.text(x_label, y_label, printformat %
                    value, rotation=angles_text / (np.pi) * 180)

        if is_widthvalue:
            head_width = headwidthstretch * linewidth
        arrow = FancyArrow(x_vec[0], y_vec[0], deltax, deltay, width=linewidth,
                           antialiased=is_antialiased,
                           edgecolor=color_outline, facecolor=color_fill,
                           head_width=head_width, head_length=length_arrowhead,
                           length_includes_head=True,
                           fill=True, shape=arrowshape)
        patches.append(arrow)

    if is_fill:
        alpha_patch = alpha
        patchcollection = PatchCollection(
            patches, cmap=mpl.cm.jet, alpha=alpha_patch)
        patchcollection.set_array(values)
        ax.add_collection(patchcollection)
        plt.colorbar(patchcollection)
    else:
        for patch in patches:
            ax.add_patch(patch)


def get_resultshape(edges, id_edge, dispacement):
    """
    Return resultshape coords for this edge.
    """
    shape = np.array(edges.shapes[id_edge], np.float32)
    n_vert = len(shape)
    resultshape = np.zeros(shape.shape, np.float32)
    #laneshapes = np.zeros((n_lanes,n_vert,3), np.float32)
    angles_perb = get_angles_perpendicular(shape)
    if edges.types_spread[id_edge] == 0:  # right spread

        resultshape[:, 0] = shape[:, 0] + np.cos(angles_perb) * dispacement
        resultshape[:, 1] = shape[:, 1] + np.sin(angles_perb) * dispacement
        return resultshape, angles_perb
    else:
        return shape.copy(), angles_perb


def show_plot():
    plt.show()


class Resultplotter(Process):

    def __init__(self, results, name='Plot results with Matplotlib',
                 info="Creates plots of different results using matplotlib",
                 logger=None):

        self._init_common('resultplotter', parent=results, name=name,
                          info=info, logger=logger)

        print 'Resultplotter.__init__', results, self.parent
        attrsman = self.get_attrsman()

        # edgeresultes....
        attrnames_edgeresults = {}
        edgeresultattrconfigs = self.parent.edgeresults.get_group_attrs(
            'results')
        edgeresultattrnames = edgeresultattrconfigs.keys()
        edgeresultattrnames.sort()
        for attrname in edgeresultattrnames:
            attrconfig = edgeresultattrconfigs[attrname]

            attrnames_edgeresults[
                attrconfig.format_symbol()] = attrconfig.attrname

        #attrnames_edgeresults = {'Entered':'entered'}
        self.edgeattrname = attrsman.add(cm.AttrConf('edgeattrname', 'entered',
                                                     choices=attrnames_edgeresults,
                                                     groupnames=['options'],
                                                     name='Edge Quantity',
                                                     info='The edge related quantity to be plotted.',
                                                     ))

        self.is_show_network = attrsman.add(cm.AttrConf('is_show_network', False,
                                                        groupnames=['options'],
                                                        name='Show network',
                                                        info='Shows a schematic network in the background.',
                                                        ))

        self.is_show_title = attrsman.add(cm.AttrConf('is_show_title', True,
                                                      groupnames=['options'],
                                                      name='Show tile',
                                                      info='Shows title and unit.',
                                                      ))

        self.resultwidth = attrsman.add(cm.AttrConf('resultwidth', 10.0,
                                                    groupnames=['options'],
                                                    name='Result width',
                                                    unit='m',
                                                    info='Maximum width of graphical resuls on map.',
                                                    ))

        self.length_arrowhead = attrsman.add(cm.AttrConf('length_arrowhead', 10.0,
                                                         groupnames=[
                                                             'options'],
                                                         name='Arrow length',
                                                         unit='m',
                                                         info='Length of arrowhead on result map.',
                                                         ))

        self.is_widthvalue = attrsman.add(cm.AttrConf('is_widthvalue', False,
                                                      groupnames=['options'],
                                                      name='Value width?',
                                                      info='If True, the arrow width of the graphical representation is proportional to the result value.',
                                                      ))

        self.is_colorvalue = attrsman.add(cm.AttrConf('is_colorvalue', True,
                                                      groupnames=['options'],
                                                      name='Value color?',
                                                      info='If True, the arrows of the graphical representation are filled with a colour representing the result value.',
                                                      ))

        self.color_outline = attrsman.add(cm.AttrConf('color_outline', np.array([0.0, 0.0, 0.0, 0.95], dtype=np.float32),
                                                      groupnames=['options'],
                                                      perm='wr',
                                                      metatype='color',
                                                      name='Outline color',
                                                      info='Outline color of result arrows in graphical representation. Only valid if no color-fill is chosen.',
                                                      ))

        self.color_fill = attrsman.add(cm.AttrConf('color_fill', np.array([0.3, 0.3, 1.0, 0.95], dtype=np.float32),
                                                   groupnames=['options'],
                                                   perm='wr',
                                                   metatype='color',
                                                   name='Fill color',
                                                   info='Fill color of result arrows in graphical representation. Only valid if no color-fill is chosen.',
                                                   ))

        self.alpha_results = attrsman.add(cm.AttrConf('alpha_results', 0.8,
                                                      groupnames=['options'],
                                                      name='Result transparency',
                                                      info='Transparency of result arrows in graphical representation.',
                                                      ))

        self.printformat = attrsman.add(cm.AttrConf('printformat', '%.1f',
                                                    choices=OrderedDict([
                                                        ('Show no values', ''),
                                                        ('x', '%.d'),
                                                        ('x.x', '%.1f'),
                                                        ('x.xx', '%.2f'),
                                                        ('x.xxx', '%.3f'),
                                                        ('x.xxxx', '%.4f'),
                                                    ]),
                                                    groupnames=['options'],
                                                    name='Value formatting',
                                                    info='Value formatting of displayed text in graphical representation.',
                                                    ))

        self.is_grid = attrsman.add(cm.AttrConf('is_grid', False,
                                                groupnames=['options'],
                                                name='Show grid?',
                                                info='If True, shows a grid on the graphical representation.',
                                                ))

        self.axis = None

    def show(self):
        # print 'show',self.edgeattrname
        # if self.axis == None:
        axis = init_plot()
        # else:
        net = self.parent.get_scenario().net
        if self.is_show_network:
            plot_net(axis, net)

        if (self.edgeattrname is not ""):
            resultattrconf = getattr(
                self.parent.edgeresults, self.edgeattrname)
            ids = self.parent.edgeresults.get_ids()
            if len(ids > 0):
                title = resultattrconf.get_info(
                ) + resultattrconf.format_unit(show_parentesis=True)  # format_symbol()
                #plot_edgevalues(axis, ids, self.parent.edgeresults.ids_edge, resultattrconf)
                plot_edgevalues_patches(axis, ids,
                                        self.parent.edgeresults.ids_edge,
                                        resultattrconf,
                                        width_max=self.resultwidth,
                                        alpha=self.alpha_results,
                                        printformat=self.printformat,
                                        color_outline=self.color_outline,
                                        color_fill=self.color_fill,
                                        is_antialiased=True,
                                        is_fill=self.is_colorvalue,
                                        is_widthvalue=self.is_widthvalue,
                                        length_arrowhead=self.length_arrowhead,
                                        )

                axis.set_title(title)

        axis.axis('equal')
        # ax.legend(loc='best',shadow=True)

        axis.grid(self.is_grid)
        axis.set_xlabel('West-East [m]')
        axis.set_ylabel('South-North [m]')
        show_plot()

    def do(self):
        # print 'do',self.edgeattrname
        self.show()

    def get_scenario(self):
        return self._scenario
