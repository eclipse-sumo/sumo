#!/usr/bin/env python
"""
@file    testSuiteTools.py
@author  Matthias Wagner
@author  Michael Behrisch
@date    2008-02-11
@version $Id$

Test suite to run all tests in the detector package.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import unittest, os, sys
import tools

class ToolsTestSuite(unittest.TestSuite):
    '''
    Test suite class for the package detector.
    All test have to be added in import and in method addTestCases.
    Running this class will run all test cases, the results displayed on the 
    command line.
    '''
    passedList = []
    failedList = []
    errorList = []

    def __init__(self):
        #setUp = self.__setUpFunc
        unittest.TestSuite.__init__(self)
        #self.addTest(self.suite())
        self.addTestCases()

    def setUp(self):
        '''
        To prepare the test environment.
        Note: This method does not replace or overwrite setUp() in any TestCase class! 
        Note: This method is not run when starting this test suite as "Python unit-test" 
              in the eclipse environment or similar unit test environments! 
        '''
        print "Start Test ..."

    def run(self, result):
        '''
        Method being called to execute the tests.
        @param result A result object for the presentation of the results
        '''
        self.setUp()

        unittest.TestSuite.run(self, result)

        self.tearDown()

        if len(result.errors) > 0:
            print "\nErrors:"
            print "===================="
            for error in result.errors:
                print "+++ Test:", error[0], "\n", error[1]
        if len(result.failures) > 0:
            print "\nFailures:"
            print "===================="
            for failure in result.failures:
                print "+++ Test:", failure[0], "\n", failure[1]

        print "Summary:", result.testsRun, "Tests run"
        print "===================="
        print "Test Passed:", result.testsRun - (len(result.failures) + len(result.errors))
        print "Test Failures:", len(result.failures)
        print "Test Errors:", len(result.errors)
        for test in self.passedList:
            print test

    def tearDown(self):
        '''
        Finishes the test environment.
        Note: This method does not replace or overwrite tearDown() in any TestCase class!
        Note: This method is not run when starting this test suite as "Python unit-test" 
              in the eclipse environment or similar unit test environments!
        '''
        print "... Test End."

    def addTestCases(self):
        '''
        Method to add all test classes to this suite.
        Currently each test class has to be added manually here and be imported.
        TODO: Apply or write a tool to automatically add the test cases.
        '''
        self.addTest(unittest.TestLoader().loadTestsFromTestCase(tools.district.testEdgesInDistrict.TestEdgesInDistrict))

class ToolsTestResult(unittest.TestResult):
    '''
    Class to collect the results and format the output during the test run.
    '''
    shouldStop = False
    def __init__(self):
        unittest.TestResult.__init__(self)

    def addError(self, test, err):
        '''
        Overwitten from @see unittest.TestResult
        '''
        #print "ERROR:  ", test, err
        print test, "... Error!"
        unittest.TestResult.addError(self, test, err)
    def addFailure(self, test, err):
        '''
        Overwitten from @see unittest.TestResult
        '''
        #print "Failed: ", test, err
        print test, "... Failed!"
        unittest.TestResult.addFailure(self, test, err)
    def addSuccess(self, test):
        '''
        Overwitten from @see unittest.TestResult
        '''
        print test, "... Passed."
        unittest.TestResult.addSuccess(self, test)

if __name__ == '__main__':
    testSuite = ToolsTestSuite()
    testSuite.run(ToolsTestResult())

