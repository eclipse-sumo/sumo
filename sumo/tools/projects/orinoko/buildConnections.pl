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


$spurattrs = "Spurattribute.csv";
if(defined($ARGV[0])) {
	$spurattrs = $ARGV[0];
}


open(INDAT, "< $spurattrs");
while(<INDAT>) {
	$line = <INDAT>;
	($Datum_Dateneingabe, $LSA_ID, $Knoten_ID, $Fahrstreifen_Nr, $RistkVon_Ref, $RistkBis_Ref, $Fahrstreifen_Laenge, $Signalgr_Nr, $Signalgr_Bez, $Spur_Typ) = split(";", $line);
	if(substr($Datum_Dateneingabe, 0, 1) ne "!" && $RistkVon_Ref eq $RistkBis_Ref) {
		$RistkVon_Ref =~ s/^\s|\s$|\.//g;
		$between{$RistkVon_Ref} = 1;
	}
}
close(INDAT);


# write connections
open(INDAT, "< $spurattrs");
open(OUTDAT, "> connections.con.xml");
print OUTDAT "<connections>\n";
$lastRef = "";
$lastOffset = 0;
$lastLSA = "";
$lastLine = "";
$tmp = <INDAT>;
$ok = 1;
while($ok==1) {
	$line = <INDAT>;
	if($line eq "") {
		$ok = 0;
		$line = "111;-1";
	}
#	$line = <INDAT>;
	($Datum_Dateneingabe, $LSA_ID, $Knoten_ID, $Fahrstreifen_Nr, $RistkVon_Ref, $RistkBis_Ref, $Fahrstreifen_Laenge, $Signalgr_Nr, $Signalgr_Bez, $Spur_Typ) = split(";", $line);
	if(substr($Datum_Dateneingabe, 0, 1) ne "!") {
		$hadSameConnection2 = 0;
		if($lastLine eq $line) {
			$hadSameConnection2 = 1;
			$sameConnectionOffset = $sameConnectionOffset + 1;
		} else {
			$sameConnectionOffset = 0;
		}

		$lastLine = $line;

		$RistkVon_Ref =~ s/^\s|\s$|\.//g;
		$RistkBis_Ref =~ s/^\s|\s$|\.//g;
		if($RistkVon_Ref ne $lastRef) {
			$lastOffset = $Fahrstreifen_Nr;
			$lastRef = $RistkVon_Ref;
			$lastDestLane = 0;
			$lastDestEdge = "";
		}
		if($RistkVon_Ref ne "" && defined($between{$RistkVon_Ref}) && $between{$RistkVon_Ref}==1 && $RistkVon_Ref ne $RistkBis_Ref) {
			$RistkVon_Ref = $RistkVon_Ref."/s";
		}

		if($Signalgr_Nr ne "") {
			if(defined($controlledInner{$LSA_ID})) {
				$controlledInner{$LSA_ID} = $controlledInner{$LSA_ID}.";".$RistkVon_Ref;
			} else {
				$controlledInner{$LSA_ID} = $RistkVon_Ref;
			}
		}


		$hadSameConnection = 0;
		if($RistkBis_Ref ne $lastDestEdge) {
			$lastDestLane = 0;
			$lastDestEdge = $RistkBis_Ref;
		} else {
			if($lastFahrstreifen_Nr ne $Fahrstreifen_Nr) {
				$lastDestLane = $lastDestLane + 1;
			} else {
				if($Signalgr_Nr ne $last_Signalgr_Nr) {
					$hadSameConnection = 1;
				} else {
					$lastDestLane = $lastDestLane + 1;
				}
#				$hadSameConnection = 1;
			}
		}
		$lastFahrstreifen_Nr = $Fahrstreifen_Nr;
		$last_Signalgr_Nr = $Signalgr_Nr;

	        # close a previously began tls-definition
		if($LSA_ID ne $lastLSA && $lastLSA ne "") {
			print $LSA_ID."<->".$lastLSA."\n";
			print OUTDAT2 "\n";													
			print OUTDAT2 "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n";
			print OUTDAT2 "\n";		
			print OUTDAT2 "Signalprogrammdaten\n";
									
			open(INDAT2, "< SG-Daten.csv");
			$lastProgID = -1;
			while(<INDAT2>) {
				$tmpo = $_;
				($nLSA_ID, $nSignalprogramm_ID, $nSignalgr_Nr, $nRE, $nGE, $nrotgelb, $ngelb) = split("\;", $tmpo);
				$ngelb =~ s/\n//g;
				if($nLSA_ID eq $lastLSA) {
					if($nSignalprogramm_ID!=$lastProgID) {
						print OUTDAT2 "\n";
						print OUTDAT2 "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n";
						print OUTDAT2 "\n";
						print OUTDAT2 "Name Signalprogamm	S".$nSignalprogramm_ID."\n";
						print OUTDAT2 "Signalprogramm-ID	S".$nSignalprogramm_ID."\n";
						print OUTDAT2 "Umlaufzeit [s]	".$Umlaufzeiten{$nSignalprogramm_ID}."\n";
						print OUTDAT2 "Versatzzeit	0\n";
						print OUTDAT2 "GSP	".$GSPs{$nSignalprogramm_ID}."\n";
						print OUTDAT2 "Anzahl_Bereiche	2				//STRETCH-Parameter\n";				
						print OUTDAT2 "StretchUmlaufAnz	3				//STRETCH-Parameter\n";				
						print OUTDAT2 "\n";
						print OUTDAT2 "Bereich	von_Sekunde	bis_Sekunde		Faktor	//STRETCH-Parameter\n";				
						print OUTDAT2 "B1	60	70		2	//STRETCH-Parameter\n";
						print OUTDAT2 "B2	32	34		1	//STRETCH-Parameter\n";				
						print OUTDAT2 "\n";
						print OUTDAT2 "Signalguppe		DRG	DG	FA1	FE1	FA2	FE2	FA3	FE3\n";
						$lastProgID = $nSignalprogramm_ID;
					}
					if( ($nRE!=0||$nGE!=0) && ($nRE!=127||$nGE!=1) && ($nRE!=0||$nGE!=1) && ($nRE!=0||$nGE!=0) ) {
						$FA = $nRE + $nrotgelb;
						if($FA>$Umlaufzeiten{$nSignalprogramm_ID}) {
							$FA = $FA - $Umlaufzeiten{$nSignalprogramm_ID};
						}
						print OUTDAT2 "K".$nSignalgr_Nr."\t\t".$nrotgelb."\t".$ngelb."\t".($FA)."\t".$nGE."\n";
					}
				}
			}
			close(INDAT2);
			close(OUTDAT2);
		}

		# begin a new tls-definition if it's a new one
		if($LSA_ID ne $lastLSA) {
			open(INDAT2, "< Signalprogramm-Daten.csv");
			$signalProgs = "";
			$noSignalProgs = 0;
			while(<INDAT2>) {
				($nLSA_ID, $nSignalprogramm_ID, $nUmlaufzeit, $nGSP) = split("\;", $_);
				if($nLSA_ID eq $LSA_ID) {
					if($signalProgs ne "") {
						$signalProgs = $signalProgs.";";
					}
					$signalProgs = $signalProgs."S".$nSignalprogramm_ID;
					$noSignalProgs = $noSignalProgs + 1;
					$Umlaufzeiten{$nSignalprogramm_ID} = $nUmlaufzeit;
					$GSPs{$nSignalprogramm_ID} = $nGSP;
				}
			}
			close(INDAT2);
			$lastLSA = $LSA_ID;
			if($LSA_ID != -1) {
				$name = $LSA_ID.".prg.txt";
				open(OUTDAT2, "> $name");
				print OUTDAT2 "Kreuzungsdaten\n";
				print OUTDAT2 "\n";
				print OUTDAT2 "Knotenpunkt - Bezeichnung:	Breslauer Str. / Gleiwitzer Str.				// Angabe optional - nur zur Visualisierung in gui notwendig\n";
				print OUTDAT2 "Knotenpunkt - Nummer:	".$LSA_ID."				// Angabe optional - nur zur Visualisierung in gui notwendig\n";	
				print OUTDAT2 "KP-ID:	".$LSA_ID."				// Angabe der Kreunzungs-ID - Auf Eindeutigkeit achten!	\n";				
				print OUTDAT2 "Anzahl Signalprogramme:	".$noSignalProgs."				// Anzahl der zu verwendenden Signalprogramme dieser Kreuzung\n";
				print OUTDAT2 "Signalprogramme:	\"".$signalProgs."\"				// Angabe aller Signalprogramm-ID's der an dieser Kreuzung zu verwendenden Signalprogramme.// Hier nicht genannte Signalprogramme werden nicht benutzt!\n";
				print OUTDAT2 "Umschaltverfahren:	GSP				// Bezeichnung des an dieser Kreuzung zu verwendenden LSA-Umschalteverfahrens\n";
				print OUTDAT2 "Synchronität:	ja				// Angabe, ob Synchronität bezüglich der in der WAUT angegebenen Refernzzeit erforderlich ist.\n";
				print OUTDAT2 "WAUT-ID:	W01_IST				// ID der an dieser Kreuzung zu verwendenden WAUT\n";	
				print OUTDAT2 "\n";
				print OUTDAT2 "Zuordnungstabelle Signalgruppen <--> Verkehrsströme\n";
				print OUTDAT2 "Signalgruppe	Lane_von	Nach			// Lane_von = Lane auf der sich die SG befindet\n";
			}
            
		}
		$lastLSA = $LSA_ID;
	
		$lane = $Fahrstreifen_Nr - $lastOffset;
		if($RistkVon_Ref eq $RistkBis_Ref) {
			$RistkBis_Ref = $RistkBis_Ref."/s";
			$splitK{$RistkVon_Ref} = $LSA_ID;
			$splitP{$RistkVon_Ref} = $Fahrstreifen_Laenge;
		}
		if($RistkVon_Ref ne "" && $hadSameConnection==0) {
			if($Signalgr_Nr ne "") {
				print OUTDAT "   <connection from=\"".$RistkVon_Ref."\" to=\"".$RistkBis_Ref."\" lane=\"".$lane.":".$lastDestLane."\"/>\n";
			} else {
				print OUTDAT "   <connection from=\"".$RistkVon_Ref."\" to=\"".$RistkBis_Ref."\" lane=\"".$lane.":".$lastDestLane."\" uncontrolled=\"1\"/>\n";
			}
		}
		if($Signalgr_Nr ne "") {
			print OUTDAT2 "K".$Signalgr_Nr."\t".$RistkVon_Ref."_".$lane."\t".$RistkBis_Ref."_".$lastDestLane."\n";
		}
		$laneID = $RistkVon_Ref."_".$lane;

		$lengths{$laneID} = $Fahrstreifen_Laenge;
		$edges{$RistkVon_Ref} = 1;

		# patch lane number information
		if(defined($lanes{$RistkVon_Ref})) {
			if($lanes{$RistkVon_Ref}<$lane) {
				$lanes{$RistkVon_Ref} = $lane;
			}
		} else {
			$lanes{$RistkVon_Ref} = $lane;
		}
		# patch lane number information
		if(defined($lanes{$RistkBis_Ref})) {
			if($lanes{$RistkBis_Ref}<$lastDestLane) {
				$lanes{$RistkBis_Ref} = $lastDestLane;
			}
		} else {
			$lanes{$RistkBis_Ref} = $lastDestLane;
		}

		# store maximum edge length
		if(defined($elengths{$RistkVon_Ref})) {
			if($elengths{$RistkVon_Ref}<$Fahrstreifen_Laenge) {
				$elengths{$RistkVon_Ref} = $Fahrstreifen_Laenge;
			}
		} else {
			$elengths{$RistkVon_Ref} = $Fahrstreifen_Laenge;
		}
if($RistkVon_Ref eq "-547337856" || $RistkBis_Ref eq "-547337856") {
	print $lengths{$laneID}." - ".$elengths{$RistkVon_Ref}."\n";
}

		# store type information
		if(defined($type{$RistkVon_Ref})) {
			if($type{$RistkVon_Ref}==9) {
				$type{$RistkVon_Ref} = $Spur_Typ;
			}
		} else {
			$type{$RistkVon_Ref} = $Spur_Typ;
		}

		$Knoten_ID =~ s/^\s|\s$|\.//g;
		$lights{$Knoten_ID} = $LSA_ID;

	}
}
print OUTDAT "</connections>\n";
close(OUTDAT);
close(INDAT);



open(INDAT, "< lanes.txt");
while(<INDAT>) {
	$line = $_;
	if(index($line, "\:")>=0 && length($line)>0 && substr($line[0], 0, 1) ne "!") {
		($edge, $laneno) = split("\:", $line);
		if(index($edge, "_")>0) {
			$laneno =~ s/\s//g;
			$lengths{$edge} = $laneno;
			$edge = substr($edge, 0, index($edge, "_"));
			if(defined($elengths{$edge})) {
				if($elengths{$edge}<$laneno) {
					$elengths{$edge} = $laneno;
				}
			} else {
				$elengths{$edge} = $laneno;
			}
		} else {
			$laneno =~ s/\s//g;
			$lanes{$edge} = $laneno - 1;
		}
		$edges{$edge} = 1;
	}
}
close(INDAT);

# patch edges
open(INDAT, "< nuernberg_vls.edg.xml");
open(OUTDAT, "> nuernberg_vls.edg.xml.new.xml");
$skipNext = 0;
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "<edge ") {
		$skipNext = 0;
		$id = getAttr($tmp, "id");
		if(defined($edges{$id})) {
			$length = getAttr($tmp, "length");
			$tmp =~ s/\/\>/\>/g;
			$laneno = $lanes{$id} + 1;
			$tmp =~ s/nolanes=\"(.*?)\"/nolanes=\"$laneno\"/g;
			if(defined($splitK{$id})) {
				$tmp =~ s/shape=\"(.*?)\"//g;
				$tmp2 = $tmp;
				$tmp2 =~ s/tonode=\"(.*?)\"/tonode=\"$id\/s\"/g;
				print OUTDAT $tmp2;
			} else {
				print OUTDAT $tmp;
			}
			$lane = 0;

			if($type{$id}!=9) {
				$skipNext = 1;
				while(defined($lengths{$id."_".$lane})) {
					if($lengths{$id."_".$lane}<$elengths{$id}) {
						print OUTDAT "      <lane id=\"".$lane."\" forceLength=\"".($lengths{$id."_".$lane})."\"/>\n";
					}
					$lane = $lane + 1;
				}
			}
			print OUTDAT "   </edge>\n";
			if(defined($splitK{$id})) {
				$splitF{$id} = getAttr($tmp, "fromnode");
				$splitT{$id} = getAttr($tmp, "tonode");
				$laneno = $lanes{$id."/s"} + 1;
				$tmp =~ s/nolanes=\"(.*?)\"/nolanes=\"$laneno\"/g;
				$tmp =~ s/fromnode=\"(.*?)\"/fromnode=\"$id\/s\"/g;
				$tmp =~ s/id=\"(.*?)\"/id=\"$id\/s\"/g;
				print OUTDAT $tmp;
				if($type{$id}!=9) {
					$skipNext = 1;
					while(defined($lengths{$id."/s_".$lane})) {
						if($lengths{$id."/s_".$lane}<$elengths{$id."/s"}) {
							print OUTDAT "      <lane id=\"".$lane."\" forceLength=\"".($lengths{$id."/s_".$lane})."\"/>\n";
						}
						$lane = $lane + 1;
					}
				}
				print OUTDAT "   </edge>\n";
			}
		} else {
			if(defined($lanes{$id})) {
				$laneno = $lanes{$id} + 1;
				$tmp =~ s/nolanes=\"(.*?)\"/nolanes=\"$laneno\"/g;
			}
			print OUTDAT $tmp;
		}
	} else {
		if($tmp =~ "</edges" || $skipNext==0) {
			print OUTDAT $tmp;
		}
	}
}
close(OUTDAT);
close(INDAT);

# read controlled inner edges
#open(INDAT, "< controlledInner.txt");
#while(<INDAT>) {
#	$line = $_;
#	if(index($line, "\:")>=0 && length($line)>0 && substr($line[0], 0, 1) ne "!") {
#		($tls, $edge) = split("\:", $line);
#		$edge =~ s/\s//g;
#		if(defined($controlledInner{$tls})) {
#			$controlledInner{$tls} = $controlledInner{$tls}.";".$edge;
#		} else {
#			$controlledInner{$tls} = $edge;
#		}
#	}
#}
#close(INDAT);


# write nodes 
open(INDAT, "< nuernberg_vls.nod.xml");
open(OUTDAT, "> nuernberg_vls.nod.xml.new.xml");
print OUTDAT "<nodes>\n";
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "<node ") {
		$id = getAttr($tmp, "id");
		$nodex{$id} = getAttr($tmp, "x");
		$nodey{$id} = getAttr($tmp, "y");

		if(defined($lights{$id})) {
			$tmp =~ s/\/\>.*//g;
			$tmp = $tmp." type=\"traffic_light\" tl=\"".$lights{$id}."\" controlled_inner=\"".$controlledInner{$lights{$id}}."\"/>\n";
		}
		print OUTDAT $tmp;
	}
}
close(INDAT);
foreach $id (keys(%splitK)) {
	if($id ne "") {
#		print $id."\n";
#		print "a: ".$splitF{$id}." ".$splitT{$id}."\n";
#		print "b: ".$nodex{$splitF{$id}}." ".$nodey{$splitF{$id}}."\n";
#		print "c: ".$nodex{$splitT{$id}}." ".$nodey{$splitT{$id}}."\n";
		$xq = ($nodex{$splitF{$id}}-$nodex{$splitT{$id}});
		$yq = ($nodey{$splitF{$id}}-$nodey{$splitT{$id}});
#		print "xq: ".$xq."\n";
#		print "yq: ".$yq."\n";
		$xq = $xq * $xq;
		$yq = $yq * $yq;
		$len = sqrt($xq + $yq);
#		print "xq: ".$xq."\n";
#		print "yq: ".$yq."\n";
#		print "len: ".$len."\n";
		$x = $nodex{$splitF{$id}} + (($nodex{$splitT{$id}} - $nodex{$splitF{$id}}) / $len * $splitP{$id});
		$y = $nodey{$splitF{$id}} + (($nodey{$splitT{$id}} - $nodey{$splitF{$id}}) / $len * $splitP{$id});

		print OUTDAT " <node id=\"".$id."/s\" x=\"".$x."\" y=\"".$y."\" type=\"traffic_light\" tl=\"".$splitK{$id}."\"/>\n";
	}
}
print OUTDAT "</nodes>\n";
close(OUTDAT);

