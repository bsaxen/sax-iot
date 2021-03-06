<?php
//=============================================
// File.......: server.php
// Date.......: 2019-04-24
// Author.....: Benny Saxen
// Description: IoT Server
//=============================================

//=============================================
// Library
class iotDoc {
    public $sys_ts;
    public $id;
    public $channel_id;
    public $msg_channel;
}

$obj = new iotDoc();

//=============================================
$date         = date_create();
$obj->sys_ts  = date_format($date, 'Y-m-d H:i:s');
//=============================================
function deleteApplication($id)
//=============================================
{
  // remove reg file
  //$filename = str_replace("/","_",$id);
  $filename = 'archive/'.$id.'.arc';
  if (file_exists($filename)) unlink($filename);
  // remove directory content
  $dirname = 'applications/'.$id;
  array_map('unlink', glob("$dirname/*.*"));

  rmdir($dirname);
}
//=============================================
function saveChannelMsg($obj)
//=============================================
{
  $f_file = 'applications/'.$obj->id.'/channel_'.$obj->channel_id.'.json';
  //echo $f_file;
  $doc = fopen($f_file, "w");
  if ($doc)
  {
        fwrite($doc, "{\n");
        fwrite($doc, "   \"sys_ts\":   \"$obj->sys_ts\",\n");
        fwrite($doc, "   \"id\":       \"$obj->id\",\n");
        fwrite($doc, "   \"channel\":  \"$obj->channel_id\",\n");
        fwrite($doc, "   \"msg\": $obj->msg_channel\n");
        fwrite($doc, "}\n ");
        fclose($doc);
  }
  return;
}

//=============================================
function initLog($obj)
//=============================================
{
  $f_file = 'applications/'.$obj->id.'/log.txt';
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
  $f_file = 'applications/'.$obj->id.'/log.txt';
  $doc = fopen($f_file, "a");
  if ($doc)
  {
        fwrite($doc, "$obj->sys_ts $obj->log\n");
        fclose($doc);
  }
  return;
}


//=============================================
function listAllApplications()
//=============================================
{
  $do = "ls archive"."/"."*.arc > archive/archive.work";
  //echo $do;
  system($do);
  $file = fopen('archive/archive.work', "r");
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
function listAllChannels($app)
//=============================================
{
  $term1 = 'applications/'.$app.'/*.json';
  $term2 = 'applications/'.$app.'/channels.work';
  $do = 'ls '.$term1.' > '.$term2;
  //echo $do;
  system($do);
  $file = fopen($term2, "r");
  if ($file)
  {
    while(!feof($file))
    {
      $line = fgets($file);
      //echo $line;
      if (strlen($line) > 2)
      {
          $dd = 'applications/'.$app.'/';
          $line = str_replace($dd," ",$line);
          $line = trim($line);
          echo $line.':';
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
     if ($do == 'list_applications')
     {
       listAllApplications();
       exit;
     }



    // Check if id is given
    $error = 1;
    if (isset($_GET['id']))
    {

      // Create device register
      $obj->id = $_GET['id'];
      $obj->id  = str_replace(":","_",$obj->id);

      $error = 0;

      $ok = 0;
      $dir = 'applications/'.$obj->id;
      if (is_dir($dir)) $ok++;
      $file = 'archive/'.$obj->id.'.arc';
      if (file_exists($file)) $ok++;

      if ($ok == 0) // New application - archive!
      {
         mkdir($dir, 0777, true);
        //===========================================
        // Archive
        //===========================================
        // Create archive directory if not exist
        $dir = 'archive';
        if (!is_dir($dir))
        {
           mkdir($dir, 0777, true);
        }
        //$filename = str_replace("/","_",$obj->id);
        $filename = 'archive/'.$obj->id.'.arc';
        $doc = fopen($filename, "w");
        fwrite($doc, "$gs_ts $ts $obj->id");
        fclose($doc);
      }
      if ($ok == 1) // un-complete
      {
        $error = 3;
        echo "Error: archive broken";
      }
    }
    else
    {
      $error = 2;
      echo "Error: no app id specified";
    }


    // API when id is available
    if($error == 0)
    {
      $do = $_GET['do'];
      if ($do == 'list_channels')
      {
        listAllChannels($obj->id);
        exit;
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
        deleteApplication($obj->id);
      }

      if ($do == 'channel')
      {
        $obj->channel_id   = $_GET['cid'];
        $obj->msg_channel = "{\"no_data\":\"0\"}";
        if (isset($_GET['json'])) {
          $obj->msg_channel = $_GET['json'];
        }
        saveChannelMsg($obj);
      }

 } // error
} // do
else
  echo "Server ok";
// End of file
?>
