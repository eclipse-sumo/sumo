from collections import defaultdict
from traci import constants

occ = defaultdict(list)
for c, val in constants.__dict__.items():
    if isinstance(val, int):
        occ[val].append(c)

print("Duplicate constant use:")
for val, clist in occ.items():
    if len(clist) > 1:
        print("%s : %s" % (hex(val), ' '.join(sorted(clist))))


print("unused constants:")
for i in range(257):
    if len(occ[i]) == 0:
        print(hex(i))

