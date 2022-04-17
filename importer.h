//
// Created by Diego Azulay on 13/04/22.
//

#ifndef INTERACTIVEGRAPHICS_IMPORTER_H
#define INTERACTIVEGRAPHICS_IMPORTER_H

#include <string>
#include <vector>

class Importer
{
public:
    static unsigned int loadTextureFromFile(const char *path, const std::string &directory);
    static unsigned int loadCubeMapFromFiles(std::vector<std::string> faces, const std::string &directory);
    static unsigned int loadHDRTextureFromFile(const char *path, const std::string &directory);
};


#endif //INTERACTIVEGRAPHICS_IMPORTER_H
