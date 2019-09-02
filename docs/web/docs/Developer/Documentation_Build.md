---
title: Developer Documentation Build
permalink: /Developer/Documentation_Build/
---

When accessing SUMO web space, you will find an index page, and several
additional HTML pages. You will also find a link to a wiki and to the
[GitHub presence](https://github.com/DLR-TS/sumo). In the following, it
is described how these documentation parts are build, and mirrored.

After some tries, we decided to work as following:

  - The documentation is "developed" within the wiki; the wiki refers to
    the state of the package as-is within the current head of the master
    branch of the main [Git repository](https://github.com/DLR-TS/sumo).
  - The HTML documentation is generated from the wiki

# Documentation Parts

## Wiki

**scope**

  -
    The wiki should include the complete user documentation, as well as
    finalized developer documents, and any kind of additional
    documentation as long as it is in a state which can be presented.
    Incomplete or outdated wiki documents should have the prefix
    "Purgatory/" and should not be linked from any other documents.

**rights**

  -
    The rights to edit the wiki are given to known users on request.

**backup**

  -
    <font color="red">Redescribe</font>; Wiki-pages can be mirrored
    using *tools/build/mirrorWiki.py*

There are currently two options for looking up information from wiki
versions before 0.13.1

  - Use the wiki-history for an individual wiki page
  - Use the *sumo-user.pdf* included in older releases. This pdf was
    compiled from the wiki at the time of the release and should contain
    most of the wiki content.

## HTML pages

**scope**

  -
    HTML pages contain all information stored in the wiki at the time of
    the last release.

**rights**

  -
    HTML pages are generated and uploaded by SUMO developers which have
    access to the web space.

**backup**

  -
    A HTML-backup of the wiki is created as part of the linux build. The
    file  */Makefile.am* defines the task *userdoc* which calls the
    script *tools/build/buildHTMLDocs.py* to create an offline-browsable
    mirror (excluding special pages such as history and discussion). The
    backup is included in the regular release starting at version
    0.13.1.

# HTML pages generation

The HTML documentation pages are built from the wiki. All scripts needed
are located in /tools/build. `buildHTMLDocs.py` is used to retrieve the
HTML-representation of wiki-pages. If started with a parameter, this
parameter is assumed to be the name of the page to retrieve. If no
parameter is given, `getWikiPages.py` retrieves the list of all pages,
first, from Special::AllPages.

Each of the pages to retrieve is read from the wiki and stripped so that
it contains the article HTML only. Links are patched to match the depth
of the folder the page is located in. `buildHTMLDocs.py` changes the
links included in this result and retrieves the embedded images (only
the "normal" version, no thumbnails) and saves the resulting HTML with
no header/tail into a file located in "mirror" named using the articles'
name. The folder "mirror" is generated.

All images found in the read pages are retrieved and stored in
"mirror/images".

The HTML template is located in /docs/wiki as "index.html".
`getWikiPages.py` reads the wiki-text of "SUMO User Documentation",
extracts the content list, parses it into HTML, and embeds it into
"index.html" between the comments and .

Then, the pages to convert stored in "mirror" are read and their content
is inserted into "index.html" between the comments and . The so obtained
HTML-pages are stored into "docs".

The images stored in "mirror/images" are copied to "docs/images".