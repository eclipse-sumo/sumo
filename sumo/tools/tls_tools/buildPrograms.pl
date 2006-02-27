#!/usr/bin/perl
# ***************************************************************************
#                          buildPrograms_Routes.pl
# Converts program descriptions as exported from the Excel template
#  into their SUMO-representation
#                             -------------------
#   project              : SUMO
#   subproject           : route tools
#   begin                : Mon, 27.02.2006
#   copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
#   author               : Daniel Krajzewicz
#   email                : Daniel.Krajzewicz@dlr.de
# ***************************************************************************

#***************************************************************************
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#***************************************************************************
# $Log$
# Revision 1.1  2006/02/27 13:17:30  dkrajzew
# tls-conversion script added
#
#

if(!defined($ARGV[2])) {
	print "Syntax-Error!\n";
	print "Syntax: buildPrograms.pl <SUMO_NET> <EXCEL_TXT> <OUTPUT_FILE>\n";
	die;
}

$Knotenpunkt_Bezeichnung = "";
$Knotenpunkt_Nummer = "";
$KP_ID = "";
$Anzahl_Signalprogramme = "";
$Signalprogramme = "";
$Umschaltverfahren = "";
$Synchronitaet = "";
$WAUT_ID = "";

@programs;

sub getAttr($$) {
	my $text = $_[0];
	my $attrname = $_[1];
	my $beg = index($text, $attrname);
	if($beg<0) {
		return "";
	}
	$beg = index($text, "\"", $beg);
	my $end = index($text, "\"", $beg+1);
	return substr($text, $beg+1, $end-$beg-1);
}

sub parseSingle($) {
	my $txt = $_[0];
	my $beg = index($txt, "\:");
	my $value = substr($txt, $beg+1);
	$value =~ s/\/\/.*$//g;
	$value =~ s/^\s*|\s*$//g;
	return $value;
}

sub parseSingle2($$) {
	my $pre = $_[0];
	my $txt = $_[1];
	my $value = substr($txt, length($pre)+1);
	$value =~ s/\/\/.*$//g;
	$value =~ s/^\s*|\s*$//g;
	return $value;
}

sub encodeSUMO($$$) {
	my $index = $_[0];
	my $noKs = $_[1];
	my $phase = "";
	my $brake = "";
	my $yellow = "";
	my $i;
	for($i=$noKs-1; $i>=0; $i--) {
		if(substr($color[$i], $index, 1) eq "G") {
			$phase = $phase."1";
			$brake = $brake.$_[2];
			$yellow = $yellow."0";
		}
		if(substr($color[$i], $index, 1) eq "Y") {
			$phase = $phase."0";
			$brake = $brake.$_[2];
			$yellow = $yellow."1";
		}
		if(substr($color[$i], $index, 1) eq "R") {
			$phase = $phase."0";
			$brake = $brake."1"; # !!!
			$yellow = $yellow."0";
		}
	}
	$ret = "phase=\"".$phase."\" brake=\"".$brake."\" yellow=\"".$yellow."\"";
	return $ret;
}

sub nextIsSame($$) {
	my $index = $_[0];
	my $noKs = $_[1];
	my $i;
	for($i=0; $i<$noKs; $i++) {
		if(substr($color[$i], $index, 1) ne substr($color[$i], $index+1, 1)) {
			return 0;
		}
	}
	return 1;
}

open(INDAT, "< $ARGV[1]") || die "Can not open '$ARGV[1]'";
# parse global information
while($WAUT_ID eq "") {
	$line = <INDAT>;
	if($line =~ "Knotenpunkt - Bezeichnung") {
		$Knotenpunkt_Bezeichnung = parseSingle($line);
	}
	if($line =~ "Knotenpunkt - Nummer") {
		$Knotenpunkt_Nummer = parseSingle($line);
	}
	if($line =~ "KP-ID") {
		$KP_ID = parseSingle($line);
	}
	if($line =~ "Anzahl Signalprogramme") {
		$Anzahl_Signalprogramme = parseSingle($line);
	}
	if($line =~ "^Signalprogramme") {
		$Signalprogramme = parseSingle($line);
		@programs = split("\;", $Signalprogramme );
	}
	if($line =~ "Umschaltverfahren") {
		$Umschaltverfahren = parseSingle($line);
	}
	if($line =~ "Synchronität") {
		$Synchronitaet = parseSingle($line);
	}
	if($line =~ "WAUT-ID") {
		$WAUT_ID = parseSingle($line);
	}
}
print "----- Global Information parsed -----\n";
print "Knotenpunkt_Bezeichnung: ".$Knotenpunkt_Bezeichnung."\n";
print "Knotenpunkt_Nummer:      ".$Knotenpunkt_Nummer."\n";
print "KP_ID:                   ".$KP_ID."\n";
print "Anzahl_Signalprogramme:  ".$Anzahl_Signalprogramme."\n";
print "Signalprogramme:         ".$Signalprogramme."\n";
print "Umschaltverfahren:       ".$Umschaltverfahren."\n";
print "Synchronitaet:           ".$Synchronitaet."\n";
print "WAUT_ID:                 ".$WAUT_ID."\n";


#--------------
# now we know what tls shall be parsed; read in SUMO-network information
@tls_cons;
open(INDAT2, "< $ARGV[0]") || die "Could not open '$ARGV[0]'.";
while(<INDAT2>) {
	$line = $_;
	if($line =~ "\<succ edge") {
		$from_lane = getAttr($line, "lane");
	}
	if($line =~ "\<succ edge") {
		$from_lane = getAttr($line, "lane");
	}
	if($line =~ "\<succlane") {
		$tl = getAttr($line, "tl");
		if($tl eq $KP_ID) {
			$to_lane = getAttr($line, "lane");
			$linkno = getAttr($line, "linkno");
			$yield = getAttr($line, "yield");
			$dir = getAttr($line, "dir");
			$con = $from_lane.";".$to_lane.";".$linkno.";".$yield.";".$dir;
			push @tls_cons, $con;
		}
	}
}


#--------------



$noSignals = 0;
%signals;
# parse the connections
$endReached = 0;
while($endReached==0) {
	$line = <INDAT>;
	if($line =~ "Signalgruppe" && $line =~ "Lane_von" && $line =~ "Nach") {
		$endReached = 1;
	}
}
$endReached = 0;
$currentKsP = "";
%kindices;
%gks;
while($endReached==0) {
	$line = <INDAT>;
	$tmp = $line;
	$tmp =~ s/^\s*|\s*$//g;
	if(length($tmp)==0) {
		$endReached = 1;
	} else {
		if(substr($line, 0, 1) eq "\t") {
			($bla, $from, $cto) = split("\t", $line);
		} else {
			($currentKsP, $from, $cto) = split("\t", $line);
		}
		@currentKs = split("\,", $currentKsP);
		foreach $k (@currentKs) {
			if(substr($k, 0, 1) ne "K") {
				$k = "K".$k;
			}
			if(!exists $gks{$k}) {
				$gks{$k} = 1;
				$noSignals++;
			}
			$cto =~ s/\/\/.*$//g;
			$cto =~ s/\s*//g;
			$cto =~ s/\"//g;

			@tos = split("\;", $cto);
			foreach $to (@tos) {
				$to =~ s/^\s*|\s*$//g;
				foreach $con (@tls_cons) {
					($pfrom, $pto, $plinkno, $pyield, $pdir) = split("\;", $con);
					$add = 0;
					if($pfrom eq $from) {
						if($to eq "all_succlanes") {
							$add = 1;
						}
						if($to eq $pdir) {
							$add = 1;
						}
						if($to eq $pto) {
							$add = 1;
						}
					}
					if($add==1) {
						$val = $k.";".$from.";".$pto;
						push @Kdefs, $val;
						if(exists $kindices{$k}) {
							$Pindices = $kindices{$k};
							my @indices = @$Pindices;
							$contains = 0;
							foreach $index (@indices) {
								if($index==$plinkno) {
									$contains = 1;
								}
							}
							if($contains==0) {
								push @indices, $plinkno;
								$kindices{$k} = \@indices;
							}
						} else {
							my @indices = ();
							push @indices, $plinkno;
							$kindices{$k} = \@indices;
						}
					}
				}
			}
		}
	}
}

print "----- Connections parsed ----\n";
foreach $con (@Kdefs) {
	print $con."\n";
}


# parse signal plans
$haveMore = 1;
$noRead = 0;
@prog_defs;
%phase_defs;
%bereich_defs;

while($noRead<=$#programs) {
	# parse program-global values
	$Name_Signalprogamm = "";
	$Signalprogramm_ID = "";
	$Umlaufzeit = 0;
	$Versatzzeit = 0;
	$GSP = 0;
	$Anzahl_Bereiche = 0;
	$StretchUmlaufAnz = 0;
	$endReached = 0;
	while($endReached==0) {
		$line = <INDAT>;
		if($line =~ "Name Signalprogamm") {
			$Name_Signalprogamm = parseSingle2("Name Signalprogamm", $line);
			$endReached = 1;
		}
	}
	$endReached = 0;
	while($endReached==0) {
		$line = <INDAT>;
		$tmp = $line;
		$tmp =~ s/^\s*|\s*$//g;
		if(length($tmp)==0) {
			$endReached = 1;
		} else {
			if($line =~ "Signalprogramm-ID") {
				$Signalprogramm_ID = parseSingle2("Signalprogramm-ID", $line);
			}
			if($line =~ "Umlaufzeit") {
				$Umlaufzeit = parseSingle2("Umlaufzeit \[s\]", $line);
			}
			if($line =~ "Versatzzeit") {
				$Versatzzeit = parseSingle2("Versatzzeit", $line);
			}
			if($line =~ "GSP") {
				$GSP = parseSingle2("GSP", $line);
			}
			if($line =~ "Anzahl_Bereiche") {
				$Anzahl_Bereiche = parseSingle2("Anzahl_Bereiche", $line);
			}
			if($line =~ "StretchUmlaufAnz") {
				$StretchUmlaufAnz = parseSingle2("StretchUmlaufAnz", $line);
			}
		}
	}
	$val = $Name_Signalprogamm.";".$Signalprogramm_ID.";".$Umlaufzeit.";".$Versatzzeit.";".$GSP.";".$Anzahl_Bereiche.";".$StretchUmlaufAnz;
	push @prog_defs, $val;
	print "\n";
	print " ----- parsed global program settings -----\n";
	print " Name_Signalprogamm: ".$Name_Signalprogamm."\n";
	print " Signalprogramm_ID:  ".$Signalprogramm_ID."\n";
	print " Umlaufzeit:         ".$Umlaufzeit."\n";
	print " Versatzzeit:        ".$Versatzzeit."\n";
	print " GSP:                ".$GSP."\n";
	print " Anzahl_Bereiche:    ".$Anzahl_Bereiche."\n";
	print " StretchUmlaufAnz:   ".$StretchUmlaufAnz."\n";

	# parse bereiche
	my @bereiche;
	$endReached = 0;
	while($endReached==0) {
		$line = <INDAT>;
		if($line =~ "Bereich" && $line =~ "von_Sekunde" && $line =~ "bis_Sekunde") {
			$endReached = 1;
		}
	}
	$endReached = 0;
	while($endReached==0) {
		$line = <INDAT>;
		$tmp = $line;
		$tmp =~ s/^\s*|\s*$//g;
		if(length($tmp)==0) {
			$endReached = 1;
		} else {
			$line =~ s/\/\/.*$//g;
			$line =~ s/^\s*|\s*$//g;
			($bname, $bbeg, $bend, $bfactor, $bfactor2) = split("\t", $line);
			$val = $bname.";".$bbeg.";".$bend.";".$bfactor2;
			push @bereiche, $val;
		}
	}
	print " ----- bereiche parsed ----\n";
	foreach $b (@bereiche) {
		print " ".$b."\n";
	}
	$bereich_defs{$Name_Signalprogamm} = \@bereiche;


	# parse plan
	my @phases;
	$endReached = 0;
	while($endReached==0) {
		$line = <INDAT>;
		if($line =~ "Signalguppe") {
			$endReached = 1;
		}
	}
	$endReached = 0;
	while($endReached==0) {
		$line = <INDAT>;
		$tmp = $line;
		$tmp =~ s/^\s*|\s*$//g;
		if(length($tmp)==0) {
			$endReached = 1;
		} else {
			$line =~ s/\/\/.*$//g;
			$line =~ s/^\s*|\s*$//g;
			@vals = split("\t", $line);
			@ks = split("\,", $vals[0]);
			foreach $k (@ks) {
				$k =~ s/^\s*|\s*$//g;
				if(substr($k, 0, 1) ne "K") {
					$k = "K".$k;
				}
				$val = $k.";".$vals[2].";".$vals[3].";".$vals[4].";".$vals[5];
				push @phases, $val;
				if($#vals>5) {
					$val = $k.";".$vals[2].";".$vals[3].";".$vals[6].";".$vals[7];
					push @phases, $val;
				}
				if($#vals>7) {
					$val = $k.";".$vals[2].";".$vals[3].";".$vals[8].";".$vals[9];
					push @phases, $val;
				}
			}
		}
	}
	$phase_defs{$Name_Signalprogamm} = \@phases;
	print " ----- phases parsed ----\n";
	foreach $p (@phases) {
		print " ".$p."\n";
	}
	$noRead = $noRead + 1;
}
close INDAT;



# build and write results
open(OUTDAT, "> $ARGV[2]") || die "Could not open '$ARGV[2]'";
print OUTDAT "<additional>\n";

	# build tls-logics
for($i=0; $i<=$#programs; $i++) {
	($Name_Signalprogamm, $Signalprogramm_ID, $Umlaufzeit, $Versatzzeit, $GSP, $Anzahl_Bereiche, $StretchUmlaufAnz) = split("\;", $prog_defs[$i]);
	$tphases = $phase_defs{$Name_Signalprogamm};
	@pphases = @$tphases;
	$tbereiche = $bereich_defs{$Name_Signalprogamm};
	@pbereiche = @$tbereiche;

	# set all to red
	@color = ();
	for($j=0; $j<$noSignals; $j++) {
		$color[$j] = "";
		for($k=0; $k<$Umlaufzeit; $k++) {
			$color[$j] = $color[$j]."R";
		}
	}
	# set green and yellow phases
	foreach $phase (@pphases) {
		($name, $drg, $dg, $a, $e) = split("\;", $phase);
		if(defined($a) && $a!=$e) {
			# set green
			$ue = $e;
			if($e<$a) {
				$ue = $Umlaufzeit;
			}
			for($j=$a; $j<$ue; $j++) {
				$Pindices = $kindices{$name};
				@indices = @$Pindices;
				foreach $index (@indices) {
					$color[$index] = substr($color[$index], 0, $j)."G".substr($color[$index], $j+1);
				}
			}
			if($e<$a) {
				for($j=0; $j<$e; $j++) {
					$Pindices = $kindices{$name};
					@indices = @$Pindices;
					foreach $index (@indices) {
	    					$color[$index] = substr($color[$index], 0, $j)."G".substr($color[$index], $j+1);
					}
				}
			}
			# set yellow
			$by = $e;
			$ey = $by + $dg;
			for($j=$by; $j<$ey; $j++) {
				$Pindices = $kindices{$name};
				@indices = @$Pindices;
				foreach $index (@indices) {
					$color[$index] = substr($color[$index], 0, $j)."Y".substr($color[$index], $j+1);
#if($name eq "K8" && $index==0) {
#	print $color[$index]."\n";
#}
				}
			}
		}
	}
	# join same phases
	@durations = ();
	@phases = ();

#	for($j=0; $j<$noSignals; $j++) {
#		for($k=0; $k<$Umlaufzeit; $k++) {
#			print $j.":".$color[$j]."\n";
#		}
#	}

	$yield = 1;

	$encoded = encodeSUMO(0, $noSignals, $yield);
	push @phases, $encoded;
	push @durations, 1;
	$index = 0;
	for($j=0; $j<$Umlaufzeit-1; $j++) {
		if(nextIsSame($j, $noSignals)==1) {
			$durations[$index] = $durations[$index] + 1;
		} else {
			$encoded = encodeSUMO($j+1, $noSignals, $yield);
			push @phases, $encoded;
			push @durations, 1;
			$index++;
		}
	}

	print OUTDAT "   <tl-logic type=\"static\">\n";
	print OUTDAT "      <key>".$KP_ID."</key>\n";
	print OUTDAT "      <subkey>".$Signalprogramm_ID."</subkey>\n";
	print OUTDAT "      <phaseno>".$#durations."</phaseno>\n";
	print OUTDAT "      <offset>0</offset>\n";
#	print OUTDAT "      <inclanes>".$Name_Signalprogamm."</inclanes>\n";

	for($j=0; $j<=$#pbereiche; $j++) {
		($name, $from, $to, $fac) = split("\;", $pbereiche[$j]);
		print OUTDAT "         <param key=\"".$name.".begin\" value=\"".$from."\"/>\n";
		print OUTDAT "         <param key=\"".$name.".end\" value=\"".$to."\"/>\n";
		print OUTDAT "         <param key=\"".$name.".factor\" value=\"".$fac."\"/>\n";
	}
	print OUTDAT "         <param key=\"StretchUmlaufAnz\" value=\"".$StretchUmlaufAnz."\"/>\n";
	print OUTDAT "         <param key=\"GSP\" value=\"".$GSP."\"/>\n";

	for($j=0; $j<=$#durations; $j++) {
		print OUTDAT "         <phase duration=\"".$durations[$j]."\" ".$phases[$j]."/>\n";
	}
	print OUTDAT "   </tl-logic>\n";
	print OUTDAT "\n";
}

print OUTDAT "</additional>\n";
close(OUTDAT);

