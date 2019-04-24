<?php
//=============================================
// File.......: application.php
// Date.......: 2019-04-24
// Author.....: Benny Saxen
// Description: IoT Application Manager
//=============================================
session_start();

$sel_channel = $_SESSION["channel"];

$sel_domain = $_SESSION["domain"];
$sel_domain = readDomainUrl($sel_domain);

$sel_application = $_SESSION["application"];
$doc = 'http://'.$sel_domain.'/application/'.$sel_application;
$chan = 1;
$sel_desc = getPayload($doc,$chan,'test');

$flag_show_channel  = $_SESSION["flag_show_channel"];
$flag_show_log      = $_SESSION["flag_show_log"];

// Configuration
//=============================================
// No configuration needed
//=============================================
$date         = date_create();
$ts           = date_format($date, 'Y-m-d H:i:s');
$now          = date_create('now')->format('Y-m-d H:i:s');
$g_rssi       = 0;
$g_action     = 0;
//echo "<br>$ts $now<br>";
//=============================================
// library
//=============================================
//=============================================
function readDomainUrl($file)
//=============================================
{
  $file = $file.'.domain';
  //echo $file;
  $fh = fopen($file, "r");
  if ($fh)
  {
      while(! feof($fh))
      {
        $url = fgets($fh);
      }
      fclose($fh);
  }
  //echo $url;
  return $url;
}
//=============================================
function generateRandomString($length = 15)
//=============================================
{
    return substr(sha1(rand()), 0, $length);
}
//=============================================
function prettyTolk( $json )
//=============================================
{
    global $rank,$g_nn;
    $result = '';
    $level = 0;
    $nn = 1;
    $in_quotes = false;
    $in_escape = false;
    $ends_line_level = NULL;
    $json_length = strlen( $json );

    for( $i = 0; $i < $json_length; $i++ ) {
        $char = $json[$i];
        $new_line_level = NULL;
        $post = "";
        if( $ends_line_level !== NULL ) {
            $new_line_level = $ends_line_level;
            $ends_line_level = NULL;
        }
        if ( $in_escape ) {
            $in_escape = false;
        } else if( $char === '"' )
        {
            $in_quotes = !$in_quotes;
        }
        else if( ! $in_quotes )
        {
            if($word)
            {
              $tmp = $rank[$nn];
              //echo ("$word nn=$nn level=$level<br>");
              //if($tmp > 0 && $tmp != $level) echo "JSON Error: $word<br>";
              $rank[$nn] = $level;
            }

            $word = '';
            switch( $char )
            {
                case '}': case ']':
                    $level--;
                    $ends_line_level = NULL;
                    $new_line_level = $level;
                    break;

                case '{': case '[':
                    $level++;

                case ',':
                    $ends_line_level = $level;
                    break;

                case ':':
                    $nn++;
                    //echo "nn=$nn<br>";
                    $post = " ";
                    break;

                case " ": case "\t": case "\n": case "\r":
                    $char = "";
                    $ends_line_level = $new_line_level;
                    $new_line_level = NULL;
                    break;
            }
        } else if ( $char === '\\' ) {
            $in_escape = true;
        }
        else {
          $word .= $char;
        }
        if( $new_line_level !== NULL ) {
            $result .= "\n".str_repeat( "\t", $new_line_level );
        }
        $result .= $char.$post;
        //echo "$level $char<br>";
    }
    $g_nn = $nn-1;
    return $result;
}

//=============================================
function generateForm($inp,$color)
//=============================================
{
  global $rank,$g_nn;

  $id = 'void';

  $jsonIterator = new RecursiveIteratorIterator(new RecursiveArrayIterator(json_decode($inp, TRUE)),RecursiveIteratorIterator::SELF_FIRST);
  echo("<table border=0>");
  $nn = 0;
  foreach ($jsonIterator as $key => $val) {
    $nn++;
    if ($key == 'id') $id = $val;
    echo "<tr>";
    for($ii=1;$ii<$rank[$nn];$ii++)echo "<td></td>";

      if(is_array($val))
      {
        echo "<td color=\"#C5FD69\">$key</td>";
      }
      else
      {
          echo "<td>$key</td><td bgcolor=\"$color\">$val</td><tr>";
      }
      echo "</tr>";
   }
   echo "</table>";
   //if ($id == 'void') $id = generateRandomString(12);
   if ($g_nn != $nn)echo("ERROR: Key duplicate in JSON structure: $nn $g_nn<br>");
   return $id;
}
//=============================================
function addDomain ($domain)
//=============================================
{
  echo("[$domain]");
  $filename = str_replace("/","_",$domain);
  $filename = $filename.'.domain';
  $fh = fopen($filename, 'w') or die("Can't add domain $domain");
  fwrite($fh, "$domain");
  fclose($fh);
}
//=============================================
function restApi($api,$domain,$application)
//=============================================
{
  $call = 'http://'.$domain.'/server.php?do='.$api.'&id='.$application;
  $res = file_get_contents($call);
}

//=============================================
function getPayload($uri,$chan,$par)
//=============================================
{
  $url       = $uri.'/channel_'.$chan.'.json';
  $json      = file_get_contents($url);
  $json      = utf8_encode($json);
  $dec       = json_decode($json, TRUE);
  $res       = $dec['msg'][$par];
  return $res;
}
//=============================================
function getHeader($uri,$chan,$par)
//=============================================
{
  $url       = $uri.'/channel_'.$chan.'.json';
  $json      = file_get_contents($url);
  $json      = utf8_encode($json);
  $dec       = json_decode($json, TRUE);
  $res       = $dec[$par];
  return $res;
}
//=============================================
function getStatus($uri,$chan)
//=============================================
{
  $period    = getPayload($uri,$chan,'period');
  $timestamp = getHeader($uri,$chan,'sys_ts');
  $now       = date_create('now')->format('Y-m-d H:i:s');
  $diff = strtotime($now) - strtotime($timestamp);
  $res = 999;
  $bias = 1;

  if ($diff > $period + $bias)
  {
    $res = $diff - $period - $bias;
  }
  else {
    $res = 0;
  }
  return ($res);
}


//=============================================
// End of library
//=============================================


//=============================================
// Back-End
//=============================================

if (isset($_GET['do'])) {

  $do = $_GET['do'];

  if($do == 'add_domain')
  {
    $form_add_domain = 1;
  }

  if($do == 'select')
  {
    if (isset($_GET['domain']))
    {
      $sel_domain = $_GET['domain'];
      $_SESSION["domain"] = $sel_domain;
      $sel_domain = readDomainUrl($sel_domain);
    }
    if (isset($_GET['application']))
    {
      $sel_application = $_GET['application'];
      $_SESSION["application"]   = $sel_application;
      $doc = 'http://'.$sel_domain.'/applications/'.$sel_application;
      //$chan = 1;
      //echo $doc;
      //$sel_desc = getPayload($doc,$chan,'test');
      //echo $sel_desc;
    }
    if (isset($_GET['channel']))
    {
      $sel_channel = $_GET['channel'];
      $_SESSION["channel"] = $sel_channel;
    }
  }


  if($do == 'delete')
  {
    $what   = $_GET['what'];
    if ($what == 'domain')
    {
      $fn = $sel_domain.'.domain';
      if (file_exists($fn)) unlink($fn);
    }
    if ($what == 'application')
    {
      restApi('delete',$sel_domain,$sel_application);
    }
    if ($what == 'log')
    {
      restApi('clearlog',$sel_domain,$sel_application);
    }
  }

  if($do == 'rest')
  {
    $api         = $_GET['api'];
    $url         = $_GET['url'];
    $application = $_GET['application'];
    restApi($api,$url,$application);
  }

}

if (isset($_POST['do'])) {
  $do = $_POST['do'];

  if ($do == 'add_domain')
  {
    $dn = $_POST['domain'];
    if (strlen($dn) > 2)addDomain($dn);
  }

}

//=============================================
// Front-End
//=============================================
$data = array();

   echo "<html>
      <head>
      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />
      <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>
      <style>

      #container {
      color: #5E9EC7;
      background-color: cornsilk;
      float: left;
      width: 1000px;
      height: 900px;
      }

      #channel {
      color: #336600;
      //background-color: grey;
      float: left;
      width: 400px;

      }

      #log {
      color: #336600;
      //background-color: yellow;
      float: left;
      width: 600px;

      }

      html {
          min-height: 100%;
      }

      body {
          background: -webkit-linear-gradient(left, #CA7320, #B9CDDC);
          background: -o-linear-gradient(right, #CA7320, #B9CDDC);
          background: -moz-linear-gradient(right, #CA7320, #B9CDDC);
          background: linear-gradient(to right, #CA7320, #B9CDDC);
          background-color: #5E9EC7;
      }
      /* Navbar container */
   .navbar {
     overflow: hidden;
     background-color: #333;
     font-family: Arial;
   }

   /* Links inside the navbar */
   .navbar a {
     float: left;
     font-size: 16px;
     color: white;
     text-align: center;
     padding: 14px 16px;
     text-decoration: none;
   }

   /* The dropdown container */
   .dropdown {
     float: left;
     overflow: hidden;
   }

   /* Dropdown button */
   .dropdown .dropbtn {
     font-size: 16px;
     border: none;
     outline: none;
     color: white;
     padding: 14px 16px;
     background-color: inherit;
     font-family: inherit; /* Important for vertical align on mobile phones */
     margin: 0; /* Important for vertical align on mobile phones */
   }

   /* Add a red background color to navbar links on hover */
   .navbar a:hover, .dropdown:hover .dropbtn {
     background-color: red;
   }

   /* Dropdown content (hidden by default) */
   .dropdown-content {
     display: none;
     position: absolute;
     background-color: #f9f9f9;
     min-width: 160px;
     box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
     z-index: 1;
   }

   /* Links inside the dropdown */
   .dropdown-content a {
     float: none;
     color: black;
     padding: 12px 16px;
     text-decoration: none;
     display: block;
     text-align: left;
   }

   /* Add a grey background color to dropdown links on hover */
   .dropdown-content a:hover {
     background-color: #ddd;
   }

   /* Show the dropdown menu on hover */
   .dropdown:hover .dropdown-content {
     display: block;
   }
      </style>
         <title>Application</title>
      </head>
      <body > ";

//=========================================================================
// body
//=========================================================================
?>

<?php

      echo("<h1>Application Manager $sel_domain $sel_application $sel_channel $now</h1>");
      echo "<div class=\"navbar\">";

      echo "<a href=\"application.php?do=add_domain\">Add Domain</a>";



       echo "<div class=\"dropdown\">
            <button class=\"dropbtn\">Select Domain
              <i class=\"fa fa-caret-down\"></i>
            </button>
            <div class=\"dropdown-content\">
            ";

            $do = 'ls *.domain > domain.list';
            system($do);
            $file = fopen('domain.list', "r");
            if ($file)
            {
              {
                $line = fgets($file);
                if (strlen($line) > 2)
                {
                    $line = trim($line);
                    $domain = str_replace(".domain", "", $line);
                    echo "<a href=application.php?do=select&domain=$domain>$domain</a>";
                }
              }
            }
            echo "</div></div>";

            echo "<div class=\"dropdown\">
                  <button class=\"dropbtn\">Select Application
                    <i class=\"fa fa-caret-down\"></i>
                  </button>
                  <div class=\"dropdown-content\">
                  ";

                  $request = 'http://'.$sel_domain.'/server.php?do=list_applications';
                  //echo $request;
                  $ctx = stream_context_create(array('http'=>
                   array(
                     'timeout' => 2,  //2 Seconds
                       )
                     ));
                  $res = file_get_contents($request,false,$ctx);
                  $data = explode(":",$res);
                  $num = count($data);

                  for ($ii = 0; $ii < $num; $ii++)
                  {
                    $application = str_replace(".arc", "", $data[$ii]);
                    if (strlen($application) > 2)
                    {
                      $doc = 'http://'.$sel_domain.'/applications/'.$application;
                      $chan = 1;
                      //$status = getStatus($doc,$chan);
                      //$desc = getPayload($doc,$chan,'desc');
                      //$temp = $application;
                      //if ($status == 0)
                      //{
                        echo "<a style=\"background: green;\" href=application.php?do=select&application=$application>$application</a>";
                      //}
                      //else {
                      //  echo "<a style=\"background: red;\" href=application.php?do=select&application=$application>$temp $desc</a>";
                      //}
                     }
                   }
          echo "</div></div>";

          echo "  <div class=\"dropdown\">
              <button class=\"dropbtn\">Select Channel
                <i class=\"fa fa-caret-down\"></i>
              </button>
              <div class=\"dropdown-content\">
               ";

              $request = 'http://'.$sel_domain.'/server.php?do=list_channels&id='.$sel_application;
              //echo $request;
              $ctx = stream_context_create(array('http'=>
              array(
                         'timeout' => 2,  //2 Seconds
                           )
              ));
              $res = file_get_contents($request,false,$ctx);
              $data = explode(":",$res);
              $no_of_channels = count($data) - 1;

              for ($ii = 0; $ii < $no_of_channels; $ii++)
              {
                $channel = str_replace(".json", "", $data[$ii]);
                $channels[$ii] = $channel;
                if (strlen($channel) > 2)
                {
                     echo "<a href=application.php?do=select&channel=$channel>$channel</a>";
                }
              }
              echo "</div></div>";

      echo "<div class=\"dropdown\">
            <button class=\"dropbtn\">Delete
              <i class=\"fa fa-caret-down\"></i>
            </button>
            <div class=\"dropdown-content\">
            ";

      echo "<a href=application.php?do=delete&what=domain>$sel_domain</a>";
      echo "<a href=application.php?do=delete&what=application>$sel_application</a>";
      echo "<a href=application.php?do=delete&what=log>clear log $sel_application</a>";
      echo "</div></div>";

      echo "</div>";

   //=============================================
;
if ($form_add_domain == 1)
{
  echo "
  <form action=\"#\" method=\"post\">
    <input type=\"hidden\" name=\"do\" value=\"add_domain\">
    Add Domain<input type=\"text\" name=\"domain\">
    <input type= \"submit\" value=\"Add Domain\">
    </form> ";
}

//  echo "<div id=\"container\">";

for ($ii = 0; $ii < $no_of_channels; $ii++)
{

  echo "<div id=\"channel\">";
  echo $channels[$ii];
  $doc = 'http://'.$sel_domain.'/applications/'.$sel_application.'/'.$channels[$ii].'.json';
  //echo $doc;
  $json   = file_get_contents($doc);
  if ($json)
  {
    $result = prettyTolk( $json);
    $id = generateForm($json,"#A2D5F8");
  }
  //echo ("<br>payload<br><iframe style=\"background: #FFFFFF;\" src=$doc width=\"400\" height=\"300\"></iframe>");
    echo "</div>";
}


  $rnd = generateRandomString(3);
    echo "<div id=\"log\">";
  $doc = 'http://'.$sel_domain.'/applications/'.$sel_application.'/log.txt?ignore='.$rnd;
  echo ("<br>log<br><iframe id= \"ilog\" style=\"background: #FFFFFF;\" src=$doc width=\"500\" height=\"600\"></iframe>");
    echo "</div>";

//  echo "</div";


echo "</body></html>";
// End of file
