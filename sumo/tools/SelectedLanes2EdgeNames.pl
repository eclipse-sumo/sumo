if(!defined($ARGV[0])) {
    print "Syntax-Error!!!\n";
    print "Syntax: SelectedLanes2EdgeNames.pl <FILE>\n";
}

open(INDAT, "< $ARGV[0]");
while(<INDAT>) {
    $tmp = $_;
    $tmp =~ s/lane:(.*)\_.*\s$/${1}/g;
    $edges{$tmp} = 1;
}
close INDAT;
foreach $key (keys(%edges)) {
    print $key."\n";
}
#