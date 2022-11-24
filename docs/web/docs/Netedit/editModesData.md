---
title: Edit Data Modes
---

# Data specific modes ![](../images/supermodedata.png)

EdgeData are elements associated with one or several edges, used for multiple purposes (especially Demand). They have their own mode, selectable through the top button or by pressing F4.

![](../images/DataMode1.png)

Data mode overview

Edge datas are defined within a general set (DataSet), and within a certain time interval

![](../images/DataMode2.png)Defining a new data set

![](../images/DataMode3.png)Defining a data interval

![](../images/DataMode4.png)Defining parameters 

## Edge data mode ![](../images/edgedata.png)

EdgeData are created over a single edge.

![](../images/DataMode5.png)Click over an edge to create a edgeData

![](../images/DataMode6.png)Inspecting an edge data

## Edge relation mode ![](../images/edgereldata.png)

EdgeRelDatas are created over two edges

![](../images/DataMode7.png)Click over first edge

![](../images/DataMode8.png)Click over second edge to create path

![](../images/DataMode9.png)Press enter to create edge rel data

## TAZ relation mode ![](../images/modetazreldata.png)

This mode helps to visualize the existing relation and the traffic demand amount between any two TAZ (Traffic Analysis Zone) after loading the respective xml file containing the information about start TAZ, end TAZ, interval, vehicle type and demand count. So, with this mode, users can have a better overview about the distribution of traffic demand between Tazs. The first steps are:
 1. load the network file.
 2. load the file containing the taz-polygon information under "File\Additionals and Shapes\Load Additionals..."
 3. load the file containing the counts of the OD-relations.

The precise way to load files and the available features are explained below.

### Data loading
The basic data format is as following:

```xml
<data>
    <interval id="DEFAULT_VEHTYPE" begin="0.00" end="86400.99"> \
        <tazRelation from="17" to="20" count="3965"/> \
        <tazRelation from="17" to="24" count="3100"/> \
        <tazRelation from="20" to="34" count="3192"/> \
        <tazRelation from="22" to="20" count="4575"/> \
        <tazRelation from="22" to="24" count="3576"/> \
        <tazRelation from="23" to="35" count="3267"/> \
      
    ... information about further TAZs ...
</data>
```
Other attributes can also be included in the file, such as color.

Before loading the data related to TAZ-relations the respective network should be loaded in netedit firstly. After that, the respective TAZ file containing the polygon information about each TAZ needs to be loaded with "Load Additionals" by selecting *File-\>Additionals and Shapes* in netedit. An example of a TAZ file is shown below.

```xml
<additional>
    <taz id="1" shape="23356.47,20662.79 22463.50,20524.87 22401.18,21140.95 23162.64,21353.48 23356.47,20662.79" color="51,128,255"> \
        <param key="Id" value="59"/> \
      
    ... information about further TAZs ...
    </taz>

</additional>
```
Now, file(s) containing TAZ-Relations can be loaded with "Load Data Elements" by selecting *File-\>Data Elements*. After that, the button "Data" in the tool bar should be pressed. All TAZ-relations will then be shown, and can be further hidden by pressing the icons "Draw TAZRel only from" and "Draw TAZRel only to" under the mode "Data" (see the illustration below).

![](../images/drawRelFromTo.png)

After that, if you want to see the relations between all TAZs again, you need to go back to the mode "Network" and then go to the mode "Data" again. By activating the aforementioned two icons, you can see all TAZ-relations.

### Display the TAZ polygons by different colors
You can click "Polygons" in the window shown after selecting *Edit-\>Edit Visualisation* and change the colors randomly or by selection. You can also directly click the color palette icon "View Settings" in the tool bar to open the respective window and make color changes.

![](../images/taz_overview_color.png)

Polygons can also be unfilled by clicking the icon "Draw TAZ fill" (see the picture below). However, it is much easier to select TAZ when they are filled.

![](../images/tazfill.png)

The way to draw the relation line can be either from centroid to centroid or from border to border by clicking "Draw TAZREL drawing mode" (see the example below).

![](../images/centroid.png)

### Display the relation lines by color according to the amount of TAZ demands 
It is also possible to display the relation lines with different colors according to the respective amount of TAZ demands. The way to set it is shown below.

![](../images/coloredDemand.png)

The demand range for each color can be further adjusted/customized as well.

### Change the width size of the TAZ relation lines
The width of the relation lines can be adjusted. It is especially useful for better visualisation when there are many edges in the given network. Firstly, you click "Data" in the window shown after selecting *Edit-\>Edit Visualisation* or after clicking the color palette icon "View Settings". After inputting the desired width size in the rubric "Exaggerate tazRelation width by" you click "openGL", press the button "Recalculate boundaries" and then press "OK" so that the adjusted relation lines can be properly shown. The buttons/rubrics you need to click are illustrated below.

![](../images/setWidth_0.png)
![](../images/setWidth.png)

### Inspect a specific TAZ relation
A TAZ can be selected under the Mode "Inspect". It is easier to do the selection when (1) the icon "Draw TAZ fill" is activated, and (2) the display of TAZ-relation lines is off. The latter one is under implementation. So, you firstly click some TAZ which can be seen after (1) under the inspect mode. Then you click the desired TAZ. After that, you can see the TAZ-relation lines from/to the selected TAZ. You can also choose only to show the relation lines either to or from the selected TAZ. The respective illustration is shown below.

![](../images/selectSingleTaz_from.png)
![](../images/selectSingleTaz_to.png)

### Select TAZ relations

#### with one demand amount criterion
If only certain TAZ relations should be observed, such as with larger demand amount, it is possible to make TAZ-relation selection. The buttons/items you need to click/choose are circled in red in the picture below. Then you need to input your selection criterion in the rubric (3000 in the example below), circled in blue. After that, you can see the amount of the selected TAZ-relations, circled in yellow. So, in this example, it means that 34 TAZ-relations have a demand amount larger than 3000.

![](../images/select_0.png)

#### with two or more demand amount criteria
If the TAZ-relations with a demand amount between two criteria need to be observed, you need to further remove the TAZ-relations with a demand amount larger than the higher criterion after you selecting the TAZ-relations with the lower criterion, as illustrated below, i.e. click the rubric "remove" (circled in green) , change the criterion to 5000 (circled in brown) and press the ENTER button in this example, and then you can see that 19 TAZ-relations (circled in green) are selected.

![](../images/select_1.png)

If you have more than two criteria, you need to repeat the aforementioned procedure.

#### only see/keep the data of the selected TAZ
To only see the selected TAZ relations you need to further press the button "Reduce" (see the illustration below). You can again adjust the width size of the TAZ-relations to get a proper view.

![](../images/select_2.png)

!!! caution 
    The "Reduce" action is mainly to help to get a better view about the demand distribution of the selected TAZ. Only the data of the selected TAZ relations will be kept after clicking "Reduce". If the save action (not save as) is made after the action "Reduce", the respective file will be overwritten and the data of other unselected TAZ will be lost. You can of course save the selected data with the "save as" action if it is your intention (see the next section).


### Save the selected TAZ relations

The selected TAZ relations after the "Reduce" action and the respective attributes can be easily saved in XML format (see the illustration below).

![](../images/saveSelected.png)

