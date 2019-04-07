<?php
$label = $_GET['label'];

$data = array();

$file = $label.'.map';
$fh = fopen($file, "r");
if ($file)
{
    while(! feof($fh))
    {
      $data = fgets($fh);
      //$data = explode(",", $line);
    }
    fclose($fh);
}
print $data;
return;
?>
