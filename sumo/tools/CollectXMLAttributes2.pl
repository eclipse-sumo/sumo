if(!defined($ARGV[2])) {
	print "Syntax-Error!\n";
	print "Syntax: CollectAtttributes2XML.pl <FILE> <TAGNAME> <ATTRNAME>\n";
	die;
}


$tag = "\<".$ARGV[1];
$attr = $ARGV[2]."=";
$run = 0;
open(INDAT, "< $ARGV[0]");
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ $tag) {
		$beg = index($tmp, $attr);
		if($beg==-1) {
			$vals[$run][$line] = "N/A";
		} else {
			$beg = index($tmp, "\"", $beg);
			$end = index($tmp, "\"", $beg+1);
			$val = substr($tmp, $beg+1, $end-$beg-1);
			print $val."\n";
		}
		$line++;
	}
}
close(INDAT);
