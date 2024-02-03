---
title: GitStuff
---

# Miscellaneous Git Topics

## Resolve LF status error

Occasionally, when adding binary files or files with different line endings to the
repository, a working copy may enter a state with changes that can not
be reverted (by stash or checkout).

```
warning: CRLF will be replaced by LF in build_config/wix/sumo.wxs.
The file will have its original line endings in your working directory.
```

Solution
([stackoverflow](https://stackoverflow.com/questions/1967370/git-replacing-lf-with-crlf)):

```
git rm --cached -r .
git reset --hard
```

## Plot number of issues over time

https://sumo.dlr.de/ghis/


## Git commit hooks

We are using [pre-commit](https://pre-commit.com). To enable it for your checkout it should suffice to do
the following calls in your git repo:

```
pip install pre-commit
pre-commit install
```

It will enable checks for trailing whitespace, LF at the end of every file as well as parsable yaml files
and a maximum file size of 500kb. Every commit which does not pass the tests will fail.
Checks are not performed for the tests directory. Whitespace errors will
be repaired automatically, so that you can issue another commit right afterwards. More checks will probably be added later.
If you want to run the checks manually without committing,
you can just run `pre-commit`. If you want to check all files (not only the ones which will be part of the commit)
run `pre-commit run --all`.

The configuration can be found in `.pre-commit-config.yaml`.
