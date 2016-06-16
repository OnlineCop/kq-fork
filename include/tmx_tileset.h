#ifndef __TMX_TILESET
#define __TMX_TILESET

class KTmxTileset {
public:
  KTmxTileset() : imagedata(nullptr) {}

  uint32_t firstgid;
  string name;
  string sourceimage;
  Raster *imagedata;
  vector<KTmxAnimation> animations;
  int width;
  int height;
};

#endif // __TMX_TILESET
