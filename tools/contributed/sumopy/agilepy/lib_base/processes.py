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

# @file    processes.py
# @author  Joerg Schweizer
# @date    2012


import subprocess
import os
import sys
import types
import pickle


#SUMOPYDIR = os.path.join(os.path.dirname(__file__),"..")
#TOOLSDIR = os.path.join(SUMOPYDIR,"..")

#APPDIR =  os.path.join(os.path.dirname(__file__),"..")


import classman as cm
from misc import filepathlist_to_filepathstring, filepathstring_to_filepathlist, ff, P


# p = psutil.Process(the_pid_you_want) !!
# print p.status
# if p.status == psutil.STATUS_ZOMBIE:

##processlist = psutil.get_process_list()
##
# for p in processlist:
# if psutil.pid_exists(p.pid):
# print '  pid = ',p.pid
# print '    name =',p.name
# print '    running =',p.is_running()
# print '    path =',p.path
# print '    cmdline',p.cmdline
##
##
##cmd = 'nohup sumo --no-step-log --no-duration-log --no-warnings --step-length %.3f -c %s'%(time_simstep, filepath_netconfig)
##
# print '\n Starting command:',cmd
##p = subprocess.Popen(cmd, shell=True)
# print "p.pid",p.pid, "p.poll=",p.poll()
# p.wait()
# print '\n\nreturncode',p.poll(),p.returncode
# print 'p.stdout=\n', p.stdout
# call(cmd)

# in windows : start command3


def call(cmd):
    """
    Calls a sytem commend lime. Returns 1 if executed successfully.
    """
    # ensure unix compatibility
    print(cmd)
    # if isinstance(cmd, str):
    #    cmd = filter(lambda a: a!='', cmd.split(' '))
    # subprocess.call(cmd)
    p = subprocess.Popen(cmd, shell=True)
    p.wait()
    return p.returncode == 0


class Process(cm.BaseObjman):
    def __init__(self, ident,  **kwargs):
        self._init_common(ident, **kwargs)

    def _init_common(self,  ident, parent=None, name=None, **kwargs):
        self._init_objman(ident=ident, parent=parent, name=name, **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))
        self.optiongroupnames = ['options']
        #self.net = attrsman.add(   cm.ObjConf( network.Network(self) ) )
        self.status = attrsman.add(cm.AttrConf(
            'status', 'preparation',
            groupnames=['_private', 'parameters'],
            perm='r',
            name='Status',
            info='Process status: preparation-> running -> success|error.'
        ))
        #self._logger.w(self.get_name(), key='action')
        #self._logger.w('Prepare', key='message')

    def _set_status(self, status):
        self.status = status

    def get_status(self):
        return self.status

    def get_kwoptions(self):
        return self.get_attrsman().get_group_attrs('options')

    def run(self):
        if self.is_ready():
            logger = self.get_logger()
            self.status = 'running'
            logger.start('Run process: %s' % self.get_name())

            #logger.w('start', key='message')
            #
            ans = self.do()
            #
            #logger.w(self.status, key='message')
            if ans == True:
                self.status = 'success'
            else:
                self.status = 'error'
            logger.stop('Finished %s with status %s.' % (self.get_name(), self.status))

            return self.status == 'success'
            # f self.status == 'error':
            #    return True
            # self.status = 'success' # self.status = 'error'
            # return True
        else:
            logger.stop('Cannot start %s.' % (self.get_name(),))
            return False

    def aboard(self):
        self.status = 'aboarded'

    def do(self):
        #self.status = 'success'
        return True

    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass

    def is_ready(self):
        """
        Returns True if process is ready to run.

        """
        return True

    def save_options(self, filepath):
        optiondata = {}
        for attrconf in self.get_attrsman().get_configs(filtergroupnames=self.optiongroupnames):
            optiondata[attrconf.attrname] = attrconf.get_value()

        cm.save_obj(optiondata, filepath, is_not_save_parent=False)

        try:
            f = open(filepath, 'wb')
        except:
            print 'WARNING in save: could not open', filepath
            return False

        # print '  before',is_not_save_parent,parent,obj.parent
        pickle.dump(optiondata, f, protocol=2)
        f.close()

    def load_options(self, filepath):
        try:
            f = open(filepath, 'rb')
        except:
            print 'WARNING in load_options: could not open', filepath
            return None

        # try:
        # print '  pickle.load...'
        optiondata = pickle.load(f)
        f.close()

        attrsman = self.get_attrsman()
        for attrname, value in optiondata.iteritems():
            if attrsman.has_attrname(attrname):
                attrsman.get_config(attrname).set_value(value)


class Options:
    """
    Dummy option class to fake option parsing
    """

    def __init__(self, **kwargs):
        self._optionattrs = []
        self._cmlvaluemaps = []
        self._transdir = {}
        self._filepathattrs = []
        for attr, value in kwargs.iteritems():
            self.add_option(attr, value)

    def add_option(self, attr='', value='', cml=None,
                   is_filepath=False, cmlvaluemap=None):
        setattr(self, attr, value)
        self._optionattrs.append(attr)
        self._cmlvaluemaps.append(cmlvaluemap)
        if cml is not None:
            self._transdir[attr] = cml
        if is_filepath:
            self._filepathattrs.append(attr)

    def set_transdir(self, **transdir):
        """
        Sets a dictionary to translate python compatible
        option names into the command line optionnames,
        only in case the command line options are not identical 
        with python attributes (for example if 
        command line options contain '.' or '-').
        Format of  transdir is python attribute as key and
        command line option (as string, WITH preceeding'--') as value.

        """
        self._transdir = transdir

    def get_optionstring(self):
        # print 'get_optionstring'
        s = ''
        for attr, cmlvaluemap in zip(self._optionattrs, self._cmlvaluemaps):
            value = getattr(self, attr)
            cmlattr = self._transdir.get(attr, attr)
            is_continue = True
            if cmlvaluemap is not None:
                if cmlvaluemap.has_key(value):
                    is_continue = False  # take value from mapping
                    if P == '"':  # windows
                        s += ' '+cmlattr+' "%s"' % cmlvaluemap[value]
                    else:
                        s += ' '+cmlattr+" '%s'" % cmlvaluemap[value]
            # print '  option',attr,cmlattr, 'cmlvaluemap',cmlvaluemap

            if is_continue:

                if attr in self._filepathattrs:
                    if value != '':
                        s += ' '+cmlattr+' %s' % filepathlist_to_filepathstring(value.split(','))
                elif type(value) == types.BooleanType:
                    if value:
                        s += ' '+cmlattr
                elif type(value) in [types.StringTypes, types.UnicodeType]:
                    if P == '"':  # windows
                        s += ' '+cmlattr+' "%s"' % value
                    else:
                        s += ' '+cmlattr+" '%s'" % value
                else:
                    s += ' '+cmlattr+' %s' % value
        return s


class CmlMixin:
    def init_cml(self, command, is_run_background=False, is_nohup=False, workdirpath=None):

        self.optiongroupname = 'cml-options'
        self.optiongroupnames.append(self.optiongroupname)
        attrsman = self.get_attrsman()
        self.pathmetatypes = ['filepath', 'dirpath', 'filepaths', 'dirpaths']
        self.workdirpath = workdirpath
        self._command = attrsman.add(cm.AttrConf(
            '_command', command,
            groupnames=['_private'],
            perm='r',
            name='command',
            info='Command to be executed.'
        ))
        self.pid = attrsman.add(cm.AttrConf(
            'pid', -1,
            groupnames=['_private'],
            perm='r',
            name='Process ID',
            info="The system's Process ID",
        ))

        self.is_run_background = attrsman.add(cm.AttrConf(
            'is_run_background', is_run_background,
            groupnames=['parameters', 'advanced'],
            perm='rw',
            name='Run in background',
            info='If set, process will run in background.',
        ))

        self.is_nohup = attrsman.add(cm.AttrConf(
            'is_nohup', is_nohup,
            groupnames=['parameters', 'advanced', ],
            perm='rw',
            name='No hangup',
            info="""If set, process will run in the background and will continue to run after logout. (Currently on UNIX platforms only.) """,
        ))

    def add_option(self, option, value, **kwargs):
        kwargs0 = {'cml': None,
                   'groupnames': [],
                   'perm': 'rw',
                   'is_save': True,
                   'name': None,
                   'info': '',
                   }

        kwargs0.update(kwargs)
        if not (self.optiongroupname in kwargs0['groupnames']):
            kwargs0['groupnames'] += [self.optiongroupname]

        # print '\nadd_option', option, value,kwargs0
        default = self.get_attrsman().add(cm.AttrConf(option, value, **kwargs0))
        setattr(self, option, default)

    def get_options(self):
        print '\nget_options'
        options = Options()
        for attrconfig in self.get_attrsman().get_configs(is_all=True):
            if self.optiongroupname in attrconfig.groupnames:
                print '  option', attrconfig.attrname, attrconfig.groupnames, 'is path', attrconfig.get_metatype() in self.pathmetatypes, 'has cmlmap', hasattr(attrconfig, 'cmlvaluemap')
                is_enabled = True
                if hasattr(attrconfig, 'is_enabled'):
                    print ' is_enabled=', attrconfig.is_enabled(self), attrconfig.get_value()
                    is_enabled = attrconfig.is_enabled(self)
                if is_enabled:  # disabeled options are simply not added
                    if hasattr(attrconfig, 'cmlvaluemap'):
                        cmlvaluemap = attrconfig.cmlvaluemap
                    else:
                        cmlvaluemap = None
                    is_filepath = attrconfig.get_metatype() in self.pathmetatypes
                    options.add_option(attrconfig.attrname, attrconfig.get_value(), attrconfig.cml,
                                       is_filepath=is_filepath, cmlvaluemap=cmlvaluemap)

        return options

    def print_options(self):
        print 'Options of process ident:', self.ident
        print ' Keywordoptions:'
        for attrconfig in self.get_attrsman().get_configs(filtergroupnames=[self.optiongroupname]):
            print '  ', attrconfig.attrname, '=', attrconfig.get_value()

    def reset_cml(self, cml):
        self._command = cml

    def get_cml(self, is_changecwd=False, is_without_command=False):
        """
        Returns commandline with all options.
        To be overridden by costum class.
        """
        # is_changecwd is obsolete, chenge in different directory
        # if self.workdirpath is not None
        options = self.get_options()
        optionstr = options.get_optionstring()
        print 'get_cml command', self._command, 'workdirpath', self.workdirpath
        if True:  # self.workdirpath is None:
            if is_without_command:
                cml = optionstr
            else:
                cml = P+self._command+P + optionstr
        else:
            cml = 'cd '+P+self.workdirpath+P+' ;'\
                + P+self._command+P + optionstr

        # print '  now call get_optionstring',options.get_optionstring
        return cml

    def run_cml(self, cml=None):
        if cml is None:
            cml = self.get_cml()
        attrsman = self.get_attrsman()
        if self.workdirpath is not None:
            wd = self.workdirpath
            #    os.chdir(self.workdirpath)
            self._subprocess = subprocess.Popen(cml, shell=True, cwd=wd)

        else:
            wd = os.getcwd()
            self._subprocess = subprocess.Popen(cml, shell=True)

        attrsman.pid.set(self._subprocess.pid)
        attrsman.status.set('running')
        print 'run_cml cml=', cml
        print '  pid = ', self.pid, 'cwd', wd
        if not self.is_run_background:
            self._subprocess.wait()
            # if self.workdirpath is not None:
            #    os.chdir(wd)

            if self._subprocess.returncode == 0:
                attrsman.status.set('success')
                return True

            else:
                attrsman.status.set('error')
                return False


# print '\n Starting command:',cmd
##p = subprocess.Popen(cmd, shell=True)
# print "p.pid",p.pid, "p.poll=",p.poll()
# p.wait()
# print '\n\nreturncode',p.poll(),p.returncode
# print 'p.stdout=\n', p.stdout
# call(cmd)
###############################################################################
if __name__ == '__main__':
    """
    Test
    """

    # for testing only to get the object browser...
    sys.path.append(os.path.join(APPDIR, "lib_meta_wx"))
