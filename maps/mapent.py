#!/usr/bin/python
import maptiled
import glob
def luav(v):
    if type(v) == str:
        return '"%s"' % v
    elif type(v) == bytes:
         return '"%s"' % v.decode()
    else:
        return str(v)
    
def luak(k):
    if type(k) == str:
        if k.isalnum():
            return k
    return '["%s"]' % (str(k))

def ento(ent):
    kvs = ['%s=%s' % (luak(k), luav(v)) for k, v in vars(ent).items()]
    return "{%s}" % (",".join(kvs))

def mapo(map):
    return '{%s}' % (",".join(ento(ent) for ent in map.entities))

def fileo(filename):
    with open(filename,'rb') as fd:
        raw = maptiled.decompress(fd.read())
    
    (map, _) = maptiled.SMap.from_data(raw, 0)
    return '%s=%s' % (luak(filename), mapo(map))

print("{%s}" % (",".join([fileo(f) for f in glob.glob("*.map")])))
