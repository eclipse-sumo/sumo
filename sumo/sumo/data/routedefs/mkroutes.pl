#!/usr/bin/perl
# Generates a simple route file for an example
# Call: mkroutes.pl <ROUTEDEF> <OUTPUT>
#  (both parameter are files)
#  <ROUTEDEF> must have the syntax of the example ".routedef"-files
#  <OUTPUT> will be the generated SUMO-Routes file

if($ARGV[1]) {
    # read in the definition file
    open(INDAT, "< $ARGV[0]") || die "Could not open '".$ARGV[0]."'";
    $no = 0;
    while($a=<INDAT>) {
	if($a !~ '\#' && $a =~ '\:') {
	    $routes[$no] = $a;
	    $no++;
	}
    }
    close(INDAT);
    # build the output
    open(OUTDAT, "> $ARGV[1]") || die "Could not open '".$ARGV[1]."'";
    print OUTDAT "<routes>\n\n";
    print OUTDAT "   <vtype id=\"0\" maxspeed=\"20\" length=\"5.0\" accel=\"0.8\" decel=\"4.5\" sigma=\"0.5\"/>\n\n";

    for($i=0; $i<$no; $i++) {
	($name, $route, $norep, $vehoff, $noveh) = split(':', $routes[$i]);
	# print the route
	print OUTDAT "   <route id=\"".$name."\">";
	for($j=0; $j<$norep; $j++) {
	    if($j>0) {
		print OUTDAT " ";
	    }
	    print OUTDAT $route;
	}
	print OUTDAT "</route>\n";
	# print the leaving vehicles
	$time = 0;
	for($j=0; $j<$noveh; $j++) {
	    print OUTDAT "   <vehicle type=\"0\" id=\"".$name.$j."\" depart=\"".$time."\" route=\"".$name."\"/>\n";
	    $time = $time + $vehoff;
	}
	print OUTDAT "\n";
    }
    print OUTDAT "</routes>\n";
    close(OUTDAT);
} else {
  print "Syntax Error!!!\n";
  print "Syntax: mkroutes.pl <DEF_FILE> <OUTPUT_FILE>\n"
}

