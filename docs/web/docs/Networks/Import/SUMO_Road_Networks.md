---
title: SUMO Road Networks and Patching
---

[netconvert](../../netconvert.md) allows a round-trip reimport of
generated SUMO road networks. Use the option **--sumo-net-file** {{DT_FILE}} for reading a SUMO road
network, the shorter options **--sumo-net** {{DT_FILE}} and **--sumo** {{DT_FILE}} are synonymes. An example call:

```
netconvert --sumo-net-file mySUMOnet.net.xml -o mySUMOnet2.net.xml
```

If your original network has been generated with the same SUMO version,
both the read network ("mySUMOnet.net.xml") and
the generated one ("mySUMOnet2.net.xml") should be same besides the
meta-information in the XML header. Please note that this is also
a simple way to update your SUMO network from an earlier version
although it is usually more advisable to regenerate the network from the original data.

# Patching

Re-importing SUMO networks allows you to apply changes to an existing
road network using additional
[XML-files](../../Networks/PlainXML.md).
An example would be patching the type of a node.

prepare a file like this: patch.nod.xml :

```xml
<nodes>
    <node id="id_of_the_node_you_want_to_modify" type="right_before_left"/>
<nodes>
```

and patch the network like this:

```
netconvert --sumo-net-file your.net.xml --node-files patch.nod.xml -o yourpatched.net.xml
```

## Patching in stages
Most of the time you will want to modify your SUMO network after the initial import.
To do so you can of course edit the file directly with netedit and store the changed
network. If you want to track your changes however and maybe reapply them when your
input changes (for instance an update from OpenStreetMap) you should save them as patch files.

Those patches can be genereated automatically with [netdiff.py](../../Tools/Net.md#netdiffpy).

Since you might have enabled settings like junction joining or removal of geometry nodes,
those patch files may not be applicable directly on the import of the raw data but will need
a second call to netconvert. You can sometimes to avoid this second step (see the linked
explanations to netdiff) but in the following situations it is probably unavoidable:

- correction of merged tram and road edges together with analysis of railway topology (ptline-output)
- 
