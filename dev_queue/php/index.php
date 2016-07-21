<?php

// Our required includes
require_once("query_post_validation.php");

$n=php_uname('n');
$dir = "queue_data/";

echo "<p><h1>LDM Services Infomation Page</h1></p>\n";
echo "<hr />\n";
echo "<p>\n";
echo "<b>LDM product queue</b><br />\n";
echo "<ul>\n";
echo "<li><a href=\"ldm_products.php?n=${n}&dir=${dir}\">${n}</a></li>\n";	
echo "</ul>\n";
echo "</p>\n";
?>
