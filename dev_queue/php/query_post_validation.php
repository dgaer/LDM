<?php
// ------------------------------- //
// -------- Start of File -------- //
// ------------------------------- //
// ----------------------------------------------------------- //
// PHP Source Code File
// PHP Interpreter Version: 5.1.6 or higher
// Original Author: Douglas.Gaer@noaa.gov
// File Creation Date: 02/05/2009
// Date Last Modified: 04/16/2014
//
// Version control: 1.24
//
// Support Team: 
//
// Contributors:
// ----------------------------------------------------------- //
// ------------- Program Description and Details ------------- //
// ----------------------------------------------------------- //
/*
PHP validation functions for all page the accept $_GET[] and
$_POST[] variables. This PHP library has for main functions:

function ValidateQuery($request, $validation_str, &$clean_str)
function ValidateQueryWithRegEx($request, $regex, &$clean_str)
function ValidateQueryWithArray($request, $validation_array, &$clean_str)
function ValidateQueryWithRegExArray($request, $regex_array, &$clean_str)

Internet References:

http://bothack.wordpress.com/2007/04/11/secure-php-programming/
http://www.w3schools.com/php/default.asp

*/
// ----------------------------------------------------------- // 

function CleanQueryAsString($unsafe_buffer, $max_len = 255)
{
  // Our main driver function
  
  $safe_buffer = "";
  if(!isSet($unsafe_buffer)) return $safe_buffer;

  if(strlen($unsafe_buffer) > $max_len) {
    // If we went past this length some is trying to buffer overflow this GET or POST
    $safe_buffer = "";
    return $safe_buffer;
  }
  
  // Check for hackers that input HTML/XML code as a post or query
  $ascii_buffer = htmlentities($unsafe_buffer);
  // Strip any HTML/XML TAGs the may have escaped htmlentities() call 
  $safe_buffer = strip_tags($ascii_buffer);

  // Input we always reject no matter what.
  // This check protects against hackers trying to buffer overflow into
  // our system path or try to break in with malicious scripting 
  $reject_always = array("/etc", "/usr", "/var",
			 "/home", "/boot",
			 "/proc", "/opt", "/www",
			 "javascript", "vbscript"
			 );
  foreach($reject_always as $key=>$val) {
    if(version_compare(PHP_VERSION, '5.0.0', '<')) {
      $pos = strpos($safe_buffer, $val);
    }
    else {
      $pos = stripos($safe_buffer, $val);
    }
    if($pos !== false) {
      // We have been hacked so reset this string
      $safe_buffer = "";
      break;
    }
  }
  return $safe_buffer;
}

function CleanQueryAsInt($unsafe_buffer, $low_range = 0, $high_range = 255) {
  $safe_buffer = CleanQueryAsString($unsafe_buffer);
  if(!$safe_buffer) return 0;
  if(($safe_buffer < $low_range) || ($safe_buffer > $high_range)) {
    return 0;
  }
  return $safe_buffer;
}

function ValidateQuery($request, $validation_str, &$clean_str)
{
  if(!isSet($request)) return 0;

  $has_str = 0;
  $safe_buffer = CleanQueryAsString($request);
  if($safe_buffer) {
    if(version_compare(PHP_VERSION, '5.0.0', '<')) {
      $pos = strpos($safe_buffer, $validation_str);
    }
    else {
      $pos = stripos($safe_buffer, $validation_str);
    }
    if($pos !== false) {
      // We have found the validation string
      $has_str = 1;
    }
  }
  $clean_str = $safe_buffer;
  return $has_str;
}

function ValidateQueryWithRegEx($request, $regex, &$clean_str)
{
  if(!isSet($request)) return 0;
  
  $has_str = 0;
  $safe_buffer = CleanQueryAsString($request);
  if($safe_buffer) {
    if(preg_match($regex, $safe_buffer)) {
      // We have found the validation regular expression
      $has_str = 1;
    }
  }

  $clean_str = $safe_buffer;
  return $has_str;
}

function ValidateQueryWithArray($request, $validation_array, &$clean_str)
{
  $has_str = 0;
  reset($validation_array);
  foreach($validation_array as $key=>$val) {
    if(ValidateQuery($request, $val, $clean_str)) {
      $has_str = 1;
      break;
    }
  }
  return $has_str;
}

function ValidateQueryWithRegExArray($request, $regex_array, &$clean_str)
{
  $has_str = 0;
  reset($regex_array);
  foreach($regex_array as $key=>$val) {
    if(ValidateQueryWithRegEx($request, $val, $clean_str)) {
      $has_str = 1;
      break;
    }
  }
  return $has_str;
}

function HasPattern($s, $p)
{
  $pos = strpos($s, $p);
  if($pos === false) {
    return false;
  }
  return true;
}

function HasSpammerTagsWithType($field, &$type)
{
  $type = "none";
    
  if(HasPattern("$field", "://")) {
    $type = "url"; 
    return true;
  }

  if(is_email_address($field)) {
    $type = "email"; 
    return true;
  }
  
  // This field is ok
  return false;
}

function HasSpammerTags($field)
{
  // TODO: Add all spammer checks here
  if(HasPattern("$field", "://")) {
    return true;
  }

  // This field is ok
  return false;
}

function CleanBodyText($unsafe_buffer, $max_len = 65535)
{
  $safe_buffer = "";
  if(!isSet($unsafe_buffer)) return $safe_buffer;

  if(strlen($unsafe_buffer) > $max_len) {
    // If we went past this length some is trying to buffer overflow this GET or POST
    $safe_buffer = "";
    return $safe_buffer;
  }

  // Check for hackers that input HTML/XML code as a post or query
  $safe_buffer = htmlentities($unsafe_buffer);

  return $safe_buffer;
}

function AddToBlacklist($fname, $email_name_phone_ip)
{
  if($email_name_phone_ip == "") return;
  if($email_name_phone_ip == "\n") return;
  if($email_name_phone_ip == "\r\n") return;

  if(file_exists("$fname")) {
    $fp = fopen("$fname", 'r+');
    if($fp) {
      while(!feof($fp)) {
	$line = fgets($fp, 1024);
	$line = chop($line); // Remove line feeds
	$line = trim($line); // Remove leading and trailing spaces
	if(($email_name_phone_ip) == ($line)) {
	  // This email, name, phone, or ip address has been added
	  fclose($fp);
	  return;
	}
      }
      $line = $email_name_phone_ip . "\n";
      fwrite($fp, "$line");
      fclose($fp);
    }
  }
}

function IsBlacklisted($fname, $email_name_phone_ip)
{
  if($email_name_phone_ip == "") return false;
  if($email_name_phone_ip == "\n") return false;
  if($email_name_phone_ip == "\r\n") return false;
  
  if(file_exists("$fname")) {
    $fp = fopen("$fname",'r');
    if($fp) {
      while(!feof($fp)) {
	$line = fgets($fp, 1024);
	$line = chop($line); // Remove line feeds
	$line = trim($line); // Remove leading and trailing spaces
	if($line == "") continue;
	if(($email_name_phone_ip) == ($line)) {
	  // Email, name, phone, or ip address has been blacklisted
	  return true;
	  fclose($fp);
	}
      }
      fclose($fp);
    }
  }
  return false;
}

function clean_args($var,$default='',$method='GET',$replace='[^\\n\/A-Za-z0-9:().\\\-_ ]') {
  if ($method=='GET') $info=isset($_GET[$var])?$_GET[$var]:$default;
  elseif ($method=='POST') $info=isset($_POST[$var])?$_POST[$var]:$default;
  else $info=isset($_REQUEST[$var])?$_REQUEST[$var]:$default;
  $info=$replace?preg_replace('/'.$replace.'/','',$info):$info;
  return $info;
}

function is_email_address($email) {

  if($email == "") return false;

  $results = array();
  
  // First, we check that there's one @ symbol, and that the lengths are right
  if (!preg_match("/^[^@]{1,64}@[^@]{1,255}$/", $email, $results)) {
    // Email invalid because wrong number of characters in one section, or wrong number of @ symbols.
    return false;
  }

  // Test code
  // echo "<pre>";
  // print_r($results);
  // echo "</pre>";

  // Split it into sections to make life easier
  $email_array = explode("@", $email);
  $local_array = explode(".", $email_array[0]);
  for ($i = 0; $i < sizeof($local_array); $i++) {
    if (!preg_match("@^(([A-Za-z0-9!#$%&#038;'*+/=?^_`{|}~-][A-Za-z0-9!#$%&#038;'*+/=?^_`{|}~\.-]{0,63})|(\"[^(\\|\")]{0,62}\"))$@", $local_array[$i], $results)) {
      return false;
    }
  }

  // Test code
  // echo "<pre>";
  // print_r($results);
  // echo "</pre>";
  
  if (!preg_match("/^\[?[0-9\.]+\]?$/", $email_array[1])) { // Check if domain is IP. If not, it should be valid domain name
    $domain_array = explode(".", $email_array[1]);
    if (sizeof($domain_array) < 2) {
      return false; // Not enough parts to domain
    }
    for ($i = 0; $i < sizeof($domain_array); $i++) {
      if (!preg_match("/^(([A-Za-z0-9][A-Za-z0-9-]{0,61}[A-Za-z0-9])|([A-Za-z0-9]+))$/", $domain_array[$i], $results)) {
	return false;
      }
    }
  }

  // Test code
  // echo "<pre>";
  // print_r($results);
  // echo "</pre>";
  
  return true;
}

// ----------------------------------------------------------- //
// ------------------------------- //
// --------- End of File --------- //
// ------------------------------- //
?>