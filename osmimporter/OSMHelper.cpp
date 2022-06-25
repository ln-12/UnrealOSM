#include "OSMHelper.h"

// convert a single node to UTM coordinates
std::vector<OSMImporter::Vec3> OSMHelper::convertNode(const osmium::Node& node, std::vector<float>& heightValues,
                                                      long startX, long startY) {
    std::vector<OSMImporter::Vec3> convertedCoordinates;

    double x, y;
    LatLonToUTMXY(node.location().lat(), node.location().lon(), 33, x, y);

    auto z = getHeightAtPosition(x, y, heightValues, startX, startY);

    auto vec = OSMImporter::Vec3(x, y, z);
    convertedCoordinates.emplace_back(vec);

    return convertedCoordinates;
}

// convert a list of nodes to UTM coordinates
std::vector<OSMImporter::Vec3> OSMHelper::convertNodes(const osmium::NodeRefList& nodes, std::vector<float>& heightValues,
                                                       long startX, long startY) {
    std::vector<OSMImporter::Vec3> convertedCoordinates;

    double x, y;

    for(auto node: nodes) {
        LatLonToUTMXY(node.location().lat(), node.location().lon(), 33, x, y);

        auto z = getHeightAtPosition(x, y, heightValues, startX, startY);

        auto vec = OSMImporter::Vec3(x, y, z);
        convertedCoordinates.emplace_back(vec);
    }

    return convertedCoordinates;
}

// source: https://helloacm.com/cc-function-to-compute-the-bilinear-interpolation/
inline double OSMHelper::BilinearInterpolation(double q11, double q12, double q21, double q22, double x1, double x2, double y1, double y2, double x, double y) {
    double x2x1, y2y1, x2x, y2y, yy1, xx1;
    x2x1 = x2 - x1;
    y2y1 = y2 - y1;
    x2x = x2 - x;
    y2y = y2 - y;
    yy1 = y - y1;
    xx1 = x - x1;
    return 1.0 / (x2x1 * y2y1) * (
            q11 * x2x * y2y +
            q21 * xx1 * y2y +
            q12 * x2x * yy1 +
            q22 * xx1 * yy1
    );
}

double OSMHelper::getHeightAtPosition(double x, double y, std::vector<float>& heightValues, long startX, long startY) {
    int x1 = floor(x);
    int x2 = x1 + 1;
    int y1 = floor(y);
    int y2 = y1 + 1;

    if(x2 > (startX + XYZSize) || y2 > (startY + XYZSize)) {
        // we are out of bounds for this tile, so return invalid value
        return 0.0;
    }

    // get the height at the 4 points between which (X,Y) is located
    unsigned long index = (x1 - startX) + XYZSize * (y1 - startY);
    double q11 = 0.0f;
    if (index > 0 && index < heightValues.size()) {
        q11 = heightValues[index];
    } else {
        // TODO some kind of error handling or better fallback
        std::cout << "Index not found: " << (long long)index << " (X: " << x << ", Y: " << y << ")" << std::endl;
    }



    index = (x1 - startX) + XYZSize * (y2 - startY);
    double q12 = 0.0f;
    if (index > 0 && index < heightValues.size()) {
        q12 = heightValues[index];
    } else {
        std::cout << "Index not found: " << (long long)index << " (X: " << x << ", Y: " << y << ")" << std::endl;
    }



    index = (x2 - startX) + XYZSize * (y1 - startY);
    double q21 = 0.0f;
    if (index > 0 && index < heightValues.size()) {
        q21 = heightValues[index];
    } else {
        std::cout << "Index not found: " << (long long)index << " (X: " << x << ", Y: " << y << ")" << std::endl;
    }



    index = (x2 - startX) + XYZSize * (y2 - startY);
    double q22 = 0.0f;
    if (index > 0 && index < heightValues.size()) {
        q22 = heightValues[index];
    } else {
        std::cout << "Index not found: " << (long long)index << " (X: " << x << ", Y: " << y << ")" << std::endl;
    }

    // interpolate value
    return BilinearInterpolation(q11, q12, q21, q22, x1, x2, y1, y2, x, y);
}

// taken from https://github.com/ue4plugins/StreetMap/blob/master/Source/StreetMapRuntime/PolygonTools.h
double OSMHelper::calculateArea(const std::vector<OSMImporter::Vec3>& Polygon) {
    const uint32_t PointCount = Polygon.size();

    double HalfArea = 0.0f;
    uint32_t P = PointCount - 2;
    for (uint32_t Q = 0; Q < PointCount; P = Q++)
    {
        HalfArea += Polygon[P].x() * Polygon[Q].y() - Polygon[Q].x() * Polygon[P].y();
    }

    return std::abs(HalfArea * 0.5);
}

// taken from https://github.com/ue4plugins/StreetMap/blob/master/Source/StreetMapRuntime/PolygonTools.h
bool OSMHelper::isPointInsidePolygon(const std::vector<OSMImporter::Vec3>& Polygon, const OSMImporter::Vec3 Point) {
    const int NumCorners = Polygon.size();
    int PreviousCornerIndex = NumCorners - 1;
    bool bIsInside = false;

    for (int CornerIndex = 0; CornerIndex < NumCorners; CornerIndex++)
    {
        if (((Polygon[CornerIndex].y() < Point.y() && Polygon[PreviousCornerIndex].y() >= Point.y()) || (Polygon[PreviousCornerIndex].y() < Point.y() && Polygon[CornerIndex].y() >= Point.y())) &&
            (Polygon[CornerIndex].x() <= Point.x() || Polygon[PreviousCornerIndex].x() <= Point.x()))
        {
            bIsInside ^= (Polygon[CornerIndex].x() + (Point.y() - Polygon[CornerIndex].y()) / (Polygon[PreviousCornerIndex].y() - Polygon[CornerIndex].y()) * (Polygon[PreviousCornerIndex].x() - Polygon[CornerIndex].x()) < Point.x());
        }

        PreviousCornerIndex = CornerIndex;
    }

    return bIsInside;
}