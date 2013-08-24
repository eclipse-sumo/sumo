<?php
require_once('./database.php');

function query($query) {
    global $handle, $database;
    $res = mysql($database, $query, $handle);
    if (mysql_errno() > 0)
        echo mysql_errno().": ".mysql_error()." ($query)<br>";
    return $res;
}

$gameIDs = array();
$res = query("SELECT * FROM games");
while ($row = mysql_fetch_assoc($res)):
    $gameID = $row["gameID"];
    $gameIDs[$row["title"]] = $gameID;
    $titles[$row["gameID"]] = $row["fulltitle"];
endwhile;
mysql_free_result($res);

if (!isset($_GET["game"])):
    $_GET["game"]="TLS";
endif;

$gameID = $gameIDs[$_GET["game"]];
if (isset($_GET["name"], $_GET["category"], $_GET["points"])):
    query("INSERT highscore VALUES(0, $gameID, '" . $_GET["category"] . "', " . $_GET["points"] . ", '" . $_GET["name"] . "', '" . $_GET["instance"] . "', '')");
    exit();
endif;

if (isset($_GET["top"])):
    $res = query("SELECT * FROM highscore WHERE gameID='$gameID' ORDER BY category, points DESC, id");
    $cat = "";
    while ($row = mysql_fetch_assoc($res)):
        if ($row[category] != $cat):
            if ($cat != ""):
                echo "\n";
            endif;
            $cat = $row[category];
            echo "$cat $row[name],$row[instance],$row[points]";
            $count = $_GET["top"]-1;
        else:
            if ($count > 0):
                echo ":$row[name],$row[instance],$row[points]";
                $count--;
            endif;
        endif;
    endwhile;
    exit();
endif;

$res = query("SELECT DISTINCT category FROM highscore WHERE gameID=$gameID ORDER BY category");
if (mysql_num_rows($res) == 0):
    print("Es sind momentan keine Eintr&auml;ge f&uuml;r dieses Spiel vorhanden.");
    exit();
endif;

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

<head>
 <title>SUMO - Status of daily build and tests</title>
 <link rel="stylesheet" type="text/css" href="sumo.css"/>

  <link rel="schema.DC" href="http://purl.org/dc/elements/1.1/" />
  <META NAME="DC.Title" CONTENT="SUMO - Status of daily build and tests">
  <META NAME="DC.Subject" CONTENT="road traffic simulation package SUMO (Simulation of Urban MObility)">
  <META NAME="DC.Description" CONTENT="">
  <META NAME="DC.Publisher" CONTENT="Institute of Transportation Systems at the German Aerospace Center">
  <META NAME="DC.Type" CONTENT="Text">
  <META NAME="DC.Format" SCHEME="IMT" CONTENT="text/html">
  <META NAME="DC.Identifier" CONTENT="http://sumo-sim.org">
  <META NAME="DC.Language" SCHEME="ISO639-1" CONTENT="en">
  <META NAME="DC.Relation" SCHEME="URL" CONTENT="http://sumo-sim.org/index.html">
  <META NAME="DC.Rights" CONTENT="(c) ITS/DLR">
  <META NAME="DC.Date.X-MetadataLastModified" SCHEME="ISO8601" CONTENT="2011-03-04">
  <META http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
</head>

<body id="bmission">
<div id="container">



 <div id="logo" align="left"><img src="logo.png" width="128" height="128" align="left"
     alt="SUMO-Simulation of Urban Mobility" style="margin-left:10px;
     margin-right:20px;"/>
     <div id="logo_text1"> SUMO </div>
     <div id="logo_text2"> Simulation of Urban MObility</div>
 </div>

 <div id="subsMenu"><ul>
  <li class="sub"><a href="/">Home</a></li>
  <li class="sub"><a href="/wiki/Downloads">Download</a></li>
  <li class="sub"><a href="/userdoc/">Documentation</a></li>
  <li class="sub"><a href="/wiki/">Wiki</a></li>
  <li class="sub"><a href="/trac/">Trac</a></li>
  <li class="sub"><a href="/blog/">Blog</a></li>
  <li class="sub"><a href="/userdoc/Contact.html">Contact</a></li>
  <li class="sub"><a href="http://sourceforge.net/projects/sumo/">SF-Project</a></li>
 </ul></div>

<h2>Das Spiel</h2>
<p>Auf dieser Seite pr&auml;sentieren wir das Ampelspiel zur Langen Nacht der Wissenschaften.</p>
<p>Sie k&ouml;nnen das vollst&auml;ndige <a href="sumogame.zip">Spiel herunterladen</a> und unten
auch die globale Highscoreliste sehen.
Zum Spielen sollte das heruntergeladene Archiv in einen Ordner entpackt werden und die darin enthaltene 
Datei runner.exe gestartet werden.
Anschlie&szlig;end geht ein winziges Fensterchen auf in dem die Sie w&auml;hlen k&ouml;nnen, ob eine Einzelkreuzung oder vier Kreuzungen gespielt werden.
Durch einen Mausklick in der N&auml;he der gew&auml;hlten Ampel wird die n&auml;chste Fahrtrichtung freigegeben.
Spielzeit sind 180 Simulationssekunden, danach gibt es f&uuml;r jeden Milliliter gesparten Kraftstoff einen Punkt
und man kommt in die Highscores (oder eben nicht).
</p>

<hr class="thin"/>

  <form method="get" action="scores.php">
  <input type="hidden" name="game" value="<?php echo htmlentities($_GET["game"])?>"/>
  <h2>Bestenliste f&uuml;r
  <select name="category" onchange="this.form.submit()" style="font-weight:bold;font-size:0.9em;padding:0;margin:0;">
<?php
$found = false;
while ($row = mysql_fetch_array($res)):
    $cat = $row["category"];
    if ($_GET["category"]==$cat)
        $found = true;
    echo "<option value=\"$cat\"".($_GET["category"]==$cat ? " selected=\"selected\"" : "").">$cat</option>\n";
endwhile;
if (!$found)
    $_GET["category"] = mysql_result($res, 0, "category");
mysql_free_result($res);
?>
  </select>
  </h2>
  </form>

<table>
  <tr>
    <th>Platz</th>
    <th>Punkte</th>
    <th>Name</th>
</tr>
<?php
$res = query("SELECT * FROM highscore ".
             "WHERE gameID='$gameID' AND category='" . $_GET["category"] . "' ORDER BY points DESC, id");
$i = 1;
while ($row = mysql_fetch_assoc($res))
    echo "<tr class=\"".($i%2 ? "odd" : "even")."\"><td>".$i++."</td><td>$row[points]</td><td>$row[name]</td></tr>\n";
mysql_free_result($res);
?>
</table>
 </div>

 <div id="footer">
   <div>(c) 2011-2013, German Aerospace Center, Institute of Transportation Systems</div>
   <div>Layout based on <a href="http://www.oswd.org/design/preview/id/3365">"Three Quarters"</a> by "SimplyGold"</div>
 </div>
                                                       
 
</div></body>

</html>
