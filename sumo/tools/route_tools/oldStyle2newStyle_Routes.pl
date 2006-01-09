#!/usr/bin/perl
# ***************************************************************************
#                          oldStyle2newStyle_Routes.pl
# Converts routes from old style (route is not in vehicle) to
#  new style (route is within the vehicle
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
# Revision 1.1  2006/01/09 13:46:12  dkrajzew
# new scripts added
#
#

if(!defined($ARGV[0])) {
	print "Syntax-Error!\n";
	print "Syntax: oldStyle2newStyle_Routes.pl <SUMO_ROUTE_FILE>\n";
	die;
}

open(INDAT, "< $ARGV[0]");
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "\<route ") {
		$lastroute = $tmp;
	} elsif ($tmp =~ "\<vehicle ") {
		$vehicle = $tmp;
		$vehicle =~ s/\/\>/\>/g;
		print $vehicle;
		print "   ".$lastroute;
		print "   </vehicle>\n";
	} else {
		print $tmp;
	}

}
close(INDAT);
