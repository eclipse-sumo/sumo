---
title: Git and Issue Workflow
---

# General philosophy

The following are just guidelines to understand the way we work.
Feel free to deviate ;-).

The SUMO development follows a "Code First" approach.
When in doubt between a meeting and writing code, then rather write code.
If you are unsure between a design document and code, prefer code.
This results in a relatively high development speed but also in a lack of formal documentation.

Diversity is considered a bonus. We do not agree upon or recommend a standard platform or IDE to work
with or develop for SUMO. We try to enable everything which has a C++11 capable compiler
and actively support the three major desktop platforms (Windows, Linux, macOS).

# Git and Release Workflow

We develop in the "main" branch, without a dedicated "dev" branch.
There are no branches for individual releases (just tags) and we do not backport critical
fixes, so there are no parallel development lines. In other words there will never be a
SUMO 1.9.1 if SUMO 1.10.0 has already been released, see also [Versioning](../Versioning.md#release_version).

Although it is strongly encouraged to let the tests run before push, it is not being enforced
by hooks or by mandatory pull requests. It even happens from time to time that a push may
break the build. We have support for hooks enabled but they are voluntary.

Every developer is free to use feature branches but it is mainly a "backup" tool
and rarely used for working together.

# Issue Workflow

We are using GitHub issues for a lot of different things. It is a storage for ideas
(mainly in the [backlog](https://github.com/eclipse-sumo/sumo/milestone/16)),
a place to ask [questions](https://github.com/eclipse-sumo/sumo/issues?q=is%3Aopen+is%3Aissue+label%3Aquestion)
and the central place where all current development tasks are documented.

We do issue triage every second week in the developer meeting. Send us a [message](../Contact.md) if you want to take part.
The goal is that in the end every issue is in (at least) one of the following states:

- labelled as "question" or "review needed"
- has a (numbered) milestone and at least one assignee
- is in the backlog
- is closed

Everyone is free to work on any issue even if it is assigned to someone else.
When starting the work you should of course contact the assignee and assign yourself as well.
Assigning an issue to a milestone does not give a guarantee but we will do our best especially
if the issue has the "important" label or is considered a regression.

It is strongly recommended to write [tests](Tests.md) for your code and to consider an issue only
finished if it has a test. We are not closing issues for lack of activity (except for questions see below)
and not close the comments even after it has been resolved.

## Questions

Every question deserves an answer even if it is only a link to the docs. If you think the question has been answered
close the issue. If you need more info from the original author add the "awaiting reply" tag.
After another two weeks the issue may be closed unless you suspect there is a real bug behind it.
