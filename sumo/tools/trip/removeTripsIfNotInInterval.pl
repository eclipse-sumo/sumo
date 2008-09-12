#!/usr/bin/perl
# ***************************************************************************
#                          removeTripsIfNotInInterval.pl
# Removes all trips from a trip definition file which departure time lies
#  not within the given interval
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
if(!defined($ARGV[2])) {
	print "Syntax-Error.\n";
	print "Syntax: removeTripsIfNotInInterval.pl <SUMO_TRIPS_FILE> <MIN_TIME> <MAX_TIME>\n";
	die;
}

sub getAttr($$) {
	$text = $_[0];
	$attrname = $_[1];
	$beg = index($text, $attrname);
	$beg = index($text, "\"", $beg);
	$end = index($text, "\"", $beg+1);
	return substr($text, $beg+1, $end-$beg-1);
}

$min = $ARGV[1];
$max = $ARGV[2];
open(INDAT, "< $ARGV[0]");
$name = " ".$ARGV[1]." ";
while(<INDAT>) {
	$tmp = $_;
	if($tmp=~"<tripdef ") {
		$depart = getAttr($tmp, "depart");
		if($depart>=$min && $depart<=$max) {
			print $tmp;
		}
	} else {
		print $tmp;
	}
}
close(INDAT);
