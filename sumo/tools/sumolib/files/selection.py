

def read(file, lanes2edges=True):
    ret = {}
    fd = open(file)
    for line in fd:
        vals = line.strip().split(":")
        if lanes2edges and vals[0]=="lane":
            vals[0] = "edge"
            vals[1] = vals[1][:vals[1].rfind("_")]
        if vals[0] not in ret:
            ret[vals[0]] = set()
        ret[vals[0]].add(vals[1])
    fd.close()
    return ret
    
def write(fdo, entries):
    for t in entries:
        writeTyped(fdo, t, entries[t])

def writeTyped(fdo, typeName, entries):
    for e in entries:
        fdo.write("%s:%s" % (typeName, entries))                          
