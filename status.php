<?php
//=============================================
// File.......: status.php
// Date.......: 2019-05-11
// Author.....: Benny Saxen
// Description: IoT Status
//=============================================
session_start();

$sel_domain = $_SESSION["domain"];
$sel_domain = readDomainUrl($sel_domain);

$date         = date_create();
$ts           = date_format($date, 'Y-m-d H:i:s');
$now          = date_create('now')->format('Y-m-d H:i:s');
$g_rssi       = 0;
$g_action     = 0;

//=============================================
// library
//=============================================
//=============================================
function readDomainUrl($file)
//=============================================
{
  $file = $file.'.domain';
  //echo $file;
  $file = fopen($file, "r");
  if ($file)
  {
      while(! feof($file))
      {
        $url = fgets($file);
      }
      fclose($file);
  }
  //echo $url;
  return $url;
}
//=============================================
function addDomain($domain)
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
function generateRandomString($length = 15)
//=============================================
{
    return substr(sha1(rand()), 0, $length);
}

//=============================================
function restApi($api,$domain,$device)
//=============================================
{
  //echo("RestApi [$api] domain=$domain device=$device<br>");
  $call = 'http://'.$domain.'/gateway.php?do='.$api.'&id='.$device;
  //echo $call;
  $res = file_get_contents($call);
}

//=============================================
function getDesc($uri)
//=============================================
{
  $url       = $uri.'/static.json';
  $json      = file_get_contents($url);
  $json      = utf8_encode($json);
  $dec       = json_decode($json, TRUE);
  $desc      = $dec['msg']['desc'];
  return $desc;
}
//=============================================
function getStatus($uri)
//=============================================
{
  $url       = $uri.'/static.json';
  $json      = file_get_contents($url);
  $json      = utf8_encode($json);
  $dec       = json_decode($json, TRUE);
  $period    = $dec['msg']['period'];

  $url       = $uri.'/dynamic.json';
  $json      = file_get_contents($url);
  $json      = utf8_encode($json);
  $dec       = json_decode($json, TRUE);
  $timestamp = $dec['sys_ts'];
  
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

  if($do == 'send_action')
  {
    $form_send_action = 1;
  }

  if($do == 'select')
  {
    if (isset($_GET['domain']))
    {
      $sel_domain = $_GET['domain'];
      $_SESSION["domain"] = $sel_domain;
      $sel_domain = readDomainUrl($sel_domain);
    }
  }

  if($do == 'rest')
  {
    $api   = $_GET['api'];
    $url   = $_GET['url'];
    $topic = $_GET['topic'];
    restApi($api,$url,$topic);
  }

}


if (isset($_POST['do'])) 
{
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
      color: #336600;
      background-color: cornsilk;
      float: left;
      width: 1000px;
      height: 900px;
      }

      #status {
      color: #336600;
      //background-color: grey;
      float: left;
      width: 400px;

      }

      #static {
      color: #336600;
      //background-color: grey;
      float: left;
      width: 400px;

      }

      #dynamic {
      color: #336600;
      //background-color: red;
      float: left;
      width: 400px;

      }

      #payload {
      color: #336600;
      //background-color: blue;
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
          background: -webkit-linear-gradient(left, #93B874, #191970);
          background: -o-linear-gradient(right, #93B874, #191970);
          background: -moz-linear-gradient(right, #93B874, #191970);
          background: linear-gradient(to right, #93B874, #191970);
          background-color: #93B874;
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
         <title>Status</title>
      </head>
      <body > ";

//=========================================================================
// body
//=========================================================================
?>
<script type="text/javascript">



window.onload = function(){

    var tid = setInterval(getData, 3000);
    function getData() {
        console.log("Getting  data");
        $.ajax({
            url:		'ajaxManager.php',
            /*dataType:	'json',*/
            dataType:	'text',
            success:	setData,
            type:		'GET',
            data:		{
                domain: '<?php echo("$sel_domain")?>',
                device: '<?php echo("$sel_device")?>'
            }
        });
    }
    function setData(result)
    {
        console.log("data!");
        console.log(result);
        var resArray = result.split("=");
        var n = resArray.length;
        console.log(n);

        var i;
        var input;
        for (i = 1; i <= n;i++)
        {
          console.log(resArray[i]);
          var id = 'no';
          id = id.concat(i.toString());
          input = document.getElementById(id);
          input.value = resArray[i];
          if (resArray[i] == 0)
            input.style.background = "green";
          if (resArray[i] > 0)
            input.style.background = "red";
        }
    }
}
</script>


<?php
      echo("<b>Device Status $sel_domain $now</b>");
      echo "<div class=\"navbar\">";
              
        echo "<a href=\"status.php?do=add_domain\">Add Domain</a>";

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
                    echo "<a href=status.php?do=select&domain=$domain>$domain</a>";
                }
              }
            }
            echo "</div></div>";

      echo "</div>";

      // Ajax fields
      $request = 'http://'.$sel_domain."/gateway.php?do=list_devices";
      //echo $request;
      $ctx = stream_context_create(array('http'=>
       array(
         'timeout' => 2,  //2 Seconds
           )
         ));
      $res = file_get_contents($request,false,$ctx);
      $data = explode(":",$res);
      $num = count($data);

      $nn = 0;
      echo "<div id=\"status\">";
      echo "<table>";
      for ($ii = 0; $ii < $num; $ii++)
      {
        echo "<tr>";
        $id = str_replace(".reg", "", $data[$ii]);
        if (strlen($id) > 2)
        {
          $nn += 1;
          echo "<td>$nn</td>";
          $device = $id;
          
          //$feedback = restApi('list_feedback',$sel_domain,$device);
          //echo "<td>$feedback</td>";
          
          $doc = 'http://'.$sel_domain.'/devices/'.$device;
          $status = getStatus($doc);
          $desc = getDesc($doc);
            
          echo "<td><a href=\"manager.php?do=select&device=$id\" target=\"_blank\">$desc</a></td>";
          $temp = $device;
          if ($status == 0)
          {
            echo("<td><input style=\"background: green;\" id=\"no$nn\" type=\"text\" name=\"n_no\" size=8 value=$status /></td>");
          }
          else {
            echo("<td><input style=\"background: red;\" id=\"no$nn\" type=\"text\" name=\"n_no\" size=8 value=$status /></td>");
          }
        }
        echo ("</tr>");
      }
     echo "</table>";
     echo "</div>";
   //=============================================
if ($form_add_domain == 1)
{
  echo "
  <form action=\"#\" method=\"post\">
    <input type=\"hidden\" name=\"do\" value=\"add_domain\">
    Add Domain<input type=\"text\" name=\"domain\">
    <input type= \"submit\" value=\"Add Domain\">
    </form> ";
}
echo "</body></html>";
// End of file
