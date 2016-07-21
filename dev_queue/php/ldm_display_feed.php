<?php

// Our required includes
require_once("query_post_validation.php");
$n = CleanQueryAsString($_GET['n'], 255);
$feed = CleanQueryAsString($_GET['feed'], 100);
$dir = CleanQueryAsString($_GET['dir'], 1024);

echo "<p><h1>${n}<br />${feed} Data Feed</h1></p>\n";
echo "<hr />\n";

$myDirectory = opendir("${dir}/${feed}");
unset($dirArray);
while($entryName = readdir($myDirectory)) {
  $dirArray[] = $entryName;
}

closedir($myDirectory);
$indexCount = count($dirArray);
sort($dirArray);
$pcount = 1;
echo "Current product count = <b>" . $indexCount. "</b><br/>\n";
echo "<table border=\"1\">\n";
for($index=0; $index < $indexCount; $index++) {
  if (substr("$dirArray[$index]", 0, 1) != "."){ // don't list hidden files
    $pos = strpos($dirArray[$index], "_group_file.xml");
    $display_name = substr("${dirArray[$index]}", 0, $pos);
    $link = "<a href=\"display_product_info.php?file=${display_name}&feed=${feed}&n=${n}&dir=${dir}\">${display_name}</a>";
    if($pcount % 2 == 0) {
      echo "<tr><td bgcolor=\"yellow\">" . $pcount . "</td>\n";
      echo "<td bgcolor=\"yellow\">${link}</td></tr>\n";
    }
    else {
      echo "<tr><td>" . $pcount . "</td>\n";
      echo "<td>${link}</td></tr>\n";
    }
    $pcount++;
  }
}
echo "</table>\n";

?>
