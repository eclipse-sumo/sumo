---
title: How To create a CI Test with GitHub Actions
---

## Continuous Integration Tests

If you have a project that depends on SUMO, maintaining compatibility with newer SUMO versions can pose an issue.
While striving to keep backwards compatibility, breaking changes for your project might still be introduced.
Since we cannot possibly test integration with all projects out there that depend on SUMO, this document shall assist you in setting up CI tests for your project.

### Using GitHub Actions

One popular way to perform CI tests is to create a [GitHub Actions](https://docs.github.com/en/actions) workflow.
A GitHub Actions workflow is defined by a [YAML](https://yaml.org/) file in your GitHub `master/main` branch, under directory `.github/workflows`.
In the following, we will go through an example workflow (from the [Veins project](https://github.com/sommer/veins)), commenting on the code (for a complete GitHub Actions guide, see the [official documentation](https://docs.github.com/en/actions/learn-github-actions)):

```yml
{% raw %}
name: linux-build  # workflow name as stated under your project's GitHub 'Actions' tab

on:  # define triggers on which to run this workflow
  push:
  pull_request:
  schedule:
    - cron:  '42 0 * * *'

jobs:  # define individual jobs to run
  build:  # ID of this job
    runs-on: ubuntu-latest  # VM type to run this job on
    env:  # define environment variables for this job
      OMNET_VERSION: 5.6.2
    strategy:  # define job variations
      fail-fast: false  # do not cancel all in-progress jobs if any job variation fails
      matrix:  # define variation parameters and their values
        sumo_build: [ubuntu, ppa, nightly]

    steps:  # define this job's individual steps
    - name: Cloning Veins  # this step's name
      uses: actions/checkout@v2  # predefined and reusable action for this step (see https://github.com/actions/checkout)
      with:  # set values for the predefined action's parameters
        path: src/veins

    - name: Cloning OMNeT++
      uses: actions/checkout@v2
      with:
        repository: omnetpp/omnetpp
        ref: omnetpp-${{ env.OMNET_VERSION }}  # evaluate the environment variable OMNET_VERSION defined above
        path: src/omnetpp

    - name: Preparing Build System
      run: |  # define a custom multi-line command (instead of a reusable action) for this step
        if [[ "${{ matrix.sumo_build }}" == "ppa" ]]; then sudo add-apt-repository ppa:sumo/stable; fi
        sudo apt-get update
        sudo apt-get install build-essential gcc g++ bison flex perl python3 libxml2-dev zlib1g-dev default-jre doxygen graphviz

    - name: Installing SUMO from repo
      if: matrix.sumo_build != 'nightly'  # only run this step if the condition is true
      run: |
        sudo apt-get install sumo sumo-tools
        echo "SUMO_HOME=/usr/share/sumo" >> $GITHUB_ENV

    - name: Installing SUMO from PyPI
      if: matrix.sumo_build == 'nightly'
      run: |
        python -m pip install --index-url https://test.pypi.org/simple/ eclipse-sumo
        python -c "import sumo; print('SUMO_HOME=' + sumo.SUMO_HOME)" >> $GITHUB_ENV

    - name: Adapting PATH
      run: |
        echo "PATH=$PWD/src/omnetpp/bin:$SUMO_HOME/bin:$PATH" >> $GITHUB_ENV
        cat $GITHUB_ENV

    - name: Building OMNeT
      run: |
        cd src/omnetpp
        cp configure.user.dist configure.user
        ./configure WITH_TKENV=no WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no
        make -j4

    - name: Building Veins
      run: |
        cd src/veins
        ./configure
        make -j4

    - name: Running Veins Test
      run: |
        cd src/veins/examples/veins
        echo $SUMO_HOME; sumo
        ../../bin/veins_launchd -vv --daemon -L veins_launchd.log
        ./run -u Cmdenv

    - name: Upload Test log
      if: ${{ always() }}
      uses: actions/upload-artifact@v2  # see https://github.com/actions/upload-artifact
      with:
          name: veins-${{ matrix.sumo_build }}.log
          path: |
            src/veins/examples/veins/veins_launchd.log
{% endraw %}
```

To summarize, the basic structure of the workflow consists of the top-level code blocks [`name`](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#name), [`on`](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#on), and [`jobs`](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions#jobs), defining the workflow's name, trigger(s), and job(s), respectively.
A CI test job for your project should then typically follow these steps:

    1. Clone required git repositories
    2. Set up build environment
    3. Build/Install
    4. Run test script(s)

While there are common steps useful for most (if not all) projects' CI test workflows in the example above, note that it includes some Veins-specific steps, which need to be adapted to your project's needs.
