---
title: Purgatory/Roadmap
permalink: /Purgatory/Roadmap/
---

## <font color="green">0.9.6 "We need a release"</font>

- <font color="green">solve [GUILoadThread
  bug](http://sourceforge.net/tracker/index.php?func=detail&aid=1718704&group_id=45607&atid=443421)</font>
- <font color="green">recheck whether netconvert indeed crashes</font>
- <font color="green">improve on documentation (route loading and
  other [route
  details](http://sourceforge.net/tracker/index.php?func=detail&aid=1639308&group_id=45607&atid=443421))</font>
- <font color="green">automatic doc building (pdf and html)</font>

## <font color="green">0.9.7 "Testing is fun"</font>

- Make all tests work on all platforms (a permanent goal which was
achieved partly)
  - ok: MSVC8
  - errors: NETCONVERT, NETGENERATE, SUMO, with gcc
  - still missing: SUMO-GUI
- <font color="green">Remove all memory leaks reported by nvwa</font>
  - <font color="green">ok: DUAROUTER, JTRROUTER, NETCONVERT,
    NETGENERATE, OD2TRIPS, DFROUTER, SUMO, SUMO-GUI</font>
- <font color="green">[specification concerning car and route
properties](../Specification.md) (acceleration, deceleration,
route start / end)</font>
- <font color="green">use OutputDevice</font>
  - <font color="green">done: NETCONVERT, NETGENERATE, OD2TRIPS,
    SUMO, SUMO-GUI, DUAROUTER, JTRROUTER, DFROUTER</font>
  - <font color="green">I have kept configuration output using plain
    ofstream</font>
- <font color="green">check the status of the following files and dirs
(in src)</font> (removed)
  - <font color="green">giant_main.cpp</font> (removed)
  - <font color="green">netedit</font> (removed)
  - <font color="green">netedit_main.cpp</font> (removed)
  - <font color="green">tagreader.cpp</font> (removed)
  - <font color="green">tools dir</font> (removed)
- [PROJ and GDAL
optional](http://sourceforge.net/tracker/index.php?func=detail&aid=1648747&group_id=45607&atid=443424)
- <font color="green">consolidate interfaces</font>
  - <font color="green">sockets</font>
- <font color="green">release-building patches</font>
  - <font color="green">move generated pdfs into &lt;SUMO&lowbar;DIST&gt;/docs</font>
  - <font color="green">extra targets for docs & tests?</font>
  - <font color="green">links FAQ, Bibliography shall point to the
    wiki</font>
  - <font color="green">remove man pages from the documentation
    section</font>
  - <font color="green">remove additional data section</font>

## 0.10 "Clean"

- <font color="green">better tests for detector output</font>
- <font color="green">refactor detectors</font>

## 0.11 "Another One Bites the Dust"

## 0.12 "Scotty is dead"

- develop general statement about SUMO's future (what should be inside
  and what is definitely out of scope)
- resolve issues related to physics
  ([1639478](http://sourceforge.net/tracker/index.php?func=detail&aid=1639478&group_id=45607&atid=443421),
  [1639479](http://sourceforge.net/tracker/index.php?func=detail&aid=1639479&group_id=45607&atid=443421),
  [1645229](http://sourceforge.net/tracker/index.php?func=detail&aid=1645229&group_id=45607&atid=443421),
  [1653274](http://sourceforge.net/tracker/index.php?func=detail&aid=1653274&group_id=45607&atid=443421),
  [1673339](http://sourceforge.net/tracker/index.php?func=detail&aid=1673339&group_id=45607&atid=443421))
- better collision and deadlock detection
- remove beaming / transporter
- no overlapping cars at edge borders (reported by Luebeck, no
  testcase / bugreport yet)
- tests concerning car and route properties (acceleration,
  deceleration, route start / end)
- evaluate/add some real-life examples
- implement [Specification](../Specification.md)

- describe exception handling (see [Developer/Implementation
  Notes/Error
  Handling](../Developer/Implementation_Notes/Error_Handling.md))
- document on coding conventions (see
  [Developer/CodeStyle](../Developer/CodeStyle.md))

- <font color="green">maybe introduce unit testing</font>
- write specifications and tests concerning basic functionality (lane
  changing, junction behavior)
- describe and evaluate the implemented car-following model
- consolidate (and publish(?)) inner-junction traffic
- rework (and publish(\!)) lane-changing
- ns2connection
- refactoring
  - recheck and describe interaction between files and outputs used by SUMO
  - remove static dictionaries, use one map implementation
  - recheck "module" dependencies
  - check the tools dir and translate needed ones to python
  - use html instead of shtml in generated pages?

- features
  - consolidate vehicle class ideas and usage
  - add support for network database processing in NETCONVERT
  - correct NETCONVERT's SUMO-network importer

## 0.13 "XML consistency"

- <font color="green">remove character data</font>
- <font color="green">introduce option tag with key, value
  attributes</font>
- develop an XML Schema and implement an
  [XSBC](http://xmsf.sourceforge.net/xsbc.html) parser

## 0.14 "Speed"

- Multithreaded simulation
- all calculations integer based instead of float (improves precision
  / predictability)

## 1.0

- update the about dialog ;-)

## After being retired

- As soon as we do not longer support routes defined in characters,
  one should revisit whether "myActiveRouteID" is really needed