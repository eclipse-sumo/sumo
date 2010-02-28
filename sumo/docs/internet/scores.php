<?php
extract($_GET);
$handle = mysql_connect(localhost, wwwrun) or print("Konnte Datenbank-Verbindung nicht herstellen!");
$database = "games";

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
  $gameIDs[$row["title"]] = $row["gameID"];
  $titles[$row["gameID"]] = $row["fulltitle"];
endwhile;
mysql_free_result($res);

$gameID = $gameIDs[strtolower($game)];

if (!$gameID):
  virtual("header.html?Bestenliste: $game");
  echo "<span class=\"wichtig\">Fehler:</span> Unbekanntes Spiel.";
  virtual('footer.html');
  exit;
endif;

virtual("header.html?".$titles[$gameID]);
$res = query("SELECT DISTINCT category FROM highscore WHERE gameID='$gameID' ORDER BY category");
if (mysql_num_rows($res) == 0):
  echo "Es sind momentan keine Eintr&auml;ge f&uuml;r dieses Spiel vorhanden.";
  virtual('footer.html');
  exit;
endif;

?>
  <form method="get" action="scores.php">
  <input type="hidden" name="game" value="<?php echo htmlentities($game)?>"/>
  <h2>Bestenliste f&uuml;r
  <select name="category" onchange="this.form.submit()" style="font-weight:bold;font-size:0.9em;padding:0;margin:0;">
<?php
$gefunden = false;
while ($row = mysql_fetch_array($res)):
  $cat = $row["category"];
  if ($category==$cat)
    $gefunden = true;
  echo "<option value=\"$cat\"".($category==$cat ? " selected=\"selected\"" : "").">$cat</option>\n";
endwhile;
if (!$gefunden)
  $category = mysql_result($res, 0, "category");
mysql_free_result($res);
?>
  </select>
  Knoten</h2>
  </form>

<table>
  <tr>
    <th>Platz</th>
    <th>Punkte</th>
    <th>Name</th>
</tr>
<?php
$res = query("SELECT * FROM highscore ".
             "WHERE gameID='$gameID' AND category='$category' ORDER BY points DESC, id");
$i = 1;
while ($row = mysql_fetch_assoc($res))
  echo "<tr class=\"".($i%2 ? "odd" : "even")."\"><td>".$i++."</td><td>$row[points]</td><td>$row[name]</td></tr>\n";
mysql_free_result($res);
?>
</table>

<?php virtual('footer.html'); ?>