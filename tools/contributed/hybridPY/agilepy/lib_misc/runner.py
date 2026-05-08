

class Runner:
    """Class to manage the execution of processes in a script.
    The class initialized with the following list of tasks:
    [
    ('taskname1', True|False),
    ('taskname2', True|False),
    :
    :
    ]
    The first argument is the task name, the second is whether the task should be executed or not.
    
    """
    def __init__(self, tasks):
        self._tasknames = []
        self._taskinfos = []
        self._taskindex = -1
        for taskname, info in tasks:
            self._tasknames.append(taskname)
            self._taskinfos.append(info)
            
    
    def has_task(self, taskname):
        """Returns True if taskname must be executed"""
        if self._tasknames.count(taskname) > 0:
            ind = self._tasknames.index(taskname)
            
            # check if info foresees this task
            if self._taskinfos[ind]: # could be modified in the future
                self._taskindex = ind
                return True
                
            else:
                return False
            
        else:
            return False
        
    
    def get_index_current(self):
        """
        Returns index of currently executed task
        """
        return self._taskindex

    def format_task_current(self):
        # print 'format_task_current',self._taskindex,self._tasknames
        return "%03d_%s"%(self._taskindex, self._tasknames[self._taskindex])
        
