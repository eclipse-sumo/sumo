#!/usr/bin/perl
# ***************************************************************************
#                          splitTripsByHours.pl
# Splits the given trip definition file by hours
# The defaults will split the trip definitions file into files which
#  contain vehicle emissions between the hours 0 and 24
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
# Revision 1.3  2006/10/31 12:24:59  dkrajzew
# code beautifying
#
# Revision 1.2  2006/02/23 11:41:24  dkrajzew
# debugging
#
# Revision 1.1  2006/02/13 07:42:09  dkrajzew
# some tools for trip manipulation added
#
if(!defined($ARGV[0])) {
	print "Syntax-Error.\n";
	print "Syntax: removeTripsIfNotInInterval.pl <SUMO_TRIPS_FILE> [<MIN_HOUR> <MAX_HOUR>]\n";
	die;
}

$min = 0;
if(defined($ARGV[1])) {
	$min = $ARGV[1];
}
$max = 23;
if(defined($ARGV[2])) {
	$max = $ARGV[2];
}

for($hour=$min; $hour<=$max; $hour++) {
	$out = $ARGV[0];
	@tmp = split('\.', $out);
	$tmp[0] = $tmp[0]."_".$hour;
	$out = join('.', @tmp);
	$begin = $hour * 3600;
	$end = $begin + 3599;
	print "Building trips between ".$begin." and ".$end."\n";
	system("removeTripsIfNotInInterval.pl ".$ARGV[0]." ".$begin." ".$end." > ".$out);
}

