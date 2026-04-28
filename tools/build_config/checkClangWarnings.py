#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkClangWarnings.py
# @author  Michael Behrisch
# @date    2026-04-28

"""
Fetches the daily clang build log from sumo.dlr.de, parses compiler
warnings, uses an AI model to generate fixes for each affected source
file, and writes a Markdown PR-body summary.

Intended to be run by the daily GitHub Actions workflow
"clang-warning-check".  The workflow then opens a draft PR containing
the AI-patched source files.

Authentication
--------------
By default the script authenticates against the GitHub Models inference
endpoint (https://models.inference.ai.azure.com) using the standard
GITHUB_TOKEN that GitHub Actions provides automatically.  Set
OPENAI_API_KEY (and optionally --endpoint) to use a different provider.
"""

from __future__ import print_function

import argparse
import os
import re
import sys
import urllib.request
from collections import defaultdict

LOG_URL = "https://sumo.dlr.de/daily/clangDmake.log"
GITHUB_MODELS_ENDPOINT = "https://models.inference.ai.azure.com"
# Use a capable but cost-efficient model by default.
DEFAULT_MODEL = "openai/gpt-4.1-mini"

# Only attempt AI fixes on these file extensions.
_FIXABLE_EXTENSIONS = {".cpp", ".cxx", ".cc", ".c", ".h", ".hpp", ".hxx"}

# Matches lines like:
#   /abs/path/to/file.cpp:123:45: warning: some message [-Wsome-flag]
# The warning flag part is optional (some clang warnings omit it).
_WARNING_RE = re.compile(
    r'^(.+?):(\d+):(\d+):\s+warning:\s+(.+?)(?:\s+\[(-W[^\]]+)\])?\s*$'
)

# Heuristic: strip everything up to and including "/sumo/" to obtain a
# repository-relative path.  Falls back to the raw absolute path when the
# prefix cannot be found.
_SUMO_ROOT_MARKER = "/sumo/"

_FIX_PROMPT = """\
You are an expert C++ developer reviewing SUMO (Simulation of Urban MObility).
The following source file produces compiler warnings from clang.
Fix ALL listed warnings while making the smallest possible changes.

Rules:
- Return ONLY the complete fixed source file — no explanations, no markdown fences.
- Do not reformat or re-style code that is unrelated to the warnings.
- Preserve the original copyright/license header exactly.
- If a warning cannot be safely fixed without broader context, leave that \
line unchanged and add a short inline comment: // FIXME: <warning>

File: {rel_path}

Warnings (line:col  flag  message):
{warnings_text}

Source code:
{source_code}"""


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _repo_relative(path):
    idx = path.find(_SUMO_ROOT_MARKER)
    if idx != -1:
        return path[idx + len(_SUMO_ROOT_MARKER):]
    return path


def _strip_fences(text):
    """Remove markdown code fences if the AI wrapped its answer in them."""
    stripped = text.strip()
    if not stripped.startswith("```"):
        # No fences — return as-is so we don't lose trailing newlines.
        return text
    # Drop the opening fence line (```cpp, ``` etc.)
    inner = stripped[stripped.index("\n") + 1:]
    if inner.endswith("```"):
        inner = inner[:inner.rindex("\n")]
    # Ensure a single trailing newline (conventional for source files).
    return inner.rstrip("\n") + "\n"


# ---------------------------------------------------------------------------
# Log fetching and parsing
# ---------------------------------------------------------------------------

def fetch_log(url):
    req = urllib.request.Request(url, headers={"User-Agent": "SUMO-CI/1.0"})
    with urllib.request.urlopen(req, timeout=60) as response:
        return response.read().decode("utf-8", errors="replace")


def parse_warnings(log_content):
    warnings = []
    for line in log_content.splitlines():
        m = _WARNING_RE.match(line)
        if m:
            filepath, lineno, col, message, flag = m.groups()
            warnings.append({
                "file": filepath,
                "rel_file": _repo_relative(filepath),
                "line": int(lineno),
                "col": int(col),
                "message": message.strip(),
                "flag": flag or "",
            })
    return warnings


# ---------------------------------------------------------------------------
# AI-based fixing
# ---------------------------------------------------------------------------

def _call_ai(client, model, rel_path, file_warnings, source_code):
    warnings_text = "\n".join(
        f"  {w['line']}:{w['col']}  {w['flag'] or '(no flag)'}  {w['message']}"
        for w in sorted(file_warnings, key=lambda x: x["line"])
    )
    prompt = _FIX_PROMPT.format(
        rel_path=rel_path,
        warnings_text=warnings_text,
        source_code=source_code,
    )
    response = client.chat.completions.create(
        model=model,
        messages=[{"role": "user", "content": prompt}],
        temperature=0,
    )
    return _strip_fences(response.choices[0].message.content)


def apply_ai_fix(rel_path, file_warnings, repo_root, client, model):
    """
    Ask the AI to fix *file_warnings* in *rel_path*.
    Returns True if the file was modified, False otherwise.
    """
    _, ext = os.path.splitext(rel_path)
    if ext.lower() not in _FIXABLE_EXTENSIONS:
        print(f"  SKIP {rel_path}: unsupported extension '{ext}'", file=sys.stderr)
        return False

    abs_path = os.path.join(repo_root, rel_path)
    if not os.path.isfile(abs_path):
        print(f"  SKIP {rel_path}: not found in repository", file=sys.stderr)
        return False

    with open(abs_path, encoding="utf-8", errors="replace") as fh:
        original = fh.read()

    print(
        f"  Requesting AI fix for {rel_path} ({len(file_warnings)} warning(s))…",
        file=sys.stderr,
    )
    try:
        fixed = _call_ai(client, model, rel_path, file_warnings, original)
    except Exception as exc:
        print(f"  ERROR calling AI for {rel_path}: {exc}", file=sys.stderr)
        return False

    if not fixed or fixed == original:
        print(f"  No changes produced for {rel_path}", file=sys.stderr)
        return False

    with open(abs_path, "w", encoding="utf-8") as fh:
        fh.write(fixed)
    print(f"  Fixed {rel_path}", file=sys.stderr)
    return True


# ---------------------------------------------------------------------------
# PR body generation
# ---------------------------------------------------------------------------

def generate_pr_body(all_warnings, fixed_files, skipped_files, log_url):
    by_file = defaultdict(list)
    for w in all_warnings:
        by_file[w["rel_file"]].append(w)

    by_flag = defaultdict(int)
    for w in all_warnings:
        if w["flag"]:
            by_flag[w["flag"]] += 1

    lines = [
        "This PR was automatically generated by the daily "
        "[clang-warning-check](../../actions/workflows/clang-warning-check.yml) workflow.",
        "",
        f"**Source log:** [{log_url}]({log_url})",
        "",
        f"Total warnings parsed: **{len(all_warnings)}** across **{len(by_file)}** file(s).",
        "",
    ]

    if by_flag:
        lines += [
            "### Warning types",
            "",
            "| Flag | Count |",
            "| --- | ---: |",
        ]
        for flag, count in sorted(by_flag.items(), key=lambda x: -x[1]):
            lines.append(f"| `{flag}` | {count} |")
        lines.append("")

    if fixed_files:
        lines += ["### Files with AI-generated fixes", ""]
        for f in sorted(fixed_files):
            count = len(by_file.get(f, []))
            lines.append(f"- `{f}` — {count} warning(s)")
        lines.append("")

    if skipped_files:
        lines += ["### Files skipped (not found in repo or unsupported type)", ""]
        for f in sorted(skipped_files):
            count = len(by_file.get(f, []))
            lines.append(f"- `{f}` — {count} warning(s)")
        lines.append("")

    lines += [
        "> [!WARNING]",
        "> AI-generated fixes should be reviewed carefully before merging.",
        "> The changes are intended to eliminate warnings but may require manual",
        "> adjustment — particularly for complex or context-sensitive issues.",
    ]
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description=(
            "Fetch the daily clang build log, apply AI-generated fixes to "
            "warned source files, and write a Markdown PR-body summary."
        )
    )
    parser.add_argument(
        "--url", default=LOG_URL, help="URL of the clang build log"
    )
    parser.add_argument(
        "--repo-root",
        default=".",
        help="Path to the repository root (default: current directory)",
    )
    parser.add_argument(
        "--token",
        default=None,
        help=(
            "API token for the AI endpoint.  Falls back to the OPENAI_API_KEY "
            "environment variable, then GITHUB_TOKEN."
        ),
    )
    parser.add_argument(
        "--endpoint",
        default=None,
        help=(
            "OpenAI-compatible API base URL.  Defaults to the GitHub Models "
            f"endpoint ({GITHUB_MODELS_ENDPOINT}) when GITHUB_TOKEN is used, "
            "or https://api.openai.com/v1 when OPENAI_API_KEY is used."
        ),
    )
    parser.add_argument(
        "--model",
        default=DEFAULT_MODEL,
        help=f"Model name to use for fixes (default: {DEFAULT_MODEL})",
    )
    parser.add_argument(
        "--max-files",
        type=int,
        default=20,
        help="Maximum number of source files to fix per run (default: 20)",
    )
    parser.add_argument(
        "--pr-body-output",
        default=None,
        help="Write the pull-request body Markdown to this file (default: stdout)",
    )
    args = parser.parse_args()

    # ------------------------------------------------------------------
    # Resolve API token and endpoint
    # ------------------------------------------------------------------
    openai_key = os.environ.get("OPENAI_API_KEY")
    github_token = os.environ.get("GITHUB_TOKEN")
    token = args.token or openai_key or github_token

    if not token:
        print(
            "ERROR: no API token found.  Set OPENAI_API_KEY or GITHUB_TOKEN, "
            "or pass --token.",
            file=sys.stderr,
        )
        sys.exit(2)

    if args.endpoint:
        endpoint = args.endpoint
    elif openai_key or (args.token and not github_token):
        endpoint = "https://api.openai.com/v1"
    else:
        endpoint = GITHUB_MODELS_ENDPOINT

    try:
        import openai
    except ImportError:
        print(
            "ERROR: 'openai' package is required.  Run: pip install openai",
            file=sys.stderr,
        )
        sys.exit(2)

    client = openai.OpenAI(api_key=token, base_url=endpoint)

    # ------------------------------------------------------------------
    # Fetch and parse the build log
    # ------------------------------------------------------------------
    print(f"Fetching build log from {args.url} …", file=sys.stderr)
    try:
        log_content = fetch_log(args.url)
    except Exception as exc:
        print(f"ERROR: could not fetch {args.url}: {exc}", file=sys.stderr)
        sys.exit(2)

    warnings = parse_warnings(log_content)
    print(f"Parsed {len(warnings)} warning(s).", file=sys.stderr)

    if not warnings:
        msg = "No clang warnings found in the latest build. :white_check_mark:\n"
        if args.pr_body_output:
            with open(args.pr_body_output, "w", encoding="utf-8") as fh:
                fh.write(msg)
        else:
            print(msg)
        return

    # ------------------------------------------------------------------
    # Group by repo-relative file and apply AI fixes
    # ------------------------------------------------------------------
    by_file = defaultdict(list)
    for w in warnings:
        by_file[w["rel_file"]].append(w)

    # Process files with most warnings first; respect --max-files
    ranked = sorted(by_file.items(), key=lambda kv: -len(kv[1]))[: args.max_files]
    if len(by_file) > args.max_files:
        print(
            f"Limiting to {args.max_files} of {len(by_file)} files "
            "(use --max-files to change).",
            file=sys.stderr,
        )

    repo_root = os.path.abspath(args.repo_root)
    fixed_files = []
    skipped_files = []
    for rel_path, file_warnings in ranked:
        ok = apply_ai_fix(rel_path, file_warnings, repo_root, client, args.model)
        if ok:
            fixed_files.append(rel_path)
        else:
            skipped_files.append(rel_path)

    print(
        f"Done: {len(fixed_files)} file(s) fixed, {len(skipped_files)} skipped.",
        file=sys.stderr,
    )

    # ------------------------------------------------------------------
    # Write PR body
    # ------------------------------------------------------------------
    pr_body = generate_pr_body(warnings, fixed_files, skipped_files, args.url)
    if args.pr_body_output:
        with open(args.pr_body_output, "w", encoding="utf-8") as fh:
            fh.write(pr_body)
    else:
        print(pr_body)


if __name__ == "__main__":
    main()
