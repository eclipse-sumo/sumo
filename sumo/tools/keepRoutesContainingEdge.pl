if(!defined($ARGV[2])) {
	print "Syntax-Error.\n";
	print "Syntax: removeRoutesContainingEdge.pl <FILE> <EDGE> <PROBABILITY>\n";
	die;
}

open(INDAT, "< $ARGV[0]");
open(OUTDAT, "> $ARGV[0].conv");
$name1 = " ".$ARGV[1]." ";
$name2 = ">".$ARGV[1]." ";
$name3 = " ".$ARGV[1]."<";
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "route id" && $tmp !~ $name1 && $tmp !~ $name2 && $tmp !~ $name3) {
		$r = rand(1);
		if($r>$ARGV[2]) {
			print OUTDAT $tmp;
		} else {
			$removed = 1;
  		}
	} else {
		if($tmp !~ "vehicle id" || $removed==0) {
			print OUTDAT $tmp;
		}
		$removed = 0;
	}
}
close(INDAT);
close(OUTDAT);
