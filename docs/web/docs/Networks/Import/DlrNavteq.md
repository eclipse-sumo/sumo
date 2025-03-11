---
title: DlrNavteq
---

The DLR-Navteq importer reads Navteq's GDF databases converted using
Elmar's tools. The importer reads unsplitted network versions. The
option for reading them is **--dlr-navteq-prefix** {{DT_FILE}} or **--dlr-navteq** {{DT_FILE}} for short. Please note that the prefix
must be given only, example:

```
netconvert --dlr-navteq=berlin_ --output-file=MySUMOFile.net.xml
```

# Format
The DLR-Navteq format encodes geometries in a tabular structure with the following columns separated by tabs `\t`:

| Column index | Meaning                                    |
|--------------|--------------------------------------------|
| 1            | Geometry feature ID                        |
| 2            | City                                       |
| 3            | Type                                       |
| 4            | Name                                       |
| 5            | X value of the first geometry point        |
| 6            | Y value of the first geometry point        |
| ...          |                                            |
| 5 + 2n       | X value of the nth geometry point          |
| 6 + 2n       | Y value of the nth geometry point          |

The X and Y values can be supplied in geographic coordinates and will be projected during the import.
