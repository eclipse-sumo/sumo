#!/usr/bin/perl
# ***************************************************************************
#                          removeGeometryInTrips.pl
# Removes all edges from a trip definition file which have been removed
#  during the removal of geometry nodes while building the network
# !!! This tools does notyet handle changes when on-/off-ramps were built
# The result is printed on the screen (use > to pipe it to a file)
#                             -------------------
#   project              : SUMO
#   subproject           : trip tools
#   begin                : Thu, 09.02.2006
#   copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
# Revision 1.1  2006/02/13 07:42:09  dkrajzew
# some tools for trip manipulation added
#

if(!defined($ARGV[1])) {
	print "Syntax-Error!\n";
	print "Syntax: removeGeometryInTrips.pl <SUMO_TRIPS_FILE> <MAP_FILE>\n";
	die;
}

%edgemap = {};

open(INDAT, "< $ARGV[1]");
while(<INDAT>) {
	$tmp = $_;
	$tmp =~ s/\n//g;
	@edges = split("\t", $tmp);
	$to = $edges[0];
	foreach $edge (@edges) {
		($edgeid, $length) = split("\:", $edge);
		$edgemap{$edgeid} = $to;
	}
}


open(INDAT, "< $ARGV[0]");
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "\<tripdef ") {
		$tmp =~ s/from=\"(.*?)\"/from=\"$edgemap{$1}\"/g;
		$tmp =~ s/to=\"(.*?)\"/to=\"$edgemap{$1}\"/g;
		if($tmp =~ "from=\"\"" && $tmp =~"to=\"\"") {
			print "Error!\n";
			print "An unassigned edge occured!\n";
			die;
		}
	}
	print $tmp;

}
close(INDAT);
