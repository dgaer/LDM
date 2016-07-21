<?php

// Our required includes
require_once("query_post_validation.php");
$n = CleanQueryAsString($_GET['n'], 255);
$dir = CleanQueryAsString($_GET['dir'], 1024);

$is_valid = 0;

$myDirectory = opendir($dir);
while($entryName = readdir($myDirectory)) {
  $dirArray[] = $entryName;
}
closedir($myDirectory);
$indexCount = count($dirArray);
sort($dirArray);
for($index=0; $index < $indexCount; $index++) {
    $is_valid++;
}

if(!$is_valid) {
  echo "<p><h1>ERROR - Request is not a valid LDM server</h1></p>\n";
  echo "<hr />\n";
  echo "<p>Check all required input variables<br />";
  echo "hostname = ${n}<br />";
  echo "directory = ${dir}";
  echo "</p>\n";
  exit;
}

echo "<p><h1>${n}</h1></p>\n";
echo "<hr />\n";

$myDirectory = opendir($dir);
unset($dirArray);
while($entryName = readdir($myDirectory)) {
	$dirArray[] = $entryName;
}
closedir($myDirectory);
$indexCount	= count($dirArray);
sort($dirArray);
echo "<p><b>Data Feeds</b><br /><ul>\n";
for($index=0; $index < $indexCount; $index++) {
  if (substr("$dirArray[$index]", 0, 1) != "."){ // don't list hidden files
    echo "<li><a href=\"ldm_display_feed.php?n=${n}&dir=${dir}&feed=$dirArray[$index]\">$dirArray[$index]</a></li><br />\n";
  }
}
echo "</p>\n";

?>
