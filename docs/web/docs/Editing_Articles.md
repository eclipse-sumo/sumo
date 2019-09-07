---
title: Help on editing Articles
---

This documentation is written using Markdown syntax. Every page or "article" is a Markdown (.md) file located in the [official Eclipse SUMO GitHub repository]().
The static html pages are then generated using [Mkdocs](https://www.mkdocs.org/).

!!! note
    Although HTML can be written within Markdown, please avoid using it and keep everything in native Markdown as much as possible.

## Tables

[Online Tables generator](https://www.tablesgenerator.com/markdown_tables)

## Styling

Element | Code | Will show as
--------|------|--------
Bold | `**test**` | **test**
Italic | `*test*` | *test*
Bold + Italic | `***test***` | ***test***
Strikethrough | `~~test~~` | ~~test~~

**Example:**
```
Lorem *ipsum* dolor sit amet, *consetetur sadipscing elitr*, sed diam **nonumy** eirmod **tempor invidunt ut labore** et ~~dolore~~ magna ***aliquyam erat, sed diam*** voluptua.
```
***will display:***

Lorem *ipsum* dolor sit amet, *consetetur sadipscing elitr*, sed diam **nonumy** eirmod **tempor invidunt ut labore** et ~~dolore~~ magna ***aliquyam erat, sed diam*** voluptua.

<hr>

## Escape character

The escape character is `\`. When writing text inside the `` marks, no escape character are required.

**Example:**
```
`<TEST1>` using code. No escape character is required here.
<TEST2> not using the escape character. Here the element disappears.
<TEST3\> using the escape character
```
***will display:***

`<TEST1>` using code. No escape character is required here.<br>
<TEST2> not using the escape character. Here the element disappears.<br>
<TEST3\> using the escape character

## Variables

{{SUMO}}
{{DT_FILE}}
{{DT_STR}}

<hr>

## Links

When linking to a documentation page, please consider the path to the Markdown file.
<font color="red">Do not forget to add the `.md` extension in every internal link.</font> 

**Example:**
```
Go to [SUMO-GUI](SUMO-GUI.md) or [Notation](Basics/Notation.md)
```
***will display:***

Go to [SUMO-GUI](SUMO-GUI.md) or [Notation](Basics/Notation.md)

<https://www.dlr.de>
[DLR](https://www.dlr.de)

<hr>

## GitHub issues, commits and users

Element | Code | Will show as
--------|------|--------
GitHub issue | `#5697` | #5697
GitHub user | `@namdre` | @namdre
GitHub commit | `3aa106d1bf3221031cca75cfe259913d9d3e88ae` | 3aa106d1bf3221031cca75cfe259913d9d3e88ae

**Example:**
```
The issue #5697 was reported by @angelobanse. @behrisch commited 3aa106d1bf3221031cca75cfe259913d9d3e88ae to close that issue.
```
***will display:***

The issue #5697 was reported by @angelobanse. @behrisch commited 3aa106d1bf3221031cca75cfe259913d9d3e88ae to close that issue.

<hr size="10">

## Notes

The ***NOTE*** and ***CAUTION*** notes are useful for highlighting important information.
You can also use a custom note type.

**Example:**
```
!!! note
    When citing SUMO in general please use our current reference 
    publication: ["Microscopic Traffic Simulation using SUMO"](https://elib.dlr.de/124092/); 
    Pablo Alvarez Lopez, Michael Behrisch, Laura Bieker-Walz, Jakob Erdmann, Yun-Pang Flötteröd, 
    Robert Hilbrich, Leonhard Lücken, Johannes Rummel, Peter Wagner, and Evamarie Wießner. 
    IEEE Intelligent Transportation Systems Conference (ITSC) 2018.
```
***will display:***

!!! note
    When citing SUMO in general please use our current reference publication: ["Microscopic Traffic Simulation using SUMO"](https://elib.dlr.de/124092/); Pablo Alvarez Lopez, Michael Behrisch, Laura Bieker-Walz, Jakob Erdmann, Yun-Pang Flötteröd, Robert Hilbrich, Leonhard Lücken, Johannes Rummel, Peter Wagner, and Evamarie Wießner. IEEE Intelligent Transportation Systems Conference (ITSC) 2018.


**Example:**
```
!!! caution
    The list of not allowed characters is incomplete
```
***will display:***

!!! caution
    The list of not allowed characters is incomplete

!!! incomplete "Missing:"
    hello my friends

{{AdditionalFile}}

**`-s <SEED_VALUE>`**

*η<sub>recup</sub>*

<hr>

## See also

- [Markdown Cheatsheet](https://guides.github.com/pdfs/markdown-cheatsheet-online.pdf)