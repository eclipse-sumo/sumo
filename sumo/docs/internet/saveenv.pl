#!/usr/bin/perl



foreach $var (sort(keys(%ENV))) {
    $val = $ENV{$var};
    $val =~ s|\n|\\n|g;
    if(!defined($out)) {
	$out = $var."=".$val;
    } else {
	$out = $out."[][][]".$var."=".$val;
    }
}
$out = $out."\n";
open(OUTDAT, ">> entries.dat");
flock(OUTDAT, 2);
print OUTDAT $out;
close(OUTDAT);

