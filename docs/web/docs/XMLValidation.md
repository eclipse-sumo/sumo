---
title: XMLValidation
---

# Validation of XML inputs

All [SUMO applications](SUMO_at_a_Glance.md#included_applications) support
XML validation for their inputs. To enable this, the following options
can be used:

| Option  | Description  |
|---|---|
| **-X** {{DT_STR}}<br> **--xml-validation** {{DT_STR}} | Set schema validation scheme of XML inputs ("never", "local", "auto" or "always"); *default:* **local** |
| **--xml-validation.net** {{DT_STR}} | Set schema validation scheme of SUMO network inputs ("never", "local", "auto" or "always"); *default:* **never** |
| **--xml-validation.routes** {{DT_STR}} | Set schema validation scheme of SUMO route inputs ("never", "local", "auto" or "always"); *default:* **local** |

Validation is performed by activating [XML schema
processing](https://xerces.apache.org/xerces-c/schema-3.html) in the XML
parser. This catches many common input errors such as spelling mistakes
and attributes which should have been placed within another element.

Another prerequisite for validation is a schema declaration within the
root element of the input file such as

```xml
<routes xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="https://sumo.dlr.de/xsd/routes_file.xsd">
```

When setting the validation options to *always* the schemata for networks, additional files and routes
are pre-parsed from the local [schema file](#schema_files). For all other file types a missing declaration will lead to an error in this case.

The validation option *auto* (which was the default before SUMO 1.15.0) in contrast only validates the file
if the schema declaration is present. It tries to parse the schema first from the local path,
see [the section schema files](#schema_files) and falls back to the web version if *SUMO_HOME* is not set or
the file could not be found.  The current default *local* has no fallback, so validation will fail
if the local file is not accessible (or SUMO_HOME is not set). This is to prevent unintended network access and [XXE attacks](https://en.wikipedia.org/wiki/XML_external_entity_attack).

Schema validation may slowdown XML parsing considerably and is therefore
disabled for the network input by default (because networks should not
be edited by hand and therefore be valid anyway).

!!! note
    If you have large auto-generated inputs you may consider disabling schema validation altogether. See also [this FAQ](FAQ.md#whats_the_deal_with_schema_resolution_warnings_xsd_errors)

# Adding a schema declaration

Files that are written by one of the SUMO applications automatically
receive the appropriate schema declaration. When writing an input file
from scratch the schema declaration must be added manually to the root
element as follows:

```xml
    <ROOT_ELEMENT xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance"
         xsi:noNamespaceSchemaLocation="https://sumo.dlr.de/xsd/SCHEMA_FILE">
```

where *ROOT_ELEMENT* and *SCHEMA_FILE* should be set according to the
following table:

| Application Option                                    | ROOT_ELEMENT | SCHEMA_FILE          |
|-------------------------------------------------------|--------------|----------------------|
| **--route-files**, **--trip-files**, **--flow-files** | routes       | routes_file.xsd      |
| **--additional-files**                                | add          | additional_file.xsd  |
| **--node-files**                                      | nodes        | nodes_file.xsd       |
| **--edge-files**                                      | edges        | edges_file.xsd       |
| **--connection-files**                                | connections  | connections_file.xsd |
| **--tllogic-files**                                   | tlLogics     | tllogic_file.xsd     |
| **--type-files**                                      | types        | types_file.xsd       |
| **--weight-files**                                    | meandata     | meandata_file.xsd    |

!!! note
    The value of ROOT_ELEMENT is a matter of convention and may take an arbitrary value.

# Schema Files

The schema files can be found in the {{SUMO}}/data/xsd directory of your SUMO
installation. If the [environment variable *SUMO_HOME* is
set](Basics/Basic_Computer_Skills.md#sumo_home),
these files will be used when validating inputs.

Otherwise, the files are loaded from
*<https://sumo.dlr.de/xsd/SCHEMA_FILE>* which may slow down the
application (or fail if there is no internet connection).

# SUMO File Types

A complete listing of file extensions used by [SUMO
Applications](SUMO_at_a_Glance.md#included_applications) is
given at [File_Extensions](Other/File_Extensions.md).

# Disabling Schema Validation

To disable schema validation any of the following actions is sufficient

- set the option **--xml-validation never**
- Remove the `xmlns:xsi="https://www.w3.org/2001/XMLSchema-instance"` `xsi:noNamespaceSchemaLocation="https://sumo.dlr.de/xsd/....xsd"`
attribute from the top of the XML input file.
