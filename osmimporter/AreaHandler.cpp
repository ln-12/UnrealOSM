#include "AreaHandler.h"

// these templates are needed for earcut to access out data structure
// see: https://github.com/mapbox/earcut.hpp#usage
namespace mapbox {
    namespace util {
        template <>
        struct nth<0, OSMImporter::Vec3> {
            inline static auto get(const OSMImporter::Vec3& t) {
                return t.x();
            };
        };
        template <>
        struct nth<1, OSMImporter::Vec3> {
            inline static auto get(const OSMImporter::Vec3& t) {
                return t.y();
            };
        };
    }
};

// this callback is called by osmium::apply for each area in the data that matches our filters
void AreaHandler::area(const osmium::Area& area) {
    try {
        // get all tags
        const osmium::TagList &tags = area.tags();

        // output general information
        /*
        std::cout << tags["name"] << "\n";

        std::cout << "\tID: " << area.orig_id() << "\n";

        std::cout << "\tMultipolygon: ";
        if(area.is_multipolygon()) {
            std::cout << "yes\n";
        } else {
            std::cout << "no\n";
        }

        std::cout << "\tFrom way: ";
        if(area.from_way()) {
            std::cout << "yes\n";
        } else {
            std::cout << "no\n";
        }

        std::cout << "\tTags:\n";
        for (const osmium::Tag &tag : area.tags()) {
            std::cout << "\t\t" << tag << "\n";
        }

        std::cout << "\tOuter Rings:\n";
        for(const osmium::OuterRing& outr : area.outer_rings()) {
            std::cout << "\t\tSize:" << outr.size() << "\n";

            std::cout << "\t\tInner Rings:\n";
            for(const osmium::InnerRing& ir : area.inner_rings(outr)) {
                std::cout << "\t\t\tSize: " << ir.size() << " ";
            }

            std::cout << "\n";
        }
        */

        auto name = this->builder.CreateString("");
        if(tags.has_key("name")){
            std::string StringIn = tags["name"];
            std::string StringOut = "";

            for(auto& Char : StringIn){
                char nextChar[1];
                nextChar[0] = Char;

                if(strchr(nextChar, 'Ä')){
                    StringOut.push_back('A');
                    StringOut.push_back('e');
                } else if(strchr(nextChar, 'ä')){
                    StringOut.push_back('a');
                    StringOut.push_back('e');
                } else if(strchr(nextChar, 'Ü')){
                    StringOut.push_back('U');
                    StringOut.push_back('e');
                } else if(strchr(nextChar, 'ü')){
                    StringOut.push_back('u');
                    StringOut.push_back('e');
                } else if(strchr(nextChar, 'Ö')){
                    StringOut.push_back('O');
                    StringOut.push_back('e');
                } else if(strchr(nextChar, 'ö')){
                    StringOut.push_back('o');
                    StringOut.push_back('e');
                } else if(strchr(nextChar, 'ß')){
                    StringOut.push_back('s');
                    StringOut.push_back('s');
                } else if(!isalpha(Char) && !isdigit(Char) && Char != ' ' && Char != '-'){
                    // skip faulures after ä, ö, ü and ß
                } else{
                    StringOut.push_back(Char);
                }
            }
            //std::cout << "String: " << StringOut << "\t" << StringIn << std::endl;
            //if(strchr(StringIn.c_str(), 'ä') != 0){
            //    std::cout << "enthält ein ä" << std::endl;
            //}
            name = this->builder.CreateString(StringOut);
        }

        int holeStartId = -1;

        bool poi = false;
        
        if(tags.has_tag("amenity", "place_of_worship")) {
            poi = true;
        } else if(tags.has_tag("historic", "castle")) {
            poi = true;
        } else if(tags.has_tag("amenity", "arts_center")) {
            poi = true;
        } else if(tags.has_tag("building", "university")) {
            poi = true;
        }

        // check is we have some kind of building
        bool isBuilding = tags.has_key("building");

        // specify the type of the polygon and its height
        OSMImporter::EntityType entityType = OSMImporter::EntityType_ERROR;
        int8_t height = 0;

        if (isBuilding) {
            // when it is a building we use the polygon as outer ring for the roof
            entityType = OSMImporter::EntityType_POLYGON_BUILDING_ROOF;

            // parse height value
            const char *heightString = tags["height"];
            if (heightString && strcmp(heightString, "0") != 0) {
                // height is the most accurate tag, so we prefer it
                long numericLevels = strtol(heightString, nullptr, 10);
                height = numericLevels;
            } else {
                // if it is not given, we can look at the number of levels
                const char *levels = tags["building:levels"];
                if (levels) {
                    // take the level count * 4 (meters) as height
                    long numericLevels = strtol(levels, nullptr, 10);
                    height = numericLevels * 4;
                } else {
                    // no information about the height is given, so we assume 8m (= 2 levels) as fallback
                    height = 8;
                }
            }
        } else {
            // if it is no building, set the according tag
            // height doesn't matter here
            if(tags.has_key("highway") || tags.has_tag("amenity", "parking") ) {
                entityType = OSMImporter::EntityType_POLYGON_HIGHWAY;
                height = 0;
            } else if(tags.has_tag("natural", "water") || tags.has_tag("leisure", "swimming_pool")) {
                entityType = OSMImporter::EntityType_POLYGON_WATER;
                height = 0;
            } else if(tags.has_tag("natural", "wood") || tags.has_tag("landuse", "forest")) {
                entityType = OSMImporter::EntityType_POLYGON_FOREST;
                height = 0;
            } else if(tags.has_tag("landuse", "grass")) {
                entityType = OSMImporter::EntityType_POLYGON_GRASS;
                height = 0;
            } else {
                entityType = OSMImporter::EntityType_ERROR;
                height = 0;
            }
        }

        // rings of outer / inner polygons
        std::vector<OSMImporter::Vec3> outerRing;
        std::vector<std::vector<OSMImporter::Vec3>> innerRings;

        // parse all rings
        for (const auto& item : area) {
            if (item.type() == osmium::item_type::outer_ring) {
                auto &ring = static_cast<const osmium::OuterRing&>(item);
                auto newPoints = OSMHelper::convertNodes(ring, this->heightValues, this->startX, this->startY);
                outerRing.insert(std::end(outerRing), std::begin(newPoints), std::end(newPoints));
            } else if (item.type() == osmium::item_type::inner_ring) {
                auto &ring = static_cast<const osmium::InnerRing&>(item);
                auto newPoints = OSMHelper::convertNodes(ring, this->heightValues, this->startX, this->startY);
                innerRings.push_back(newPoints);
            }
        }

        holeStartId = outerRing.size();

        // if we have a forest or lawn we need to randomly plant some vegetation
        if(entityType == OSMImporter::EntityType_POLYGON_FOREST || entityType == OSMImporter::EntityType_POLYGON_GRASS) {
            // initialize bounding box
            double minX = outerRing[0].x();
            double minY = outerRing[0].y();
            double maxX = outerRing[0].x();
            double maxY = outerRing[0].y();

            // calculate AABB
            for (auto point : outerRing) {
                if (point.x() < minX) {
                    minX = point.x();
                } else if (point.x() > maxX) {
                    maxX = point.x();
                }

                if (point.y() < minY) {
                    minY = point.y();
                } else if (point.y() > maxY) {
                    maxY = point.y();
                }
            }

            // calculate the area to decide how many vegetation we have to plant
            float polygonArea = OSMHelper::calculateArea(outerRing);

            // scaling factor
            float density = 2.0f;

            // initialize random engine
            std::uniform_real_distribution<float> urdX(minX, maxX);
            std::uniform_real_distribution<float> urdY(minY, maxY);
            std::random_device rd;
            std::default_random_engine re(rd());

            // locations where one entity of vegetation should be planted
            std::vector<OSMImporter::Vec3> locations;

            unsigned int numberOfEntitites = polygonArea / 1000000 * density;

            for (unsigned int i = 0; i < numberOfEntitites; i++) {

                while (true) {
                    float x = urdX(re);
                    float y = urdY(re);

                    // check if it is inside the outer polygon
                    if (OSMHelper::isPointInsidePolygon(outerRing, OSMImporter::Vec3(x, y, 0.0f))) {
                        bool skip = false;

                        // check is it is NOT inside an inner polygon
                        for(const auto& innerRing: innerRings) {
                            if (OSMHelper::isPointInsidePolygon(innerRing, OSMImporter::Vec3(x, y, 0.0f))) {
                                skip = true;
                                break;
                            }
                        }

                        if(skip) {
                            continue;
                        }

                        // now we simply have to get the right height value for that location
                        double z = OSMHelper::getHeightAtPosition(x, y, this->heightValues, this->startX, this->startY);

                        locations.emplace_back(x, y, z);

                        break;
                    }
                }
            }

            // write the resulting entities to our file
            auto locationVector = this->builder.CreateVectorOfStructs(locations);
            auto entry = OSMImporter::CreateEntry(this->builder, entityType, height, name, poi, holeStartId, locationVector);
            this->entryList.emplace_back(entry);

            return;
        }

        // ##### EARCUT #####
        // for details, again, see https://github.com/mapbox/earcut.hpp#usage

        // The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
        // data won't have more than 65536 vertices.
        using N = uint32_t;

        // Create array
        std::vector<std::vector<OSMImporter::Vec3>> polygon;

        // Fill polygon structure with actual data. Any winding order works.
        // The first polyline defines the main polygon.
        polygon.push_back(outerRing);

        // Following polylines define holes.
        for(const auto& innerRing: innerRings) {
            polygon.push_back(innerRing);
        }

        // Run tessellation
        // Returns array of indices that refer to the vertices of the input polygon.
        // e.g: the index 6 would refer to {25, 75} in this example.
        // Three subsequent indices form a triangle. Output triangles are clockwise.
        std::vector<N> indices = mapbox::earcut<N>(polygon);

        // create the face triangles from the indices
        std::vector<N> triangles;
        for (N n = 1; n < indices.size(); n += 3) {
            triangles.push_back(indices[n - 1]);
            triangles.push_back(indices[n]);
            triangles.push_back(indices[n + 1]);
        }

        // append all vertices to one vector
        for(auto innerRing: innerRings) {
            outerRing.insert(std::end(outerRing), std::begin(innerRing), std::end(innerRing));
        }

        // values for calculating UV mappings
        // we use double to keep higher precision as long as possible
        // (and because we don't care about RAM usage ¯\_(ツ)_/¯ )
        double firstX = 0.0;
        double firstY = 0.0;
        double alpha = 0.0;

        bool first = true;
        bool second = true;

        // differentiate between roof and wall UVs
        std::vector<OSMImporter::Vec2> roofUVs;
        std::vector<OSMImporter::Vec2> uvs;

        if(isBuilding) {
            // if we have a building, all vertices should have the same height
            // so that no part of th building is floating above the ground
            float minHeight = outerRing[0].z();

            // determine minimum height
            for(unsigned int k = 1; k < outerRing.size(); k++) {
                if(outerRing[k].z() < minHeight) {
                    minHeight = outerRing[k].z();
                }
            }

            // set that height for all vertices
            for(unsigned int k = 0; k < outerRing.size(); k++) {
                outerRing[k] = OSMImporter::Vec3(outerRing[k].x(), outerRing[k].y(), minHeight);
            }

            // these values are needed to get the coordinate, which runs around the building so that
            // at corners the texture is not cut off
            double lastX = outerRing[0].x();
            double lastY = outerRing[0].y();
            double sum = 0.0;

            for(auto point: outerRing) {
                // ine the first run, we have to set up our variables
                if (first) {
                    first = false;
                    firstX = point.x();
                    firstY = point.y();
                    roofUVs.emplace_back(OSMImporter::Vec2(0.0f, 0.0f));
                } else {
                    // the roof textures should be aligned to a wall of the building instead
                    // of the world coordinate axis -> we need to rotate it
                    if (second) {
                        second = false;

                        // calculate angle of first wall
                        alpha = atan((point.y() - firstY) / (point.x() - firstX));
                    }

                    // apply angle to all coordinates
                    auto newX = (float) (cos(alpha) * (point.x() - firstX) + sin(alpha) * (point.y() - firstY));
                    auto newY = (float) (cos(alpha) * (point.y() - firstY) - sin(alpha) * (point.x() - firstX));

                    // rotate coordinates according to calculated angle so that
                    // the texture aligns with the first edge in the vertex list
                    roofUVs.emplace_back(OSMImporter::Vec2(
                            (float) newX,
                            (float) newY
                    ));
                }

                // add uv coordinates for walls (lower ring)
                double distance = sqrt(pow(lastX - point.x(), 2) + pow(lastY - point.y(), 2));
                sum += distance;

                uvs.emplace_back(OSMImporter::Vec2((float) sum, 0.0f));

                lastX = point.x();
                lastY = point.y();
            }

            // we need the sign of the area to determine the winding order of the wall triangles
            float calculatedArea = OSMHelper::calculateArea(outerRing);

            std::vector<OSMImporter::Vec3> roofVertices;
            // we will copy the outerRing, so we need that many reserved places
            roofVertices.reserve(outerRing.size());

            std::vector<N> wallTriangles;
            // reset arrays as we already used their content
            // we need two triangles (= 6 vertices) per wall
            wallTriangles.resize(outerRing.size() * 6);

            // build walls
            int originalSize = outerRing.size();
            for (int n = 0; n < originalSize; n++) {
                // copy vertices to the top of the building
                roofVertices.emplace_back(OSMImporter::Vec3(outerRing[n].x(), outerRing[n].y(), outerRing[n].z() + (float) height));

                // add walls for all vertices
                //     n     1     2     3
                //   --O-----O-----O-----O--
                //     |\    |\    |\    |
                // ... |  \  |  \  |  \  | ...
                //     |    \|    \|    \|
                //   --O-----O-----O-----O--
                //    2*n   n+1   n+2   n+3
                // first triangle: 1-2-(n+1)
                // second triangle: 2-(n+2)-(n+1)

                // depending on the winding of the polygon, we need to respect the
                // winding order for our walls
                if (calculatedArea > 0) {
                    // first triangle
                    wallTriangles.push_back(n);
                    wallTriangles.push_back(n + 1);
                    wallTriangles.push_back(n + originalSize);

                    // second triangle
                    wallTriangles.push_back(n + 1);
                    wallTriangles.push_back(n + originalSize + 1);
                    wallTriangles.push_back(n + originalSize);
                }
                else {
                    // first triangle
                    wallTriangles.push_back(n + originalSize);
                    wallTriangles.push_back(n + 1);
                    wallTriangles.push_back(n);

                    // second triangle
                    wallTriangles.push_back(n + originalSize);
                    wallTriangles.push_back(n + originalSize + 1);
                    wallTriangles.push_back(n + 1);
                }

                uvs.emplace_back(uvs[n].x(), -height);
            }

            // write the roof to our file
            auto vertexVector = this->builder.CreateVectorOfStructs(roofVertices);
            auto triangleVector = this->builder.CreateVector(triangles);
            auto uvVector = this->builder.CreateVectorOfStructs(roofUVs);
            auto entry = OSMImporter::CreateEntry(this->builder, OSMImporter::EntityType_POLYGON_BUILDING_ROOF, height, name, poi, holeStartId, vertexVector, triangleVector, uvVector);
            this->entryList.emplace_back(entry);

            // write the walls to our file
            outerRing.insert(std::end(outerRing), std::begin(roofVertices), std::end(roofVertices));
            vertexVector = this->builder.CreateVectorOfStructs(outerRing);
            triangleVector = this->builder.CreateVector(wallTriangles);
            uvVector = this->builder.CreateVectorOfStructs(uvs);
            entry = OSMImporter::CreateEntry(this->builder, OSMImporter::EntityType_POLYGON_BUILDING_WALL, height, name, poi, holeStartId, vertexVector, triangleVector, uvVector);
            this->entryList.emplace_back(entry);
        } else {
            // if it is a normal area (lake, ...) things are much simpler
            for(auto point: outerRing) {
                if (first) {
                    first = false;
                    firstX = point.x();
                    firstY = point.y();
                    uvs.emplace_back(OSMImporter::Vec2((float) firstX, (float) firstY));
                } else {
                    // we don't care about the angle of the other textures
                    uvs.emplace_back(OSMImporter::Vec2(
                            (float) (firstX - point.x()),
                            (float) (firstY - point.y()))
                    );
                }
            }

            

            // just add all points to our file
            auto vertexVector = this->builder.CreateVectorOfStructs(outerRing);
            auto triangleVector = this->builder.CreateVector(triangles);
            auto uvVector = this->builder.CreateVectorOfStructs(uvs);
            auto entry = OSMImporter::CreateEntry(this->builder, entityType, height, name, poi, holeStartId, vertexVector, triangleVector, uvVector);
            this->entryList.emplace_back(entry);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }
}