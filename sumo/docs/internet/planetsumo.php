<!DOCTYPE html>

<html lang="en">
<head>
  <title>SUMO - Planet SUMO</title>
  <meta charset="UTF-8">
  <link rel="stylesheet" type="text/css" href="sumo.css">
  <link rel="license" href=
  "http://creativecommons.org/licenses/by-sa/3.0/">
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

<div id="shortIntro">
<h2>Planet SUMO</h2>
<p>This is the place where you can upload your scenarios to make them available to the community and
have them executed regularly with the most recent version of sumo. If you prefer to follow all the scenarios
you can head over to the <a href="https://github.com/planetsumo/sumo/tree/planet">Planet SUMO GitHub repository</a>. You can of course also submit your
scenario there via a pull request.
</p>
<h3>What to submit</h3>
<p>The minimal scenario consists of three files:
<ul><li>a network file (.net.xml),</li><li>a route file (.rou.xml), and</li><li>a configuration (.sumocfg).</li></ul>
You can also provide additional files for instance the sources of the network file or additional detector files and
even Python scripts if you use TraCI. Please be aware that we only accept the following file extensions for uploaded
files:
<?php
$allowedExts = array("zip", "gz", "bz2", "7z", "xml", "cfg", "txt", "sumocfg", "netccfg");
foreach ($allowedExts as $ext) {
    print " " . $ext;
}
?>
. So if you include Python files, you will need to zip your contribution.</p>
<h3>How to submit</h3>
<p>Please use the form below to submit at most three files together with a license. Please use a short but descriptive name
consisting preferrably of letters, numbers and underscores only. If you have a README.txt accompanying your scenario you
can safely leave the description open. Please provide an email address which lets us keep in touch. An automated confirmation
will be sent to this address.
<em>Thank you very much for sharing your scenario!</em>
</p>
<?php
if (isset($_POST["submit"])) {
    $valid = True;
    if ($_POST["scenario"] == "") {
        echo "Scenario needs a name!<br>";
        $valid = False;
    }
    if ($_POST["email"] == "") {
        echo "Contact adress needed!<br>";
        $valid = False;
    }
    if ($_POST["confirm"] == "") {
        echo "Please confirm that you have the necessary rights!<br>";
        $valid = False;
    }
    $hadFile = False;
    foreach (array("file1", "file2", "file3") as $file) {
        if ($_FILES[$file]["name"] != "") {
            $extension = end(explode(".", $_FILES[$file]["name"]));
            if (($_FILES[$file]["size"] < pow(2, 20)) && in_array($extension, $allowedExts)) {
                if ($_FILES[$file]["error"] > 0) {
                    echo "Return Code: " . $_FILES[$file]["error"] . "<br>";
                    $valid = False;
                } else {
                    echo "Upload: " . $_FILES[$file]["name"] . "<br>";
                    $scenDir = "upload/" . $_POST["scenario"];
                    if (!file_exists($scenDir)) {
                        mkdir($scenDir);
                    }
                    $target = $scenDir . "/" . $_FILES[$file]["name"];
                    if (file_exists($target)) {
                        echo $target . " already exists. ";
                        $valid = False;
                    } else {
                        move_uploaded_file($_FILES[$file]["tmp_name"], $target);
                        $hadFile = True;
                    }
                }
            } else {
                echo "Invalid file " . $_FILES[$file]["name"] . "<br>";
                $valid = False;
            }
        }
    }
    if (!$hadFile) {
        echo "Please give at least one file!<br>";
        $valid = False;
    }
    $handle = fopen($scenDir . "/description", "a");
    if (!$handle) {
        $valid = False;
    }
    if ($valid) {
        fwrite($handle, 'Description: ' . $_POST["description"] . "\n\n");
        fwrite($handle, 'License: ' . $_POST["license"] . "\n\n");
        fwrite($handle, 'E-Mail: ' . $_POST["email"] . "\n\n");
        fclose($handle);
        $header = 'From: sumo-tests@dlr.de' . "\n" .
        'CC: sumo-tests@dlr.de' . "\n" .
        'X-Mailer: PHP/' . phpversion() . "\n";
        $message = "Hi,\nthank you for uploading your scenario " . $_POST["scenario"] . "!\n" .
        'If we are able to run it with the current version of SUMO, we will upload it to https://github.com/planetsumo/sumo/tree/planet soon.' .
        "\n\nYour SUMO team";
        mail($_POST["email"], "Scenario upload successful", $message, $header);
        echo "<h3>You have successfully uploaded your scenario!</h3>";
    }
}
?>
<form action="planetsumo.php" method="post" enctype="multipart/form-data">
    <label for="scenario">Scenario name:</label>
    <input type="text" name="scenario" id="scenario" size="20"><br>
    <label for="file1">File1:</label>
    <input type="file" name="file1" id="file1"><br>
    <label for="file2">File2:</label>
    <input type="file" name="file2" id="file2"><br>
    <label for="file3">File3:</label>
    <input type="file" name="file3" id="file3"><br>
    <label for="license">License:</label>
    <select name="license">
        <option value="CC-BY-SA">Creative Commons Attribution Share Alike</option>
        <option value="GPL3">GNU General Public License v3.0</option>
        <option value="PD">Public Domain</option>
    </select><br>
    <label for="description">Description:</label><br>
    <textarea name="description" cols="50" rows="10"></textarea><br>
    <label for="email">E-mail:</label>
    <input type="text" name="email" id="email" size="50"><br>
    <input type="checkbox" name="confirm" id="confirm">
    <label for="confirm">I confirm that I have all the rights on the code and the data
    in this scenario which allow me to distribute it under the license above.</label><br>
    <input type="submit" name="submit" value="Submit">
</form>
</div>
 <div id="footer">
   <div>(c) 2011-2014, German Aerospace Center, Institute of Transportation Systems</div>
   <div>Layout based on <a href="http://www.oswd.org/design/preview/id/3365">"Three Quarters"</a> by "SimplyGold"</div>
 </div>
 
</div></body>

</html>
