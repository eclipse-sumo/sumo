"""
Simulates <n_iter> times the <scenariofile>, calling
script <simscriptfile> before each simulation run.

Usage:
python sumo_virtualpop_iterate.py  <scenariofile> <n_iter> <simscriptfile>

"""

import sys, os, subprocess, time



def start_iterations(scenariofilepath, n_iter, simscriptfilepath): 
    print('sumo_virtualpop_iterate.run',scenariofilepath)
    cmlfilepath = os.path.join(os.path.dirname(simscriptfilepath),'sumo_virtualpop_cml.bash')

    P='"'
    if os.path.isfile(cmlfilepath):
        os.remove(cmlfilepath)
    
    cml_script = 'python '+P+simscriptfilepath+P+' '+P+scenariofilepath+P+' '+cmlfilepath


    for i in range(1,n_iter+1):

        print('  Start preparation of iteration %d.'%i)
        proc=subprocess.Popen(cml_script, shell=True)
        proc.wait()
        if proc.returncode == 0:
            print('  Preparation of iteration %d successful'%i)
            f = open(cmlfilepath,"r")
            cml = f.readline()
            f.close()
            
            #time.sleep(10)
            
            print('  Start SUMO microsimulator')
            print('  cml=',cml)
            proc=subprocess.Popen(cml, shell=True)
            proc.wait()
            if proc.returncode == 0:
                print('  Microsimulation of iteration %d successful.'%i)
            else:
                print('  Error in microsimulation of iteration %d.'%i)
        else:
            print('  Error in preparation of iteration %d successful'%i)
    print('  Start preparation of iteration %d.'%i)
    proc=subprocess.Popen(cml_script, shell=True)
    proc.wait()
    if proc.returncode == 0:
        print('Save last results.')
        f = open(cmlfilepath,"r")
        cml = f.readline()
        f.close()
    else:
        print('error on the last data backup')
            
            #time.sleep(10)

if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except Exception:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
        
    scenariofilepath = os.path.abspath(sys.argv[1])
    n_iter = int(sys.argv[2])
    if len(sys.argv)>3: 
        simscriptfilepath = sys.argv[3]
    else:
        simscriptfilepath = os.path.join(APPDIR,'sumo_virtualpop.py')

    start_iterations(scenariofilepath, n_iter, simscriptfilepath)

