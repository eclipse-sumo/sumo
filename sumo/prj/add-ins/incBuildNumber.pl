
sub toTwoPlaces($) {
	if(length($_[0])==1) {
		return "0".$_[0];
	}
	return $_[0];
}

$file = "../../../src/".$ARGV[0]."_build.h";
open(INDAT, "< $file") || die "Could not open '".$file."'";
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "//") {
		push @lines, $tmp;
	}
	if($tmp =~ "NEXT_BUILD_NUMBER") {
		$version = substr($tmp, rindex($tmp, " ")+1);
	}
}
close INDAT;

$version = $version + 1;
($Second, $Minute, $Hour, $Day, $Month, $Year, $WeekDay, $DayOfYear, $IsDST) = localtime(time) ;
$Month = toTwoPlaces($Month+1);
$Day = toTwoPlaces($Day);
$Hour = toTwoPlaces($Hour);
$Minute = toTwoPlaces($Minute);
$Second = toTwoPlaces($Second);
$Year = $Year + 1900;

open(OUTDAT, "> $file");
print OUTDAT "#define NEXT_BUILD_NUMBER ".$version."\n";
$version = $version - 1;
print OUTDAT "// ".$version." was build on ".$Year."-".$Month."-".$Day.";".$Hour.":".$Minute.":".$Second."\n";
foreach $line (@lines) {
	print OUTDAT $line;
}
close OUTDAT;


