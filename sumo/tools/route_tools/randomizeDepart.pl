#!/usr/bin/perl
# ***************************************************************************
#                          randomizeDepart.pl
# Randomizes the departure time for vehicles
# The result is printed on the screen (use > to pipe it to a file)
#                             -------------------
#   project              : SUMO
#   subproject           : route tools
#   begin                : Fri, 02.11.2005
#   copyright            : (C) 2005 by DLR/IVF http://ivf.dlr.de/
#   author               : Daniel Krajzewicz
#   email                : Daniel.Krajzewicz@dlr.de
# ***************************************************************************

#***************************************************************************
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#***************************************************************************
# $Log$
# Revision 1.2  2006/10/31 12:24:59  dkrajzew
# code beautifying
#
# Revision 1.1  2006/01/09 13:46:12  dkrajzew
# new scripts added
#

if(!defined($ARGV[1])) {
	print "Syntax-Error!\n";
	print "Syntax: randomizeDepart.pl <SUMO_ROUTE_FILE> <MAX_DEPART_TIME>\n";
	die;
}

open(INDAT, "< $ARGV[0]");
while(<INDAT>) {
	$i = int(rand() * $ARGV[1]);
	$tmp = $_;
	$tmp =~ s/depart=\".*?\"/depart=\"$i\"/g;
	print $tmp;
}
close(INDAT);
