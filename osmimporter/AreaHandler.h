#ifndef OSMIMPORTER_AREAHANDLER_H
#define OSMIMPORTER_AREAHANDLER_H

#include "OSMHelper.h"
#include "earcut.hpp"
#include "string"
#include "algorithm"

// This handler is responsible for collecting all node information, joining them
// to ways and merging them into areas with multiple rings
class AreaHandler : public osmium::handler::Handler {
    flatbuffers::FlatBufferBuilder& builder;
    std::vector<flatbuffers::Offset<OSMImporter::Entry>>& entryList;
    std::vector<float>& heightValues;
    long startX;
    long startY;
    double centerX;
    double centerY;

public:
    explicit AreaHandler(flatbuffers::FlatBufferBuilder& builder,
                         std::vector<flatbuffers::Offset<OSMImporter::Entry>>& entryList,
                         std::vector<float>& heightValues,
                         long startX, long startY,
                         double centerX, double centerY):
                         builder(builder), entryList(entryList), heightValues(heightValues) {
        this->startX = startX;
        this->startY = startY;
        this->centerX = centerX;
        this->centerY = centerY;
    }

    void area(const osmium::Area& area);
};

#endif
