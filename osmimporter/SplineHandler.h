#ifndef OSMIMPORTER_SPLINEHANDLER_H
#define OSMIMPORTER_SPLINEHANDLER_H

#include "OSMHelper.h"

using N = uint32_t;

class SplineHandler {
public:
    struct VertexData {
        std::vector<OSMImporter::Vec3> vertices;
        std::vector<N> triangles;
        std::vector<OSMImporter::Vec2> uvs;
    };

    static VertexData getMeshFromSplinePoints(std::vector<OSMImporter::Vec3> splinePoints, OSMImporter::EntityType entityType,
                                              std::vector<float>& heightValues, long startX, long startY);
private:
    //This is about 115°. Everything above this is pretty heavily f*cked up.
    constexpr static double gammaLimit = 2;
};

#endif //OSMIMPORTER_SPLINEHANDLER_H
