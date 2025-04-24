#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    bibtexUpdate.py
# @author  Mirko Barthauer
# @date    2024-10-29

"""
This script queries meta data from Semantic Scholar about publications citing
one of the papers given by their unique (Semantic Scholar) identifier.
Then it writes the results to a text file in bibtex syntax. Optionally it can
read an existing bibtex file and integrate existing entries with the new ones,
keeping the old annotations / Jabref comments and adding new ones for the new
entries. It uses the JabRef group syntax version 3.
Moreover it can run a keyword discover algorithm to display the most used keywords in the papers' abstracts.
"""


# https://api.semanticscholar.org/graph/v1/paper/{paper_id}/citations /paper_id can be
# "DOI:10.18653/v1/N18-3011", Semantic Scholar hash "649def34f8be52c8b66281af98ae884c09aef38b" and others)

import os
import re
import time
import argparse
import requests
import bibtexparser
import unicodedata
from datetime import datetime

ArticleTypeMap = {
    "JournalArticle":  "Article",
    "Conference":  "InProceedings",
    "Dataset":  "Misc",
    "Book":  "Book",
    "BookSection":  "InBook"
}


def main(options):
    if options.bibtexInput is None or not os.path.exists(options.bibtexInput):
        print("The search results will not be merged with existing bibtex because no bibtex input file was found.")

    offsetLimit = 1000  # maximum number of entries the citation request of Semantic Scholar can return
    papers = []
    citationRequestTemplate = "https://api.semanticscholar.org/graph/v1/paper/%s/citations%s"
    delay = 1  # s
    foundDOI = []

    # loop through original works the result papers should cite
    for originalDOI in options.citedWorks:
        # send requests with the maximum offset but keep a delay between the requests
        loop = True
        offset = 0
        while loop:
            # remember call duration to send a limited amout of requests
            callStart = datetime.now()
            params = {
                'offset': str(offset),
                'limit': str(offsetLimit),
                'fields': 'title,url,authors,year,externalIds,citationCount,abstract,venue,journal,publicationTypes',
            }
            fullRequest = citationRequestTemplate % (originalDOI, "" if len(params) == 0 else "?%s" % "&".join([
                                                     "%s=%s" % (key, val) for key, val in params.items()]))
            print(fullRequest)
            rsp = requests.get(fullRequest)
            callEnd = datetime.now()
            results = rsp.json()
            if rsp.status_code == 400:
                requestError = results["error"]
                print(requestError)
            elif rsp.status_code == 404:
                print("The original paper with the identifier %s is not available at SemanticScholar." % originalDOI)
                break
            rsp.raise_for_status()
            total = len(results["data"])
            if total == 0:
                print('No matches found. Please try another query.')
                return
            loop = "next" in results
            # countBefore = len(papers)
            # papers.extend([d['citingPaper'] for d in results['data'] if 'externalIDs' in d['citingPaper']
            #  and 'DOI' in d['citingPaper']['externalIds']]
            #  and d['citingPaper']['citationCount'] >= options.minCitations)
            newPapers = [d['citingPaper'] for d in results['data']
                         if d['citingPaper']['externalIds'] is not None and
                         'DOI' in d['citingPaper']['externalIds'] and
                         d['citingPaper']['citationCount'] is not None and
                         d['citingPaper']['citationCount'] >= options.minCitations and
                         d['citingPaper']['externalIds']['DOI'] not in foundDOI]
            papers.extend(newPapers)
            foundDOI.extend([paper['externalIds']['DOI'] for paper in newPapers])
            # countAfterwards = len(papers)
            # print("Added %d papers to %d previously found ones from %d found entries"
            #  % ((countAfterwards - countBefore), countBefore, len(results["data"])))
            if loop:
                offset += total
                # wait a little
                deltaTime = (callEnd - callStart).total_seconds()
                if deltaTime < delay:
                    time.sleep(delay - deltaTime)

    if len(papers) == 0:
        print('No matches found. Please try another query.')
        return

    # exclude certain media
    if options.excludeMedia is not None:
        toRemove = []
        for paper in papers:
            if paper["venue"] is not None and paper["venue"] in options.excludeMedia:
                toRemove.append(paper)
        for r in toRemove:
            papers.remove(r)
        print("%d remaining papers after removal of named publications" % len(papers))

    if options.statistics:
        # compute statistics based on the search result
        from summa import keywords
        keywordsTotal = {}
        for paper in papers:
            if paper["abstract"] is None:
                continue
            abstractKeywords = keywords.keywords(paper["abstract"], scores=True)[:5]
            for abstractKeyword, score in abstractKeywords:
                if abstractKeyword not in keywordsTotal:
                    keywordsTotal[abstractKeyword] = 0
                keywordsTotal[abstractKeyword] += score

        # sorted list of most used discovered keywords
        maxKeywords = 30
        # see https://www.analyticsvidhya.com/blog/2022/01/four-of-the-easiest-and-most-effective-methods-of-keyword-extraction-from-a-single-text-using-python/  # noqa
        sortedKeywords = sorted([(keyword, score) for keyword, score in keywordsTotal.items()],
                                key=lambda x: x[1], reverse=True)[:maxKeywords]
        print("\n\n%d first automatically discovered keywords:\n" % min(maxKeywords, len(sortedKeywords)))
        i = 1
        for keyword, score in sortedKeywords:
            print("\t%-3d   %-100s   %-3f" % (i, keyword, score))
            i += 1

    # insert entries from Semantic Scholar search result
    lowerToOriginalKeyWords = {keyword.lower(): keyword for keyword in options.keywords} if options.keywords else {}

    blocks = []
    now = datetime.now()
    dateString = now.strftime("%Y%m%d")
    baseGroup = "SemanticScholar_%s" % (dateString)
    alreadyUsedKeywords = set()
    groups = [(1, baseGroup, 0, 0)]
    usedKeys = []
    newEntries = []
    for paper in papers:
        keywords = getRelevantKeyWords(paper, lowerToOriginalKeyWords) if options.keywords else []
        if len(keywords) > 0:
            extension = [(2, keyword, 0, 0) for keyword in keywords if keyword not in alreadyUsedKeywords]
            if len(extension) > 0:
                groups.extend([(2, keyword, 0, 0) for keyword in keywords if keyword not in alreadyUsedKeywords])
            alreadyUsedKeywords.update(keywords)
            keywords.append(baseGroup)
        newBlock = toBibTexBlock(paper, groups=keywords)
        generatedKey = newBlock.key
        i = 0
        while newBlock.key in usedKeys:
            newBlock.key = "%s_%d" % (generatedKey, i)
            i += 1
        newEntries.append(newBlock)
        usedKeys.append(newBlock.key)

    # sort new entries by year and citation keys
    newEntries.sort(key=lambda x: (x.get("year").value, x.key))

    # write JabRef specific comments
    # gather old groups from Jabref comment, check if some coincide with the new groups
    jabrefGroupHeader = "jabref-meta: grouping:\n0 AllEntriesGroup:;"
    if options.bibtexOutput is not None and options.bibtexInput is not None:  # combine two libraries
        print("Merge search results with old library %s" % options.bibtexInput)
        oldLibrary = bibtexparser.parse_file(options.bibtexInput)
        for comment in oldLibrary.comments:
            if comment.comment.startswith("jabref-meta: grouping:"):
                knownGroupNames = [group[2] for group in groups]
                foundGroups = re.findall(r"([0-9]+)\sStaticGroup:([^\\;]+)\\;([0-9]+)\\;([0-9]+)", comment.comment)
                for foundGroup in foundGroups:
                    indent = int(foundGroup[0])
                    name = foundGroup[1]
                    nr1 = foundGroup[2]
                    nr2 = foundGroup[3]
                    if name not in knownGroupNames:
                        groups.append((indent, name, nr1, nr2))
                break
        blocks.extend(oldLibrary.entries)
        # remove double entries present in the old library and the search results
        oldDOI = [block.get("DOI").value for block in blocks if block.get("DOI") is not None]
        newEntries = [newEntry for newEntry in newEntries if newEntry.get("DOI").value not in oldDOI]

    blocks.extend(newEntries)
    jabRefComment = bibtexparser.model.ExplicitComment("jabref-meta: databaseType:bibtex;")
    blocks.append(jabRefComment)
    groupText = "%s\n%s" % (jabrefGroupHeader, "\n".join(["%d StaticGroup:%s\\;%s\\;%s\\;0x8a8a8aff\\;\\;\\;;" % (
        indent, keyword, nr1, nr2) for indent, keyword, nr1, nr2 in groups]))
    groupComment = bibtexparser.model.ExplicitComment(groupText)
    blocks.append(groupComment)

    newLibrary = bibtexparser.library.Library(blocks)
    if options.bibtexOutput is not None:
        bibText = bibtexparser.write_string(newLibrary)
        with open(options.bibtexOutput, mode="w", encoding="UTF-8") as f:
            print("Write to %s" % options.bibtexOutput)
            f.write(bibText)


def formatName(name):
    return u"".join([unicodedata.normalize("NFKD", x)[0] for x in name if x not in ("'", "‐")])


def getRelevantKeyWords(paper, keywords):
    result = []
    lowerTitle = paper["title"].lower()
    result.extend([originalKeyword for lowerKeyword, originalKeyword in keywords.items() if lowerKeyword in lowerTitle])
    return result


def getBibtexType(semanticScholarType):
    global ArticleTypeMap
    if semanticScholarType is None or semanticScholarType[0] not in ArticleTypeMap:
        return "Article"
    return ArticleTypeMap[semanticScholarType[0]]


def toBibTexBlock(paper, groups=""):
    # generate citation key
    authors = [a["name"].strip() for a in paper["authors"]]
    lastNames = [a[a.rindex(" ")+1:] for a in [formatName(author) for author in authors]]
    citKey = "%s%d" % ("%sEtAl" % lastNames[0] if len(lastNames) > 2 else "And".join(lastNames), paper["year"])
    entry = bibtexparser.model.Entry(getBibtexType(paper["publicationTypes"]), citKey, [])
    entry.set_field(bibtexparser.model.Field("author", " and ".join(authors)))
    entry.set_field(bibtexparser.model.Field("year", paper["year"]))
    entry.set_field(bibtexparser.model.Field("title", paper["title"]))
    if entry.entry_type == "Article" and paper["venue"] is not None:
        entry.set_field(bibtexparser.model.Field("journal", paper["venue"]))
        if paper["journal"] is not None:
            for field in ("pages", "volume"):
                if field in paper["journal"]:
                    entry.set_field(bibtexparser.model.Field(field, paper["journal"][field]))
    entry.set_field(bibtexparser.model.Field("DOI", paper['externalIds']['DOI']))
    if len(groups) > 0:
        entry.set_field(bibtexparser.model.Field("groups", ",".join(groups)))
    return entry


def getOptions(args=None):
    ap = argparse.ArgumentParser(
        prog='LiteratureCrawler',
        description='Search in literature databases and collect results in bibtex')
    ap.add_argument("--bibtex-input", dest="bibtexInput", type=str)
    ap.add_argument("--bibtex-output", dest="bibtexOutput", type=str)
    ap.add_argument("--exclude-media", dest="excludeMedia", nargs="+",
                    type=str, help="Exclude results from the given media")
    ap.add_argument("--cited-works", dest="citedWorks", nargs="+", type=str,
                    help="Give works by their identifiers for which the citing papers should be searched "
                    "(SemanticScholar hash confirmed, DOI 'DOI:' don't work although mentioned in the API doc)")
    ap.add_argument("--min-citations", dest="minCitations", type=int, default=10,
                    help="Minimum citation count of a paper to be included in the output")
    ap.add_argument("--keywords", nargs="+", type=str, help="Keywords to group the results in bibtex")
    ap.add_argument("--statistics", default=False, action="store_true",
                    help="Whether to print statistics about the search result")
    options = ap.parse_args(args=args)

    return options


if __name__ == "__main__":
    main(getOptions())
