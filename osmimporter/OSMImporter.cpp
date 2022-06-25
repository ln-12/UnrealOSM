#include "AreaHandler.h"
#include "NodeWayHandler.h"
#include "OSMHelper.h"
#include <fstream>
#include "schema_generated.h"

// The type of index used. This must match the include file above
using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>;

// The location handler always depends on the index type
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

/* this check can be used when the map data covers a larger area than
   we want to process, e.g. when the input is the saxony map

// these two locations specify the boundary in which we want to process data
osmium::Location location1 { 13.3131, 50.8962 };
osmium::Location location2 { 13.3718, 50.9325 };

bool isInsideRect = false;

 for (const auto &node : nodes) {
    if (node.location().lon() > this->location1.lon() && node.location().lon() < this->location2.lon() &&
        node.location().lat() > this->location1.lat() && node.location().lat() < this->location2.lat()) {
        isInsideRect = true;
        break;
    }
}

if(!isInsideRect) {
    return;
}
*/

void readFile(const char* filename, std::vector<std::string>& lines) {
    lines.clear();
    std::ifstream file(filename);
    std::string s;
    while (std::getline(file, s)) {
        lines.push_back(s);
    }
}

inline bool fileExists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void split(const std::string& s, char delim, std::vector<std::string>& elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <OSM FILE> <XYZ FILE> <OUTPUT FILE>\n";
        std::exit(1);
    }

    std::string osmFile = argv[1];
    std::string heightFile = argv[2];
    std::string outputFile = argv[3];

    if(!fileExists(osmFile)) {
        std::cout << "OSM FILE cannot be opened!" << std::endl;
        std::cout << "Usage: " << argv[0] << " <OSM FILE> <XYZ FILE> <OUTPUT FILE>\n";
        std::exit(1);
    }

    if(!fileExists(heightFile) && std::string(argv[2]) != "-") {
        std::cout << "XYZ FILE cannot be opened!" << std::endl;
        std::cout << "Usage: " << argv[0] << " <OSM FILE> <XYZ FILE> <OUTPUT FILE>\n";
        std::exit(1);
    }

    try {
        std::cout << "Initializing..." << std::endl;

        // create and open the output file
        std::ofstream fileHandle;
        fileHandle.open(outputFile);

        std::cout << "Reserving memory..." << std::endl;

        std::vector<std::string> rawHeightData;
        std::vector<float> heightData;

        // reserve space to avoid to many memory allocations
        rawHeightData.reserve(XYZSize * XYZSize);
        heightData.resize(XYZSize * XYZSize, 0.0f);

        long startX = 0L;
        long startY = 0L;

        if(std::string(argv[2]) != "-"){
            std::cout << "Reading height map..." << std::endl;

            readFile(heightFile.c_str(), rawHeightData);

            std::cout << "Parsing height map..." << std::endl;

            // get first coordinates to calculate offset for index
            std::vector<std::string> values;
            split(rawHeightData[0], ' ', values);

            startX = std::stol(values[0]);
            startY = std::stol(values[1]);

            // parse height values
            for(const auto& line: rawHeightData) {
                std::vector<std::string> values;
                split(line, ' ', values);

                long x = std::stol(values[0]);
                long y = std::stol(values[1]);
                float z = std::stof(values[2]);

                long index = (x - startX) + XYZSize * (y - startY);

                heightData[index] = z;
            }
        }

        std::cout << "Reading map file..." << std::endl;

        // some of this code is taken from: https://github.com/osmcode/libosmium/blob/master/examples/osmium_area_test.cpp

        // Build up a serialized buffer algorithmically:
        flatbuffers::FlatBufferBuilder builder;
        std::vector<flatbuffers::Offset<OSMImporter::Entry>> entryList;

        // create the input file (.osm or .osm.pbf)
        osmium::io::File input_file{ osmFile };

        // Configuration for the multipolygon assembler. Here the default settings
        // are used, but you could change multiple settings.
        osmium::area::Assembler::config_type assembler_config;

        // Set up a custom filter. This will be used to only build
        // areas with matching tags.
        osmium::TagsFilter filter{ false };
        filter.add_rule(true, "building");
        filter.add_rule(true, "area", "yes");
        filter.add_rule(true, "amenity", "parking");
        filter.add_rule(true, "natural", "water");
        filter.add_rule(true, "leisure", "swimming_pool");
        filter.add_rule(true, "natural", "wood");
        filter.add_rule(true, "landuse", "forest");
        filter.add_rule(true, "landuse", "grass");

        // POI tags
        filter.add_rule(true, "amenity", "place_of_worship");
        filter.add_rule(true, "amenity", "arts_centre");
        filter.add_rule(true, "historic", "castle");
        filter.add_rule(true, "natural", "peak");

        // Initialize the MultipolygonManager. Its job is to collect all
        // relations and member ways needed for each area. It then calls an
        // instance of the osmium::area::Assembler class (with the given config)
        // to actually assemble one area. The filter parameter is optional, if
        // it is not set, all areas will be built.
        osmium::area::MultipolygonManager<osmium::area::Assembler> mp_manager{assembler_config, filter};

        // We read the input file twice. In the first pass, only relations are
        // read and fed into the multipolygon manager.
        std::cout << "Pass 1...\n";
        osmium::relations::read_relations(input_file, mp_manager);
        std::cout << "Pass 1 done\n";

        // Output the amount of main memory used so far. All multipolygon relations
        // are in memory now.
        std::cout << "Memory:\n";
        osmium::relations::print_used_memory(std::cerr, mp_manager.used_memory());

        // The index storing all node locations.
        index_type index;

        // The handler that stores all node locations in the index and adds them
        // to the ways.
        location_handler_type location_handler{index};

        // If a location is not available in the index, we ignore it. It might
        // not be needed (if it is not part of a multipolygon relation), so why
        // create an error?
        location_handler.ignore_errors();

        // On the second pass we read all objects and run them first through the
        // node location handler and then the multipolygon collector. The collector
        // will put the areas it has created into the "buffer" which are then
        // fed through our "handler".
        std::cout << "Pass 2...\n";
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::all};

        // read map bounds
        osmium::io::Header header = reader.header();
        auto topRight = header.box().top_right();
        auto bottomLeft = header.box().bottom_left();

        // calculate center coordinates
        double topRightX;
        double topRightY;
        LatLonToUTMXY(topRight.lat(), topRight.lon(), 33, topRightX, topRightY);

        double bottomLeftX;
        double bottomLeftY;
        LatLonToUTMXY(bottomLeft.lat(), bottomLeft.lon(), 33, bottomLeftX, bottomLeftY);

        double centerX = (topRightX + bottomLeftX) / 2.0;
        double centerY = (topRightY + bottomLeftY) / 2.0;
        
        if(std::string(argv[2]) == "-"){
            startX = centerX - 1000.0f;   
            startY = centerY - 1000.0f;     
              
            for(int row = 0; row < 2000; row++){
                for(int col = 0; col < 2000; col++){
                    heightData[row * 2000 + col] = 0.0f;
                }
            }
        }

        // Construct the handler defined above
        NodeWayHandler nodeWayHandler(builder, entryList, heightData, startX, startY, centerX, centerY);

        // create the handler to process all areas
        AreaHandler handler(builder, entryList, heightData, startX, startY, centerX, centerY);

        osmium::apply(reader,
            location_handler,
            mp_manager.handler([&handler](osmium::memory::Buffer&& buffer) {
                osmium::apply(buffer, handler);
            }),
            nodeWayHandler
        );

        // we are done reading our geometry
        reader.close();

        std::cout << "Pass 2 done\n";

        // Output the amount of main memory used so far. All complete multipolygon
        // relations have been cleaned up.
        std::cout << "Memory:\n";
        osmium::relations::print_used_memory(std::cerr, mp_manager.used_memory());

        // If there were multipolygon relations in the input, but some of their
        // members are not in the input file (which often happens for extracts)
        // this will write the IDs of the incomplete relations to stderr.
        std::vector<osmium::object_id_type> incomplete_relations_ids;
        mp_manager.for_each_incomplete_relation([&](const osmium::relations::RelationHandle& handle){
            incomplete_relations_ids.push_back(handle->id());
        });
        if (!incomplete_relations_ids.empty()) {
            std::cout << "Warning! Some member ways missing for these multipolygon relations:";
            for (const auto id : incomplete_relations_ids) {
                std::cout << " " << id;
            }
            std::cout << "\n";
        }

        // now write all data to a map file
        std::cout << "Serializing data..." << std::endl;

        // some converting for flatbuffers library
        auto entryListVector = builder.CreateVector(entryList);
        auto heightDataVec = builder.CreateVector(heightData);

        auto topRightVec = OSMImporter::Vec3(topRightX, topRightY, 0.0f);
        auto bottomLeftVec =  OSMImporter::Vec3(bottomLeftX, bottomLeftY, 0.0f);



        auto list = OSMImporter::CreateEntryList(builder,
                entryListVector,
                heightDataVec,
                startX,
                startY,
                std::lround(centerX),
                std::lround(centerY),
                &topRightVec,
                &bottomLeftVec
        );

        builder.Finish(list);

        std::cout << "Writing data to disk..." << std::endl;

        // write file
        uint8_t *buf = builder.GetBufferPointer();
        int size = builder.GetSize();

        std::ofstream ofile(outputFile, std::ios::binary);
        ofile.write((char *)buf, size);
        ofile.close();

        std::cout << "Done." << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << '\n';
        std::exit(1);
    }
}