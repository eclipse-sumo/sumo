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
    $gameIDs[$row["title"]] = $row["gameID"];
    $titles[$row["gameID"]] = $row["fulltitle"];
endwhile;
mysql_free_result($res);

if (!isset($gameIDs[$g_game])):
    print("<span class=\"wichtig\">Fehler:</span> Unbekanntes Spiel.");
    exit();
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
