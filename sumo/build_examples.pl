#!/usr/bin/perl
# This script builds all examples and stores them into
# ../nets. ../nets will be build when not existing.
#
# This program must be called inside the examples-
# folder
#
# As the first parameter, the path to the aplication
# (sumo-netconvert) is awaited

sub buildSingle($$) {
	$netc = $_[0];
	$name = $_[1];
	$call = $netc." -t ../config/types.xml -v -C ";
	$call .= "-n ".$name.".nod.xml ";
	$call .= "-e ".$name.".edg.xml ";
	$call .= "-o ../nets/".$name.".net.xml ";
	print $call."\n";
	system($call);
}

# get the application position
if($ARGV[0]) {
    $netc = $ARGV[0];
} else {
    $netc = $ENV["SUMO-NETCONVERT"];
}
if($netc eq "") {
    die "Can not determine the position of sumo-netconvert.\nPlease start with the path to this program as the first parameter.\n";
}

# build all examples
mkdir("../nets");
open(IN, "< Makefile.am") || die "Makefile.am that contains the list of examples to build could not be opened.\nPlease start the program in your data/examples - folder.\n";
while(<IN>) {
    $a = $_;
    $a =~ s/\s*?(.*?)\.nod\.xml/ExAMPLEB$1ExAMPLEE/g;
    @b = split("ExAMPLEB", $a);
    foreach $ex (@b) {
	$beg = rindex($ex, " ") + 1;
	$ex = substr($ex, $beg, index($ex, "ExAMPLEE")-$beg);
	if(length($ex)>0) {
	    buildSingle($netc, $ex);
	}
    }
}
close IN;


