#!/usr/bin/perl
# Generates all routes by starting the "mkroutes.pl"-Skript
# with every *.routedef file within this folder;
# the generated route-files are written to ../routes

opendir(DIR, "./") || die "can't open ./: $!";
@files = grep { -f "./$_" && substr($_, length($_)-8) eq "routedef"  } readdir(DIR);
closedir DIR;
foreach $ex (@files) {
	$ex = substr($ex, 0, length($_)-9);
	$call = "./mkroutes.pl ".$ex.".routedef ../routes/".$ex.".rou.xml";
	print "Executing:".$call."\n";
	system($call);
}
