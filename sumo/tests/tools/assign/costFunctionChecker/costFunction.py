def constant(edge, weight):
    if edge == "middle" or edge == "left":
        return weight
    return 0

def w(edge, weight):
    if edge == "middle":
        return weight+2
    if edge == "left":
        return weight
    return 1
