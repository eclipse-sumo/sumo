if(!defined($ARGV[2])) {
	print "Syntax-Error.\n";
	print "Syntax: removeRoutesContainingEdge.pl <FILE> <MIN_TIME> <MAX_TIME>\n";
	die;
}

$min = $ARGV[1];
$max = $ARGV[2];
open(INDAT, "< $ARGV[0]");
open(OUTDAT, "> $ARGV[0].conv");
$name = " ".$ARGV[1]." ";
while(<INDAT>) {
	$tmp = $_;
	if($tmp=~"route id") {
		$route = $tmp;
	} elsif($tmp=~"vehicle id") {
		$beg = index($tmp, "depart=");
		$beg = $beg + 8;
		$end = index($tmp, "\"", $beg);
		$depart = substr($tmp, $beg, $end-$beg);
		if($depart>=$min&&$depart<=$max) {
			print OUTDAT $route;
			print OUTDAT $tmp."\n";
		}
	} else {
		$bla = $tmp;
		$bla =~ s/\s*//g;
		if(length($tmp)!=0) {
			print OUTDAT $tmp;
		}
	}
}
close(INDAT);
close(OUTDAT);