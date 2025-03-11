---
title: Managing Publications
---

Publications are stored in an extended
[bibtex-file](https://sourceforge.net/apps/trac/sumo/browser/trunk/sumo/docs/sumo.bib)
generated using [JabRef](https://www.jabref.org/).

# Organising

[JabRef](https://www.jabref.org/) allows not only to enter your
references, but also to group them. We use the following way to organise
the references:

- **sumo**: All that cite SUMO; in fact, there are some other
references in the bibtex-file, for example in case of reports on
projects where SUMO was used, but the paper itself does not mention
SUMO or papers on theory.
- **projects**: a classification by the project
  - **iTETRIS**: reports on work done in iTETRIS
  - ... further projects ...
- **topic**:
  - **V2X**: vehicular communication papers
  - **TLS**: traffic lights paper
  - ... further topics ...
- **organisation**: a classification by organisation
  - **DLR/TS/VM**: the traffic management group at the Institute of
    Transportation Systems at the German Aerospace Center
  - ... further organisations ...

When adding a new entry, please complete its description, first. Then,
right-click on the entry in the list and assign it to the groups it
belongs to - at least organisation and topics.

# Update

Although the publications can be edited manually using JabRef, there is an automated approach to add 
new entries from the search engine [Semantic Scholar](https://www.semanticscholar.org/). The script 
**SUMO_HOME/tools/build_config/bibtexUpdate.py** can be used to query the most cited articles which 
refer to one of the SUMO reference publications. The script is called like this:
```
python tools/build_config/bibtexUpdate.py --bibtex-input docs/sumo.bib --bibtex-output docs/sumo.bib --cited-works "b1914c912dea62703856d89fe3724675a6139b71" "0e62ded610aeb17cc65f9f7159477e48248a98a2" --min-citations 100 --exclude-media "SUMO Conference Proceedings" "International Conference on Simulation of Urban Mobility" "arXiv.org"
```
The example call takes the current version of the bibtex file and overwrites it. 
It queries the works citing the SUMO reference publications given by their Semantic Scholar hashes 
in **--cited-works** and includes only those with at least **--min-citations**. Publications can be excluded from the 
search result if contained in journals/conferences named in **--exclude-media**.

The script can be executed within Github using the **bibtex** workflow which then generates a pull request with the updated 
bibtex file.
