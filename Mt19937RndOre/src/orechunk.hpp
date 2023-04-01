#pragma once
#include <Windows.h>
#include <string>
#include <random>

#include <llapi/EventAPI.h>
#include <llapi/LoggerAPI.h>
#include <llapi/LLAPI.h>
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockInstance.hpp>

#include <llapi/DynamicCommandAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/GlobalServiceAPI.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "version.h"

extern Logger logger;
Logger Nlog(PLUGIN_NAME);

struct SingleOre {
    Block* block;
    long double oreP;
    inline bool operator < (const SingleOre& ot) const{
        return oreP < ot.oreP;
    }
    inline bool operator < (const long double& ot) const {
        return oreP < ot;
    }
    SingleOre(){}
    SingleOre(Block* blk, long double ldb) { block = blk, oreP = ldb; }
};
struct OreXYZ {
    int x, y, z, world;
    //OreXYZ(){}
    OreXYZ(const BlockPos& pos) {
        x = pos.x, y = pos.y, z = pos.z, world=0;
    }
    OreXYZ(int x1,int y1,int z1,int world1) {
        x = x1, y = y1, z = z1, world = world1;
    }
};
/*
class OreChunk {
private:
    vector<OreXYZ> inputXYZ;
    vector<SingleOre> oreChunk;
    std::mt19937 mtRand;
    std::uniform_real_distribution<long double> uniformRoute;
    bool isinit;
public:
    OreChunk()  : mtRand(std::random_device{}()), uniformRoute(0, LDBL_MAX) {
        isinit = false;
    }
	inline void loadFromObject(const rapidjson::Value::Object& obj) {
        for (auto& i : obj["P"].GetArray()) 
            inputXYZ.push_back({ i.GetArray()[0].GetInt(),i.GetArray()[1].GetInt(),i.GetArray()[2].GetInt(), 0}),
            oreChunk.push_back(SingleOre(nullptr, i.GetArray()[3].GetDouble()*100.0));
	}
    OreChunk( const rapidjson::Value::Object& obj) {
        OreChunk(), loadFromObject(obj);
    }
    inline void initOreBlock() {
        for (int i = 0; i < inputXYZ.size(); ++i)
            oreChunk[i].block = Level::getBlock(BlockPos(inputXYZ[i].x, inputXYZ[i].y,inputXYZ[i].z), inputXYZ[i].world);
        //stable_sort(oreChunk.begin(), oreChunk.end());
        if(!isinit){
            long double sum = 0, chen;
            for (auto& i : oreChunk) sum += i.oreP;
            chen = LDBL_MAX / sum, sum = 0;
            for (auto& i : oreChunk) i.oreP = sum += chen * i.oreP;
        }
        isinit = true;
    }

    inline Block* getRandBlock() {
        //std::mt19937 test(std::random_device{}());
        //std::uniform_real_distribution<long double> aaa(0, LDBL_MAX);

        Nlog.info(
            std::to_string(uniformRoute(mtRand)));
        return (oreChunk)[(
            std::lower_bound(oreChunk.begin(), oreChunk.end(), uniformRoute(mtRand)) - oreChunk.begin())
            %oreChunk.size()].block;
    }

};
*/
