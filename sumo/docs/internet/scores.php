<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
	<link rel="stylesheet" type="text/css" href="./css/sumo.css">
	<head>
		<title>SUMO - Simulation of Urban MObility  -  Home</title>
<link rel="schema.DC" href="http://purl.org/dc/elements/1.1/" />
<META NAME="DC.Title" CONTENT="SUMO - Simulation of Urban MObility  -  Home">
<META NAME="DC.Creator.PersonalName" CONTENT="Krajzewicz, Daniel">
<META NAME="DC.Subject" CONTENT="road traffic simulation package SUMO (Simulation of Urban MObility)">
<META NAME="DC.Description" CONTENT="">
<META NAME="DC.Publisher" CONTENT="Institute of Transport Research at the German Aerospace Centre">
<META NAME="DC.Contributor" CONTENT="Center for Applied Informatics, Cologne">
<META NAME="DC.Type" CONTENT="Text">
<META NAME="DC.Format" SCHEME="IMT" CONTENT="text/html">
<META NAME="DC.Identifier" CONTENT="http://sumo.sourceforge.net">
<META NAME="DC.Language" SCHEME="ISO639-1" CONTENT="en">
<META NAME="DC.Relation" SCHEME="URL" CONTENT="http://sumo.sourceforge.net/index.html">
<META NAME="DC.Coverage" CONTENT="Introduction, Navigation">
<META NAME="DC.Rights" CONTENT="(c) IFS/DLR">
<META NAME="DC.Date.X-MetadataLastModified" SCHEME="ISO8601" CONTENT="2003-07-10">

	</head>
	<body>

        <table border="0">
          <tr>
              <td><img src="./images/64x64_web.gif" width="64" height="64"></td>
              <td><img src="./images/sumo_logo.gif" width="72" height="17"><br>
              <img src="./images/sumo_full.gif" width="211" height="16"></td>
          </tr>
          </table>
		<hr>


<!-- Navigation bar begin -->
		<table border="0" ID="Table1">
		<tr><td valign="top" width="120">
		<div class="navbar">
			<a href="index.shtml" class="SUMOMainLink">Home</a><br>
			<a href="docs/documentation.shtml" class="SUMOMainLink">Documentation</a><br>
			<a href="downloads/downloads.shtml" class="SUMOMainLink">Downloads</a><br>
			<a href="contact.shtml" class="SUMOMainLink">Contact</a><br>

			<a href="http://sumo.sourceforge.net/wiki/" class="SUMOMainLink">wiki</a><br/>

			<center><br>
			<a href="http://www.dlr.de/ts/" class="SUMOMainLink">
            <img src="./images/logo_dlr.gif" width="40" height="42"></a>
<br>

			<a href="http://sourceforge.net/projects/sumo/"><img src="http://sourceforge.net/sflogo.php?group_id=45607&type=1" width="88" height="31" border="0" alt="SourceForge.net Logo"></a>
			</center>
		</div>
		</td><td width="20"></td>
<!-- Navigation bar end -->


		<td valign="top">
		<div class="SUMOMainText">
<!-- Text area here -->
<h1>SUMO - <u>S</u>imulation of <u>U</u>rban <u>MO</u>bility</h1>
<h2>Das Spiel</h2>
<p>Auf dieser Seite pr&auml;sentieren wir das Ampelspiel zur Langen Nacht der Wissenschaften.</p>
<p>Sie k&ouml;nnen das vollst&auml;ndige <a href="sumogame.zip">Spiel herunterladen</a> und unten
auch die globale Highscoreliste sehen.
Zum Spielen sollte das heruntergeladene Archiv in einen Ordner entpackt werden und die darin enthaltene 
Datei runner.exe gestartet werden.
Anschließend geht ein winziges Fensterchen auf in dem die Sie wählen können, ob eine Einzelkreuzung oder vier Kreuzungen gespielt werden.
Durch einen Mausklick in der Nähe der gewählten Ampel wird die nächste Fahrtrichtung freigegeben.
Spielzeit sind 180 Simulationssekunden, danach gibt es für jeden Milliliter gesparten Kraftstoff einen Punkt
und man kommt in die Highscores (oder eben nicht).
</p>

<hr class="thin"/>

<?php
require_once('./database.php');
import_request_variables("g", "g_");

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

if (!isset($g_game)):
    $g_game="TLS";
endif;

$gameID = $gameIDs[$g_game];
if (isset($g_name, $g_category, $g_points)):
    query("INSERT highscore VALUES(0, $gameID, '$g_category', $g_points, '$g_name', '$g_instance', '')");
    exit();
    endif;

if (isset($g_top)):
    $res = query("SELECT * FROM highscore WHERE gameID='$gameID' ORDER BY category, points DESC, id");
    $cat = "";
    while ($row = mysql_fetch_assoc($res)):
        if ($row[category] != $cat):
            if ($cat != ""):
                echo "\n";
            endif;
            $cat = $row[category];
            echo "$cat $row[name],$row[instance],$row[points]";
            $count = $g_top-1;
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
  <form method="get" action="scores.php">
  <input type="hidden" name="game" value="<?php echo htmlentities($g_game)?>"/>
  <h2>Bestenliste f&uuml;r
  <select name="category" onchange="this.form.submit()" style="font-weight:bold;font-size:0.9em;padding:0;margin:0;">
<?php
$found = false;
while ($row = mysql_fetch_array($res)):
    $cat = $row["category"];
    if ($g_category==$cat)
        $found = true;
    echo "<option value=\"$cat\"".($g_category==$cat ? " selected=\"selected\"" : "").">$cat</option>\n";
endwhile;
if (!$found)
    $g_category = mysql_result($res, 0, "category");
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
             "WHERE gameID='$gameID' AND category='$g_category' ORDER BY points DESC, id");
$i = 1;
while ($row = mysql_fetch_assoc($res))
    echo "<tr class=\"".($i%2 ? "odd" : "even")."\"><td>".$i++."</td><td>$row[points]</td><td>$row[name]</td></tr>\n";
mysql_free_result($res);
?>
</table>

	</body>
</html>
