#!/usr/bin/python
import sys
import struct
import argparse
import array
import xml.etree.ElementTree as ET

MAX_ENTITIES_PER_MAP = 50
SHADOW_TILE_OFFSET=200

WINDOW_BITS = 12
LENGTH_BITS  = 4
MIN_MATCH = 3
WINDOW_SIZE = (1<<WINDOW_BITS)
MAX_MATCH = MIN_MATCH + (1<<LENGTH_BITS) - 1

def decompress(input):
    buffer = bytearray(WINDOW_SIZE)
    out = bytearray()
    read_ptr = 4
    write_ptr = WINDOW_SIZE - MAX_MATCH
    while read_ptr < len(input):
        (flag,) = struct.unpack('B', input[read_ptr])
        read_ptr += 1
        for bit in range(8):
            if flag & (1 << bit):
                if read_ptr < len(input):
                    out += input[read_ptr]
                    buffer[write_ptr % WINDOW_SIZE] = input[read_ptr]
                    write_ptr += 1
                    read_ptr += 1
            else:
                if read_ptr < len(input)-1:
                    pair = struct.unpack('BB', input[read_ptr:read_ptr+2])
                    read_ptr += 2
                    offset = pair[0] | ((pair[1] & 0xF0) << 4)
                    length = (pair[1] & ((1 << LENGTH_BITS)-1)) + MIN_MATCH
                    for k in range(length):
                        c = buffer[(offset + k) % WINDOW_SIZE]
                        buffer[(write_ptr + k) % WINDOW_SIZE] = c
                        out.extend([c])
                    write_ptr += length
    return out
# Go through a tile map array and make it into an array of Zones
def extract_zones(data, width, height):
    def findone():
        for x in xrange(width):
            for y in xrange(height):
                if get(x,y) > 0:
                    return x,y
        return None
    def get(x,y):
        if x >=0 and y >= 0 and x < width and y < height:
            return data[x + width * y]
        else:
            return None
    def reset(x, y, w=1, h=1):
        for i in xrange(w):
            for j in xrange(h):
                data[x + i + width * (y + j)] = 0
    def same(x, y, w, h):
        t = get(x, y)
        for i in xrange(w):
            for j in xrange(h):
                if get(x + i, y + j) != t:
                    return False
        return True 
    def extend(x, y):
        w = 1
        h = 1
        cw = w
        ch = h
        while same(x, y, cw, h):
            w = cw
            cw += 1
        while same(x, y, w, ch):
            h = ch
            ch += 1
        return (x, y, w, h)
    segs = []
    while True:
        sp = findone()
        if not sp:
            break;
        else:
            (x, y) = sp
            t = get(x, y)
            (x,y,w,h) = extend(x,y)
            reset(x,y,w,h)
            segs.append(Zone(x, y, w, h, "Zone %d" % t))
    return segs    

# make a string from an array maybe with nulls in it.
def nps(d):
    ix = d.find('\0')
    if ix <0:
        return str(d)
    else:
        return str(d[:ix])

# List of all the tilesents names and images
class Tileset:
    def __init__(self, name, source):
        self.name = name
        self.source = source
        self.load()
    def load(self):
        self.xml = ET.parse(self.source)
        root = self.xml.getroot()
        self.tileheight = int(root.get('tileheight'))
        self.tilewidth = int(root.get('tilewidth'))
        image = root.find('image')
        self.width = int(image.get('width'))
        self.height = int(image.get('height'))
    def tile_count(self):
        return self.width / self.tilewidth * self.height / self.tileheight
    def __str__(self):
        return "<Tileset %s, from %s with %d tiles>" % (self.name, self.source, self.tile_count())
    def as_xml(self):
        return ET.Element("tileset",
                          {
                              "firstgid":str(self.first_gid),
                              "name":self.name,
                              "tilewidth":str(self.tilewidth),
                              "tileheight":str(self.tileheight),
                              "source":self.source
                          })
def create_tileset(index):
    name, tsx = [
        ("land", None),
        ("newtown","newtown.tsx"),
        ("castle", "castle.tsx"),
        ("Incave", "cave.tsx"),
        ("village.pcx", None),
        ("mount.pcx", None),
        ("shrine.pcx", None),
        ("fortress.pcx", None),
    ][index]
    if tsx:
        return Tileset(name, tsx)
    else:
        raise Exception("Tileset '%s' has not been defined" % name)

character_tileset = Tileset("uschrs", "uschrs.tsx")
entity_tileset = Tileset("entities", "entities.tsx")
# TODO shadows are in the misc tileset starting at ID200
misc_tileset = Tileset("misc", "misc.tsx")
obstacle_tileset = Tileset("obstacles", "obstacles.tsx")

# make an xml properties element
def mkprops(dict):
      properties = ET.Element("properties")
      for (k,v) in dict.items():
          ET.SubElement(properties, "property", {"name":k, "value":str(v)})
      return properties
# make an xml object element from a marker
# make an xml object from a tuple of (x,y,w,h,index)
def mkzone(z):
    (x, y, w, h, index) = z
    return ET.Element("object",
                    {
                        "name":"Zone %d" % index,
                        "x":str(x * 16),
                        "y":str(y * 16),
                        "width":str(w * 16),
                        "height":str(h * 16)
                    })
# make an xml data element with these tile indexes
def mktiles(ixs):
    data = ET.Element("data", {"encoding":"csv"})
    data.text = ','.join([str(ix) for ix in ixs])
    return data

class SMap:
    @staticmethod
    def from_data(data, offset):
        m = SMap()
        (m.map_no,
         m.zero_zone,
         m.map_mode,
         m.can_save,
         ts,
         m.use_sstone,
         m.can_warp,
         m.extra_byte,
         m.xsize,
         m.ysize,
         m.pmult,
         m.pdiv,
         m.stx,
         m.sty,
         m.warpx,
         m.warpy,
         m.revision,
         m.extra_sdword2) = struct.unpack('<BBBBBBBBIIIIIIIIII', data[:48])
        m.tileset = create_tileset(ts)
        m.song_file = nps(data[48:64])
        m.map_desc = nps(data[64:104])
        offset = 104
        if m.revision >=1:
            (count,) = struct.unpack_from('<H', data, offset)
            m.markers = []
            offset += 2
            for i in xrange(count):
                marker, offset = Marker.from_data(data, offset)
                m.markers.append(marker)
            if m.revision >= 2:
                (count,) = struct.unpack_from('<H', data, offset)
                m.bounds = []
                offset += 2
                for i in xrange(count):
                    bounds, offset = Bounds.from_data(data, offset)
                    m.bounds.append(bounds)
            else:
                m.bounds = []
        else:
            m.markers = m.bounds = []
        # Entities
        m.entities = []
        for i in xrange(MAX_ENTITIES_PER_MAP):
            (ent, offset) = Entity.from_data(data, offset)
            if ent.eid > 0:
                m.entities.append(ent)
        for (i, e) in enumerate(m.entities):
            e.name = "Entity %d" % i
        # Tilemaps for each layer
        m.tileset.first_gid = 1
        entity_tileset.first_gid = m.tileset.first_gid + m.tileset.tile_count()
        misc_tileset.first_gid = entity_tileset.first_gid + entity_tileset.tile_count()
        obstacle_tileset.first_gid = misc_tileset.first_gid + misc_tileset.tile_count()
        m.umap = array.array('H')
        for i in xrange(m.xsize * m.ysize):
            (c,) = struct.unpack_from('H', data, offset)
            m.umap.append(c + m.tileset.first_gid)
            offset+=2
        m.bmap = array.array('H')
        for i in xrange(m.xsize * m.ysize):
            (c,) = struct.unpack_from('H', data, offset)
            m.bmap.append(c + m.tileset.first_gid)
            offset+=2
        m.fmap = array.array('H')
        for i in xrange(m.xsize * m.ysize):
            (c,) = struct.unpack_from('H', data, offset)
            m.fmap.append(c + m.tileset.first_gid)
            offset+=2
        zmap = array.array('H')
        for i in xrange(m.xsize * m.ysize):
            (c,) = struct.unpack_from('B', data, offset)
            zmap.append(c)
            offset+=1
        m.zones = extract_zones(zmap, m.xsize, m.ysize)
        m.shmap = array.array('H')
        for i in xrange(m.xsize * m.ysize):
            (c,) = struct.unpack_from('B', data, offset)
            m.shmap.append(c + misc_tileset.first_gid + SHADOW_TILE_OFFSET)
            offset+=1
        m.omap = array.array('I')
        oconv = [0, obstacle_tileset.first_gid,
                 (obstacle_tileset.first_gid + 1),
                 (obstacle_tileset.first_gid + 2),
                 (obstacle_tileset.first_gid + 3),
                 (obstacle_tileset.first_gid + 4)
        ]
        for i in xrange(m.xsize * m.ysize):
            (c,) = struct.unpack_from('B', data, offset)
            if c >= len(oconv) or c <= 0:
                c = 0
            m.omap.append(oconv[c])
            offset+=1
        return m, offset
    def as_xml(self):
        def entity_layer():
            objectgroup = ET.SubElement(root, "objectgroup",
                            {
                                "name":"entities",
                                "color":"#FF00FF",
                            })
            objectgroup.extend([ent.as_xml() for ent in self.entities])
        def bounds_layer():
            # bounds
            objectgroup = ET.SubElement(root, "objectgroup",
                                        {
                                            "name":"bounds",
                                            "color":"blue",
                                            "visible":"0"
                                        })
            objectgroup.extend([m.as_xml() for m in self.bounds])

        root = ET.Element("map",
                          {
                              "version":"1.0",
                              "orientation":"orthogonal",
                              "width":str(self.xsize),
                              "height":str(self.ysize),
                              "tilewidth":"16",
                              "tileheight":"16",
                              "backgroundcolor":"#0E0E0E"
                          })
        root.append(mkprops({
            "zero_zone":self.zero_zone,
            "can_save": self.can_save,
            "map_mode": self.map_mode,
            "use_sstone": self.use_sstone,
            "can_warp": self.can_warp,
            "pmult": self.pmult,
            "pdiv": self.pdiv,
            "stx": self.stx,
            "sty": self.sty,
            "warpx": self.warpx,
            "warpy": self.warpy
        }))
        root.extend([self.tileset.as_xml(), entity_tileset.as_xml(), misc_tileset.as_xml(), obstacle_tileset.as_xml()])
        layer = ET.SubElement(root, "layer",
                              {
                                  "name":"map",
                                  "width":str(self.xsize),
                                  "height":str(self.ysize)
                              })
        layer.append(mktiles(self.umap))

        if self.map_mode in [1,3]:
            entity_layer()
        
        layer = ET.SubElement(root, "layer",
                              {
                                  "name":"bmap",
                                  "width":str(self.xsize),
                                  "height":str(self.ysize)
                              })
        layer.append(mktiles(self.bmap))
        if self.map_mode in [0,2,4,5]:
            entity_layer()
        layer = ET.SubElement(root, "layer",
                              {
                                  "name":"fmap",
                                  "width":str(self.xsize),
                                  "height":str(self.ysize)
                              })
        layer.append(mktiles(self.fmap))
        # Shadows
        layer = ET.SubElement(root, "layer",
                              {
                                  "name":"shadows",
                                  "width":str(self.xsize),
                                  "height":str(self.ysize),
                                  "opacity":"0.5"
                              })
        layer.append(mktiles(self.shmap))
        # Obstacles
        layer = ET.SubElement(root, "layer",
                              {
                                  "name":"obstacles",
                                  "width":str(self.xsize),
                                  "height":str(self.ysize),
                                  "opacity":"0.5"
                              })
        layer.append(mktiles(self.omap))
        # Zones
        objectgroup = ET.SubElement(root, "objectgroup",
                             {
                                 "name":"zones",
                                 "color":"green"
                             })
        objectgroup.extend([zone.as_xml() for zone in self.zones])
        # markers
        objectgroup = ET.SubElement(root, "objectgroup",
                                    {
                                        "name":"markers",
                                        "color":"yellow"
                                    })
        objectgroup.extend([m.as_xml() for m in self.markers])
        # Bounds
        bounds_layer()
        return root

class Entity:
    _sfmt='<BBHHHHBBBBBBBBBBBBBBBBHIBBBB60s'
    @staticmethod
    def from_data(data, offset):
        ent = Entity()
        (ent.chrx,
         pad,
         ent.x,
         ent.y,
         ent.tilex,
         ent.tiley,
         ent.eid,
         ent.active,
         ent.facing,
         ent.moving,
         ent.movcnt,
         ent.framectr,
         ent.movemode,
         ent.obsmode,
         ent.delay,
         ent.delayctr,
         ent.speed,
         ent.scount,
         ent.cmd,
         ent.sidx,
         ent.extra,
         ent.chasing,
         dummy,
         ent.cmdnum,
         ent.atype,
         ent.snapback,
         ent.facehero,
         ent.transl,
         sc) = struct.unpack_from(Entity._sfmt, data, offset)
        ent.script = nps(sc)
        ent.name="Entity" 
        offset += struct.calcsize(Entity._sfmt)
        return ent, offset
    def as_xml(self):
        etile = entity_tileset.first_gid + 12 * self.chrx
        obj = ET.Element("object",
                         {
                             "name":self.name,
                             "x":str(self.tilex * 16),
                             "y":str(self.tiley * 16),
                             "width": "16",
                             "height":"16",
                             "type":"entity",
                             "gid":str(etile)
                         })
        obj.append(mkprops({
            "eid":self.eid,
            "chrx":self.chrx,
            "active":self.active,
            "facing":self.facing,
            "moving":self.moving,
            "movcnt":self.movcnt,
            "framectr":self.framectr,
            "movemode":self.movemode,
            "obsmode":self.obsmode,
            "delay":self.delay,
            "delayctr":self.delayctr,
            "speed":self.speed,
            "scount":self.scount,
            "cmd":self.cmd,
            "sidx":self.sidx,
            "chasing":self.chasing,
            "cmdnum":self.cmdnum,
            "atype":self.atype,
            "snapback":self.snapback,
            "facehero":self.facehero,
            "transl":self.transl,
            "script":self.script
        }))
        return obj
class Zone:
    def __init__(self, x, y, w, h, name):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.name = name
    def as_xml(self):
        return ET.Element("object",
            {
                "name":self.name,
                "x":str(self.x * 16),
                "y":str(self.y * 16),
                "width":str(self.w * 16),
                "height":str(self.h * 16)
            })

class Marker:
    _sfmt='<32shh'
    @staticmethod
    def from_data(data, offset):
        m = Marker()
        (n, m.x, m.y) = struct.unpack_from(Marker._sfmt, data, offset)
        m.name = nps(n)
        offset += struct.calcsize(Marker._sfmt)
        return m, offset
    def __init__(self):
        self.x = 0
        self.y = 0
        self.name=''
    def as_xml(self):
        return ET.Element("object",
            {
                "name":self.name,
                "x":str(self.x * 16),
                "y":str(self.y * 16),
                "width": "16",
                "height":"16",
                "type":"marker"
            })
    def __str__(self):
        return "<%s:%d,%d>" % (self.name, self.x, self.y)
class Bounds:
    _sfmt = '<5h'
    @staticmethod
    def from_data(data, offset):
        bounds = Bounds()
        (bounds.left, bounds.top, bounds.right, bounds.bottom, bounds.btile) = struct.unpack_from(Bounds._sfmt, data, offset)
        offset += struct.calcsize(Bounds._sfmt)
        return bounds, offset
    def __init__(self):
        self.left = self.top = 0
        self.right = self.bottom = 0
        self.btile = 0
    def __str__(self):
        return "<(%d,%d)x(%d,%d):%d>" % (self.left, self.top, self.right, self.bottom, self.btile)
    def as_xml(self):
        obj = ET.Element("object",
                      {
                          "name":"bounds",
                          "x":str(self.left * 16),
                          "y":str(self.top * 16),
                          "width": str(16 * (self.right - self.left + 1)),
                          "height":str(16 * (self.bottom - self.top + 1)),
                          "type":"bounds"
                      })
        obj.append(mkprops({"btile":str(self.btile)}))
        return obj


parser = argparse.ArgumentParser()
parser.add_argument('input_file', type=argparse.FileType('rb'), help="Input map file for conversion")
parser.add_argument('-o', '--output', type=argparse.FileType('w'), default=sys.stdout, help="Output tmx file, default standard output")
args = parser.parse_args()
fd = args.input_file
raw = fd.read()
fd.close()
(magic,) = struct.unpack('<I', raw[:4])
data = decompress(raw)
(m,_) = SMap.from_data(data, 0)
ET.ElementTree(m.as_xml()).write(args.output, encoding="UTF-8", xml_declaration=True)
args.output.close()
