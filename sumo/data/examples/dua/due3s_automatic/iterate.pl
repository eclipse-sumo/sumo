#!/usr/bin/perl

sub writeRouteConf($)
{
    my $step = $_[0];
    open(OUTDAT, "> due3s.dua.cfg");
    print OUTDAT "<configuration>\n";
    print OUTDAT "  <files>\n";
    print OUTDAT "      <net-file>due3s.net.xml</net-file>\n";
    print OUTDAT "      <weights></weights>\n";
    if($step==0) {
        print OUTDAT "      <trip-defs>due3s.trips.xml</trip-defs>\n";
    } else {
        print OUTDAT "      <alternatives>due3s.rou.xml.alt</alternatives>\n";
        print OUTDAT "      <weights>aggregated/due3s_300.xml</weights>\n";
    }
    print OUTDAT "      <output>due3s_new.rou.xml</output>\n";
    print OUTDAT "  </files>\n";
    print OUTDAT "  <process>\n";
    print OUTDAT "      <begin>0</begin>\n";
    print OUTDAT "      <end></end>\n";
    print OUTDAT "  </process>\n";
    print OUTDAT "</configuration>\n";
    close OUTDAT;
}

sub writeSumoConf($)
{
    my $step = $_[0];
    open(OUTDAT, "> due3s.sumo.cfg");
    print OUTDAT "<configuration>\n";
    print OUTDAT "  <files>\n";
    print OUTDAT "      <net-file>due3s.net.xml</net-file>\n";
    print OUTDAT "      <route-files>due3s.rou.xml</route-files>\n";
    print OUTDAT "      <dump-intervals>300</dump-intervals>\n";
    print OUTDAT "      <dump-basename>aggregated/due3s</dump-basename>\n";
    print OUTDAT "  </files>\n";
    print OUTDAT "  <simulation>\n";
    print OUTDAT "      <begin>0</begin>\n";
    print OUTDAT "      <end></end>\n";
    print OUTDAT "  </simulation>\n";
    print OUTDAT "</configuration>\n";
    close OUTDAT;
}

# -------------------------------------

if(!defined($ARGV[1])) {
    print "Syntax-Error!\n";
    print "Syntax: iterate.pl <SUMO_PATH> [<BEG_STEP_NUMBER(INT)>] <END_STEP_NUMBER(INT)>\n";
    die;
}
$step=0;
$end = 10;
if(defined($ARGV[2])) {
    $step = $ARGV[1];
    $end = $ARGV[2];
}
for(; $step<$end+1; $step++) {
    writeRouteConf($step);
    system("$ARGV[0]/duarouter.exe -c due3s.dua.cfg --gA=0.5 --gBeta=0.9");
    system("copy due3s_new.rou.xml due3s.rou.xml");
    system("copy due3s_new.rou.xml.alt due3s.rou.xml.alt");
    system("copy aggregated\\due3s_300.xml aggregated\\dues".$step.".xml");
    if($step<$end) {
        writeSumoConf($step);
        system("$ARGV[0]/sumo.exe -c due3s.sumo.cfg");
    }
}




