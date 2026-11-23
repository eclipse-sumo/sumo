# Contributing to Eclipse SUMO

Thanks for your interest in this project.

## Project description

Eclipse Simulation of Urban Mobility (SUMO) is a free and open traffic
simulation toolsuite. SUMO allows modeling and analyzing intermodal traffic
systems, including road vehicles, public transport, cargo logistics and
pedestrians. Included with SUMO is a wealth of supporting tools, which handle
tasks like route finding, visualization, network import and emission
calculation. SUMO can be enhanced with custom models and it provides various
APIs to remotely control and influence the simulation.

* https://eclipse.dev/sumo

## Developer resources

Information regarding source code management, builds, coding standards, and
more.

* https://sumo.dlr.de/docs/Developer/Main.html

The project maintains the following source code repositories

* https://github.com/eclipse-sumo/sumo
* https://github.com/eclipse-sumo/sumo.website

In order to allow the development of separate external modules for SUMO (which are not
part of the mainline repository) every source code directory lists the files and
subdirectories where modifications need to be made available under the EPL and in a
separate list which files / directories are considered interfaces to external modules,
as an entry point see the [contribution README in the src dir](src/README_Contributing.md).

## Pull Requests

We are happily reviewing pull requests against the master of this repository.
For details see below and https://sumo.dlr.de/docs/FAQ.html#how_do_code_contributions_work.

In order to facilitate PR review please try to structure pull requests that modify application behavior in the following manner:

* one commit that [adds a new test](https://sumo.dlr.de/docs/Developer/Tests.html#adding_a_test_to_an_existing_application)
* one commit that modifies source code
* one commit that [updates the results of all affected tests](https://sumo.dlr.de/docs/Developer/Tests.html#running_the_tests)

Please permit maintainer edits and be aware that we often prefer to rebase commits before merging.

## Eclipse Contributor Agreement

Before your contribution can be accepted by the project team contributors must
electronically sign the Eclipse Contributor Agreement (ECA).

* http://www.eclipse.org/legal/ECA.php

Commits that are provided by non-committers must have a Signed-off-by field in
the footer indicating that the author is aware of the terms by which the
contribution has been provided to the project. The non-committer must
additionally have an Eclipse Foundation account and must have a signed Eclipse
Contributor Agreement (ECA) on file.

For more information, please see the Eclipse Committer Handbook:
https://www.eclipse.org/projects/handbook/#resources-commit

## Translating

If you want to help in translating SUMO please open an account at
[Weblate](https://hosted.weblate.org/projects/eclipse-sumo/) and add your translation there.
Concerning the license and the contributor agreement the same rules apply as for code
and documentation (you need to agree to the ECA). If you want to add a new language
please send a request to sumo-dev (see below) or follow
[these steps](https://sumo.dlr.de/docs/Developer/Translating.html#new_language).

## Contact

Contact the project developers via the project's "dev" list.

* https://accounts.eclipse.org/mailing-list/sumo-dev
