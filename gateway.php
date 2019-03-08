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
  $f_file = 'devices/'.$obj->id.'/static.json';
  echo $f_file;
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"id\":       \"$obj->id\",\n");
        fwrite($doc, "   \"msg\": $obj->msg_static\n");
        fwrite($doc, "}\n ");
        fclose($doc);
  }
  return;
}
//=============================================
function saveDynamicMsg($obj)
//=============================================
{
  $f_file = 'devices/'.$obj->id.'/dynamic.json';
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"msg\":  $obj->msg_dynamic\n");
        fwrite($doc, "}\n ");
        fclose($doc);
  }
  return;
}
//=============================================
function savePayloadMsg($obj)
//=============================================
{
  $f_file = 'devices/'.$obj->id.'/payload.json';
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"msg\":  $obj->msg_payload\n");
        fwrite($doc, "}\n ");
        fclose($doc);
  }
  return;
}
//=============================================
function initLog($obj)
//=============================================
{
  $f_file = 'devices/'.$obj->id.'/log.txt';
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "$obj->sys_ts Created\n");
        fclose($doc);
  }
  return;
}
//=============================================
function saveLog($obj)
//=============================================
{
  $f_file = 'devices/'.$obj->id.'/log.txt';
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
  $do = "ls devices/".$id."/"."*_sip.feedback > devices/".$id."/feedback.work";
  //echo $do;
  system($do);
  $list_file = 'devices/'.$id.'/feedback.work';
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
          $line = 'devices/'.$line;
          $result = readFeedbackFile($line);
      }
  }
  $result = "[$no_of_lines]".$result;
  return $result;
}
//=============================================
function listAllTopics()
//=============================================
{
  $do = "ls register"."/"."*.reg > register/register.work";
  //echo $do;
  system($do);
  $file = fopen('register/register.work', "r");
  if ($file)
  {
    while(!feof($file))
    {
      $line = fgets($file);
      //echo $line;
      if (strlen($line) > 2)
      {
          $line = trim($line);
          $file2 = fopen($line, "r");
          if ($file2)
          {
                  $line2 = fgets($file2);
                  $line2 = trim($line2);
                  echo $line2.':';
          }
      }
    }
  }
}
//=============================================
// End of library
//=============================================

if (isset($_GET['do']))
{

    $do = $_GET['do'];
     if ($do == 'list_topics')
     {
       listAllTopics();
       exit;
     }
    // Check if id is given
    $error = 1;
    if (isset($_GET['id']))
    {

      // Create device register
      $obj->id = $_GET['id'];
      $error = 0;

      $ok = 0;
      $dir = 'devices/'.$obj->id;
      if (is_dir($dir)) $ok++;
      $file = 'register/'.$obj->id.'.reg';
      if (file_exists($file)) $ok++;

      if ($ok == 0) // New device - register!
      {
         mkdir($dir, 0777, true);
        //===========================================
        // Registration
        //===========================================
        // Create register directory if not exist
        $dir = 'register';
        if (!is_dir($dir))
        {
           mkdir($dir, 0777, true);
        }
        //$filename = str_replace("/","_",$obj->id);
        $filename = 'register/'.$obj->id.'.reg';
        $doc = fopen($filename, "w");
        fwrite($doc, "$gs_ts $ts $obj->id");
        fclose($doc);
      }
      if ($ok == 1) // un-complete register
      {
        $error = 3;
        echo "Error: register broken";
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
        writeFeedbackFile('devices/'.$obj->dev_id, $msg, $tag);
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
          $obj->msg_static = "{\"no_data\":\"0\"}";
        if (isset($_GET['json'])) {
          $obj->msg_static = $_GET['json'];
        }
        saveStaticMsg($obj);
        echo readFeedbackFileList($obj->id);
      }

      if ($do == 'dynamic')
      {
        $obj->msg_dynamic = "{\"no_data\":\"0\"}";
        if (isset($_GET['json'])) {
          $obj->msg_dynamic = $_GET['json'];
        }
        saveDynamicMsg($obj);
        echo readFeedbackFileList($obj->id);
      }

      if ($do == 'payload')
      {
        $obj->msg_payload = "{\"no_data\":\"0\"}";
        if (isset($_GET['json'])) {
          $obj->msg_payload = $_GET['json'];
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
