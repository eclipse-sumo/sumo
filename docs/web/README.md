# SUMO Documentation :book:

The SUMO Documentation is generated using [MkDocs](https://www.mkdocs.org/).

## Getting started

### Prerequisites
- Python > 3.5 (the *mkdocs-macros-plugin* requires it)

### Installation
First step is to install MkDocs and some MkDocs plugins:
```
pip install mkdocs

pip install mkdocs-macros-plugin
pip install mdx_gh_links
pip install pymdown-extensions
pip install mkdocs-git-revision-date-plugin
pip install mdx_truly_sane_lists
```

Next step is to clone this repository:
```
git clone https://github.com/eclipse/sumo.git
```

Navigate to this path (`sumo/docs/web`).

To run the built-in development server, use:
```
mkdocs serve
```

`mkdocs serve` re-builds the entire site everytime it detects a change. If you want to only re-build the pages that have been modified, use `mkdocs serve --dirtyreload` [read more](https://www.mkdocs.org/about/release-notes/#support-for-dirty-builds-990)

**In order to be able to use the Search function, you should run the built-in development server and access the Documentation from the localhost address (which by default is 127.0.0.1:8000) rather than from the generated static html files.**

To generate the final static html files, run:
```
mkdocs build
```
This should create the *site* directory, containing all the necessary HTML and CSS files.

## Contributing

Feel free to contribute and improve the SUMO Documentation.