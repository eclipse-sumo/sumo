---
title: GitStuff
---

# Miscellaneous Git Topics

## Resolve LF status error

Occasionally, when adding binary files or files with different line endings to the
repository, a working copy may enter a state with changes that can not
be reverted (by stash or checkout).

```
warning: CRLF will be replaced by LF in build_config/wix/sumo.wxs.
The file will have its original line endings in your working directory.
```

Solution
([stackoverflow](https://stackoverflow.com/questions/1967370/git-replacing-lf-with-crlf)):

```
git rm --cached -r .
git reset --hard
```

## Plot number of issues over time

https://sumo.dlr.de/ghis/


## Preparing a Pull Request

Follow these steps to submit a code or documentation change to [eclipse-sumo/sumo](https://github.com/eclipse-sumo/sumo) (see [Contributor Workflow](Workflow.md) for background):

1. **Sign the Eclipse Contributor Agreement (ECA)**: Create an Eclipse account and sign the [ECA](https://accounts.eclipse.org/user/eca) using the exact email address configured in `git config user.email`.
2. **Fork and create a branch**: Fork `eclipse-sumo/sumo` on GitHub, clone your fork, and create a topic branch from `main`:
   ```bash
   git checkout -b fix/12345-short-topic origin/main
   ```
3. **Enable style checks**: Install `pre-commit` (described below) so modified C++ and Python files satisfy [CodeStyle](CodeStyle.md) automatically on commit.
4. **Commit, push, and open the PR**: Reference the GitHub issue in the commit message, sign off the commit (`-s`), push to your fork, and open a Pull Request against `main`:
   ```bash
   git commit -s -m "Component: concise description (#12345)"
   git push -u origin fix/12345-short-topic
   ```
   For single-file documentation fixes, you can also click the edit icon at the top of any page on the SUMO documentation website to propose a Pull Request directly in the browser.

## Git commit hooks

We are using [pre-commit](https://pre-commit.com). To enable it for your checkout it should suffice to do
the following calls in your git repo:

```
pip install pre-commit
pre-commit install
```

It will enable checks for trailing whitespace, LF at the end of every file as well as parsable yaml files
and a maximum file size of 500kb. Furthermore it will do the [style checks](CodeStyle.md) for C++ and Python
using astyle and autopep8. Every commit which does not pass the tests will fail.
Checks are not performed for the tests directory. Style errors will
be repaired automatically, so that you can issue another commit right afterwards
(depending on your tool you might need to stage the changes first).

If you want to run the checks manually without committing,
you can just run `pre-commit`. If you want to check all files (not only the ones which will be part of the commit)
run `pre-commit run --all`.

The configuration can be found in `.pre-commit-config.yaml`.
