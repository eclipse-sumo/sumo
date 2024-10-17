---
title: Build
---

# Build Tools

## dailyBuildMSVC.py

The build script for the nightly windows build. Calls cmake and triggers
release and debug build as well as the tests and installer / zip
creation. Depends on a successful Linux build to pick up the docs.

## dailyUpdateMakeGCC.sh

The build script for the nightly Linux and macOS build. Calls cmake and
triggers release and debug build as well as the tests and the zips.

# Code Cleaning Tools

## checkStyle.py

Checks .java, .cpp, .h and .py files for the correct license header and
applies [autopep8](https://github.com/hhatto/autopep8) and
[flake8](https://github.com/PyCQA/flake8) for PEP 8 compatibility. Also
applies [astyle](https://astyle.sourceforge.net/) to all .java, .cpp, and
.h files. The position of the source tree(s) to check is given as
optional arguments or determined relative to the position of the script.
This call is part of our nightly tests. The script is able to fix some
minor whitespace / naming issues when called with the "--fix" flag. The
PEP 8 checks are quite slow, so if only a header check is wished, run
with the option "--skip-pep".

Call:

```
checkStyle.py
checkStyle.py --fix <SUMO_HOME>/tools
```

# Documentation Tools

## buildHTMLDocs.py

Converts wiki-documentation into static HTML pages. It removes the wiki
navigation and adds user doc navigation while keeping the contents
intact. This script is called by the main Makefile if "make docs" is
called. If the script is called with an argument, it tries to fetch the
wiki site with the given name, if not it retrieves all sites listed in
the [table of contents](../index.md). Call
"buildHTMLDocs.py --help" for a complete list of options.

Call:

```
buildHTMLDocs.py SUMO
```

## checkAuthors.py

Checks the log for all source files for authors and "thanks" and checks
whether they appear in the file header. It can optionally try to fix the
file header and also update the global AUTHORS file if needed.

Call:

```
checkAuthors.py
```

# Translation Tools

## updateMessageIDs.py

SUMO uses the original string from the source code as message ID in gettext.
All translations are bound to a message ID. The script offers help in changing
the message ID / the original string in cases where it is profitable to keep
existing translations. As recurrent uses of the same original string are
grouped in gettext into the same message ID (except for different context),
strings shall not be edited directly in the source code. The work flow of the
script is the following:

- The script `i18n.py` is called to ensure the `.po` files correspond to the current source code.
- For every available `.pot` file an English to English `.po` file with identical **msgid** and **msgstr** values  is created.
- The user can choose to edit the **msgstr** values in this file manually to reflect the wanted changes or use one of the predefined actions of the script (e.g. simple search/replace).
- The script reads the changes and the occurrences to patch the corresponding lines in the source code.
- The `.po` translation files are patched as well. In case the changed **msgid** already existed before, both entries are merged.

Below there are the options the script provides and how to use them.

- **--lang**: Update only the `.po` files corresponding to the given sequence of language codes
- **--start**: Will create/overwrite the English to English `.po` file in the `**SUMO_HOME**/data/po` directory
- **--strict**: Change the **msgid** value only if it corresponds completely to a given search string
- **--replace**: Followed by an even number of arguments where the odd strings are the search pattern and the following ones the corresponding replacement string
- **--search-prefix <characters>**, **--search-suffix <characters>**: Adds any of these characters as a prefix/suffix to any of the given search patterns (not with **--strict**)
- **--apply**: Patch source code and `.po` translation files
- **--remove-obsolete**: Directly remove entries which have become obsolete (otherwise only the occurrence comments in gettext are moved to the replacement **msgid**)
- **--process-languages**: Apply the search/replace logic on the translated **msgstr** values as well
- **--sumo-home <home>**: The SUMO home directory to use
- **--placeholder <placeholder>**: The placeholder string used in **msgid**
- **--numbered-placeholders**: Add an order number to every placeholder given by **--placeholder** inside every **msgid**, starting with 0

An example call to replace all minor case uses of the abbreviation "id" with the upper case version "ID" for all languages:

```
updateMessageIDs.py --start --replace "id" "ID" --apply
```

If the changes are done manually inside the English to English `.po` file, it is necessary to call the script twice. The first call is merely for creating the file to edit:

```
python updateMessageIDs.py --start
```

Then the changes are processed in the second call:

```
python updateMessageIDs.py --apply
```

## obsoleteTranslations.py

The script processes translations which have been lost due to changes of the original string in
the source code but are still present in the `.po` translation files. It can be restricted to languages given by their language codes
using the option **--lang <langCode1> [<langCode2>]**. If the said option is not given, all existing language files will be processed.

In order to write a protocol of the obsolete translations (those which are not referenced in the source code),
the script has to be called with a file path to write the protocol to as follows:
```
python obsoleteTranslations.py --output obsolete.txt
```
Adding the option **--clear** will remove the obsolete translations permanently from the `.po` files.

There is an option called **--patch** to link obsolete translations again if the updated message ID / the current source string is known .
The script is provided with a sequence of arguments which form pairs of first the outdated message ID and then new updated one.
```
python obsoleteTranslations.py --patch "Add Sidewalk" "Add sidewalk" "Overrids default shape of pedestrian sidewalk" "Overrides default shape of pedestrian sidewalk"
```
The script looks into the translation files and moves the actual translations from the obsolete to the current entry (only if the current one is still empty). The obsolete entry
is removed in case its content has been transferred to the current entry.
