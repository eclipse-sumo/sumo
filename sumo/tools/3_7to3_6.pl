#!/usr/bin/perl

open(INDAT, "< $ARGV[0]");
open(OUTDAT, "> $ARGV[1]");
while(<INDAT>) {
    $tmp = $_;
    if($tmp =~ "-- VISSIM 3.7") {
        $tmp = "-- VISSIM 3.61-06\n";
    }
    if($tmp =~ "UEBER") {
        $tmp =~ s/ 0\.000//g;
    }
    if($tmp =~ "STAU GESCHWINDIGKEIT") {
        $tmp =~ s/ LAENGE MAX [\S]*//g;
    }
    if($tmp =~ "SIM_GESCHWINDIGKEIT") {
        $tmp = "INVALID";
    }
    if($tmp =~ "SEGMENT LAENGE " && $tmp =~"AUSWERTUNG ANIMATION") {
        $tmp =~ s/SEGMENT LAENGE ([\S\s]*?) AUSWERTUNG ANIMATION/SEGMENT LAENGE $1/g;
    }

    if($tmp ne "INVALID") {
        print OUTDAT $tmp;
    }
}
close(INDAT);
close(OUTDAT);
