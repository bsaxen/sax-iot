<?php
//=============================================
// File.......: gateway.php
// Date.......: 2019-03-07
// Author.....: Benny Saxen
// Description: IoT Gateway
//=============================================

//=============================================
// Library
class sipDoc {
    public $sys_ts;
    public $id;
    public $msg_static;
    public $msg_dynamic;
    public $msg_payload;
}

$obj = new sipDoc();

//=============================================
$date         = date_create();
$obj->sys_ts  = date_format($date, 'Y-m-d H:i:s');

//=============================================
function saveStaticMsg($obj)
//=============================================
{
  $f_file = $obj->id.'/static.json';
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\"sip\": {\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"id\":       \"$obj->id\",\n");
        fwrite($doc, "   \"msg\":      \"$obj->msg_static\"\n");
        fwrite($doc, "}}\n ");
        fclose($doc);
  }
  return;
}
//=============================================
function saveDynamicMsg($obj)
//=============================================
{
  $f_file = $obj->id.'/dynamic.json';
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\"sip\": {\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"msg\":     \"$obj->msg_dynamic\"\n");
        fwrite($doc, "}}\n ");
        fclose($doc);
  }
  return;
}
//=============================================
function savePayloadMsg($obj)
//=============================================
{
  $f_file = $obj->id.'/payload.json';
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\"sip\": {\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"msg\":     \"$obj->msg_payload\"\n");
        fwrite($doc, "}}\n ");
        fclose($doc);
  }
  return;
}

//=============================================
function saveLog($obj)
//=============================================
{
  $f_file = $obj->id.'/log.sip';
  $doc = fopen($f_file, "a");
  if ($doc)
  {
        fwrite($doc, "$obj->sys_ts $obj->log\n");
        fclose($doc);
  }
  return;
}

//=============================================
function readFeedbackFile($fb_file)
//=============================================
{
  $file = fopen($fb_file, "r");
  if ($file)
  {
      $result = ":";
      while(! feof($file))
      {
        $line = fgets($file);
        //sscanf($line,"%s",$work);
        $result = $result.$line;
      }
      fclose($file);
      $result = $result.":";
      // Delete file
      if (file_exists($fb_file)) unlink($fb_file);
  }
  else
  {
      $result = ":void:";
  }
  return $result;
}
//=============================================
function readFeedbackFileList($id)
//=============================================
{
  $result = ' ';
  $do = 'ls '.$id.'/*_sip.feedback > '.$id.'/feedback.work';
  //echo $do;
  system($do);
  $list_file = $id.'/feedback.work';
  $no_of_lines = count(file($list_file));
  $file = fopen($list_file, "r");
  if ($file)
  {
      // Read first line only
      $line = fgets($file);
      //echo "line:".$line;
      if (strlen($line) > 2)
      {
          $line = trim($line);
          $result = readFeedbackFile($line);
      }
  }
  $result = "[$no_of_lines]".$result;
  return $result;
}

//=============================================
// End of library
//=============================================

if (isset($_GET['do']))
{
    $do = $_GET['do'];
    
    // Check if id is given
    $error = 1;
    if (isset($_GET['id']))
    {
      $obj->dev_id = $_GET['id'];
      $error = 0;
      if (!is_dir($obj->dev_id))
      {
         mkdir($obj->dev_id, 0777, true);
        //===========================================
        // Registration
        //===========================================
        $filename = str_replace("/","_",$obj->dev_id);
        $filename = $filename.".reg";
        $doc = fopen($filename, "w");
        fwrite($doc, "$gs_ts $ts $obj->dev_id");
        fclose($doc);
      }
    }
    else
    {
      $error = 2;
      echo "Error: no id specified";
    }


    // API when topic is available
    if($error == 0)
    {
      if ($do == 'feedback')
      {
        $msg   = $_GET['msg'];
        $tag   = $_GET['tag'];
        writeFeedbackFile($obj->dev_id, $msg, $tag);
      }
      if ($do == 'clearlog')
      {
        initLog($obj);
      }
      if ($do == 'log')
      {
        $obj->log   = $_GET['log'];
        saveLog($obj);
      }
      if ($do == 'delete')
      {
        deleteDevice($obj->dev_id);
      }


      if ($do == 'static')
      {
        if (isset($_GET['json'])) {
          $obj->msg_static = $_GET['json'];
        }
        saveStaticMsg($obj);
        echo readFeedbackFileList($obj->id);
      }

      if ($do == 'dynamic')
      {
        if (isset($_GET['json'])) {
          $obj->msg_dynamic = $_GET['json'];
        }
        saveDynamicMsg($obj);
        echo readFeedbackFileList($obj->id);
      }
      
      if ($do == 'payload')
      {
        if (isset($_GET['json'])) {
          $obj->msg_static = $_GET['json'];
        }
        savePayloadMsg($obj);
        echo readFeedbackFileList($obj->id);
      }

 } // error
} // do
else
  echo "Server ok";
// End of file
?>
