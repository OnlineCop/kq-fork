#pragma once

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
