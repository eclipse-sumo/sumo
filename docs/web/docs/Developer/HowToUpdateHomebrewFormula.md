---
title: How to update the Homebrew Formula
---

This document describes how to update the macOS Homebrew Formula for a new SUMO release.


## GitHub Repository

All relevant files can be found in the repository at https://github.com/DLR-TS/homebrew-sumo.
Note that `brew` requires the `homebrew-` prefix in order for the `brew tap dlr-ts/sumo` command to work.

## Step-by-Step Formula Update Guide

After a new SUMO release (see [HowToRelease](HowToRelease.md)), follow these steps (assuming you have already checked out the repository):

1. Run the bash script:

    ```
    ./update_formula.sh <NEW_VERSION> <SUMO_SRC_URL>
    ```

    where `<NEW_VERSION>` is the version string of the new sumo release (e.g., `1.9.2`) and
    `<SUMO_SRC_URL>` is the URL to the respective source code archive **with examples**
    (e.g., `https://sumo.dlr.de/releases/1.9.2/sumo-src-1.9.2.tar.gz`).

    This script automates (including local git commits):

   * Archiving the old formula (for the previous sumo version)
   * Creating a new temporary `sumo` git branch (**same name as the formula**)
   * Bumping the formula version (via `brew bump-formula-pr`)
   * Removing the (now obsolete) bottle block in the formula
   * Updating the symlink in `Aliases/`
   * Updating the version number in `README.md`<br/><br/>

2. Push the local commits to upstream:

    ```
    git push --set-upstream origin sumo
    ```

3. Create a pull request from the new branch to trigger the `tests.yml` workflow which performs a formula audit, install test, and bottle creation.

4. If the previous workflow ran successfully, label the PR with the `pr-pull` tag. This triggers the `publish.yml` workflow which uploads the bottles to GitHub and deletes the temporary sumo branch afterwards.
