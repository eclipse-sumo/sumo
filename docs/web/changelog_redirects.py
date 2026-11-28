#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    changelog_redirects.py
# @author  Josh
# @date    2026-11-26

"""Keep a stable documentation URL for every version in the changelog."""

from pathlib import Path
import re

from markdown import Markdown
from mkdocs.exceptions import ConfigurationError


_VERSION_HEADING = re.compile(r"^## Version ((?:pre)?[0-9]+(?:\.[0-9]+)+)(?=\s|$).*", re.MULTILINE)


def on_config(config):
    docs = Path(config.docs_dir)
    pages = [docs / "ChangeLog.md", *sorted((docs / "ChangeLog").glob("Changes_in_*_releases.md"))]
    markdown = Markdown(extensions=["toc", "attr_list"], extension_configs={"toc": config.mdx_configs["toc"]})
    redirects = config.plugins["redirects"].config["redirect_maps"]
    versions = set()
    for page in pages:
        for heading in _VERSION_HEADING.finditer(page.read_text(encoding="utf-8")):
            version = heading.group(1)
            if version in versions:
                raise ConfigurationError("Duplicate changelog heading for version %s" % version)
            versions.add(version)
            markdown.reset().convert(heading.group())
            anchor = markdown.toc_tokens[0]["id"]
            redirects["ChangeLog/%s.md" % version] = "%s#%s" % (page.relative_to(docs).as_posix(), anchor)
    return config
