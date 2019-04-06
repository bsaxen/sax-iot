<?php
$label  = $_GET['label'];
$lat    = $_GET['lat'];
$lon    = $_GET['lon'];
$status = $_GET['status'];

$roger = 1;
if (!isset($_GET['label']))  $roger  += 2;
if (!isset($_GET['lat']))    $roger  += 4;
if (!isset($_GET['lon']))    $roger  += 8;
if (!isset($_GET['status'])) $roger  += 16;

if ($roger == 1)
{
    $filename = $label.'.map';
    //echo $filename;
    $fh = fopen($filename, 'w') or die("Open file error");
    fwrite($fh, "$lat,$lon,$status");
    fclose($fh);
    print "ok";
}
else
{
    print "err=$roger";
}  
?>

