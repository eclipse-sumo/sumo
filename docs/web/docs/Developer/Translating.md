---
title: Translating
---


<div class="d-flex justify-content-evenly"><div class="align-self-center"><a class="no-arrow-link" href="https://hosted.weblate.org/engage/eclipse-sumo/"> <img src="https://hosted.weblate.org/widgets/eclipse-sumo/-/287x66-grey.png" alt="Translation status"/> </a></div><div class="align-self-center"> <a class="no-arrow-link" href="https://hosted.weblate.org/engage/eclipse-sumo/"> <img src="https://hosted.weblate.org/widgets/eclipse-sumo/-/multi-auto.svg" alt="Translation status"/> </a></div></div>

# Introduction

SUMO uses the [gettext tooling](https://www.gnu.org/software/gettext/) to maintain
a list of translated strings. The main work of translation is done by the community using
[Weblate](https://hosted.weblate.org/projects/eclipse-sumo/).
Every change at Weblate creates a commit into their clone of the SUMO repository which will
be merged (currently manually, see below) into the main line after review. The Weblate repository gets
updated automatically by a webhook installed by Eclipse at the main repo.

So if you want to add translation strings open an account at
[Weblate](https://hosted.weblate.org/projects/eclipse-sumo/) and start.

# Marking translatable strings

In the code every string which needs translation should be marked with the `TL` macro like
this: `TL("String to be translated")`. We decided against the often used `_` macro for readability.

The macro is defined in `src/utils/common/MsgHandler.h` which needs to be included (but often
already is). If you have strings which are concatenated using `+` you should replace them using the
`TLF` macro like `TLF("The vehicle '%' has a message.", id)` or one of
the formatting versions of our `WRITE_MESSAGE` macros.
So instead of `WRITE_MESSAGE("The vehicle '" + id "' has a problem.");`
use `WRITE_MESSAGEF(TL("The vehicle '%' has a problem."), id);` (this works for
`WRITE_WARNING` and `WRITE_ERROR` as well).

Please be aware that changing a translatable string in the code (even if you just fix a typo)
will invalidate (i.e. remove) all translations. This does not mean you should not do it, you
should just review the changes to the translation files as well.

The translations should not be applied to debug and developer messages. This includes everything
in src/foreign, src/libsumo, src/libtraci, src/traci_testclient and unittest.

# Using translations

Until we have a sufficient amount of translated strings the translations are disabled by default.
If you want to activate them nevertheless, you need to make sure the gettext tools and
internationalization library are installed. For Windows we have them in the SUMOLibraries, for Linux they
are probably already installed, if not use the equivalent of `sudo apt install gettext` on your system.
It is also a good idea to do `sudo apt install locales-all` to make sure all the needed locales are on your system.

The translation strings are split into two files for each language residing in `$SUMO_HOME/data/po`,
one for GUI stuff and one for the command line applications.
While it is possible to edit these files directly, it is discouraged to avoid merge conflicts with the
Weblate translations. These files still need to be compiled to be usable. Currently this is done manually
by calling `tools/build/i18n.py`. This script also does a full scan of the `src` dir and checks for new
translatable strings. If it finds some, all existing po files will be updated accordingly.

# Testing translations

All SUMO binaries understand the `--language` option which behaves exactly like the environment variable `LANGUAGE`
as described in the [gettext documentation](https://www.gnu.org/software/gettext/manual/html_node/The-LANGUAGE-variable.html).
So for testing turkish, you set `--language tr`. Please be aware that by default the `C` locale is activated which
suppresses all translations. If you want the binaries to respect the environment variable `LANGUAGE` you can do so by using
`--language=` (setting `--language` to an empty string).

# New language

If you want to add a new language either [open an issue](https://github.com/eclipse/sumo/issues) with your request
or run `tools/build/i18n.py -l <lang>` and make a pull request with the resulting po files.

# For developers

The translated strings from Weblate are not pushed automatically to our github repo because automatic pushing
is generally a bad idea and weblate does not have the rights. If you want to merge the commits in weblate into
the main github you have to do the following in your local git clone of SUMO:
```
git pull  # make sure you are up to date and on the target branch (usually main)
git remote add weblate https://hosted.weblate.org/git/eclipse-sumo/sumo-gui-netedit/  # add Weblate repo (only necessary once)
git fetch weblate  # get new commits
git merge weblate/main
```
Then you should review the changes and `git push` afterwards.
