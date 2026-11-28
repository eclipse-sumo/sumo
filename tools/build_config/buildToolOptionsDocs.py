#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2024-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    buildToolOptionsDocs.py
# @author  Michael Behrisch
# @date    2026-11-19

"""
Generate the Markdown options overview page (docs/web/docs/Tools/Options.md)
for Python tools listed in tools/build_config/templates.py.
"""

from __future__ import absolute_import
from __future__ import print_function

from collections import defaultdict
from concurrent.futures import ThreadPoolExecutor
import io
import json
import os
import re
import subprocess
import sys

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
HOME_DIR = os.path.abspath(os.path.join(THIS_DIR, "..", ".."))
TOOLS_DIR = os.path.join(HOME_DIR, "tools")
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
if THIS_DIR not in sys.path:
    sys.path.insert(0, THIS_DIR)

import sumolib  # noqa
import templates  # noqa

TYPE_TO_MACRO = {
    "bool": "{{DT_BOOL}}",
    "float": "{{DT_FLOAT}}",
    "int": "{{DT_INT}}",
    "time": "{{DT_TIME}}",
    "parseTime": "{{DT_TIME}}",
    "net_file": "{{DT_NET}}",
    "route_file": "{{DT_Routes}}",
    "route_file_list": "{{DT_Routes}}",
    "file": "{{DT_FILE}}",
    "file_list": "{{DT_FILE}}",
    "additional_file": "{{DT_FILE}}",
    "additional_file_list": "{{DT_FILE}}",
    "edgedata_file": "{{DT_FILE}}",
    "data_file": "{{DT_FILE}}",
    "sumoconfig_file": "{{DT_FILE}}",
    "sumoconfig_file_list": "{{DT_FILE}}",
    "edge_list": "{{DT_STR_LIST}}",
}

INSPECTOR_CODE = r"""
import importlib.abc
import importlib.machinery
import json
import os
import runpy
import sys

stubs = set(os.environ.get("SUMO_DOC_STUBS", "").split(",")) - {""}
if stubs:
    class _StubMeta(type):
        def __getattr__(cls, name):
            return _make_stub(cls.__name__ + "." + name)
        def __iter__(cls):
            return iter(())
        def __bool__(cls):
            return False

    def _make_stub(name):
        class _Stub(Exception, metaclass=_StubMeta):
            __path__ = []
            __file__ = name
            def __init__(self, *a, **kw):
                pass
            def __getattr__(self, k):
                return _make_stub(name + "." + k)
            def __call__(self, *a, **kw):
                return _make_stub(name)
            def __iter__(self):
                return iter(())
        _Stub.__name__ = name.split(".")[-1]
        return _Stub

    class _Finder(importlib.abc.MetaPathFinder, importlib.abc.Loader):
        def find_spec(self, fullname, path, target=None):
            if fullname.split(".")[0] in stubs:
                return importlib.machinery.ModuleSpec(fullname, self)
            return None
        def create_module(self, spec):
            return _make_stub(spec.name)
        def exec_module(self, module):
            pass

    sys.meta_path.insert(0, _Finder())

sys.path.insert(0, os.path.join(os.environ["SUMO_HOME"], "tools"))
import sumolib.options

captured = []
class _CapturedDone(BaseException):
    pass

def _capture_hook(self, namespace, toString=False):
    captured.append(self)
    raise _CapturedDone()

sumolib.options.ArgumentParser._write_config_file = _capture_hook

tool_path = os.path.abspath(sys.argv[1])
sys.path.insert(0, os.path.dirname(tool_path))
sys.argv = [tool_path, "--save-template", "stdout"]
try:
    runpy.run_path(tool_path, run_name="__main__")
except (_CapturedDone, SystemExit):
    pass

parser = captured[-1]
items = []
for a in parser._actions:
    if a.dest in ("help", "configuration_file", "save_configuration", "save_template"):
        continue
    if a.help == "==SUPPRESS==":
        continue
    short_opts = [s for s in a.option_strings if s.startswith("-") and not s.startswith("--")]
    long_opts = [s for s in a.option_strings if s.startswith("--")]
    is_pos = getattr(a, "isPositional", len(a.option_strings) == 0)
    is_bool = getattr(a, "boolean", False)
    if is_bool:
        tname = "bool"
    elif a.type is None:
        tname = "string"
    else:
        tname = getattr(a.type, "__name__", "string")
    default_val = None if a.default is None or a.default == "==SUPPRESS==" else a.default
    if isinstance(default_val, (list, tuple)):
        default_val = " ".join(map(str, default_val))
    items.append({
        "dest": a.dest,
        "short": short_opts[0] if short_opts else "",
        "long": long_opts[0] if long_opts else (a.dest if is_pos else ""),
        "positional": is_pos,
        "type": tname,
        "nargs": str(a.nargs) if a.nargs is not None else "",
        "default": default_val,
        "required": bool(getattr(a, "isRequired", a.required)),
        "help": a.help or "",
    })
print("SUMO_TOOL_JSON:" + json.dumps({"description": parser.description or "", "options": items}))
"""


def _sanitize_text(text):
    if not text:
        return ""
    text = " ".join(str(text).replace("\r", " ").replace("\n", " ").split())
    text = text.replace("|", ",")
    text = text.replace("<", "&lt;").replace(">", "&gt;")
    text = text.replace("{{", "&#123;&#123;").replace("}}", "&#125;&#125;")
    text = text.replace("{%", "&#123;%").replace("%}", "%&#125;")
    return text


def _normalize_default(val):
    if val is None:
        return None
    s = str(val)
    if s == "":
        return None
    for prefix in (HOME_DIR, HOME_DIR.replace("\\", "/")):
        if prefix and prefix in s:
            s = s.replace(prefix, "<SUMO_HOME>")
    return _sanitize_text(s)


def _dt_macro(opt):
    tname = opt["type"]
    if tname in TYPE_TO_MACRO:
        return TYPE_TO_MACRO[tname]
    if opt["nargs"] in ("+", "*"):
        return "{{DT_STR_LIST}}"
    return "{{DT_STR}}"


def inspect_tool(tool_rel):
    env = dict(os.environ)
    env["SUMO_HOME"] = HOME_DIR
    stubs = set()
    abs_tool = os.path.join(TOOLS_DIR, tool_rel)
    for _ in range(6):
        env["SUMO_DOC_STUBS"] = ",".join(sorted(stubs))
        proc = subprocess.run(
            [sys.executable, "-c", INSPECTOR_CODE, abs_tool],
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
        )
        for line in proc.stdout.splitlines():
            if line.startswith("SUMO_TOOL_JSON:"):
                return tool_rel, json.loads(line[len("SUMO_TOOL_JSON:"):]), None
        m = re.search(r"ModuleNotFoundError: No module named '([^']+)'", proc.stderr)
        if m:
            pkg = m.group(1).split(".")[0]
            if pkg not in stubs:
                stubs.add(pkg)
                continue
        return tool_rel, None, proc.stderr.strip()
    return tool_rel, None, "Too many missing optional modules"


def format_markdown(tool_entries):
    lines = [
        "---",
        "title: Tools Options Overview",
        "---",
        "",
        "# Python Tools Options Overview",
        "",
        "The command-line Python tools listed below use `sumolib.options.ArgumentParser` and",
        "therefore also support configuration files via **-c** / **--configuration-file** {{DT_FILE}},",
        "**-C** / **--save-configuration** {{DT_FILE}}, **--save-template** {{DT_FILE}}, and **-h** / **--help**.",
        "",
    ]
    grouped = defaultdict(list)
    for tool_rel, info in tool_entries:
        rel_posix = tool_rel.replace("\\", "/")
        cat = os.path.dirname(rel_posix) or "general"
        grouped[cat].append((rel_posix, info))

    cats = sorted(grouped.keys(), key=lambda c: ("" if c == "general" else c))
    for cat in cats:
        cat_title = "General (`tools/`)" if cat == "general" else "`tools/%s`" % cat
        lines.append("## %s" % cat_title)
        lines.append("")
        for rel_posix, info in sorted(grouped[cat], key=lambda x: os.path.basename(x[0]).lower()):
            base_name = os.path.basename(rel_posix)
            lines.append("### [%s]({{Source}}tools/%s)" % (base_name, rel_posix))
            lines.append("")
            desc = _sanitize_text(info.get("description", ""))
            if desc:
                lines.append(desc)
                lines.append("")
            lines.append("| Option | Description |")
            lines.append("|--------|-------------|")
            for opt in info.get("options", []):
                macro = _dt_macro(opt)
                parts = []
                if opt["short"]:
                    parts.append("**%s** %s" % (opt["short"], macro))
                if opt["long"]:
                    parts.append("**%s** %s" % (opt["long"], macro))
                opt_col = "<br> ".join(parts) if parts else "**%s** %s" % (opt["dest"], macro)

                raw_help = opt.get("help", "")
                has_pct_default = "%(default)" in raw_help
                if has_pct_default:
                    try:
                        raw_help = raw_help % {"default": opt.get("default")}
                    except Exception:
                        pass
                help_str = _sanitize_text(raw_help)
                if opt.get("required") and not opt.get("positional"):
                    help_str = (help_str + "; " if help_str else "") + "*required*"
                norm_def = _normalize_default(opt.get("default"))
                if norm_def is not None and not has_pct_default:
                    help_str = (help_str + "; " if help_str else "") + "*default:* **%s**" % norm_def
                lines.append("| %s | %s |" % (opt_col, help_str))
            lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(
        description="Build the Markdown options overview page for configurable Python tools."
    )
    ap.add_argument(
        "-o", "--output",
        default=os.path.join(HOME_DIR, "docs", "web", "docs", "Tools", "Options.md"),
        category="output",
        type=ap.file,
        help="Output Markdown file ('stdout' writes to standard output)",
    )
    ap.add_argument(
        "-t", "--tools",
        default=None,
        category="input",
        help="Comma-separated list of tool paths relative to tools/ (defaults to templates.TOOLS)",
    )
    ap.add_argument(
        "-j", "--threads",
        default=8,
        type=int,
        category="processing",
        help="Number of parallel worker threads",
    )
    return ap.parse_args(args=args)


def main(args=None):
    options = get_options(args)
    tool_list = (
        [t.strip() if t.strip().endswith(".py") else t.strip() + ".py"
         for t in options.tools.split(",") if t.strip()]
        if options.tools
        else list(templates.TOOLS)
    )
    workers = max(1, min(options.threads, len(tool_list)))
    if workers == 1:
        raw_results = [inspect_tool(t) for t in tool_list]
    else:
        with ThreadPoolExecutor(max_workers=workers) as executor:
            raw_results = list(executor.map(inspect_tool, tool_list))

    entries = []
    failed = []
    for tool_rel, info, err in raw_results:
        if info is None:
            failed.append((tool_rel, err))
            print("Warning: could not extract options from %s: %s" % (tool_rel, err), file=sys.stderr)
        else:
            entries.append((tool_rel, info))

    md = format_markdown(entries)
    if options.output == "stdout":
        sys.stdout.write(md)
    else:
        with io.open(options.output, "w", encoding="utf-8", newline="\n") as f:
            f.write(md)
    if failed:
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
