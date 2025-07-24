---
title: HowToRelease
---

## Packages

For an overview of created packages and contents see
[Downloads](../Downloads.md).

## Release steps

Below, a list of steps that should be done in order to publish a new
release is given. All necessary commits which have no ticket of their
own may refer to #563. All the steps assume you are using bash
either natively (Linux, macOS), on WSL or the git bash (or some other mingw bash).
Furthermore you should have either your passwords ready or even better ssh keys
deployed on all the remote servers in question.

### Merge phase

Major changes to the SUMO trunk should end about two weeks before the
release date. This refers especially to merges of project branches into
the trunk. It is also a good idea to inform the developers of dependent
software (Veins, VSimRTI, flow etc.) at this stage.
- send out mail to sumo-dev informing about the upcoming release so that 'contributed'-authors can check their stuff for compatibility

### Freeze phase (Release day - 7)

- check the sources
  - compile, try to remove warnings and commit the patches
  - run [tools/build_config/checkStyle.py](../Developer/CodeStyle.md) and commit
    changed files
  - check the calendar to update copyright statements
  - check whether the TraCI version needs to be incremented
    and rebuild TraCI constants in python
    (tools/traci/rebuildConstants.py)
  - check whether the network version needs to be incremented and
    update the value in NWFrame::MAJOR_VERSION. netconvert tests
    need to be updated afterwards.
  - update author information
- check the regular tests
  - put special attention to the tests which serve as examples, see
    tests/examples.txt\!
  - Win64 and gcc4_64 should have no failing tests, the other
    platforms are less important
  - If there are failing tests, which are not flagged as known bugs,
    save them after careful checking or open a ticket and assign a
    known bug.
  - recheck/rebuild the test networks (if necessary due to
    netconvert changes)
  - check for orphaned tests by running `tools/devel/orphaned_tests.py tests` and patch testsuites to add them back in
  - check the tests again
- check the documentation
  - update the [ChangeLog](../ChangeLog.md)
  - generate options documentation and xsd schemata for configuration files
    using `tools/build_config/rebuildConfigDocsAndXsd.py`
- update `tools/build_config/templates.py` to include any tools that were added or had their compatibility fixed since the last release
- check the internal tests (same procedure as above), especially the
  (to be) published scenarios
- GitHub
  - add new [milestone](https://github.com/eclipse-sumo/sumo/milestones)
    if necessary
  - check all remaining tickets and assign them to later milestones
    or to persons.
- scenarios (optional)
  - prepare scenarios for release if the previous version does not
    run with the current release or significant changes were made to
    the scenarios
- update submodules by running `git submodule update --remote` and committing
  the changes if necessary

The trunk is now frozen. All commits which do not refer to an open
ticket for the upcoming release need to be made to a separate branch.
The freeze phase should not last longer than one week. The goal is to
fix all scenarios and have all failing tests fixed, which are not
assigned to a later milestone.

### Release day - 1

All scenarios should be fixed by now.

- start and save a new version draft [in Zenodo](https://zenodo.org/) (using the sumo@dlr.de user), in order to reserve a DOI. Don't Publish it yet, and don't upload a file to it!
  - update the version doi in [CITATION.cff]({{Source}}CITATION.cff) and in the [README]({{Source}}README.md) badge to this new reserved one
- update the [ChangeLog](../ChangeLog.md) again if necessary
- check the correct email address and list of current ubuntu releases in `tools/build_config/ubuntu_release.sh`
- patch the version information using `tools/build_config/updateReleaseInfo.py {{Version}}` and double check changes
  - in src/config.h.cmake, also the HAVE_VERSION_H macro should be disabled
  - in CMakeLists.txt
  - [in mkdocs.yml]({{Source}}docs/web/mkdocs.yml) in the **extra:** section at the end
    to update the [download links](../Downloads.md)
  - [in sumo.metainfo.xml]({{Source}}build_config/package/sumo.metainfo.xml)
    for correct flatpak info
  - in [CITATION.cff]({{Source}}CITATION.cff)
  - in the [ChangeLog](../ChangeLog.md)
  - commit the changes
- recheck whether submodules changed by doing `git submodule update --remote` and commit the changes after careful inspection
- check IP due diligence
  - every library in [SUMOLibraries](https://github.com/DLR-TS/SUMOLibraries/), in src/foreign and in tools/contributed needs an entry in the [Libraries list](../Libraries_Licenses.md) and in <https://github.com/eclipse-sumo/sumo/blob/main/docs/dependencies.txt>
  - the [iplab](https://gitlab.eclipse.org/eclipsefdn/emo-team/iplab) should have a closed issue for each entry
- check the documentation
- check presence of RPMs on <https://build.opensuse.org/package/show/science:dlr/sumo_nightly>
- add a new version tag
```
> git tag -a v0_13_7 -m "tagging release 0.13.7, refs #563"
> git push --tags
```

### Release day

The nightly build should have generated all releasable packages. If not,
delay the release. (The complete documentation, tests and source
distribution build can be achieved via "make dist".) The
following things need to be in the S:\daily directory:

- the platform independent part of the distribution;
  - source and all inclusive distributions (.tar.gz, .zip) ("make dist")
- the binary part of the distribution
  - windows binary distribution (zip, unzip the x64 file and run at least sumo-gui)
  - windows installer (msi, Win32 and x64, includes docs)
- check the wheels in S:\daily\wheels
  - eclipse-sumo and libsumo for Windows, macOS, Linux x64 and Linux aarch64 (libsumo for all current python versions)
  - sumolib, traci
- check the Maven build https://ci.eclipse.org/sumo/job/maven-artifacts/ and ensure that the artifacts have been uploaded to:
  - https://repo.eclipse.org/content/repositories/sumo-releases/org/eclipse/sumo/libtraci and
  - https://repo.eclipse.org/content/repositories/sumo-releases/org/eclipse/sumo/libsumo/


If everything is fine:

- make a new folder in S:\Releases
- make new sumo.dlr.de-release
  - copy the folder from S:\Releases to the releases dir `scp -r /s/Releases/{{Version}} delphi@ts-sim-front-ba.intra.dlr.de:docs/releases`
- update the eclipse.dev/sumo website
  - modify the version number (Version) and the DOI number (DOI) [in config.yaml](https://github.com/eclipse-sumo/sumo.website/blob/source/config/_default/config.yaml) in the **Default Parameters** section
- make new sourceforge-release
  - Login on the web browser, create a new folder and upload the files at https://sourceforge.net/projects/sumo/files/sumo/version%20{{Version}}/
    - you might need to do this in several rounds because of the size
  - change default download attributes by clicking on the circled "i" after each file
    - the default for Windows is sumo-win64extra-{{Version}}.msi, for macOS sumo-{{Version}}.pkg and for all the others sumo-src-{{Version}}.tar.gz
- finish the Zenodo version draft, by uploading the `sumo-src-{{Version}}.tar.gz`, adding the release info (can also be done later) and publishing it
- Create a new Eclipse release at https://projects.eclipse.org/projects/automotive.sumo (after login there should be a "Create Release" button)
- create a new entry in [elib](https://elib.dlr.de/)
  - the easiest way to do it, is by going to [Einträge verwalten](https://elib.dlr.de/cgi/users/home?screen=Items) and clicking on the magnifying-glass-icon for an old release, then going to the "Aktionen" tab and selecting "Als Vorlage verwenden"
  - take a look at the [Eintrag von Forschungssoftware-Publikationen - Tutorial](https://wiki.dlr.de/pages/viewpage.action?pageId=711888423), or the entry for a previous release: https://elib.dlr.de/205320/
  - if necessary, talk to the elib-appointee for the department/institute
- update files at the [opensuse build
    service](https://build.opensuse.org/package/show/science:dlr/sumo)
- update the ubuntu ppa (see
<https://askubuntu.com/questions/642632/how-to-bump-the-version-of-a-package-available-in-another-users-ppa>)
  - this assumes you have the devscripts package as well as all sumo dependencies installed
  - unzip the special source release `tar xzf sumo_{{Version}}.orig.tar.gz`
  - run `cd sumo-{{Version}} && tools/build_config/ubuntu_release.sh` and enter the release comment
- start a pull request against [winget](https://github.com/microsoft/winget-pkgs/tree/master/manifests/e/EclipseFoundation/SUMO)
- upload the wheels to PyPI using `twine upload /s/daily/wheels/*{{Version}}*.whl`
  - you might need to do this with an up to date twine in a virtual environment, see https://github.com/pypi/warehouse/issues/15611
- scenarios (optional)
  - add files to [the scenario folder](https://sourceforge.net/projects/sumo/files/traffic_data/scenarios/)
  - updated README.txt
- inform the users about the new release
  - post information about the release to sumo-user@eclipse.org and
    sumo-announce@eclipse.org
  - submit news about the release on the Eclipse Newsroom <https://newsroom.eclipse.org/node/add/news>
  - tweet about it, post on Facebook and Instagram stories
- close [the milestone](https://github.com/eclipse-sumo/sumo/milestones)
  (retargeting open tickets needs to be done manually for now)
- add the latest version to the **"software version identifier"** statement in [Wikidata](https://www.wikidata.org/wiki/Q15847637) (this will update the Wikipedia articles about SUMO), making sure to select the latest version as **"preferred rank"** and setting the previous one to **"normal rank"**.

### After-release cleanup

The trunk is now open for changes again.

- wait for the automated [flatpak](https://github.com/flathub/org.eclipse.sumo) pull request to appear and built, then accept it
- re-enable HAVE_VERSION_H in src/config.h.cmake
- rename version to "git" in CMakeLists.txt
- insert a new empty "Git Main" section at the top of the [ChangeLog](../ChangeLog.md)
- commit changes
- drink your favorite beverage and/or eat cake
