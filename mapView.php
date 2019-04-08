<?php

$sel_label = "default";
//$do = "ls *.map > map.work";
//echo $do;
//system($do);


?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="utf-8">
	<title>OpenStreetMap &amp; OpenLayers - Marker Example</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
	<link rel="stylesheet" href="https://openlayers.org/en/v4.6.5/css/ol.css" type="text/css">
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
  <script src="http://www.openlayers.org/api/OpenLayers.js"></script>
    <!-- font-awesome is an iconic font, which means we can draw resolution-independent icons -->
    <link href="//netdna.bootstrapcdn.com/font-awesome/3.2.1/css/font-awesome.css" rel="stylesheet">

<script type="text/javascript">
window.onload = function()
{
    initialize_map(14.317,59.502); 
    //add_map_point(-31.8688, 151.2093);

    var tid = setInterval(getData, 5000);
    var counter = parseFloat(0.0);
    function getData() 
    {
        //counter = parseFloat(counter) + parseFloat(0.0001);
        console.log("Getting  data");
        $.ajax({
            url:		'mapAjax.php',
            /*dataType:	'json',*/
            dataType:	'text',
            success:	setData,
            type:		'GET',
            data:		{
                label: '<?php echo("$sel_label")?>'
            }
        });
    }
    function setData(result)
    {
        console.log("data!");
        console.log(result);
        var resArray = result.split(",");
        var n = resArray.length;
        console.log(n);
      
        //var lat = (parseFloat(resArray[0]) + parseFloat(counter)).toFixed(4);
        var lat = resArray[0];
        var lon = resArray[1];
        var status = resArray[2];
        console.log(lat);
        console.log(lon);
        console.log(counter);
        update_map(lat,lon);
        //add_map_point(lat,lon);
    }
}
var map;
var markers;
var zoom=16;
var old_mm;
var_new_mm;

function update_map(lat,lon)
{
  var lonLat = new OpenLayers.LonLat( lat ,lon )
          .transform(
            new OpenLayers.Projection("EPSG:4326"), // transform from WGS 1984
            map.getProjectionObject() // to Spherical Mercator Projection
          );
  markers.removeMarker(old_mm);
  //zoom = 16;
  new_mm = new OpenLayers.Marker(lonLat);
  markers.addMarker(new_mm); 
  //map.setCenter (lonLat, zoom);
  old_mm = new_mm;
}

function initialize_map (lat,lon)
{
    map = new OpenLayers.Map("map");
    map.addLayer(new OpenLayers.Layer.OSM());

    var lonLat = new OpenLayers.LonLat( lat ,lon )
          .transform(
            new OpenLayers.Projection("EPSG:4326"), // transform from WGS 1984
            map.getProjectionObject() // to Spherical Mercator Projection
          );
          
    zoom=8;

    markers = new OpenLayers.Layer.Markers( "Markers" );
    map.addLayer(markers);
    
    var mm = new OpenLayers.Marker(lonLat);
    markers.addMarker(mm);
    
    map.setCenter (lonLat, zoom);

    markers.removeMarker(mm);
}
  </script>
</head>
<body>
  <div id="map" style="width: 100vw; height: 100vh;"></div>
</body>
</html>
