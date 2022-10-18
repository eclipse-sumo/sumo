---
title: Translating
---

# Introduction

SUMO uses the [gettext tooling](https://www.gnu.org/software/gettext/) to maintain
a list of translated strings. The main work of translation is done by the community using
[Weblate](https://hosted.weblate.org/projects/eclipse-sumo/).
Every change at Weblate creates a commit into their clone of the SUMO repository which will
be merged (currently manually) into the main line after review.

So if you want to add translation strings open an account at
[Weblate](https://hosted.weblate.org/projects/eclipse-sumo/) and start.

# Marking translatable strings

In the code every string which needs translation should be marked with the `TL` macro like
this: `TL("String to be translated")`. We decided against the often used `_` macro for readability.

The macro is defined in `src/utils/common/MsgHandler.h` which needs to be included (but often
already is). If you have strings which are concatenated using `+` you should consider using the
`TLF` macro like `TLF("The vehicle '%' has a message.", id)` or one of
the formatting versions of our `WRITE_MESSAGE` macros.
So instead of `WRITE_MESSAGE("The vehicle '" + id "' has a problem.");`
use `WRITE_MESSAGEF(TL("The vehicle '%' has a problem."), id);` (this works for
`WRITE_WARNING` and `WRITE_ERROR` as well).

Please be aware that changing a translatable string in the code (even if you just fix a typo)
will invalidate (i.e. remove) all translations. This does not mean you should not do it, you
should just review the changes to the translation files as well.

# Using translations

Until we have a sufficient amount of translated strings the translations are disabled by default.
If you want to activate them nevertheless, you need to make sure the gettext tools and
internationalization library are installed. For Windows we have them in the SUMOLibraries, for Linux they
are probably already installed, if not use the equivalent of `sudo apt install gettext` on your system.

The translation strings are split into two files for each language residing in `$SUMO_HOME/data/po`,
one for GUI stuff and one for the command line applications.
While it is possible to edit these files directly, it is discouraged to avoid merge conflicts with the
Weblate translations. These files still need to be compiled to be usable. Currently this is done manually
by calling `tools/build/i18n.py`. This script also does a full scan of the `src` dir and checks for new
translatable strings. If it finds some, all existing po files will be updated accordingly.

# New language

If you want to add a new language either [open an issue](https://github.com/eclipse/sumo/issues) with your request
or run `tools/build/i18n.py -l <lang>` and make a pull request with the resulting po files.
