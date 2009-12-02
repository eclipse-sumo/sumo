<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
    <head>
        <title>Status of daily build and test</title>
    </head>
    <body>
        <center>
            <h2>Status of daily build and test</h2>
        </center>
        <table>
<?PHP
$column = 0;
$cells[$column][0] = "";
$cells[$column][1] = "Standard build";
$cells[$column][2] = "Texttest tests";
$cells[$column][3] = "Debug build";
$column++;

foreach (glob("*status.log") as $filename) {
    $prefix = substr($filename, 0, strpos($filename, "status.log"));
    $cells[$column][0] = '';
    $cells[$column][1] = '';
    $cells[$column][3] = '';
    $row = 0;
    $statusdata = file($filename);
    foreach ($statusdata as $line) {
        if (chop($line) == "--") {
            if (strstr($cells[$column][$row], "<pre>")) {
                $cells[$column][$row] .= "</pre></a>";
            }
            $row++;
            continue;
        }
        $br = strstr($line, "batchreport");
        if ($br) {
            $br_arr = split(' ', $br);
            $br_sub = explode('.', substr($br_arr[0], 12));
            if (count($br_sub) == 1) {
                $br_dir = $br_sub[0].'/test_default.html';
            } else {
                $br_dir = $br_sub[0].'/test_default.html#'.$br_sub[1];
            }
            $cells[$column][$row] .= '<a href="'.$prefix.'report/'.$br_dir.'">'.substr($br, 12).'</a><br/>';
        } else {
            if ($cells[$column][$row] == '' && $row > 0) {
                $cells[$column][$row] = '<a href="'.$line.'"><pre>';
            } else {
                $cells[$column][$row] .= $line;
            }
        }
    }

    $column++;
}
for ($j = 0; $j < count($cells[0]); $j++) {
    echo "<tr>";
    for ($i = 0; $i < $column; $i++) {
        if ($i == 0 || $j == 0) {
            echo "<th>".$cells[$i][$j]."</th>\n";
        } else {
            echo "<td>".$cells[$i][$j]."</td>\n";
        }
    }
    echo "</tr>\n";
}
?> 
        </table>
    </body>
</html>

