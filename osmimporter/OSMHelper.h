#ifndef OSMIMPORTER_OSMHELPER_H
#define OSMIMPORTER_OSMHELPER_H

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <fstream>

#include <random>

// Allow any format of input files (XML, PBF, ...)
#include <osmium/io/any_input.hpp>

// We want to use the handler interface
#include <osmium/handler.hpp>

// For osmium::apply()
#include <osmium/visitor.hpp>

#include <osmium/relations/relations_manager.hpp>

// For assembling multipolygons
#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_manager.hpp>

// For the NodeLocationForWays handler
#include <osmium/handler/node_locations_for_ways.hpp>

// For the location index. There are different types of indexes available.
// This will work for all input files keeping the index in memory.
#include <osmium/index/map/flex_mem.hpp>

#include "schema_generated.h"
#include "UTM.h"

#define XYZSize 2000

class OSMHelper {
public:
    static inline double BilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y);
    static double getHeightAtPosition(double x, double y, std::vector<float>& heightValues, long startX, long startY);

    static std::vector<OSMImporter::Vec3> convertNode(const osmium::Node& node, std::vector<float>& heightValues, long startX, long startY);
    static std::vector<OSMImporter::Vec3> convertNodes(const osmium::NodeRefList& nodes, std::vector<float>& heightValues, long startX, long startY);

    static double calculateArea(const std::vector<OSMImporter::Vec3>& Polygon);
    static bool isPointInsidePolygon(const std::vector<OSMImporter::Vec3>& Polygon, OSMImporter::Vec3 Point);
};

#endif
