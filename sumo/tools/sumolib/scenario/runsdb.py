"""
@file    runsdb.py
@author  Daniel Krajzewicz
@date    2014-07-21
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
@date    2014-09-01
"""

from __future__ import absolute_import
import sqlite3
import os


class RunsDB:

    def __init__(self):
        self.conn = None
        self.cursor = None
        # !!! if an old db is opened to be extended, this should be read from the db
        self.run = -1

    def buildDB(self, dbName="results.db", deletePrior=True):
        if deletePrior and os.path.exists(dbName):
            os.remove(dbName)
        self.conn = sqlite3.connect(dbName)
        self.cursor = self.conn.cursor()
        self.cursor.execute(
            'CREATE TABLE run (id integer, key text, value text)')
        self.cursor.execute(
            'CREATE TABLE result (runID integer, denominator integer, key text, value real)')
        self.cursor.execute(
            'CREATE TABLE timed_result (runID integer, denominator integer, aib real, aie real, key text, value real)')
        self.conn.commit()
        self.run = 0

    def open(self, dbName="results.db"):
        self.conn = sqlite3.connect(dbName)
        self.cursor = self.conn.cursor()
        self.cursor.execute("SELECT MAX(id) FROM run")
        v = self.cursor.fetchall()
        if len(v) != 0 and v[0][0] != None:
            self.run = v[0][0] + 1
        else:
            self.run = 0

    def addRun(self, scenario, kvDesc):
        if self.run < 0:
            raise "Database was not initialised"
        self.run = self.run + 1
        cid = self.run - 1
        for k in kvDesc:
            self.cursor.execute(
                "INSERT INTO run VALUES (?,?,?)", (cid, k, kvDesc[k]))
        return cid

    def addResult(self, runID, denominator, key, value):
        self.cursor.execute(
            "INSERT INTO result VALUES (?,?,?,?)", (runID, denominator, key, value))
        self.conn.commit()

    def addTimedResult(self, runID, denominator, aib, aie, key, value):
        self.cursor.execute(
            "INSERT INTO timed_result VALUES (?,?,?,?,?,?)", (runID, denominator, aib, aie, key, value))
        self.conn.commit()

    def addResults(self, results):
        self.cursor.executemany("INSERT INTO result VALUES (?,?,?,?)", results)
        self.conn.commit()

    def addTimedResults(self, results):
        self.cursor.executemany(
            "INSERT INTO timed_result VALUES (?,?,?,?,?,?)", results)
        self.conn.commit()

    def toList(self, what):
        ret = []
        for r in what:
            ret.append(r[0])
        return ret

    def getRunIDs(self):
        self.cursor.execute("SELECT DISTINCT id FROM run")
        return self.toList(self.cursor.fetchall())

    def getMatchingRunIDs(self, key, value):
        self.cursor.execute(
            "SELECT DISTINCT id FROM run WHERE key=? AND value=?;", (key, value))
        return self.toList(self.cursor.fetchall())

    def getRunsData(self, runs=None):
        if runs == None:
            runs = self.getRunIDs()
        ret = {}
        for r in runs:
            ret[r] = {}
            for row in self.cursor.execute("SELECT id FROM run WHERE id=?", (r)):
                ret[r][row[1]] = row[2]
        return ret

    def getRunID(self, kvMap):
        stmt = ""
        params = []
        for k in kvMap:
            if len(stmt) != 0:
                stmt = stmt + " AND "
            else:
                stmt = stmt + " WHERE "
            stmt = stmt + "id IN (SELECT id FROM run WHERE key=? AND value=?)"
            params.extend([k, kvMap[k]])
        self.cursor.execute("SELECT DISTINCT id FROM run %s;" % stmt, params)
        return self.toList(self.cursor.fetchall())

    """
  Returns a map:
    runID->denominator->measure->value
  """

    def fetchResults(self, runs=None, measure=None, denominator=None):
        if runs == None:
            runs = self.getRunIDs()
        ret = {}
        for r in runs:
            ret[r] = {}
            if measure == None:
                i = self.cursor.execute(
                    "SELECT * FROM result WHERE runID=?", (r,))
            else:
                if denominator == None:
                    i = self.cursor.execute(
                        "SELECT * FROM result WHERE runID=? AND key=?", (r, measure))
                else:
                    i = self.cursor.execute(
                        "SELECT * FROM result WHERE runID=? AND key=? AND denominator=?", (r, measure, denominator))
            for row in i:
                if row[1] not in ret[r]:
                    ret[r][row[1]] = {}
                if row[2] in ret[r][row[1]]:
                    pass
                    raise "set twice!!"
                ret[r][row[1]][row[2]] = row[3]
        return ret

    def getMeasures(self):
        self.cursor.execute("SELECT DISTINCT key FROM result")
        return self.toList(self.cursor.fetchall())

    def getDenominators(self):
        self.cursor.execute("SELECT DISTINCT denominator FROM result")
        return self.toList(self.cursor.fetchall())

    def getNamedRunAttribute(self, attr):
        self.cursor.execute(
            "SELECT DISTINCT value FROM run WHERE key=?;", (attr,))
        return self.toList(self.cursor.fetchall())

    def getNamedAttributeFromRun(self, rid, key):
        self.cursor.execute(
            "SELECT DISTINCT value FROM run WHERE id=? and key=?;", (rid, key))
        return self.toList(self.cursor.fetchall())

    def close(self):
        self.conn.close()
