---
title: Eclipse Migration
permalink: /Eclipse_Migration/
---

The SUMO project will move to the [Eclipse
foundation](https://projects.eclipse.org/proposals/eclipse-sumo). This
leads to a number of changes which are listed below (with a rough
timeline)

- Using [C++11](https://en.wikipedia.org/wiki/C%2B%2B11). **done,
2017/7**
- Re-license the code under the
[EPL](https://en.wikipedia.org/wiki/Eclipse_Public_License) **done,
2017/10**
- Moving the repository (complete history) to GitHub **done 2017/10**.
  - switching source-forge repository to read-only **done
    2017/10/27**
  - switching Trac to read-only **done 2017/10/27**
  - enabling [new GitHub repository](https://github.com/DLR-TS/sumo)
    **done 2017/11/01**
  - enabling [new GitHub issue
    tracker](https://github.com/DLR-TS/sumo/issues) (temporary)
    **done 2017/11/01**
  - disabling the [planetsumo](https://github.com/planetsumo/)
    repository **done 2017/11/01**
- Creating the [Eclipse repository](https://github.com/eclipse)
(history cut off) on GitHub **scheduled for 2017/12**.

  !!! note
      We want to maintain the full repository history but can not do so under the EPL. Therefore we will maintain two repositories that mirror commits going forward. Official issue tracker will be at the eclipse repo.

  - Moving the issues to the [Eclipse
    repo](https://github.com/eclipse) **scheduled for 2017/12**
  
- Using the Eclipse mailing list facilities **done 2017/9**
  - [Sumo announce](https://dev.eclipse.org/mailman/listinfo/sumo-announce)
  - [Sumo developer](https://dev.eclipse.org/mailman/listinfo/sumo-dev)
  - [Sumo user](https://dev.eclipse.org/mailman/listinfo/sumo-user)
- build system switched to [cmake](https://cmake.org/). **no date
set**
- move downloads to eclipse.org. **no date set**

Things that stay the same:

- Documentation remains at [sumo.dlr.de/docs](https://sumo.dlr.de/docs)

!!! note
    This page will be continuously updated with the state of the migration.