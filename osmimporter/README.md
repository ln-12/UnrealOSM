# OSMImporter

## Prerequisites

see [https://osmcode.org/libosmium/manual.html#dependencies](https://osmcode.org/libosmium/manual.html#dependencies)

### Install needed packages
```
sudo apt-get install git cmake make libexpat1-dev zlib1g-dev libbz2-dev libboost-dev libgdal-dev
```

### Install protozero
```
git clone https://github.com/mapbox/protozero
cd protozero
mkdir build
cd build
cmake ..
make
make install
```

### Install libosmium
```
git clone https://github.com/osmcode/libosmium
cd libosmium
mkdir build
cd build
cmake ..
make
make install
```

## Building
```
git clone --recurse-submodules https://git.informatik.tu-freiberg.de/seminar_vr/osmimporter.git
cd osmimporter
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
```

## Usage
```
cd osmimporter
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
./build/OSMImporter <OSM FILE> <XYZ FILE> <OUTPUT FILE>
```

OSM files can be downloaded from [https://www.openstreetmap.org/export](https://www.openstreetmap.org/export), the XYZ files can be obtained from [https://www.geodaten.sachsen.de/downloadbereich-dgm1-4166.html](https://www.geodaten.sachsen.de/downloadbereich-dgm1-4166.html).

## Updating file schema
For detailed instructions on how to building flatbuffers see [https://google.github.io/flatbuffers/flatbuffers_guide_building.html](https://google.github.io/flatbuffers/flatbuffers_guide_building.html)

### Setup 
```
cd osmimporter/flatbuffers
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make
```

### Usage
```
cd OSMImporter
./flatbuffers/flatc --cpp schema.fbs
```

Now copy the generated file schema_generated.h to UnrealOSM/Plugins/OSMLoader/Source/OSMLoader/Public/ (the unreal plugin directory).
