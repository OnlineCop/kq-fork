#include "settings.h"
using std::map;
using std::string;

KConfig Config;

KConfig::KConfig() {
  data.push({});
}

string keyname(const char* section, const char* key) {
  string k;
  if (section) k += section;
  k += "::";
  if (key) k += key;
  return k;
}
int KConfig::get_config_int(const char* section, const char* key, int defl) {
  auto& top = data.top();
  auto it = top.find( keyname(section, key));
  if (it != top.end()) {
    return it->second;
  } else {
    return defl;
  }
}

void KConfig::set_config_int(const char* section, const char* key, int value) {
  data.top()[keyname(section, key)] = value;
}

void KConfig::push_config_state() {
  data.push({});
}

void KConfig::pop_config_state() {
  data.pop();
}
void KConfig::set_config_file(const char*) {}
