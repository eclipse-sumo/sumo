sub getAttr($$) {
    $beg = index($_[0], $_[1]);
    $beg = index($_[0], "\"", $beg) + 1;
    $end = index($_[0], "\"", $beg);
    return substr($_[0], $beg, $end-$beg);
}

sub getEmb($) {
    $beg = index($_[0], "\>");
    return substr($_[0], $beg+1);
}


if(!defined($ARGV[0])) {
    print "Syntax-Error!!!\n";
    print "Syntax: invent2csv.pl <INVENT_DETECTOR_FILE>\n";
    die;
}

# read in the data
open(INDAT, "< $ARGV[0]");
while(<INDAT>) {
    $content = $content.$_;
}
close(INDAT);

# split by element begins
@elements = split("\<", $content);

# go through the data, write joined
foreach $element (@elements) {
    if($element =~ "location locIDREF") {
        $locIDREF = getAttr($element, "locIDREF");
    }
    if($element =~ "det detIDREF") {
        $detIDREF = getAttr($element, "detIDREF");
    }
    if($element =~ "time ") {
        $min = getAttr($element, "min");
        $sec = getAttr($element, "sec");
        $ms = getAttr($element, "ms");
    }
    if($element =~ "qKfz\>" && $element !~ "/\qKfz\>") {
        $qKfz = getEmb($element);
    }
    if($element =~ "qLkw\>" && $element !~ "/\qLkw\>") {
        $qLkw = getEmb($element);
    }
    if($element =~ "vPkw\>" && $element !~ "/\vPkw\>") {
        $vPkw = getEmb($element);
    }
    if($element =~ "vLkw\>" && $element !~ "/\vLkw\>") {
        $vLkw = getEmb($element);
    }
    # ok, should git all if this occurs, write
    if($element =~ "\/set") {
        print $locIDREF.";".$detIDREF.";".$min.";".$sec.";".$ms.";".$qKfz.";".$qLkw.";".$vPkw.";".$vLkw."\n";
    }
}



