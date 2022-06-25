#ifndef OSMIMPORTER_NODEWAYHANDLER_H
#define OSMIMPORTER_NODEWAYHANDLER_H

#include "OSMHelper.h"
#include "SplineHandler.h"

// This handler can take care of nodes and ways, we don't need it for now,
// but maybe it is useful later
class NodeWayHandler: public osmium::handler::Handler {
    flatbuffers::FlatBufferBuilder& builder;
    std::vector<flatbuffers::Offset<OSMImporter::Entry>>& entryList;
    std::vector<float>& heightValues;
    long startX;
    long startY;
    double centerX;
    double centerY;

public:
    explicit NodeWayHandler(flatbuffers::FlatBufferBuilder& builder,
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
    void node(const osmium::Node& node);
    void way(const osmium::Way& way);
};

#endif