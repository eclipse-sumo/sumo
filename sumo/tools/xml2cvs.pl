if(!defined($ARGV[1])) {
	print "Syntax-Error!\n";
	print "Syntax: xml2cvs.pl <FILENAME> <TAGNAME>\n";
	print " Prints all attributes in the order of their occurence from the given tag of the given file.\n";
	die;
}


$tag = "\<".$ARGV[1];
open(INDAT, "< $ARGV[0]") || die "Could not open ".$ARGV[0];
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ $tag) {
		$beg = index($tmp, "\"");
		while($beg!=-1) {
			$end = index($tmp, "\"", $beg+1);
			print substr($tmp, $beg+1, $end-$beg-1);
			print ";";
			$beg = index($tmp, "\"", $end+1);
		}
		print "\n";
	}
}
close(INDAT);


