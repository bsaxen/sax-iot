<?php
//=============================================
// File.......: graphGateway.php
// Date.......: 2020-10-07
// Author.....: Benny Saxen
// Description: Graph Gateway
//=============================================
$repository = 'iot';
$endpoint   = 'http://localhost:7200';
$pfx_device    = 'PREFIX : <http://rdf.simuino.com/device#>';
$pfx_home      = 'PREFIX : <http://rdf.simuino.com/home#>';
$pfx_vehicle   = 'PREFIX : <http://rdf.simuino.com/vehicle#>';
$pfx_person    = 'PREFIX : <http://rdf.simuino.com/person#>';
$pfx_work      = 'PREFIX : <http://rdf.simuino.com/work#>';
$pfx_company   = 'PREFIX : <http://rdf.simuino.com/company#>';
$pfx_pet       = 'PREFIX : <http://rdf.simuino.com/pet#>';

$date         = date_create();
$obj->sys_ts  = date_format($date, 'Y-m-d H:i:s');

//=============================================
// Library
//=============================================
$values = array();
$count = 0;
//=============================================
// Define recursive function to extract nested values
function printValues($arr) {
//=============================================
    global $count;
    global $values;
    
    // Check input is an array
    if(!is_array($arr)){
        die("ERROR: Input is not an array");
    }
    
    /*
    Loop through array, if value is itself an array recursively call the
    function else add the value found to the output items array,
    and increment counter by 1 for each value found
    */
    foreach($arr as $key=>$value){
        if(is_array($value)){
            printValues($value);
        } else{
            $values[$count] = $value;
            //echo("$count $values[$count]\n");
            $count++;
        }
    }
    
    // Return total count and values found in array
    return array('total' => $count, 'values' => $values);
  }
//=============================================
function sparqlSelectQuery($endpoint,$repository,$query)
//=============================================
{
    global $values,$count;
    $sparql = 'void';
    //echo "$query<br>";
    $query = "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> 
    PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#> 
    PREFIX owl: <http://www.w3.org/2002/07/owl#> ".$query;

    $query = urlencode($query);
    $cmd = "curl -X GET --header 'Accept: application/sparql-results+json' '".$endpoint."/repositories/".$repository."?query=".$query."'";
    $cmd = $cmd." > temp.txt";
    system($cmd);
    $json = file_get_contents('temp.txt');
    //$n = showFileContent("temp.txt");
   
    // // Decode JSON data to PHP associative array
     $arr = json_decode($json,true);
    // // Call the function and print all the values
     $count = 0;
     unset($values); 
     unset($result); 
     $result = array();

     $result = printValues($arr);

     //$sparql = $values[2];
     //echo $sparql;
   
     for ($ii = 0;$ii < $result["total"];$ii++)
     {
        $work = $result["values"][$ii];
        //echo ">> $ii $work\n";
        if ($ii == 2)
        {
          $sparql = $work;
        }
    }

    return $sparql;
}
//=============================================
function sparqlConstructQuery($endpoint,$repository,$query)
//=============================================
{

    $query = "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#> 
    PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#> 
    PREFIX owl: <http://www.w3.org/2002/07/owl#> ".$query;

    echo "$query";
    echo "\n";

    //$query = rawurlencode($query);

    $cmd = "curl -d '".$query."' -H 'Content-Type: application/sparql-query' -X POST '".$endpoint."/repositories/".$repository."'";
    $cmd = $cmd." > temp.txt";
    system($cmd);

    $n = showFileContent("temp.txt");
    if ($n == 0)
    {
      echo "Device is not defined in KG";
    }
    return;
}


//=============================================
function showFileContent($f_file)
//=============================================
{
  $nchar = 0;
  $file = fopen($f_file, "r");
  if ($file)
  {
      while(! feof($file))
      {
        $line = fgets($file);
        echo $line;
        $nchar += strlen($line);
      }
      fclose($file);
  }
  else
  {
      echo ("Sparql Query Error");
  }
  //echo "$nchar";
  return $nchar;
}


//=============================================
// End of library
//=============================================

if (isset($_GET['id']))
{

  $id = $_GET['id'];
  echo ($id);

  // $query ="$prefix  
  //           construct where { 
  //               ?s1 ?p1 :$id .
  //               :$id ?p2 ?o2 . 
  //           } ";
 
  $query ="$pfx_device 
            select ?sparql where { 
                :$id :hasSparql ?sparql . 
            } ";
    

  $res = sparqlSelectQuery($endpoint,$repository,$query);
  if ($res != 'void')
  {
    sparqlConstructQuery($endpoint,$repository,$res);
  }

} // do
else
  echo "Server ok";

//=============================================
// End of File
//=============================================

?>
