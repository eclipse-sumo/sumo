
import os
import sys
if __name__ == '__main__':
    try:
        FILEDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        FILEDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    sys.path.append(os.path.join(FILEDIR, "..", ".."))

# this is default scenario directory
# os.path.join(os.getcwd(),'testscenario')
DIRPATH_SCENARIO = os.path.join(os.path.expanduser("~"), 'Sumo')


import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
from agilepy.lib_base.processes import Process

from coremodules.network import network
from coremodules.landuse import landuse
from coremodules.demand import demand


def load_scenario(filepath, logger=None):
    scenario = cm.load_obj(filepath, parent=None)
    # scenario.set_workdirpath(os.path.dirname(filepath))
    # this will set rootname and workdir
    scenario.set_filepath(filepath)
    if logger != None:
        scenario.set_logger(logger)
    return scenario


class ScenarioCreator(Process):

    def __init__(self, rootname='myscenario', name_scenario='My Scenario', workdirpath=None, description='', logger=None):

        # init process
        self._init_common('scenariocreator',
                          name='New Scenario', logger=logger)

        if workdirpath == None:
            #workdirpath = os.getcwd()
            workdirpath = os.path.expanduser("~")
        attrsman = self.get_attrsman()
        self.rootname = attrsman.add(cm.AttrConf('rootname', rootname,
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Shortname',
                                                 info='Short name for scenario. This string is used as rootname for all files produced by this scenario. Please avoid special charracters, whitespace, accents etc. ASCII is recommented in order to remain compatible between operating systems.',
                                                 ))

        self.name_scenario = attrsman.add(cm.AttrConf('name_scenario', name_scenario,
                                                      groupnames=['options'],
                                                      perm='rw',
                                                      name='Name',
                                                      info='Scenario name, used for documentation purposes only.',
                                                      ))

        self.description = attrsman.add(cm.AttrConf('description', description,
                                                    groupnames=['options'],
                                                    perm='rw',
                                                    name='Description',
                                                    info='Short, free description of Scenario.',
                                                    ))

        self.workdirpath = attrsman.add(cm.AttrConf('workdirpath', workdirpath,
                                                    groupnames=['options'],
                                                    perm='rw',
                                                    name='Workdir',
                                                    metatype='dirpath',
                                                    info='Working directory for this scenario.',
                                                    ))

    def do(self):
        # print 'do',self.newident
        self._scenario = Scenario(self.rootname,
                                  name_scenario=self.name_scenario,
                                  description=self.description,
                                  parent=None,
                                  workdirpath=self.workdirpath,
                                  logger=self.get_logger(),
                                  )

    def get_scenario(self):
        return self._scenario


class Scenario(cm.BaseObjman):

    def __init__(self, rootname, name_scenario='myscenario',
                 description='', parent=None,
                 workdirpath=None, **kwargs):

        self._init_objman(ident='scenario', parent=parent,
                          name='Scenario', info='Main scenario instance.',
                          **kwargs)

        attrsman = self.set_attrsman(cm.Attrsman(self))

        if workdirpath != None:
            # create a directory if path is given, but does not exist
            if not os.path.isdir(workdirpath):
                os.mkdir(workdirpath)
        else:
            workdirpath = os.getcwd()
            #workdirpath = os.path.expanduser("~")

        self.name_scenario = attrsman.add(cm.AttrConf('name_scenario', name_scenario,
                                                      groupnames=['options'],
                                                      perm='rw',
                                                      name='Name',
                                                      info='Scenario name, used for documentation purposes only.',
                                                      ))

        self.description = attrsman.add(cm.AttrConf('description', description,
                                                    groupnames=['options'],
                                                    perm='rw',
                                                    name='Description',
                                                    info='Short, free description of Scenario.',
                                                    ))

        self.rootname = attrsman.add(cm.AttrConf('rootname', rootname,
                                                 groupnames=['options'],
                                                 perm='r',
                                                 is_save=True,
                                                 name='Shortname',
                                                 info='Short name for scenario. This string is defined when saving the scenario. It is used as rootname for all files produced by this scenario. Please avoid special charracters, whitespace, accents etc. ASCII is recommented in order to remain compatible between operating systems.',
                                                 ))

        self.workdirpath = attrsman.add(cm.AttrConf('workdirpath', workdirpath,
                                                    groupnames=['options'],
                                                    perm='r',
                                                    is_save=True,
                                                    name='Workdir',
                                                    metatype='dirpath',
                                                    info='Working directory for this scenario and can be changed when saving the scenario. Please avoid special charracters, whitespace, accents etc. ASCII is recommented in order to remain compatible between operating systems.',
                                                    ))

        self._init_attributes()

    def _init_attributes(self):
        attrsman = self.get_attrsman()

        self.net = attrsman.add(cm.ObjConf(network.Network(self)))

        self.landuse = attrsman.add(
            cm.ObjConf(landuse.Landuse(self, self.net)))

        self.demand = attrsman.add(cm.ObjConf(demand.Demand(self)))

    def set_filepath(self, filepath):
        """
        A new filepath will set the shortname and workdir.
        """
        names = os.path.basename(filepath).split('.')
        dirname = os.path.dirname(filepath)

        if len(names) >= 3:
            rootname = '.'.join(names[:-2])
        elif len(names) <= 2:
            rootname = names[0]

        self.set_workdirpath(dirname)
        self.set_rootfilename(rootname)

    def save(self, filepath=None):
        if filepath == None:
            filepath = self.get_rootfilepath() + '.obj'
        self.set_filepath(filepath)
        cm.save_obj(self, filepath, is_not_save_parent=False)

    def get_workdirpath(self):
        return self.workdirpath

    def set_workdirpath(self, workdirpath):
        self.workdirpath = workdirpath

    def set_rootfilename(self, filename):
        self.rootname = filename

    def get_rootfilename(self):
        """
        Centralized definition of filename bases.
        """
        return self.rootname

    def get_rootfilepath(self):
        return os.path.join(self.get_workdirpath(), self.get_rootfilename())

    def import_xml(self, is_clean_nodes=False):
        """
        Try to import xml-type files into scenario.
        """

        # print 'import_xml'
        netfilepath = self.net.get_filepath()

        if os.path.isfile(netfilepath):
            # convert and import edg,nod,con,tll...
            self.net.import_netxml(filepath=netfilepath,
                                   rootname=self.get_rootfilename())
        else:
            # import edg,nod,con,tll...
            self.net.import_xml(self.get_rootfilename(
            ), self.get_workdirpath(),  is_clean_nodes=is_clean_nodes)

        self.landuse.import_polyxml(
            self.get_rootfilename(), self.get_workdirpath())
        try:
            self.demand.import_xml(
                self.get_rootfilename(), self.get_workdirpath())
        except:
            print 'WARNING: import of demand data failed. Please check for inconsistency with trip/route and network edge IDs.'

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        self.landuse.update_netoffset(deltaoffset)
        self.demand.update_netoffset(deltaoffset)


if __name__ == '__main__':
    ##########################################################################
    # print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    if len(sys.argv) == 3:
        rootname = sys.argv[1]
        dirpath = sys.argv[2]
    else:
        rootname = 'facsp2'
        dirpath = os.path.join(os.path.dirname(
            __file__), '..', 'network', 'testnet')

    scenario = Scenario(rootname, workdirpath=dirpath, logger=Logger())

    # net.import_nodes(os.path.join('test','facsp2.nod.xml'))
    # net.import_edges(os.path.join('test','facsp2.edg.xml'))
    objbrowser(scenario)
