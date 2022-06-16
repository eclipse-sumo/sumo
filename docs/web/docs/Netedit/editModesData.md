---
title: editModesData
---

# Data specific modes

EdgeData are elements associated with one or several edges, used for multiple purposes (especially Demand). They have their own mode, selectable through the top button or by pressing F4.

![](../images/DataMode1.png)Data mode overview

Edge datas are defined within a general set (DataSet), and within a certain time interval

![](../images/DataMode2.png)Defining a new data set

![](../images/DataMode3.png)Defining a data interval

![](../images/DataMode4.png)Defining parameters 

## Edge data mode

EdgeData are created over a single edge.

![](../images/DataMode5.png)Click over an edge to create a edgeData

![](../images/DataMode6.png)Inspecting an edge data

## Edge rel data mode

EdgeRelDatas are created over two edges

![](../images/DataMode7.png)Click over first edge

![](../images/DataMode8.png)Click over second edge to create path

![](../images/DataMode9.png)Press enter to create edge rel data

## TAZ relation mode

This mode helps to visualize the existing relation and the traffic demand amount between any two TAZ (Traffic Anaylsis Zone) after loading the respective xml file containing the information about start TAZ, end TAZ, interval, vehicle type and demand count. So, with this mode, users can have an better overview about the distrubtuion of traffic demand between Tazs. The way to load a input file and the available features are explained below.

### Data loading
The data format is as following:

```xml
<data>
    <interval id="DEFAULT_VEHTYPE" begin="0.00" end="86400.99"> \
        <tazRelation from="17" to="20" color="red" count="3965"/> \
        <tazRelation from="17" to="24" color="red" count="3100"/> \
        <tazRelation from="20" to="34" color="red" count="3192"/> \
        <tazRelation from="22" to="20" color="red" count="4575"/> \
        <tazRelation from="22" to="24" color="red" count="3576"/> \
        <tazRelation from="23" to="35" color="red" count="3267"/> \
      
    ... information about further TAZs ...
</data>
```
Before loading the data related to TAZ-relations the repsective network should be loaded in netedit firstly. After that, the respective TAZ file containing the polygon information about each TAZ needs to be loaded with "Load Additionals" by selecting *File-\>Additionals and Shapes* in netedit. An example of a TAZ file is shown below.

```xml
<additional>
    <taz id="1" shape="23356.47,20662.79 22463.50,20524.87 22401.18,21140.95 23162.64,21353.48 23356.47,20662.79" color="51,128,255"> \
        <param key="Id" value="59"/> \
      
    ... information about further TAZs ...
    </taz>

</additional>
```
Now, file(s) containing TAZ-Relations can be loaded with "Load Data Elements" by selecting *File-\>Data Elements*. After that, the button "Data" in the tool bar should be pressed. All TAZ-relations will then be shown.

### Show the TAZ pologons by different colors
You can go to "Polygons" by selecting *Edit-\>Edit Visualisation* and change the colors randomly or by selection.
![taz_overview_color](https://user-images.githubusercontent.com/26454969/174117684-12456e44-96ef-450d-a871-63430346d642.png)

You can also unfill the pologons by clicking the icon "Draw TAZ fill" (see the picture below).
![tazfill](https://user-images.githubusercontent.com/26454969/174118706-fb34ffc3-9cb3-4512-a0e9-2afb2e038016.png)

The way to draw the relation line can be either from centroid to centroid or from border to border by clicking "Draw TAZREL drawing mode" (see the example below).

![centroid](https://user-images.githubusercontent.com/26454969/174121039-3ac08f61-e6a6-45e8-99eb-114ff4b3c981.png)

### Display the amount of TAZ demand by color
It is also possible to display the relation lines with different colors according to the respective amount of TAZ demand. The way to set it is shown below.

![coloredDemand](https://user-images.githubusercontent.com/26454969/174121768-e4ba38af-3b9f-441f-b98a-33275adcc27a.png)

### Select TAZ relations

### Change the width size of the TAZ relation lines





