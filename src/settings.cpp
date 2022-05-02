#include "settings.h"
#include <fstream>

using std::ifstream;
using std::map;
using std::string;

KConfig Config;

KConfig::KConfig()
{
    data.push({});
}

static string keyname(const char* section, const char* key)
{
    string k;
    if (section)
        k += section;
    k += "::";
    if (key)
        k += key;
    return k;
}
int KConfig::get_config_int(const char* section, const char* key, int defl)
{
    auto& top = data.top();
    auto it = top.find(keyname(section, key));
    if (it != top.end())
    {
        return it->second;
    }
    else
    {
        return defl;
    }
}

void KConfig::set_config_int(const char* section, const char* key, int value)
{
    data.top().emplace(keyname(section, key), value);
}

void KConfig::push_config_state()
{
    data.push({});
}

void KConfig::pop_config_state()
{
    data.pop();
}
static string strip(string s)
{
    auto l = s.find_first_not_of(" \t");
    if (l == string::npos)
    {
        return string {};
    }
    auto r = s.find_last_not_of(" \t");
    return s.substr(l, 1 + r - l);
}

void KConfig::set_config_file(const char* filename)
{
    ifstream is(filename);
    string line;
    string section;
    while (is)
    {
        std::getline(is, line);
        line = strip(line);
        if (!line.empty())
        {
            if (line.front() == '[' && line.back() == ']')
            {
                // section
                section = line.substr(1, line.size() - 1);
            }
            else
            {
                auto pos = line.find('=');
                if (pos != string::npos)
                {
                    auto key = strip(line.substr(0, pos));
                    auto val = strip(line.substr(pos + 1));
                    int iv = std::stoi(val);
                    set_config_int(section.c_str(), key.c_str(), iv);
                }
            }
        }
    }
}
