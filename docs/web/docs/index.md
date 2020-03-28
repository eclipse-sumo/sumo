<hTitles >Simulation of Urban MObility<hr style="border: 0.5px solid #338033;"></hTitles>

!!! caution "Important notice"
    Due to the Coronavirus crisis and the measures imposed, the SUMO User Conference 2020 has been rescheduled. New date: 26-28 October 2020.

!!! conference "SUMO User Conference 2020"
    The [SUMO User Conference 2020](http://sumo.dlr.de/2020/) takes place **October, 26-28, 2020** in Berlin.

<span style="float: right;">![](https://sumo.dlr.de/w/images/thumb/d/de/Eclipse_foundation_logo.jpg/250px-Eclipse_foundation_logo.jpg)</span>

"**S**imulation of **U**rban **MO**bility" (Eclipse SUMO) is an open source, highly portable, microscopic and continuous road traffic simulation package designed to handle large road networks. SUMO is [licensed](Libraries_Licenses.md) under the [Eclipse Public License V2](https://www.eclipse.org/legal/epl-v20.html). "Eclipse SUMO" is a trademark of the Eclipse Foundation. If you use SUMO, please support its development by telling us about your [Publications](Publications.md).

The content of this Documentation is freely editable according to the [wiki style](https://en.wikipedia.org/wiki/Wiki). That means, whenever You find a solution to a problem mentioned on the [mailing lists](Contact.md), feel free to add an article to this Documentation or an entry to the [frequently asked questions](FAQ.md). Write access is restricted. In order to edit this Documentation, click on the "Edit on GitHub" button on the upper right corner and submit a Pull Request.
Here is a short help on [editing articles](Editing_Articles.md).

This Documentation is continuously updated and always refers to the latest development version. Documentation for a specific release version of SUMO is included in the download of that version.

## Documentation

- [Frequently Asked Questions (FAQ)](FAQ.md)
- [SUMO User Documentation](SUMO_User_Documentation.md)
- [Screenshots](Screenshots.md)
- [Installing SUMO](Installing.md)
- [Tutorials](Tutorials.md)
- [Contact Information](Contact.md)
- [ChangeLog](ChangeLog.md)

## Downloads
**Release {{Version}}, {{ReleaseDate}}**

- MS Windows binaries: [sumo-win64-{{Version}}.msi](https://sumo.dlr.de/releases/{{Version}}/sumo-win64-{{Version}}.msi)
- Source code: [sumo-src-{{Version}}.tar.gz](https://sumo.dlr.de/releases/{{Version}}/sumo-src-{{Version}}.tar.gz)
- [Repositories for binary Linux versions](http://download.opensuse.org/repositories/home:/behrisch/)
- macOS binaries: [Installing with Homebrew](Installing/MacOS_Build.md)
- [All Downloads of this Release](Downloads.md)
- [Data/Scenarios](Data/Scenarios.md)
- [Data/Networks](Data/Networks.md)
- [Data/Traffic Data](Data/Traffic_Data.md)

**SUMO-Game**

- MS Windows binaries: [sumo-game-{{Version}}.zip](https://sumo.dlr.de/releases/{{Version}}/sumo-game-{{Version}}.zip)

## References

- [Projects](Other/Projects.md)
- [Publications](Publications.md)
- [Links](Other/Links.md)
- [Diplom- und Studienarbeiten](DiplomStudArb.md)

## Developer

- [Developer Documentation <i class="fa fa-book"></i>](Developer/Main.md)
- [GitHub project <i class="fa fa-github"></i>](https://github.com/eclipse/sumo)
- [Issue Tracking <i class="fa fa-bug"></i>](https://github.com/eclipse/sumo/issues)
- [Status page](https://sumo.dlr.de/daily/)

## Extensions
**Open Source Software**

- [TraCI](TraCI.md)

    online interaction with the simulation

- [Cadyts](Contributed/Cadyts.md)

    a tool by Gunnar Flötteröd which adapts the simulated flows to the real flows in a known net

- [SUMOPy](Contributed/SUMOPy.md)

    a tool by Joerg Schweizer supporting the whole SUMO toolchain with a GUI especially for demand modelling

- [LiSuM](Tools/LiSuM.md)

    couples [LISA+](https://www.schlothauer.de/en/software-systems/lisa/) and SUMO to simulate real-world traffic light controllers.

- [Veins](https://veins.car2x.org/)

    connects SUMO to OMNeT++

- [FLOW](https://flow-project.github.io/)

    a framework for applying reinforcement learning and custom controllers to SUMO, developed at the University of California, Berkeley. 
    [[1]](https://github.com/flow-project/flow)

- [VSimRTI](https://www.dcaiti.tu-berlin.de/research/simulation/)

    C2X environment connecting SUMO to different network and application simulators

- [SAGA](https://github.com/lcodeca/SUMOActivityGen)

    [Generate intermodal traffic demand from OSM input](https://github.com/eclipse/sumo/tree/master/tools/contributed) [howto](https://github.com/lcodeca/SUMOActivityGen/blob/master/docs/HOWTO.md)

- [iTetris](https://www.ict-itetris.eu/)

    connects SUMO to ns3, available here.
    
 - [ocit2SUMO](https://github.com/DLR-TS/sumo-ocit) Generate traffic light signal plans from OCIT® data.

**Commercial Extensions**

- [Webots](https://www.cyberbotics.com/automobile/sumo-interface.php)

    coupling with a robot simulator

- [iTranSIM](Contributed/iTranSIM.md)

    extension by online-calibration using induction loop data by Tino Morenz

- [eWorld](http://eworld.sourceforge.net/)

    an application that allows to convert and enrich roads networks
