#!/usr/bin/env python

# call jtrrouter twice and check that the output differs
import os,subprocess,random

def get_depart_lines(route_file):
    return [l for l in open(route_file) if 'depart' in l]

output_file1 = 'output1.rou.xml'
output_file2 = 'output2.rou.xml'

jtrrouter = os.environ.get('JTRROUTER_BINARY')
assert(jtrrouter)

args = ' '.join([jtrrouter,
        '--net-file input_net.net.xml', 
        '--flow-definition input_flows.flows.xml',
        '--turn-definition input_turns.turns.xml',
        '--output-file %s '
        '--sinks=end',
        '--srand %s ', # otherwise it will still be deterministic, --abs-rand does not work either
        '--randomize-flows'])

subprocess.call((args % (output_file1, random.randint(0, 2 ** 31))).split())
route_lines1 = get_depart_lines(output_file1)

subprocess.call((args % (output_file2, random.randint(0, 2 ** 31))).split())
route_lines2 = get_depart_lines(output_file2)

if route_lines1 != route_lines2:
    print('test passed. output is random')
else:
    print('test failed. output is deterministic')
