<!DOCTYPE HTML>
<html>
<head>
    <title>OpenLayers Basic Example</title>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/openlayers/2.11/lib/OpenLayers.js"></script>
    <script>
      function init() {
        map = new OpenLayers.Map("mapdiv");
        var mapnik = new OpenLayers.Layer.OSM();
        map.addLayer(mapnik);

        var lonlat = new OpenLayers.LonLat(-1.788, 53.571).transform(
            new OpenLayers.Projection("EPSG:4326"), // transform from WGS 1984
            new OpenLayers.Projection("EPSG:900913") // to Spherical Mercator
          );

        var zoom = 13;

        var markers = new OpenLayers.Layer.Markers( "Markers" );
        map.addLayer(markers);
        markers.addMarker(new OpenLayers.Marker(lonlat));

        map.setCenter(lonlat, zoom);
      }
    </script>

    <style>
    #mapdiv { width:350px; height:250px; }
    div.olControlAttribution { bottom:3px; }
    </style>

</head>

<body onload="init();">
    <p>My HTML page with an embedded map</p>
    <div id="mapdiv"></div>
</body>
</html>