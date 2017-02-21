#!/usr/bin/python

import numpy as np
import sys, getopt


def run(ifile, ofile):
    arrData = np.genfromtxt(ifile,
                            delimiter=',',
                            names=True,
                            dtype="S10,S10,float,float,S10,S10,S10,S10,float,float,float,float,S10,S10,S10,S10,S10,S10",
                            skip_header=1,
                            skip_footer=0)
    print(arrData.dtype.names)
    # edgeId =  arrData['Edge']



def main(argv):
    inputfile = ''
    outputfile = ''

    if len(argv) < 4:
        print('test.py -i <path to DFUSim network csv file> -o <path to output network file>')
        sys.exit(2)
    try:
        opts, args = getopt.getopt(argv, "hi:o:", ["ifile=", "ofile="])
    except getopt.GetoptError:
        print('test.py -i <inputfile> -o <outputfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('test.py -i <inputfile> -o <outputfile>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
    print('Input file is "', inputfile)
    print('Output file is "', outputfile)
    run(inputfile, outputfile)


if __name__ == "__main__":
    main(sys.argv[1:])
