if(!defined($ARGV[1])) {
	print "Syntax-Error!\n";
	print "Syntax: CollectAtttributes2XML.pl <TAGNAME> <ATTRNAME>\n";
	die;
}

sub getFiles() {
	opendir(DIR, "./");
	@files = grep { -f "./$_" } readdir(DIR);
	closedir DIR;
	foreach $temp (@files) {
		$list = $list.$dir.$temp."\n";
	} # foreach (generate a path description for each file)
	return($list);
}

$no = 0;
$tag = "\<".$ARGV[0];
$attr = $ARGV[1]."=";
$tmp = getFiles();
@files = split("\n", $tmp);
$run = 0;
foreach $file (@files) {
	if($file =~ "\.xml") {
		$line = 0;
		open(INDAT, "< $file");
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
					$vals[$run][$line] = $val;
				}
				$line++;
			}
		}
		close(INDAT);
		$run++;
		if($line>$no) {
			$no = $line;
		}
	}
}
#
$str = "";
foreach $file (@files) {
	if($file =~ "\.xml") {
		$str .= $file.";";
	}
}
print $str."\n";
for($i=0; $i<$no; $i++) {
	$str = "";
	$run = 0;
	foreach $file (@files) {
		if($file =~ "\.xml") {
			$str .= $vals[$run][$i].";";
			$run++;
		}
	}
	print $str."\n";
}

