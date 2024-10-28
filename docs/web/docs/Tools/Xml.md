---
title: Xml
---

## xml2csv.py

This converts any of the sumo xml files into a flat csv (character
separated values) representation. CSV-Files are easy to parse and can be
opened in [LibreOffice](https://www.libreoffice.org/) or Microsoft Excel.
Usage:

```
python tools/xml/xml2csv.py input.xml
```

With the option **--separator** {{DT_STR}} you can customize the field separator (default is
**;**), the option **--quotechar** {{DT_STR}} defines a quoting character such as ' which will be
used to quote all fields. Furthermore you can supply an xsd schema file
using **--xsd** {{DT_FILE}} to aid the parsing and get headings also for attributes which may
be not in the current file. If you give the additional **--validation** option the input
file will be validated against the schema. This requires
[lxml](https://lxml.de/) to be installed.

If you do not use the **--xsd** option
the script will determine the attributes to parse from the first line (or block) it parses.
If your file does not contain all wanted attributes in the first occurring element they will not get
parsed!

Please note that instead of an input file name you can give a number
which will be interpreted as a server port where the script listens for
incoming connections and then reads the input data from. The name for
the output file can be given using the **--output** {{DT_FILE}} option with the default being
the path of the input file with the ".xml" suffix replaced by ".csv".

The application is limited to simple hierarchies, where the root element
may contain several different child elements but all of those are
limited to only one type of child elements.

!!! note
    Excel may be able to open XML files directly. see[\[1\]](https://www.google.de/search?q=Import+an+XML+data+file+as+an+XML+table).

## csv2xml.py

This is the inverse tool to xml2csv.py. Usage:

```
python tools/xml/csv2xml.py -x schema.xsd input.csv
```

The options have the same meaning as above. For some file types as
nodes, edges and routes the conversion is hard coded for all the others
a [schema](../XMLValidation.md#adding_a_schema_declaration) must be
provided. It is not possible to automatically perform validation on the
output.

## xml2protobuf.py

If the CSV representation is still too big you can compress the output
even further using Google's [Protocol
Buffers](https://developers.google.com/protocol-buffers/). This will
require the protoc compiler as well as the protobuf packages for python
and your target language to be installed. Usage:

```
python tools/xml/xml2protobuf.py -x schema.xsd input.xml
```

The xsd schema file needs to be given the using **--xsd** {{DT_FILE}} and the **--validation** option will
enable XML validation. The additional **--protodir** {{DT_FILE}} option will give a directory
where the protobuf message definitions and the generated python module
will be saved (default is the current working dir). The script will
generate the protobuf description from the schema and then write a
protomsg file containing the binary stream.

## protobuf2xml.py

This is the inverse tool to xml2protobuf.py. Usage:

```
python tools/xml/protobuf2xml.py -x schema.xsd input.protomsg
```

The options have the same meaning as above.

<div style="border:1px solid #909090; min-height: 35px;" align="right">
<span style="float: right; margin-top: -5px;"><a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm"><img src="../images/FP7-small.gif" alt="Seventh Framework Programme"></a>
<a href="https://web.archive.org/web/20180309093847/https://amitran.eu/"><img src="../images/AMITRAN-small.png" alt="AMITRAN project"></a></span>
<span style="">This part of SUMO was developed, reworked, or extended within the project
<a href="https://web.archive.org/web/20180309093847/https://amitran.eu/">"AMITRAN"</a>, co-funded by the European Commission within the <a href="https://web.archive.org/web/20191005024529/https:/ec.europa.eu/research/fp7/index_en.cfm">Seventh Framework Programme</a>.</span></div>


## changeAttribute.py

This sets/removes the specified attribute on all tags (elements) in the input file and writes the result to a new file
If option **--value** (**-v**) is not given, the attribute is removed. Otherwise it is added/modified to the given value.

```
python tools/xml/tools/xml/changeAttribute.py -f INPUT_FILE -o OUTPUT_FILE -t TAG -a ATTRIBUTE [-v VALUE]
```

## filterElements.py

This removes the specified element from the input file subject to the following conditions:

- all occurrences of the element (default)
- all elements that have a specified attribute matching a value from a given list (**--attribute** ATTR **--remove-values VAL1,VAL2**)
- all elements that have a specified attribute not matching a value from a given list (**--attribute** ATTR **--keep-values VAL1,VAL2**)

Example call:
```
python tools/xml/tools/xml/filterElements.py -f stopoutput.xml -o filtered.xml -t stopinfo -a delay --remove-values 0
```
