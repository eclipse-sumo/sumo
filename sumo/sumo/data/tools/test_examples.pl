#!/usr/bin/perl
# get the application position
if($ARGV[0]) {
    $sumo = $ARGV[0];
} else {
    $sumo = $ENV{"SUMO"};
}
# get the path to the data-folder
if($ARGV[1]) {
    $path = $ARGV[1];
} else {
    $path = $ENV{"SUMO_DATA"};
}

if($netc eq "") {
    die "Can not determine the position of sumo.\nPlease start with the path to this program as the first parameter.\n";
}
if($path eq "") {
    die "Can not determine the position of data.\nPlease start with the path to this program as the second parameter.\n";
}

# build all examples
for($i=1; $i<5; $i++) {
    for($j=0; $j<3; $j++) {
	$call_add[0] = "-R";
	$call_add[1] = "-R -d ".$path."detectors/box".$i."l.det.xml";
	$call_add[2] = "-o out.xml";
	# build call
	$call = $sumo." -c ".$path."/config/box".$i."l.cfg ";
	$call .= $call_add[$j];
	$call .= " -e 10000";
	$call = "time ".$call;
	print $call."\n";
	system($call);
    }
}

