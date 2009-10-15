<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
        <link rel="stylesheet" type="text/css" href="../css/sumo.css">
        <head>
                <title>SUMO - Simulation of Urban MObility  -  Status of daily build and test</title>
<link rel="schema.DC" href="http://purl.org/dc/elements/1.1/" />
<META NAME="DC.Title" CONTENT="SUMO - Simulation of Urban MObility  -  Status of daily build and test">
<META NAME="DC.Creator.PersonalName" CONTENT="Krajzewicz, Daniel">
<META NAME="DC.Subject" CONTENT="road traffic simulation package SUMO (Simulation of Urban MObility)">
<META NAME="DC.Description" CONTENT="">
<META NAME="DC.Publisher" CONTENT="Institute of Transportation Systems at the German Aerospace Center (DLR)">
<META NAME="DC.Contributor" CONTENT="Center for Applied Informatics, Cologne">
<META NAME="DC.Type" CONTENT="Text">
<META NAME="DC.Format" SCHEME="IMT" CONTENT="text/html">
<META NAME="DC.Identifier" CONTENT="http://sumo.sourceforge.net">
<META NAME="DC.Language" SCHEME="ISO639-1" CONTENT="en">
<META NAME="DC.Relation" SCHEME="URL" CONTENT="http://sumo.sourceforge.net/daily/index.php">
<META NAME="DC.Coverage" CONTENT="Introduction, Navigation">
<META NAME="DC.Rights" CONTENT="(c) IFS/DLR">
<META NAME="DC.Date.X-MetadataLastModified" SCHEME="ISO8601" CONTENT="2007-06-12">

        </head>
        <body>

        <table border="0">
          <tr>
              <td><img src="/images/64x64_web.gif" width="64" height="64"></td>
              <td><img src="/images/sumo_logo.gif" width="72" height="17"><br>
              <img src="/images/sumo_full.gif" width="211" height="16"></td>
          </tr>
          </table>
                <hr>


<!-- Navigation bar begin -->
                <table border="0" ID="Table1">
                <tr><td valign="top" width="120">
                <div class="navbar">
                        <a href="/index.shtml" class="SUMOMainLink">Home</a><br>
                        <a href="/overview.shtml" class="SUMOMainLink">Overview</a><br>
                        <a href="/docs/documentation.shtml" class="SUMOMainLink">Documentation</a><br>
                        <a href="/screens/screenshots.shtml" class="SUMOMainLink">Screenshots</a><br>
                        <a href="/downloads/downloads.shtml" class="SUMOMainLink">Downloads</a><br>
                        <a href="/participants.shtml" class="SUMOMainLink">Participants</a><br>
                        <a href="/projects.shtml" class="SUMOMainLink">Projects</a><br>
                        <a href="/links/links.shtml" class="SUMOMainLink">Links</a><br>
                        <a href="/disclaimer.shtml" class="SUMOMainLink">Disclaimer</a><br>
                        <a href="/contact.shtml" class="SUMOMainLink">Contact</a><br>

                        <a href="http://sumo.sourceforge.net/wiki/" class="SUMOMainLink">wiki</a><br/>

                        <center><br>
                        <a href="http://www.dlr.de/fs/" class="SUMOMainLink">
            <img src="/images/logo_dlr.gif" width="40" height="42"></a>
                        <a href="http://www.zaik.uni-koeln.de/" class="SUMOMainLink">
            <img src="/images/logo_zaik.gif" width="40" height="42"></a><br>

                        <a href="http://sourceforge.net/projects/sumo/"><img src="http://sourceforge.net/sflogo.php?group_id=45607&type=1" width="88" height="31" border="0" alt="SourceForge.net Logo"></a>
                        </center>
                </div>
                </td><td width="20"></td>
<!-- Navigation bar end -->
                <td valign="top">
                <div class="SUMOMainText">
<!-- Text area here -->
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
<!-- End of Text area -->
                </div>
                </td></tr></table>
                <p></p><hr>

        </body>
</html>

