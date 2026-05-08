
import types
import numpy as np
import time, re

# default file path priming
# this did depend on operating system, now " for all
P = '"'
import platform
#if platform.system()=='Windows':
if platform.system()=='Linux':
    IS_LINUX = True
else:
    IS_LINUX = False

def string_to_float(s):
    """Returns a float from a string while cleaning it"""
    q = ''
    is_sample = False
    for c in s:
        if c.isdigit():
            q += c
        elif c == '.':
            q += c
        else:
            break
    return float (q)

def find_char_in_string(s, ch):
    return [i for i, ltr in enumerate(s) if ltr == ch]

def clean_datarow(row, is_remove_quotes = True, is_commas_in_quotes = True):
    row.strip()
    row = row.replace('\n', '')
    if is_remove_quotes:
        row = row.replace('"', '')
        
    if is_commas_in_quotes:
        row = ''.join([item if index % 2 == 0 else re.sub(',', '', item) for index, item in enumerate(row.split('"')) ])
    
    return row

def get_inversemap(m):
    return {v: k for k, v in list(m.items())}


def random_choice_dist2(n, b):
    """
    Returns the absolute distribution of a random choice sample of size n
    having the choice between len(b) options where each option has
    the probability represented in vector b.
    """


def random_choice_dist(n, b):
    """
    Returns the absolute distribution of a random choice sample of size n
    having the choice between len(b) options where each option has
    the probability represented in vector b.
    """
    if np.__version__>='1.7.0':
        return np.bincount(np.random.choice(b.size, n, p=b.flat),
                    minlength=b.size).reshape(b.shape)
    else:
        return np.bincount(np.searchsorted(np.cumsum(b), np.random.random(n)),minlength=b.size).reshape(b.shape)


def random_choice(n, b):
    """
    Returns the  random choice sample of size n
    having the choice between len(b) options where each option has
    the probability represented in vector b.
    """
    #print 'misc.random_choice'
    if np.__version__>='1.7.0':
        #print '  b',b.size,b,b.flat
        #print '  n',n
        return np.clip(np.random.choice(b.size, n, p=b.flat),0,len(b)-1)
    else:
        return np.clip(np.searchsorted(np.cumsum(b), np.random.random(n)),0,len(b)-1)

#def random_choice1d(n, b):
#    """
#    Returns the  random choice sample of size n
#    having the choice between len(b) options where each option has
#    the probability represented in vector b.
#    """
#
#    return np.argmax(np.random.rand(n)*b.flat)

def format_seconds(time, is_round_to_min = True):
    """
    Formats seconds in hh:mm:ss
    """
    h, rem = divmod(time,3600)
    m, rem = divmod(rem,60)
    if is_round_to_min:
        if rem> 30.0: m += 1
        return '%02d:%02d'%(h,m)
    else:
        return '%02d:%02d:%02d'%(h,m,rem)
    
def get_seconds_from_timestr(   t_data, t_offset = None,
                    sep_date_clock = ' ', sep_date = '-', sep_clock = ':',
                    is_float = True):
    """
    Returns time in seconds after t_offset.
    If no offset is geven, the year 1970 is used.
    Time string format:
        2012-05-02 12:57:08.0
    """
    if t_offset  is None:
        t_offset = time.mktime((1970,1,1,0,0,0,0,0,0))# year 2000

    if len(t_data.split(sep_date_clock))!=2:
        return None
    (date, clock) = t_data.split(sep_date_clock) 

    if (len(clock.split( sep_clock))==3)&(len(date.split(sep_date))==3):
        (day_str,month_str,year_str) = date.split(sep_date)
        (hours_str,minutes_str,seconds_str) = clock.split(sep_clock)
        t=time.mktime(( int(year_str),int(month_str),int(day_str),
                        int(hours_str),int(minutes_str),int(float(seconds_str)),0,0,0))-t_offset
        if is_float:
            return float(t)
        else:
            return int(t)
    else:
        return None


def format_filepath(filepath):
    return ff(filepath)

def ff(filepath):
    return P+filepath+P

def filepathlist_to_filepathstring(filepathlist, sep=','):
    if type(filepathlist)==list:
        if len(filepathlist) == 0:
            return ''
        else:
            filepathstring = ''
            for filepath in filepathlist[:-1]:
                fp = filepath.replace('"','')
                filepathstring += P+fp+P+sep
            filepathstring += P+filepathlist[-1]+P
            return filepathstring
    else:
        fp = filepathlist.replace('"','')
        return P+filepathlist+P

def filepathstring_to_filepathlist(filepathstring, sep=','):
    filepaths=[]
    for filepath in filepathstring.split(sep):
        filepaths.append(P+filepath.strip().replace('"','')+P)
    return filepaths

def dict_to_str(d, intend = 0):
    s = ''
    for key, value in d.items():
        s += intend*" "+"%s: %s\n"%(key, value)
        
    return s
    
