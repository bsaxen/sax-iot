<?php
//=========================================================================
// File.......: ajaxManager.php
// Date.......: 2019-03-08
// Author.....: Benny Saxen
// Description: Ajax Device Manager
//=========================================================================
//=============================================
function getStatus($uri)
//=============================================
{
  global $g_feedback;

  $url = $uri.'/static.json';
  //echo "$url<br>";
  $json = file_get_contents($url);
  $json = utf8_encode($json);
  $res = json_decode($json, TRUE);
  $period     = $res['msg']['period'];

  $url = $uri.'/dynamic.json';
  //echo "$url<br>";
  $json = file_get_contents($url);
  $json = utf8_encode($json);
  $res = json_decode($json, TRUE);
  $timestamp   = $res['sys_ts'];
  $now = date_create('now')->format('Y-m-d H:i:s');

  $diff = strtotime($now) - strtotime($timestamp);
  //echo "now=$now ts=$timestamp diff= $diff";

  if ($diff > $period)
  {
    $res = $diff;
  }
  else {
    $res = 0;
  }
  return ($res);
}

//=========================================================================
function getJsonDevicePar($url,$par)
//=========================================================================
{

  $options = array(
    'http' => array(
        'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
        'method'  => 'GET'
    )
  );
  $context  = stream_context_create($options);
  $result = file_get_contents($url, false, $context);
  $streams = json_decode($result,true);
  $ts = $streams['msg'][$par];
  return $ts;
}
//=========================================================================
function getJsonDomain($sel_domain)
//=========================================================================
{

  $request = 'http://'.$sel_domain."/gateway.php?do=list_topics";
  //echo $request;
  $ctx = stream_context_create(array('http'=>
   array(
     'timeout' => 2,  //2 Seconds
       )
     ));
  $res = file_get_contents($request,false,$ctx);
  $data = explode(":",$res);
  $num = count($data)-1;
  //echo  "num=".$num." ";
  $answer = '';
  for ($ii = 0; $ii < $num; $ii++)
  {
    //echo $ii;
    //echo  "z".$data[$ii]."x";
    $device = $data[$ii];
    if (strlen($device) > 2)
    {
      //$topic = explode("_",$id);

      //$topic_num = count($topic);
      //echo $topic_num;
      //$link = 'http://'.$url;
      //$device = $topic[0];

      //for ($jj=1;$jj<$topic_num;$jj++)
      //   $device = $device."/$topic[$jj]";
      $doc = 'http://'.$sel_domain.'/devices/'.$device;
      //echo  $doc;
      $status = getStatus($doc);
      //echo $status;
      $answer = $answer.'='.$status;
     }
   }
  //echo $answer;
  return $answer;
}

//=========================================================================

$domain = $_GET['domain'];
$device = $_GET['device'];

$url = 'http://'.$domain.'/devices/'.$device.'/dynamic.json';

//echo $url;
//$no = getJsonData($url,'no');
//$ts = getJsonData($url,'sys_ts');
$answer = getJsonDomain($domain);
echo $answer;

?>
