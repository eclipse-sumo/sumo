---
title: Purgatory/JoiningNetworksWithDemands
permalink: /Purgatory/JoiningNetworksWithDemands/
---

## Step1: Build joined Network

### Step1.1: Make Network Parts unique

Both networks must have distinct ids. The most straight forward approach
seemed to be to use a prefix. The edges and nodes of both networks were
changed by adding an "a" and a "b", respectively. This was done using a
text editor. Because edges reference nodes and connections, all three
files - edges, nodes, and connections - must have been changed. The
following replacements were done:

- in nodes:
  - id=" to id="a
- in edges:
  - id=" to id="a
    - watch out - replacing the id also replaces lane ids; this
      must be reset afterwards
  - from=" to from="a
  - to=" to to="a
- in connections:
  - from=" to from="a
  - to=" to to="a
  - additionally, the prohibitions were searched and changed
  - watch out - some to-definitions are empty (dead ends) must be
    reset to these afterwards

### Step1.2: Find and remove overlapping Parts

For the used networks, their overlapping parts, edges, and nodes were
determined. For each combination, it was decided which shall be kept -
the other one will be not included in the final network. I decided to
set up a map for edges as following:

```
a86->b2[0]
a85->b1[1]
b44->a67
b81->a11
b43->a68
b40[0]->a72[0],a72[1]
b41->a17
b42->a71
b40[1]->a69
b48->a181,a18
b54->a224[1]
a136->b49[0]
a135->b50[1]a
a179->b47
a180->b46
a84->b38[0],b38[0]a
a12->b45
a83->b39[1][1][1]
a137->b36
a138->b37
a121->b35[1][1][1][1]
a114->b12
```

The meaning is that the edge in front of the "-\>" will be later
replaced by the one after the "-\>". One may note that in some places,
two edges were found, where in the other network, only one existed.

The edges in front of the "-\>" were removed from the according edges
file. They of course must also be replaced within the connections file.
Due to the relatively large effort, this was done using a tiny script.
"mapEdgesInConnections.py" obtains the map-file (assuming it is named
"edgemap.txt", and a connections file and simply replaces all
occurrences of the edges' to replace ids by the ids of the corresponding
edges to keep. The connections file must be given as first parameter, as
output a file with the given file name with appended ".mod.xml" is
generated.

Corresponding junctions were also determined and written as above. The
last entry is in brackets, because it was assumed to be not
participating in the later following re-projection of one of the
networks.

```
b1->a9
b36->a15
a81->b39
(a57->b32)
```

Of course, the from/to information within the edges file must be mapped,
too. Due to the small number of replacements to be done, this was done
using a text editor. Also, the information about "controlled inner
edges" was patched.

### Step1.3: Determine and apply the Network Offsets

As both networks have their origin at around (0,0), we have to apply an
offset to one of the networks, so that they are not over each other. We
use the previously built road networks and the positions of the
overlapping nodes found in step 1.2.

```
a9/b1: 583.69,1061.56  /  929.79,278.76   --> -346.1,782.8
a15/b36: 798.63,985.75  /  1161.21,244.44 --> -362.58,741.31
a81/b39: 852.00,1127.00  / 1199.56,342.43 --> -347.56,784.57
```

As may be noted, the a15/b36 combination diverges from the others. A
visual inspection proves that the nodes' positions differ from their
actual shapes. That's why the middle node is removed from the
computation. The mean offset (a-b) is then:

```
-346.83,783.685
```

This offset must be applied to one network's node positions and edge
shapes. Due to large number, again two scripts were prepared,
xmlnodes_applyOffset.py and xmledges_applyOffset.py.

### Step1.4: Join Networks

Simply call netconvert using all generated files.

## Step2: Join Routes

-----

583,69-929,79=-346,1 1061,56-278,76=782,8 798,63-1161,21=-362,58
985,75-244,44=741,31 852,00-1199,56=-347,56 1127,00-342,43=784,57

(-346,1+-347,56)/2=-346,83 (782,8+784,57)/2=783,685