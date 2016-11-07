
# oh my god....:
import platform
import types
global IS_WIN
if platform.system() == 'Windows':
    IS_WIN = True
else:
    IS_WIN = False

import numpy as np


def get_inversemap(map):
    return {v: k for k, v in map.items()}


def random_choice_dist(n, b):
    """
    Returns the absolute distribution of a random choice sample of size n
    having the choice between len(b) options where each option has 
    the probability represented in vector b.
    """
    if np.__version__ >= '1.7.0':
        return np.bincount(np.random.choice(b.size, n, p=b.flat),
                           minlength=b.size).reshape(b.shape)
    else:
        return np.bincount(np.searchsorted(np.cumsum(b), np.random.random(n)), minlength=b.size).reshape(b.shape)


def random_choice(n, b):
    """
    Returns the  random choice sample of size n
    having the choice between len(b) options where each option has 
    the probability represented in vector b.
    """
    if np.__version__ >= '1.7.0':
        return np.random.choice(b.size, n, p=b.flat)
    else:
        return np.searchsorted(np.cumsum(b), np.random.random(n))


def filepathlist_to_filepathstring(filepathlist, sep=',', is_primed=False):
    if IS_WIN & is_primed:
        p = '"'
    else:
        p = ''
    # print 'filepathlist_to_filepathstring',IS_WIN,p,filepathlist
    if type(filepathlist) == types.ListType:
        if len(filepathlist) == 0:
            return ''
        else:
            filepathstring = ''
            for filepath in filepathlist[:-1]:
                fp = filepath.replace('"', '')
                filepathstring += p + fp + p + sep
            filepathstring += p + filepathlist[-1] + p
            return filepathstring
    else:
        fp = filepathlist.replace('"', '')
        return p + filepathlist + p


def filepathstring_to_filepathlist(filepathstring, sep=',', is_primed=False):
    if IS_WIN & is_primed:
        p = '"'
    else:
        p = ''
    filepaths = []
    for filepath in filepathstring.split(sep):
        filepaths.append(p + filepath.strip().replace('"', '') + p)
    return filepaths
