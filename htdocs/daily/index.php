<!DOCTYPE html>

<html lang="en" style="width: 100%">
<head>
  <title>SUMO - Status of daily build and tests</title>
  <meta charset="UTF-8">
  <link rel="stylesheet" type="text/css" href="../sumo.css">
  <link rel="license" href=
  "http://creativecommons.org/licenses/by-sa/3.0/">
</head>

<body id="bmission" style="width: 100%">
<div id="container" style="width: 100%">



 <div id="logo" align="left"><img src="../logo.png" width="128" height="128" align="left"
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

    <h2>Status of daily build and tests</h2>
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
    $haveIndex = FALSE;
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
            if (!$haveIndex) {
                $haveTable = FALSE;
                $testIndex = file($prefix.'report/index.html');
                foreach ($testIndex as $iline) {
                    if (strstr($iline, '</center')) {
                        break;
                    }
                    if (strstr($iline, '<table')) {
                        $haveTable = TRUE;
                    }
                    if ($haveTable) {
                        $iline = str_replace("href=\"", "href=\"".$prefix."report/", $iline);
                        $cells[$column][$row] .= str_replace(array("<h3>", "</h3>"), "", $iline);
                    }
                }
                $haveIndex = TRUE;
            }
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
 </div>

 <div id="footer">
   <div>(c) 2011-2014, German Aerospace Center, Institute of Transportation Systems</div>
   <div>Layout based on <a href="http://www.oswd.org/design/preview/id/3365">"Three Quarters"</a> by "SimplyGold"</div>
 </div>
                                                       
 
</div></body>

</html>
