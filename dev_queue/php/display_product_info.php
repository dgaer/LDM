<?php

// Our required includes
require_once("query_post_validation.php");

// We must validate $n and $f
$file = CleanQueryAsString($_GET['file'], 1024);
$dir = CleanQueryAsString($_GET['dir'], 1024);
$feed = CleanQueryAsString($_GET['feed'], 100);
$n = CleanQueryAsString($_GET['n'], 255);

$is_valid = 1;
if((!IsSet($file)) || ($file == "")) $is_valid = 0;
if((!IsSet($feed)) || ($feed == "")) $is_valid = 0;

if(!$is_valid) {
  echo "<p><h1>ERROR - Request is not a valid product</h1></p>\n";
  echo "<hr />\n";
  echo "<p>Check all required input variables</p>\n";
  exit;
}


$indir = "${dir}/${feed}";
$pfile = "${indir}/${file}_group_file.xml";

echo "<p><h1>${n}<br /> ${feed} Product</h1></p>\n";
echo "<hr />\n";

PrintGroupFile($pfile);

function PrintGroupFile($filepath){
  $fh = fopen($filepath, "r");
  if(!$fh) {
    echo "<p><h1>ERROR - Error reading product file</h1></p>\n";
    echo "<hr />\n";
    echo "<p>Check all required input variables</p>\n";
    exit;
  }
 
  $data = fread($fh, filesize($filepath));
  fclose($fh);

  $xml = new SimpleXMLElement($data);

  $EpochTime = $xml-> EpochTime;
  $TimeString = $xml -> TimeString;
  $Signature = $xml -> Signature;
  $Feedtype = $xml -> Feedtype;
  $SeqNumber = $xml -> SeqNumber;
  $Size = $xml ->Size;
  $Origin = $xml -> Origin;
  $GroupKey = $xml -> GroupKey;
  $ProductIDs = $xml -> ProductIDs->children();
  $ProductName = $xml -> ProductName;
  $ProductDesc = $xml -> ProductDesc;

  if($ProductName == "") $ProductName = "&nbsp;-&nbsp;";
  if($ProductDesc == "") $ProductDesc = "&nbsp;-&nbsp;";

  $count = 1;

  putenv("TZ=America/New_York");
  putenv("TZ=GMT");
  $lasttime = date("m/d/Y h:i:s A T", (int)$EpochTime);
  echo "<p>\n";
  echo "Product Group:&nbsp;$GroupKey&nbsp;<br /><br />\n";
  echo "Last Updated:&nbsp;&nbsp;&nbsp;$lasttime&nbsp;<br />\n";
  putenv("TZ=America/New_York");
  $lasttime = date("m/d/Y h:i:s A T", (int)$EpochTime);
  echo "Last Updated:&nbsp;&nbsp;&nbsp;$lasttime&nbsp;<br />\n";
  putenv("TZ=America/Chicago");
  $lasttime = date("m/d/Y h:i:s A T", (int)$EpochTime);
  echo "Last Updated:&nbsp;&nbsp;&nbsp;$lasttime&nbsp;<br />\n";
  echo "</p>\n";
  echo "<table border=\"1\">\n";
  echo "<tr><td>&nbsp;Product Name&nbsp;</td><td>&nbsp;$ProductName&nbsp;</td></tr>\n";
  echo "<tr><td>&nbsp;Description&nbsp;</td><td>&nbsp;$ProductDesc&nbsp;</td></tr>\n";
  echo "<tr><td>&nbsp;Feedtype&nbsp;</td><td>&nbsp;$Feedtype&nbsp;</td></tr>\n";
  foreach($ProductIDs as $id) {
    echo "<tr><td bgcolor=\"yellow\">&nbsp;ID&nbsp;$count&nbsp;</td>\n";
    echo "<td bgcolor=\"yellow\">&nbsp;$id&nbsp;</td></tr>\n";
    $count++;
  }
  $appBytes = formatBytes((int)$Size); 
  echo "<tr><td>&nbsp;Last Size&nbsp;</td><td><b>&nbsp;$Size&nbsp;</b>bytes&nbsp;\n";
  echo "~&nbsp;$appBytes&nbsp;</td></tr>\n";
  echo "<tr><td>&nbsp;Last Time&nbsp;</td><td>&nbsp;$TimeString&nbsp;</td></tr>\n";
  echo "<tr><td>&nbsp;Last Origin&nbsp;</td><td>&nbsp;$Origin&nbsp;</td></tr>\n";
  echo "<tr><td>&nbsp;Last Seq Number&nbsp;</td><td>&nbsp;$SeqNumber&nbsp;</td></tr>\n";
  echo "<tr><td>&nbsp;Last Signature&nbsp;</td><td>&nbsp;$Signature&nbsp;</td></tr>\n";
  echo "</table>\n";
}

function formatBytes($size, $precision = 2)
{
  $base = log($size) / log(1024);
  $suffixes = array('B', 'KB', 'MB', 'GB', 'TB'); 
  return round(pow(1024, $base - floor($base)), $precision) . $suffixes[floor($base)];
}
?>
