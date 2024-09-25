---
title: GSoC
---

# GSoC 2015

[Here is the list of steps for the application.](https://www.google-melange.com/gsoc/document/show/gsoc_program/google/gsoc2015/help_page#1._How_does_a_mentoring_organization)
This page should answer all the questions on the list which need more
than yes/no or a list of names

## Why do we apply

- outreach
- gain more developers

## Ideas list

*At a minimum, all the ideas on your Ideas List should include the
expected outcome of the project, a potential mentor, the skills and/or
languages required to complete the project, and a general "difficulty"
level.*

### Implementing macroscopic assignment models

- Goal: Have more than just iterative assignment (At least Lohse etc.)
- Mentor: @behrisch
- Skills: C++, some math
- Difficulty: medium

### VISSIM importer

- Convert at least basic VISSIM networks from recent versions into the
  SUMO format
- Mentor: @behrisch
- Skills: C++, XML
- Difficulty: medium

### Application Launcher

- Currently, the command line must be used frequently when preparing a
  simulation. To make life easier for many users, a launcher
  application for setting options and selecting input files via
  dialogs should be created. A wizard-type interface should be added
  to streamline common tasks.
- Mentor: @namdre
- Skills: python
- Difficulty: medium (easy, if experience with GUI frameworks exists)

### Intersection analysis

Smartphone app (android, of course\!) that can, from the position of the
phone, compute the corresponding intersection geometry by using
openstreetmap data, supported eventually by measurements from the phone
itself (distances e.g.). It then writes these data to a SUMO simulation
file, and has eventually the ability to collect additional traffic data.
Can be done e.g. as a simple counting app, via the video-camera, the
microphone, the magnetic field sensor,... (not all of these make sense,
of course, other sensors might be more suitable)

In addition to that, it runs a truly fast queueing model on the phone
itself to determine the optimal cycle time, green splits, delays, and
the like. This, too, can be supported by measurements from the
intersection, i.e. the saturation flows for the various connections.

- Mentor: @behrisch, Peter Wagner
- Skills: Android development
- Difficulty: medium (easy, if only a frontend to sumo is built)

### Left-hand traffic

Currently, SUMO only simulates right-hand traffic and therefore cannot
simulate some parts of the world.

- Mentor: @namdre
- Skills: C++
- Difficulty: medium (need to be able to read and comprehend quite a
  bit of existing code)

# GSoC 2012 Application

This is the Application for the SUMO project by the Core Development
team of the DLR.

## Describe your organization.

SUMO is a highly portable, microscopic road traffic simulation package
designed to handle large road networks. It is mainly developed by
employees of the Institute of Transportation Systems at the German
Aerospace Center. SUMO is open source, licensed under the GNU GPL.

Using SUMO you can simulate how a given traffic demand which consists of
single vehicles moves through a given road network. The Application
suite consists of tool for preparing the road network, computing route
choices and managing various simulation outputs. Thus, a large set of
traffic management topics can be addressed [(see our one-page
overview)](https://sumo.dlr.de/docs/SUMO_at_a_Glance.html).

Designed to be easy to use, yet powerful and flexible, GnuCash allows
you to track bank accounts, stocks, income and expenses. As quick and
intuitive to use as a checkbook register, it is based on professional
accounting principles to ensure balanced books and accurate reports.

## Why is your organization applying to participate in Google Summer of Code 2012? What do you hope to gain by participating?

The aim of the SUMO project lies in strengthening the microscopic-traffic
simulation community. Instead of multiple short-lived projects with low
comparability we wish to establish a standard simulation package for
everyone to use and improve. While the first goal has been met and SUMO
is now widely used, the second goal remains open. There have been only a
few contributions to the code base. In participating in the Google
Summer of Code, we hope to engage new developers to the benefit of all
our users.

## Did your organization participate in past Google Summer of Codes? If so, please summarize your involvement and the successes and challenges of your participation.

No.

## What license(s) does your project use?

GNU GPLv3

## What is the URL for your Ideas page?

[GSOC2012_ProjectIdeas](#project_ideas_for_google_summer_of_code_2012)

## What is the main development mailing list for your organization?

The main development mailing list is
\[sumo-devel@lists.sourceforge.net\] For details see [our contact
page](https://sumo.dlr.de/docs/Contact.html)

## What is the main IRC channel for your organization?

We do not have an IRC channel

## Does your organization have an application template you would like to see students use? If so, please provide it now.

    Contact
    -------
    Student name:
    Location (Home town, country):
    Email:
    Phone number:

    Background/Programming Information
    -----------------------------------
    What programming experience do you have so far (languages, operating systems, compilers)?
    Have you worked on an open source project(s) before, and which one(s)?
    What is your connection to the traffic simulation community
    Have you ever built SUMO from source?

## Who will be your backup organization administrator?

Daniel Krajzewicz

## What criteria did you use to select the individuals who will act as mentors for your organization? Please be as specific as possible.

Our mentors are members of the core development team. They have in-depth
knowledge of the code base and our processes. As experienced developers
they all have mentoring experiences. Since the core team is small our
mentors are self-selected.

## What is your plan for dealing with disappearing students?

We are confident that careful initial selection and close communication
will prevent students from disappearing.

## What is your plan for dealing with disappearing mentors?

Our mentors have been given clearance from their employer to mentor
during their work hours, thus we expect no disappearances. In case of
sickness or other emergencies we have backup mentors.

## What steps will you take to encourage students to interact with your project's community before, during and after the program?

We assume that our students already have some connections to the
traffic simulation community. Before the program we will encourage
interested parties to discuss and refine project ideas via our mailing
list, blog and personal communication. During the program we will seek
to build a relationship based on our common interests which continues
after the program.

# Project Ideas for Google Summer of Code 2012

!!! note
    Prerequisites for all projects: C++ coding on Linux or Windows

## Implement [left-hand traffic](https://en.wikipedia.org/wiki/Left-hand_traffic)

SUMO uses right-hand traffic and therefore presents obstacles when
simulating traffic in many parts of the world.

## Implement GUI-frontends for some of the command-line-only applications

The use of command-line applications presents a steep learning curve to
many users. Even a generic tool for setting options may be of great
help.

## Enhance Capabilities of the Simulation Viewer

The [sumo-gui](sumo-gui.md) application depends on configuration
files. Since editing configuration files is difficult for many users,
[sumo-gui](sumo-gui.md) should provide for a mechanism to load
the required files via menus.

## Implement a movement model for pedestrians

Right now, SUMO can account for pedestrians when simulating inter-modal
traffic. However, the model is quite coarse. The time for crossing roads
is not modelled. Also, there is no flow model for crowded side walks.

## Open for your Ideas

If you are planning to work with SUMO in your bachelor or masters
thesis, what extra feature do you need to get started?
