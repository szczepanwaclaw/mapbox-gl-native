#pragma once

#include <mbgl/util/constants.hpp>
#include <mbgl/util/geo.hpp>

#include <array>
#include <vector>
#include <string>
#include <cstdint>

namespace mbgl {

class Tileset {
public:
    std::vector<std::string> tiles;
    uint8_t minZoom = 0;
    uint8_t maxZoom = 22;
    std::string attribution;

    // TileJSON also includes center, zoom, and bounds, but they are not used by mbgl.
};

} // namespace mbgl
