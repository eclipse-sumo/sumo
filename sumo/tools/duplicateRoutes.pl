if(!defined($ARGV[0])) {
    print "duplicateRoutes.pl <ROUTE_FILE> <NEW_FILE>\n";
    die;
}

sub patchEdges($$) {
    $line = $_[0];
    $end = index($line, ">");
    $beg = index($line, "<", $end);
    $path = substr($line, $end+1, $beg-$end-1);
    $path =~ s/(\S*) /$_[1]${1} /g;
    $path =~ s/ (\S*)$/ $_[1]${1}/g;
    $ret = substr($line, 0, $end+1);
    $ret = $ret.$path;
    $e2 = substr($line, $beg);
    $ret = $ret.$e2;
    return $ret;
}

open(INDAT, "< $ARGV[0]");
open(OUTDAT, "> $ARGV[1]");
while(<INDAT>) {
    $tmp = $_;
    if($tmp=~"route id") {
        # patch route
        #   patch orig
        $orig = $tmp;
                $orig =~ s/id=\"(.*?)\"/id=\"orig-${1}\"/g;
        $orig = patchEdges($orig, "orig-");
        #   patch ois
        $ois = $tmp;
                $ois =~ s/id=\"(.*?)\"/id=\"ois-${1}\"/g;
        $ois = patchEdges($ois, "ois-");
        print OUTDAT $orig."\n";
        print OUTDAT $ois."\n";
    } else {
        if($tmp=~"vehicle id") {
            # patch vehicle
            $orig = $tmp;
                    $orig =~ s/id=\"(.*)\"/id=\"orig-${1}\"/g;
                    $orig =~ s/route=\"(.*)\"/route=\"orig-${1}\"/g;
            #   patch ois
            $ois = $tmp;
                    $ois =~ s/id=\"(.*)\"/id=\"ois-${1}\"/g;
                    $ois =~ s/route=\"(.*)\"/route=\"ois-${1}\"/g;
            print OUTDAT $orig."\n";
            print OUTDAT $ois."\n";
        } else {
            print OUTDAT $tmp."\n";
        }
    }
}
close(INDAT);
close(OUTDAT);


