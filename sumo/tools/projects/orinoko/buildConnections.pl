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



# write connections
open(INDAT, "< Spurattribute.csv");
open(OUTDAT, "> connections.con.xml");
print OUTDAT "<connections>\n";
$lastRef = "";
$lastOffset = 0;
$lastLSA = "";
$lastLine = "";
$tmp = <INDAT>;
while(<INDAT>) {
	$line = $_;
	($Datum_Dateneingabe, $LSA_ID, $Knoten_ID, $Fahrstreifen_Nr, $RistkVon_Ref, $RistkBis_Ref, $Fahrstreifen_Laenge, $Signalgr_Nr, $Signalgr_Bez, $Spur_Typ) = split(";", $line);
	$SignalgrTest = $Signalgr_Bez;
	$SignalgrTest =~ s/\d//g;
	if(substr($Datum_Dateneingabe, 0, 1) ne "!" && $line ne $lastLine && $SignalgrTest eq "") {
		$lastLine = $line;

		$RistkVon_Ref =~ s/^\s|\s$|\.//g;
		$RistkBis_Ref =~ s/^\s|\s$|\.//g;
		if($RistkVon_Ref ne $lastRef) {
			$lastOffset = $Fahrstreifen_Nr;
			$lastRef = $RistkVon_Ref;
			$lastDestLane = 0;
			$lastDestEdge = "";
		}
		if($RistkBis_Ref ne $lastDestEdge) {
			$lastDestLane = 0;
			$lastDestEdge = $RistkBis_Ref;
		} else {
			$lastDestLane = $lastDestLane + 1;
		}

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
                    if($nRE!=0) {
                        $FA = $nRE+$ngelb;
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
                }
			    $Umlaufzeiten{$nSignalprogramm_ID} = $nUmlaufzeit;
			    $GSPs{$nSignalprogramm_ID} = $nGSP;
            }
		    close(INDAT2);
		    $lastLSA = $LSA_ID;
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
        $lastLSA = $LSA_ID;

		$lane = $Fahrstreifen_Nr - $lastOffset;
		print OUTDAT "   <connection from=\"".$RistkVon_Ref."\" to=\"".$RistkBis_Ref."\" lane=\"".$lane.":".$lastDestLane."\"/>\n";
		print OUTDAT2 "K".$Signalgr_Nr."\t".$RistkVon_Ref."_".$lane."\t".$RistkBis_Ref."_".$lastDestLane."\n";
		$laneID = $RistkVon_Ref."_".$lane;
		$lengths{$laneID} = $Fahrstreifen_Laenge;
		$edges{$RistkVon_Ref} = 1;
		if(defined($lanes{$RistkVon_Ref})) {
			if($lanes{$RistkVon_Ref}<$lane) {
				$lanes{$RistkVon_Ref} = $lane;
			}
		} else {
			$lanes{$RistkVon_Ref} = $lane;
		}
		if(defined($elengths{$RistkVon_Ref})) {
			if($elengths{$RistkVon_Ref}<$Fahrstreifen_Laenge) {
				$elengths{$RistkVon_Ref} = $Fahrstreifen_Laenge;
			}
		} else {
			$elengths{$RistkVon_Ref} = $Fahrstreifen_Laenge;
		}
		if(defined($type{$RistkVon_Ref})) {
			if($type{$RistkVon_Ref}==9) {
				$type{$RistkVon_Ref} = $Spur_Typ;
			}
		} else {
			$type{$RistkVon_Ref} = $Spur_Typ;
		}
#		print $RistkVon_Ref.": ".$lane."\n";
		$Knoten_ID =~ s/^\s|\s$|\.//g;
		$lights{$Knoten_ID} = $LSA_ID;
	}
}
print OUTDAT "</connections>\n";
close(OUTDAT);
close(INDAT);



# patch edges
open(INDAT, "< nuernberg_vls.edg.xml");
open(OUTDAT, "> nuernberg_vls.edg.xml.new.xml");
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "<edge ") {
		$id = getAttr($tmp, "id");
		if(defined($edges{$id})) {
			$length = getAttr($tmp, "length");
			$tmp =~ s/\/\>/\>/g;
			$laneno = $lanes{$id} + 1;
			$tmp =~ s/nolanes=\"(.*?)\"/nolanes=\"$laneno\"/g;
			print OUTDAT $tmp;
			$lane = 0;
			if($type{$id}!=9) {
				while(defined($lengths{$id."_".$lane})) {
					if($lengths{$id."_".$lane}<$elengths{$id}) {
						print OUTDAT "      <lane id=\"".$lane."\" forceLength=\"".($lengths{$id."_".$lane})."\"/>\n";
					}
					$lane = $lane + 1;
				}
			}
			print OUTDAT "   </edge>\n";
		} else {
			print OUTDAT $tmp;
		}
	} else {
		print OUTDAT $tmp;
	}
}
close(OUTDAT);
close(INDAT);


# write nodes
open(INDAT, "< nuernberg_vls.nod.xml");
open(OUTDAT, "> nuernberg_vls.nod.xml.new.xml");
while(<INDAT>) {
	$tmp = $_;
	if($tmp =~ "<node ") {
		$id = getAttr($tmp, "id");
		if(defined($lights{$id})) {
			$tmp =~ s/\/\>.*//g;
			$tmp = $tmp." type=\"traffic_light\" tl=\"".$lights{$id}."\"/>\n";
		}
	}
	print OUTDAT $tmp;
}
close(OUTDAT);
close(INDAT);

