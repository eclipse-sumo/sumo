if(!defined($ARGV[0])) {
    print "Syntax-Error!!!\n";
    print "Syntax: Lines2CSV.pl <FILE>\n";
}

open(INDAT, "< $ARGV[0]");
$had = 0;
while(<INDAT>) {
    $tmp = $_;
    if($had==1) {
        print ";";
    }
    $tmp =~ s/\s$//g;
    print $tmp;
    $had = 1;
}
close INDAT;
