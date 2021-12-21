#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    docgen.py
# @author  Joerg Schweizer
# @date    2012

# """
##
##
# """
##__author__ = """Joerg Schweizer"""
##
##
##import networkx as nx
##

import numpy as np
from matplotlibtools import save_fig, init_plot

# needed for doc gen + import numpy as np
from os import system, path, getcwd, chdir
import types

##############################################################################
# convenience functions for Doc
ARRAYTYPES = (types.TupleType, np.ndarray, types.ListType, types.XRangeType)
INTTYPES = (types.IntType, np.int32, np.int64, np.int0, np.int16, np.int8, types.LongType)
FLOATTYPES = (types.FloatType, np.float64)


def is_arraytype(obj):
    """
    Returns True if n is an array type of any kind
    """
    return type(obj) in ARRAYTYPES


def is_integer(n):
    """
    Returns True if n is an integer type of any kind
    """
    return type(n) in INTTYPES


def is_float(x):
    """
    Returns True if n is a float type of any kind
    """
    return type(x) in FLOATTYPES


class Document:
    def __init__(self, filename, workdir=None, preample=None, is_compile=False, is_maketitle=True):
        """
        Create a document and open as file
        filename = filename, excluding path and extension .tex
        preample =  enything that goes before begin document
                    If preample is given then an enire Latex document
                    will be created. Otherwise the document will be
                    created as an \\input{} file within another document  
        """
        # print 'Doc.__init__',filename
        if not workdir:
            workdir = path.dirname(filename)
            if not workdir:
                workdir = getcwd()

        self.workdir = workdir
        # print '  workdir',workdir
        self.filename = filename
        self.path_file = path.join(self.workdir, filename)+'.tex'
        # print '  path_file',self.path_file
        self.f = open(self.path_file, 'w')
        self.preample = preample
        self.is_maketitle = is_maketitle
        if self.preample:
            self.f.write(preample+"""\n\\begin{document}\n""")
            if self.is_maketitle:
                self.f.write("""\n\maketitle""")
        # return self.f
        self.is_compile = is_compile

    def section(self, name, mode='', label=None):

        self.f.write("""\n\\section"""+mode+"""{"""+name + """}\n""")
        if label:
            self.f.write("""\\label{"""+label + """}\n""")

    def subsection(self, name, mode='', label=None):

        self.f.write("""\n\\subsection"""+mode+"""{"""+name + """}\n""")
        if label:
            self.f.write("""\\label{"""+label + """}\n""")

    def subsubsection(self, name, mode='', label=None):

        self.f.write("""\n\\subsubsection"""+mode+"""{"""+name + """}\n""")
        if label:
            self.f.write("""\\label{"""+label + """}\n""")

    def new_line(self, mode=""):
        self.f.write(mode+"""\\\\ \n\n""")

    def hline(self):
        self.f.write("""\\hline \n""")

    def newpage(self):
        self.f.write("""\\newpage \n""")

    def make_tableofcontents(self):
        self.f.write("""\\tableofcontents
    \\thispagestyle{empty}\\clearpage  \n""")

    def clearpage(self):
        self.f.write("""\\clearpage \n""")

    def write_objvaltable(self, obj, groupname=None,
                          attrsname="Attributes", valuesname="Value", symbolsname=None,
                          **kwargs):

        attrsman = obj.get_attrsman()

        if (groupname is None) | (not attrsman.has_group(groupname)):
            attrconfigs = attrsman.get_configs()

        else:
            attrconfigs = attrsman.get_group(groupname)
            attrsname = groupname

        # first make a table of scalars only
        if symbolsname is None:
            self.begin_tabular(firstrow=[attrsname.capitalize(), valuesname.capitalize()], **kwargs)
        else:
            self.begin_tabular(firstrow=[attrsname.capitalize(), symbolsname.capitalize(),
                                         valuesname.capitalize()], **kwargs)

        for attrconfig in attrconfigs:
            if not attrconfig.is_colattr():
                if symbolsname is None:
                    self.add_tabular_row([attrconfig.get_name(),
                                          attrconfig.format_value(show_unit=True, show_parentesis=False)])
                else:
                    if hasattr(attrconfig, 'symbol'):
                        symbol = "$%s$" % attrconfig.symbol
                    else:
                        symbol = ""

                    self.add_tabular_row([attrconfig.get_name(),
                                          symbol,
                                          attrconfig.format_value()
                                          + "$\,%s$" % attrconfig.format_unit(show_parentesis=False),
                                          ])
        self.end_tabular()

    def begin_tabular(self, firstrow=[], firstcol=[], sep="|", align="c", n_cols=0, n_rows=0,
                      is_hline=True, is_vline=True, is_centered=True, stretch=1.2):
        self.tab_firstrow = firstrow
        self.tab_firstcol = firstcol
        self.tab_rowcount = 0
        self.tab_sep = sep
        self.tab_is_hline = is_hline
        self.tab_is_vline = is_vline
        self.tab_is_centered = is_centered

        if self.tab_is_centered:
            self.begin_center()

        format = ""
        if len(firstrow) > 0:
            self.tab_n_cols = len(firstrow)
        else:
            self.tab_n_cols = n_cols

        if len(firstcol) > 0:
            self.tab_n_rows = len(firstcol)
            if is_vline:
                format = "|"+align+"||"
            else:
                format = align+"|"
        else:
            self.tab_n_rows = n_rows
            if is_vline:
                format = "|"

        for col in range(self.tab_n_cols):
            if col == (self.tab_n_cols-1):
                if is_vline:
                    form_col = align+"|"
                else:
                    form_col = align
            else:
                form_col = align+sep

            format += form_col

        if stretch != None:
            self.f.write("""\\renewcommand\\arraystretch{%s}\n""" % stretch)

        self.f.write("""\\begin{tabular}{"""+format + """}\n""")
        self._make_tableheader()

    def _make_tableheader(self):

        if self.tab_is_hline:
            self.f.write("""\\hline\\hline  \n""")
        elif len(self.tab_firstrow) > 0:
            self.f.write("""\\hline \n """)
        else:
            self.f.write("""\n """)

        if len(self.tab_firstrow) > 0:
            if len(self.tab_firstcol) > 0:

                self.f.write("\t &")  # make empty top left table corner

            for col in range(self.tab_n_cols):
                if col == (self.tab_n_cols-1):
                    sep = "\t \\\\ \n"
                else:
                    sep = "\t &"

                self._write_arrayelement(self.tab_firstrow[col], sep, is_math=False)

            if self.tab_is_hline:
                self.f.write("""\\hline\\hline  \n""")
            else:
                self.f.write("""\\hline \n """)

    def add_tabular_row(self, row):

        # print 'add_tabular_row',self.tab_rowcount
        if len(self.tab_firstcol) > 0:
            self._write_arrayelement(self.tab_firstcol[self.tab_rowcount], "\t &",
                                     is_math=False)

        for col in range(self.tab_n_cols):
            if col == (self.tab_n_cols-1):
                sep = "\t\\\\ \n"
            else:
                sep = "\t &"

            self._write_arrayelement(row[col], sep, is_math=False)

        if self.tab_is_hline:
            self.f.write("""\\hline \n""")

        self.tab_rowcount += 1

    def end_tabular(self):
        if self.tab_is_hline:
            self.f.write("""\\hline \n""")

        self.f.write("\\end{tabular} \n")

        if self.tab_is_centered:
            self.end_center()

    def begin_equation(self, mode="$", label=None):
        if label != None:
            mode = 'enum'
        self.eqn_mode = mode
        if self.eqn_mode in ("$", "inline"):
            self.f.write("$")
        elif self.eqn_mode in ("*", "nolabels"):
            self.f.write("""\\begin{equation*}\n""")
        elif self.eqn_mode in ("enum", "enumerated"):
            self.f.write("""\\begin{equation}\n""")
        else:
            self.eqn_mode = "["
            self.f.write("""\\[\n""")

        if label != None:
            self.write("""\\label{%s}""" % label)

    def end_equation(self):
        if self.eqn_mode in ("$", "inline"):
            self.f.write("$\n")
        elif self.eqn_mode in ("*", "nolabels"):
            self.f.write("""\\end{equation*}\n""")
        elif self.eqn_mode in ("enum", "enumerated"):
            self.f.write("""\\end{equation}\n""")
        else:
            self.eqn_mode = "["
            self.f.write("""\\]\n""")

    def begin_description(self):
        self.f.write("\n\\begin{description}\n")

    def item(self, item, label=None):
        if label == None:
            self.f.write("\n\\item %s\n" % item)
        else:
            self.f.write("\n\\item[%s] %s\n" % (label, item))

    def end_description(self):
        self.f.write("\\end{description} \n")

    def begin_array(self, n_col=1, brace="(", arraystretch=1):
        self.f.write("\\renewcommand\\arraystretch{%.2f}" % arraystretch)
        self.f.write("\\left"+brace+"\\begin{array}{"+n_col*"c"+"}\n")

    def end_array(self, brace=")"):
        self.f.write("\\end{array}\\right"+brace+" \n")

    def matrix(self, matrix, format=None, is_math=True, leftbrace='(', rightbrace=')', arraystretch=1):
        # print 'pmatrix',matrix
        # n_row,n_col=matrix.shape
        n_row = len(matrix)
        n_col = len(matrix[0])
        self.begin_array(n_col, leftbrace, arraystretch)
        for row in xrange(0, n_row):
            for col in xrange(0, n_col):
                # print '  ',col,row,matrix[row,col]
                if col == (n_col-1):
                    sep = "\\\\ \n"
                else:
                    sep = " & "
                # pick matrix element at row,col and decide what to do
                elem = matrix[row][col]
                self._write_arrayelement(elem, sep, format_default=format, is_math=is_math)

        self.end_array(rightbrace)

    def colvec(self, vec, format=None, is_math=True):
        # print 'pmatrix',matrix
        n_row = len(vec)
        sep = "\\\\ \n"
        self.begin_array(1)
        for col in xrange(0, n_row):
            self._write_arrayelement(vec[col], sep, format_default=format, is_math=is_math)

        self.end_array()

    def rowvec(self, vec, format=None, is_math=True):
        n_row = len(vec)

        self.begin_array(n_row)
        for row in xrange(0, n_row):
            # print '  ',col,row,matrix[row,col]
            if row == (n_row-1):
                sep = "\\\\ \n"
            else:
                sep = " & "
            self._write_arrayelement(vec[row], sep, format_default=format, is_math=is_math)

        self.end_array()

    def _write_arrayelement(self, elem, sep, format_default=None, is_math=False):
        """
        Write to document the formated characters dependend on type 
        """
        # print '_write_arrayelement'
        # print '  elem,format_default',elem,type(elem),type(elem)==np.float64,format_default
        if is_math:
            mathsep = ""
        else:
            mathsep = "$"  # create in-line mat env

        if is_arraytype(elem):
            self.f.write(mathsep)
            if is_arraytype(elem[0]):
                self.matrix(elem, is_math=False)  # write entire matrix
            else:
                self.rowvec(elem, is_math=False)  # write entire matrix
            self.f.write(mathsep+sep)

        elif format_default != None:  # scalar with default format
            self.f.write((format_default+sep) % (elem))

        elif is_integer(elem):
            format = "%d"
            self.f.write((mathsep+format+mathsep+sep) % (elem))

        elif is_float(elem):
            format = "%.2f"
            self.f.write((mathsep+format+mathsep+sep) % (elem))

        else:  # probably a string, just as is
            self.f.write(elem+sep)

    def begin_center(self):
        self.f.write("""\n\\begin{center}\n""")

    def end_center(self):
        self.f.write("""\\end{center}\n""")

    def include_graphics(self, filename, options=None, is_centered=True,
                         method='matplotlib', label=None, caption=None, envoptions='h!'):
        print 'include_graphics ', filename
        path_file = path.join(self.workdir, filename)
        if options is None:
            options = 'width = \\textwidth'
        is_figure_env = (label != None) | (caption != None)
        if is_figure_env:
            if label != None:
                self.write("""\\begin{figure}[%s]""" % envoptions)
            else:
                self.write("""\\begin{figure*}[%s]""" % envoptions)

        if is_centered:
            self.begin_center()
        # print '  method=',method
        if method == 'matplotlib':

            workdir = getcwd()
            chdir(self.workdir)
            # print '  safe matplotlib figure and convert to pdf'
            ffilename = save_fig(filename)
            # print '  done safe'
            chdir(workdir)
            self.f.write("""\\includegraphics["""+options+"""]{"""+ffilename + """}\n""")
        else:
            self.f.write("""\\includegraphics["""+options+"""]{"""+filename + """}\n""")

        if is_centered:
            self.end_center()
        if is_figure_env:
            if caption != None:
                self.write("""\\caption{%s}""" % caption)
            if label != None:
                self.write("""\\label{%s}""" % label)

            if label != None:
                self.write("""\\end{figure}""")
            else:
                self.write("""\\end{figure*}""")

    def save_graphics_matplotlib(self, filename):
        """
        Save current matplotlib graphics in working directory.
        """
        workdir = getcwd()
        chdir(self.workdir)
        save_fig(filename)
        chdir(workdir)

    def write(self, text):
        self.f.write(text+"\n")

    def stream(self, text):
        self.f.write(text)

    def end(self):
        if self.preample:
            self.f.write("""\\end{document}\n""")
        self.f.close()
        if self.is_compile:
            print 'compile latex ', self.path_file
            system('pdflatex ' + self.path_file)


def ref(label, reftype=None):
    if reftype == None:
        return '~\\ref{%s}' % label
    else:
        return '%s~\\ref{%s}' % (reftype, label)
