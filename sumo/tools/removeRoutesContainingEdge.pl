if(!defined($ARGV[2])) {
	print "Syntax-Error.\n";
	print "Syntax: removeRoutesContainingEdge.pl <FILE> <EDGE> <PROBABILITY>\n";
	die;
}

open(INDAT, "< $ARGV[0]");
open(OUTDAT, "> $ARGV[0].conv");
$name = " ".$ARGV[1]." ";
while(<INDAT>) {
	$tmp = $_;
	if($removed==1) {
		$removed = 0;
	} else {
		$removed = 0;
		if($tmp =~ $name) {
			$r = rand(1);
			if($r>$ARGV[2]) {
				print OUTDAT $tmp;
			} else {
				$removed = 1;
  			}
		} else {
			print OUTDAT $tmp;
		}
	}
}
close(INDAT);
close(OUTDAT);