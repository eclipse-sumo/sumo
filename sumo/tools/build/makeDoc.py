#!/usr/bin/env python
"""
@file    makeDoc.py
@author  Michael.Behrisch@dlr.de
@date    2010-04-03
@version $Id$

Grab the Mediawiki pages and fit them into a big html page and a pdf.

Copyright (C) 2010 DLR/TS, Germany
All rights reserved
"""
import sys, urllib2, xml.dom.minidom, subprocess
from optparse import OptionParser

_HEADLINES = ["h9", "h8", "h7", "h6", "h5", "h4", "h3", "h2", "h1"]

def getCleanDom(url):
    request = "%s&printable=yes" % url
    if options.verbose:
        print request
    reply = "".join(urllib2.urlopen(request).readlines()[:-8])+'</body></html>'
    dom = xml.dom.minidom.parseString(reply)
    useless = dom.getElementsByTagName("script")
    for node in dom.getElementsByTagName("div"):
        if node.getAttribute("class") in ["portlet", "printfooter"]:
            useless.append(node)
        if node.getAttribute("id") in ["contentSub", "jump-to-nav"]:
            useless.append(node)
    for node in dom.getElementsByTagName("table"):
        if node.getAttribute("class") == "toc":
            useless.append(node)
    for node in dom.getElementsByTagName("h3"):
        if node.getAttribute("id") == "siteSub":
            useless.append(node)
    for n in useless:
        n.parentNode.removeChild(n)
        n.unlink()
    return dom

def fixLinks(base, root, title):
    for node in root.getElementsByTagName("img"):
        if node.getAttribute("src")[0] == "/":
            node.setAttribute("src", base+node.getAttribute("src"))
    for node in root.getElementsByTagName("a"):
        if node.getAttribute("id") == "top" and node.getAttribute("name") == "top" :
            node.setAttribute("id", title)
            node.setAttribute("name", title)
        if node.hasAttribute("href"):
            ref = node.getAttribute("href")
            if ref[0] == '/':
                if '#' in ref:
                    node.setAttribute("href", '#'+ref.split('#')[-1])
                else:
                    node.setAttribute("href", '#'+ref.split('=')[-1])

def retrieveList(base, node, anchorMap, depth=1):
    result = []
    for child in node.childNodes:
        if child.nodeType == node.ELEMENT_NODE and child.tagName == "li":
            for a in child.childNodes:
                if a.nodeType == node.ELEMENT_NODE and a.tagName == "a":
                    result.append(a)
                    ref = a.getAttribute("href")
                    if ref[0] == '/' and not '#' in ref and not 'action=edit' in ref:
                        child = getCleanDom(base+ref)
                        for node in child.getElementsByTagName("div"):
                            if node.getAttribute("id") == "content":
                                anchorMap[a] = node
                        fixLinks(base, anchorMap[a], ref.split('=')[-1])
                        for idx, head in enumerate(_HEADLINES[depth:]):
                            for node in anchorMap[a].getElementsByTagName(head):
                                node.tagName = _HEADLINES[idx]
                if a.nodeType == node.ELEMENT_NODE and a.tagName == "ul":
                    result += retrieveList(base, a, anchorMap, depth+1)
    return result

def retrieveDocuments(url):
    if not url[:7] == "http://":
        url = "http://"+url
    base = url
    if '/' in url[7:]:
        base = url[:7+url[7:].index('/')]
    parent = getCleanDom(url)
    for node in parent.getElementsByTagName("link"):
        if node.getAttribute("href")[0] == "/":
            node.setAttribute("href", base+node.getAttribute("href"))
    anchorMap = {}
    for node in parent.getElementsByTagName("h1"):
        sib = node.nextSibling
        while sib.nodeType != node.ELEMENT_NODE:
            sib = sib.nextSibling
        if sib.tagName == "ul":
            for l in retrieveList(base, sib, anchorMap):
                if l in anchorMap:
                    sib.parentNode.insertBefore(anchorMap[l], sib)
            sib.parentNode.removeChild(sib)
            sib.unlink()
    return parent.toxml('utf8')

if __name__ == "__main__":
    optParser = OptionParser()
    optParser.add_option("-v", "--verbose", action="store_true",
                         default=False, help="tell me what you are doing")
    optParser.add_option("-u", "--url", help="input url",
                         default="http://sourceforge.net/apps/mediawiki/sumo/index.php?title=SUMO_User_Documentation")
    optParser.add_option("-o", "--output", default="sumo-user.html",
                         help="output html file", metavar="FILE")
    optParser.add_option("-p", "--pdf", default="sumo-user.pdf",
                         help="output pdf file", metavar="FILE")
    (options, args) = optParser.parse_args()
    out = open(options.output, 'w')
    out.write(retrieveDocuments(options.url))
    out.close()
    subprocess.call(["htmldoc", "--webpage", "-f", options.pdf, options.output])
