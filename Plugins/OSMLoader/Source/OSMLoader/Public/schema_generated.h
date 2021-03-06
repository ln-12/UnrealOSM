// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SCHEMA_OSMIMPORTER_H_
#define FLATBUFFERS_GENERATED_SCHEMA_OSMIMPORTER_H_

#include "flatbuffers/flatbuffers.h"

namespace OSMImporter {

struct Vec3;

struct Vec2;

struct Entry;
struct EntryBuilder;

struct EntryList;
struct EntryListBuilder;

enum EntityType {
  EntityType_ERROR = 0,
  EntityType_NODE_HIGHWAY_STREET_LAMP = 1,
  EntityType_NODE_TREE = 2,
  EntityType_NODE_VEGETATION = 3,
  EntityType_POLYLINE_HIGHWAY_DEFAULT = 4,
  EntityType_POLYLINE_HIGHWAY_PRIMARY = 5,
  EntityType_POLYLINE_HIGHWAY_SECONDARY = 6,
  EntityType_POLYLINE_HIGHWAY_RESIDENTIAL = 7,
  EntityType_POLYLINE_HIGHWAY_SERVICE = 8,
  EntityType_POLYLINE_HIGHWAY_CYCLEWAY = 9,
  EntityType_POLYLINE_HIGHWAY_FOOTWAY = 10,
  EntityType_POLYLINE_RIVER = 11,
  EntityType_POLYLINE_STREAM = 12,
  EntityType_POLYGON_HIGHWAY = 13,
  EntityType_POLYGON_BUILDING_ROOF = 14,
  EntityType_POLYGON_BUILDING_WALL = 15,
  EntityType_POLYGON_WATER = 16,
  EntityType_POLYGON_FOREST = 17,
  EntityType_POLYGON_GRASS = 18,
  EntityType_NODE_POI_RELIGION = 19,
  EntityType_NODE_POI_CASTLE = 20,
  EntityType_NODE_POI_ARTS_CENTER = 21,
  EntityType_NODE_POI_PEAK = 22,
  EntityType_MIN = EntityType_ERROR,
  EntityType_MAX = EntityType_NODE_POI_PEAK
};

inline const EntityType (&EnumValuesEntityType())[23] {
  static const EntityType values[] = {
    EntityType_ERROR,
    EntityType_NODE_HIGHWAY_STREET_LAMP,
    EntityType_NODE_TREE,
    EntityType_NODE_VEGETATION,
    EntityType_POLYLINE_HIGHWAY_DEFAULT,
    EntityType_POLYLINE_HIGHWAY_PRIMARY,
    EntityType_POLYLINE_HIGHWAY_SECONDARY,
    EntityType_POLYLINE_HIGHWAY_RESIDENTIAL,
    EntityType_POLYLINE_HIGHWAY_SERVICE,
    EntityType_POLYLINE_HIGHWAY_CYCLEWAY,
    EntityType_POLYLINE_HIGHWAY_FOOTWAY,
    EntityType_POLYLINE_RIVER,
    EntityType_POLYLINE_STREAM,
    EntityType_POLYGON_HIGHWAY,
    EntityType_POLYGON_BUILDING_ROOF,
    EntityType_POLYGON_BUILDING_WALL,
    EntityType_POLYGON_WATER,
    EntityType_POLYGON_FOREST,
    EntityType_POLYGON_GRASS,
    EntityType_NODE_POI_RELIGION,
    EntityType_NODE_POI_CASTLE,
    EntityType_NODE_POI_ARTS_CENTER,
    EntityType_NODE_POI_PEAK
  };
  return values;
}

inline const char * const *EnumNamesEntityType() {
  static const char * const names[24] = {
    "ERROR",
    "NODE_HIGHWAY_STREET_LAMP",
    "NODE_TREE",
    "NODE_VEGETATION",
    "POLYLINE_HIGHWAY_DEFAULT",
    "POLYLINE_HIGHWAY_PRIMARY",
    "POLYLINE_HIGHWAY_SECONDARY",
    "POLYLINE_HIGHWAY_RESIDENTIAL",
    "POLYLINE_HIGHWAY_SERVICE",
    "POLYLINE_HIGHWAY_CYCLEWAY",
    "POLYLINE_HIGHWAY_FOOTWAY",
    "POLYLINE_RIVER",
    "POLYLINE_STREAM",
    "POLYGON_HIGHWAY",
    "POLYGON_BUILDING_ROOF",
    "POLYGON_BUILDING_WALL",
    "POLYGON_WATER",
    "POLYGON_FOREST",
    "POLYGON_GRASS",
    "NODE_POI_RELIGION",
    "NODE_POI_CASTLE",
    "NODE_POI_ARTS_CENTER",
    "NODE_POI_PEAK",
    nullptr
  };
  return names;
}

inline const char *EnumNameEntityType(EntityType e) {
  if (flatbuffers::IsOutRange(e, EntityType_ERROR, EntityType_NODE_POI_PEAK)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesEntityType()[index];
}

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec3 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Vec3() {
    memset(static_cast<void *>(this), 0, sizeof(Vec3));
  }
  Vec3(float _x, float _y, float _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Vec3, 12);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec2 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;

 public:
  Vec2() {
    memset(static_cast<void *>(this), 0, sizeof(Vec2));
  }
  Vec2(float _x, float _y)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
};
FLATBUFFERS_STRUCT_END(Vec2, 8);

struct Entry FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef EntryBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ENTITYTYPE = 4,
    VT_HEIGHT = 6,
    VT_NAME = 8,
    VT_POI = 10,
    VT_HOLESTARTID = 12,
    VT_VERTICES = 14,
    VT_TRIANGLES = 16,
    VT_UVS = 18
  };
  OSMImporter::EntityType entityType() const {
    return static_cast<OSMImporter::EntityType>(GetField<int8_t>(VT_ENTITYTYPE, 0));
  }
  int8_t height() const {
    return GetField<int8_t>(VT_HEIGHT, 0);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  bool poi() const {
    return GetField<uint8_t>(VT_POI, 0) != 0;
  }
  int32_t holeStartId() const {
    return GetField<int32_t>(VT_HOLESTARTID, 0);
  }
  const flatbuffers::Vector<const OSMImporter::Vec3 *> *vertices() const {
    return GetPointer<const flatbuffers::Vector<const OSMImporter::Vec3 *> *>(VT_VERTICES);
  }
  const flatbuffers::Vector<uint32_t> *triangles() const {
    return GetPointer<const flatbuffers::Vector<uint32_t> *>(VT_TRIANGLES);
  }
  const flatbuffers::Vector<const OSMImporter::Vec2 *> *uvs() const {
    return GetPointer<const flatbuffers::Vector<const OSMImporter::Vec2 *> *>(VT_UVS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_ENTITYTYPE) &&
           VerifyField<int8_t>(verifier, VT_HEIGHT) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyField<uint8_t>(verifier, VT_POI) &&
           VerifyField<int32_t>(verifier, VT_HOLESTARTID) &&
           VerifyOffset(verifier, VT_VERTICES) &&
           verifier.VerifyVector(vertices()) &&
           VerifyOffset(verifier, VT_TRIANGLES) &&
           verifier.VerifyVector(triangles()) &&
           VerifyOffset(verifier, VT_UVS) &&
           verifier.VerifyVector(uvs()) &&
           verifier.EndTable();
  }
};

struct EntryBuilder {
  typedef Entry Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_entityType(OSMImporter::EntityType entityType) {
    fbb_.AddElement<int8_t>(Entry::VT_ENTITYTYPE, static_cast<int8_t>(entityType), 0);
  }
  void add_height(int8_t height) {
    fbb_.AddElement<int8_t>(Entry::VT_HEIGHT, height, 0);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Entry::VT_NAME, name);
  }
  void add_poi(bool poi) {
    fbb_.AddElement<uint8_t>(Entry::VT_POI, static_cast<uint8_t>(poi), 0);
  }
  void add_holeStartId(int32_t holeStartId) {
    fbb_.AddElement<int32_t>(Entry::VT_HOLESTARTID, holeStartId, 0);
  }
  void add_vertices(flatbuffers::Offset<flatbuffers::Vector<const OSMImporter::Vec3 *>> vertices) {
    fbb_.AddOffset(Entry::VT_VERTICES, vertices);
  }
  void add_triangles(flatbuffers::Offset<flatbuffers::Vector<uint32_t>> triangles) {
    fbb_.AddOffset(Entry::VT_TRIANGLES, triangles);
  }
  void add_uvs(flatbuffers::Offset<flatbuffers::Vector<const OSMImporter::Vec2 *>> uvs) {
    fbb_.AddOffset(Entry::VT_UVS, uvs);
  }
  explicit EntryBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  EntryBuilder &operator=(const EntryBuilder &);
  flatbuffers::Offset<Entry> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Entry>(end);
    return o;
  }
};

inline flatbuffers::Offset<Entry> CreateEntry(
    flatbuffers::FlatBufferBuilder &_fbb,
    OSMImporter::EntityType entityType = OSMImporter::EntityType_ERROR,
    int8_t height = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    bool poi = false,
    int32_t holeStartId = 0,
    flatbuffers::Offset<flatbuffers::Vector<const OSMImporter::Vec3 *>> vertices = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint32_t>> triangles = 0,
    flatbuffers::Offset<flatbuffers::Vector<const OSMImporter::Vec2 *>> uvs = 0) {
  EntryBuilder builder_(_fbb);
  builder_.add_uvs(uvs);
  builder_.add_triangles(triangles);
  builder_.add_vertices(vertices);
  builder_.add_holeStartId(holeStartId);
  builder_.add_name(name);
  builder_.add_poi(poi);
  builder_.add_height(height);
  builder_.add_entityType(entityType);
  return builder_.Finish();
}

inline flatbuffers::Offset<Entry> CreateEntryDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    OSMImporter::EntityType entityType = OSMImporter::EntityType_ERROR,
    int8_t height = 0,
    const char *name = nullptr,
    bool poi = false,
    int32_t holeStartId = 0,
    const std::vector<OSMImporter::Vec3> *vertices = nullptr,
    const std::vector<uint32_t> *triangles = nullptr,
    const std::vector<OSMImporter::Vec2> *uvs = nullptr) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto vertices__ = vertices ? _fbb.CreateVectorOfStructs<OSMImporter::Vec3>(*vertices) : 0;
  auto triangles__ = triangles ? _fbb.CreateVector<uint32_t>(*triangles) : 0;
  auto uvs__ = uvs ? _fbb.CreateVectorOfStructs<OSMImporter::Vec2>(*uvs) : 0;
  return OSMImporter::CreateEntry(
      _fbb,
      entityType,
      height,
      name__,
      poi,
      holeStartId,
      vertices__,
      triangles__,
      uvs__);
}

struct EntryList FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef EntryListBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_LIST = 4,
    VT_HEIGHTMAP = 6,
    VT_STARTX = 8,
    VT_STARTY = 10,
    VT_CENTERX = 12,
    VT_CENTERY = 14,
    VT_TOPRIGHT = 16,
    VT_BOTTOMLEFT = 18
  };
  const flatbuffers::Vector<flatbuffers::Offset<OSMImporter::Entry>> *list() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<OSMImporter::Entry>> *>(VT_LIST);
  }
  const flatbuffers::Vector<float> *heightMap() const {
    return GetPointer<const flatbuffers::Vector<float> *>(VT_HEIGHTMAP);
  }
  int64_t startX() const {
    return GetField<int64_t>(VT_STARTX, 0);
  }
  int64_t startY() const {
    return GetField<int64_t>(VT_STARTY, 0);
  }
  int64_t centerX() const {
    return GetField<int64_t>(VT_CENTERX, 0);
  }
  int64_t centerY() const {
    return GetField<int64_t>(VT_CENTERY, 0);
  }
  const OSMImporter::Vec3 *topRight() const {
    return GetStruct<const OSMImporter::Vec3 *>(VT_TOPRIGHT);
  }
  const OSMImporter::Vec3 *bottomLeft() const {
    return GetStruct<const OSMImporter::Vec3 *>(VT_BOTTOMLEFT);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_LIST) &&
           verifier.VerifyVector(list()) &&
           verifier.VerifyVectorOfTables(list()) &&
           VerifyOffset(verifier, VT_HEIGHTMAP) &&
           verifier.VerifyVector(heightMap()) &&
           VerifyField<int64_t>(verifier, VT_STARTX) &&
           VerifyField<int64_t>(verifier, VT_STARTY) &&
           VerifyField<int64_t>(verifier, VT_CENTERX) &&
           VerifyField<int64_t>(verifier, VT_CENTERY) &&
           VerifyField<OSMImporter::Vec3>(verifier, VT_TOPRIGHT) &&
           VerifyField<OSMImporter::Vec3>(verifier, VT_BOTTOMLEFT) &&
           verifier.EndTable();
  }
};

struct EntryListBuilder {
  typedef EntryList Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_list(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<OSMImporter::Entry>>> list) {
    fbb_.AddOffset(EntryList::VT_LIST, list);
  }
  void add_heightMap(flatbuffers::Offset<flatbuffers::Vector<float>> heightMap) {
    fbb_.AddOffset(EntryList::VT_HEIGHTMAP, heightMap);
  }
  void add_startX(int64_t startX) {
    fbb_.AddElement<int64_t>(EntryList::VT_STARTX, startX, 0);
  }
  void add_startY(int64_t startY) {
    fbb_.AddElement<int64_t>(EntryList::VT_STARTY, startY, 0);
  }
  void add_centerX(int64_t centerX) {
    fbb_.AddElement<int64_t>(EntryList::VT_CENTERX, centerX, 0);
  }
  void add_centerY(int64_t centerY) {
    fbb_.AddElement<int64_t>(EntryList::VT_CENTERY, centerY, 0);
  }
  void add_topRight(const OSMImporter::Vec3 *topRight) {
    fbb_.AddStruct(EntryList::VT_TOPRIGHT, topRight);
  }
  void add_bottomLeft(const OSMImporter::Vec3 *bottomLeft) {
    fbb_.AddStruct(EntryList::VT_BOTTOMLEFT, bottomLeft);
  }
  explicit EntryListBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  EntryListBuilder &operator=(const EntryListBuilder &);
  flatbuffers::Offset<EntryList> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<EntryList>(end);
    return o;
  }
};

inline flatbuffers::Offset<EntryList> CreateEntryList(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<OSMImporter::Entry>>> list = 0,
    flatbuffers::Offset<flatbuffers::Vector<float>> heightMap = 0,
    int64_t startX = 0,
    int64_t startY = 0,
    int64_t centerX = 0,
    int64_t centerY = 0,
    const OSMImporter::Vec3 *topRight = 0,
    const OSMImporter::Vec3 *bottomLeft = 0) {
  EntryListBuilder builder_(_fbb);
  builder_.add_centerY(centerY);
  builder_.add_centerX(centerX);
  builder_.add_startY(startY);
  builder_.add_startX(startX);
  builder_.add_bottomLeft(bottomLeft);
  builder_.add_topRight(topRight);
  builder_.add_heightMap(heightMap);
  builder_.add_list(list);
  return builder_.Finish();
}

inline flatbuffers::Offset<EntryList> CreateEntryListDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<OSMImporter::Entry>> *list = nullptr,
    const std::vector<float> *heightMap = nullptr,
    int64_t startX = 0,
    int64_t startY = 0,
    int64_t centerX = 0,
    int64_t centerY = 0,
    const OSMImporter::Vec3 *topRight = 0,
    const OSMImporter::Vec3 *bottomLeft = 0) {
  auto list__ = list ? _fbb.CreateVector<flatbuffers::Offset<OSMImporter::Entry>>(*list) : 0;
  auto heightMap__ = heightMap ? _fbb.CreateVector<float>(*heightMap) : 0;
  return OSMImporter::CreateEntryList(
      _fbb,
      list__,
      heightMap__,
      startX,
      startY,
      centerX,
      centerY,
      topRight,
      bottomLeft);
}

inline const OSMImporter::EntryList *GetEntryList(const void *buf) {
  return flatbuffers::GetRoot<OSMImporter::EntryList>(buf);
}

inline const OSMImporter::EntryList *GetSizePrefixedEntryList(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<OSMImporter::EntryList>(buf);
}

inline bool VerifyEntryListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<OSMImporter::EntryList>(nullptr);
}

inline bool VerifySizePrefixedEntryListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<OSMImporter::EntryList>(nullptr);
}

inline void FinishEntryListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<OSMImporter::EntryList> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedEntryListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<OSMImporter::EntryList> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace OSMImporter

#endif  // FLATBUFFERS_GENERATED_SCHEMA_OSMIMPORTER_H_
