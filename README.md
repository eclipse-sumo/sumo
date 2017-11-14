[![Build Status](https://travis-ci.org/DLR-TS/sumo.svg?branch=master)](https://travis-ci.org/DLR-TS/sumo)

SUMO - Simulation of Urban MObility 
===================================

What is SUMO
------------

["Simulation of Urban MObility" (SUMO)](http://sumo.dlr.de/) is an open source,
highly portable, microscopic traffic simulation package designed to handle
large road networks and different modes of transport.

It is mainly developed by employees of the [Institute of Transportation Systems
at the German Aerospace Center](http://www.dlr.de/ts).


Where to get it
---------------

You can download SUMO from SourceForge via our [downloads site](http://sumo.dlr.de/wiki/Downloads).

As the program is still under development and is extended continuously, we advice you to
use the latest sources from our GitHub repository. Using a command line client
the following command should work:

        git clone --recursive https://github.com/DLR-TS/sumo.git

(or if you still prefer subversion)

        svn co https://github.com/DLR-TS/sumo.git


Mailing List
------------

To stay informed, we have a mailing list for SUMO. You can subscribe at
https://dev.eclipse.org/mailman/listinfo/sumo-user.
Messages to the list can be sent to sumo-user@eclipse.org.
SUMO announcements will be made through the sumo-announce@eclipse.org list;
you can subscribe to this list at https://dev.eclipse.org/mailman/listinfo/sumo-announce.


Build and Installation
----------------------

For Windows we provide pre-compiled binaries and Visual Studio project files.
Using Linux a simple "./configure && make" should be enough for the distributions, if you
have installed all needed libraries properly. Using the repository checkout you
need to issue "make -f Makefile.cvs" before "./configure && make" in order to run
the autoconf utilities creating configure and the Makefiles.
If configure does not find the libraries or includes needed, please check
"./configure --help" for information on how to specify the paths needed.

For [detailed build instructions have a look at our wiki](http://sumo.dlr.de/wiki/Developer/Main#Build_instructions).


Getting started
---------------

To get started with SUMO, take a look at the docs/tutorial and examples directories,
which contain some example networks with routing data and configuration files.
There is also user documentation provided in the docs/ directory and on the
homepage.


Bugs
----

Please use for bugs and requests the [GitHub bug tracking tool](https://github.com/DLR-TS/sumo/issues)
or file them to the list sumo-user@eclipse.org. Before
filing a bug, please consider to check with a current repository checkout
whether the problem has already been fixed.


License
-------

SUMO is licensed under the [Eclipse Public License Version 2](https://eclipse.org/legal/epl-v20.html).
For the licenses of the different libraries and supplementary code information is in the
subdirectories and the [wiki](http://sumo.dlr.de/wiki/License).

Derivative Works
----------------
EPL requires that "derivative works" be licensed under the terms of the EPL
whereas "separate modules of software" may be licensed arbitrarily. Please follow the links
below for lists of files where modifications are considered derivative work.

We currently consider all modifications to [src](src/README_Contributing.md) and [tools](tools/README_Contributing.md) (including the subdirectories) as derivative work except for the following cases:
- TraCI client applications that use the public TraCI client libraries
- separate modules residing in [src](src/README_Contributing.md)
- car following models that inherit from MSCFModel (excluding [modifications of the existing models](src/microsim/cfmodels/README_Contributing.md))
- lane changing models that inherit from MSAbstractLaneChangeModel (excluding [modifications of the existing models](src/microsim/lcmodels/README_Contributing.md))
- simulation output modules (excluding [changes to the existing classes](src/microsim/output/README_Contributing.md))
- vehicle device modules (excluding [changes to the existing classes](src/microsim/devices/README_Contributing.md))
- network import modules (excluding [changes to the existing classes](src/netimport/README_Contributing.md))
- network export modules (excluding [changes to the existing classes](src/netwrite/README_Contributing.md))
- software that build upon the existing Python and Java tools libraries (excluding [changes to the existing](tools/README_Contributing.md))

