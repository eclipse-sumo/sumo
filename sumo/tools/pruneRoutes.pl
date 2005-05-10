if(!defined($ARGV[3])) {
	print "Syntax-Error!\n";
	print "Syntax: pruneRoutes.pl <INPUTFILE> <OUTPUTFILE> <BEG_TIME> <END_TIME>\n";
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

open(INDAT, "< $ARGV[0]");
open(OUTDAT, "> $ARGV[1]");
$tbeg = $ARGV[2];
$tend = $ARGV[3];
$haveroute = 0;
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "\<route ") {
		$haveroute = 1;
		$route = $tmp;
	}

	if($haveroute==0) {
		print OUTDAT $tmp;
	} else {
		if($tmp =~ "\<vehicle") {
			$depart = getAttr($tmp, "depart");
print $depart;
			if($depart>=$tbeg && $depart<=$tend) {
				print OUTDAT $route;
				print OUTDAT $tmp;
print " kept\n";
			} else {
print " dismissed\n";
			}
			$haveroute = 0;
		} else {
			if($tmp !~ "\<route " && length($tmp)>2 ) {
				print OUTDAT $tmp;
			}
		}
	}
}
close(INDAT);
close(OUTDAT);
