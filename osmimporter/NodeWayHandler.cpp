#include "NodeWayHandler.h"
#include "OSMHelper.h"

// this callback is called for each node (single point) in the data
void NodeWayHandler::node(const osmium::Node& node) {
    OSMImporter::EntityType entityType = OSMImporter::EntityType_ERROR;

    // get tags
    const osmium::TagList &tags = node.tags();

    auto name = this->builder.CreateString("");
    if(tags.has_key("name")){
        name = this->builder.CreateString(tags["name"]);
    }

    bool poi = false;

    // set type
    if (tags.has_tag("natural", "tree")) {
        entityType = OSMImporter::EntityType_NODE_TREE;
    } else if (tags.has_tag("highway", "street_lamp")) {
        entityType = OSMImporter::EntityType_NODE_HIGHWAY_STREET_LAMP;
    } else if (tags.has_tag("amenity", "place_of_worship")) {
        entityType = OSMImporter::EntityType_NODE_POI_RELIGION;
        poi = true;
    } else if (tags.has_tag("historic", "castle")) {
        entityType = OSMImporter::EntityType_NODE_POI_CASTLE;
        poi = true;
    } else if (tags.has_tag("amenity", "arts_centre")) {
        entityType = OSMImporter::EntityType_NODE_POI_ARTS_CENTER;
        poi = true;
    } else if (tags.has_tag("natural", "peak")) {
        entityType = OSMImporter::EntityType_NODE_POI_PEAK;
        poi = true;
    } else {
        return;
    }

    // height is not needed
    int8_t height = 0;

    // write to file
    auto outerRing = OSMHelper::convertNode(node, this->heightValues, this->startX, this->startY);
    auto outerRingVector = this->builder.CreateVectorOfStructs(outerRing);
    auto entry = OSMImporter::CreateEntry(this->builder, entityType, height, name, poi, -1, outerRingVector);
    this->entryList.emplace_back(entry);
}

// this callback is called for all ways (streets, ...) in the data
void NodeWayHandler::way(const osmium::Way& way) {
    OSMImporter::EntityType entityType = OSMImporter::EntityType_ERROR;

    // get tags
    const osmium::TagList& tags = way.tags();

    auto name = this->builder.CreateString("");
    if(tags.has_key("name")){
        name = this->builder.CreateString(tags["name"]);
    }

    // get nodes that make up the way
    const osmium::WayNodeList& nodes = way.nodes();

    // set type

    // streets
    if(tags.has_key("highway") && !tags.has_tag("area", "yes") && !tags.has_tag("highway", "platform")) {
        if(tags.has_tag("highway", "cycleway")) {
            entityType = OSMImporter::EntityType_POLYLINE_HIGHWAY_CYCLEWAY;
        } else if(tags.has_tag("highway", "footway") || tags.has_tag("highway", "pedestrian") ||
                  tags.has_tag("highway", "steps") || tags.has_tag("highway", "path")) {
            entityType = OSMImporter::EntityType_POLYLINE_HIGHWAY_FOOTWAY;
        } else if(tags.has_tag("highway", "primary")) {
            entityType = OSMImporter::EntityType_POLYLINE_HIGHWAY_PRIMARY;
        } else if(tags.has_tag("highway", "secondary")) {
            entityType = OSMImporter::EntityType_POLYLINE_HIGHWAY_SECONDARY;
        } else if(tags.has_tag("highway", "residential") || tags.has_tag("highway", "unclassified")) {
            entityType = OSMImporter::EntityType_POLYLINE_HIGHWAY_RESIDENTIAL;
        } else if(tags.has_tag("highway", "service")) {
            entityType = OSMImporter::EntityType_POLYLINE_HIGHWAY_SERVICE;
        }
    }

    // river / stream
    if(tags.has_key("waterway")) {
        if(tags.has_tag("waterway", "stream")) {
            entityType = OSMImporter::EntityType_POLYLINE_STREAM;
        } else if(tags.has_tag("waterway", "river")) {
            entityType = OSMImporter::EntityType_POLYLINE_RIVER;
        }
    }

    if(entityType == OSMImporter::EntityType_ERROR) {
        return;
    }

    // height is not needed
    int8_t height = 0;

    // convert all points to UTM coordinates
    auto splinePoints = OSMHelper::convertNodes(nodes, this->heightValues, this->startX, this->startY);

    // we need a triangulated mesh, not a vector of points, so calculate it
    SplineHandler::VertexData vertexData = SplineHandler::getMeshFromSplinePoints(splinePoints, entityType, this->heightValues, this->startX, this->startY);

    // write to file
    auto vertexVector = this->builder.CreateVectorOfStructs(vertexData.vertices);
    auto triangleVector = this->builder.CreateVector(vertexData.triangles);
    auto uvVector = this->builder.CreateVectorOfStructs(vertexData.uvs);
    auto entry = OSMImporter::CreateEntry(this->builder, entityType, height, name, false, -1, vertexVector, triangleVector, uvVector);
    this->entryList.emplace_back(entry);
}