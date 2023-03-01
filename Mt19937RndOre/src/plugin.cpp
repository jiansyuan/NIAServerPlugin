/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */


//jiansyuan&alkalisalt 2023.1.24
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

#include<fstream>

#include "orechunk.hpp"
#include "version.h"


inline string readfile(string filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    char buf[1024]; 
    string result;
    while (int n = fgets(buf, sizeof buf, fp) != NULL) result.append(buf);
    return fclose(fp), result;
}
inline void writefile(string filename, string str2) {
    std::fstream file;
    file.open(filename, std::fstream::out), file<<str2.c_str(), file.close();
}


std::unordered_map<std::string, vector<std::pair<OreXYZ, long double> > > inChunkMap;

inline bool isFileExists_ifstream(string name) {
    std::ifstream f(name.c_str());
    return f.good();
}
inline bool chunkcmp(const std::pair<OreXYZ, long double>& x, const std::pair<OreXYZ, long double>& y) {
    return x.second < y.second;
}

void initplugin() {
    if (!std::filesystem::exists("plugins/NiaServer")) std::filesystem::create_directories("plugins/NiaServer");
tryag:;
    if (isFileExists_ifstream("plugins/NiaServer/settings.cfg")) {
        rapidjson::Document doc;
        auto str = readfile("plugins\\NiaServer\\settings.cfg");
        doc.Parse(str.c_str()); 
        for (auto& i : doc["OreChunks"].GetArray()) {
            auto name = i.GetObj()["Name"].GetString();
            auto arry = i.GetObj()["P"].GetArray();
            vector<std::pair<OreXYZ, long double> > tmp;
            for (auto& j : arry) 
                tmp.push_back({ {j.GetArray()[0].GetInt(), j.GetArray()[1].GetInt(), j.GetArray()[2].GetInt(), 0}, j.GetArray()[3].GetDouble()*100 });
            long double sum = 0, chen = 0;
            for (auto& j : tmp) sum += j.second;
            chen = LDBL_MAX / sum, sum = 0;
            for (auto& j : tmp) sum += j.second * chen, j.second = sum;
            stable_sort(tmp.begin(), tmp.end(), chunkcmp);
            inChunkMap.insert({ name, tmp });
        }
    } else {
        rapidjson::StringBuffer buf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> wrt(buf);
        wrt.SetIndent('\t', 1), wrt.SetFormatOptions(rapidjson::kFormatSingleLineArray);
        wrt.StartObject();
        wrt.Key("OreChunks"),wrt.StartArray();
        wrt.StartObject();
        wrt.Key("Name"),wrt.String("OreChunk1");
        wrt.Key("P"); wrt.StartArray();
        wrt.StartArray(), wrt.Int(300), wrt.Int(100), wrt.Int(300), wrt.Double(0.5), wrt.EndArray();
        wrt.StartArray(), wrt.Int(300), wrt.Int(101), wrt.Int(300), wrt.Double(0.5), wrt.EndArray();
        wrt.EndArray();
        wrt.EndObject();
        wrt.StartObject();
        wrt.Key("Name"), wrt.String("OreChunk2");
        wrt.Key("P"); wrt.StartArray();
        wrt.StartArray(), wrt.Int(300), wrt.Int(100), wrt.Int(300), wrt.Double(0.1), wrt.EndArray();
        wrt.StartArray(), wrt.Int(300), wrt.Int(101), wrt.Int(300), wrt.Double(0.5), wrt.EndArray();
        wrt.EndArray();
        wrt.EndObject();
        wrt.EndArray(), wrt.EndObject();
       // std::cout << res << std::endl;
       writefile("plugins\\NiaServer\\settings.cfg" , buf.GetString());
       goto tryag;
    }
}
inline string strXYZ(int x, int y, int z) {return std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z) + ' ';}
inline string strXYZ(const OreXYZ& blkpos) { return std::to_string(blkpos.x) + ' ' + std::to_string(blkpos.y) + ' ' + std::to_string(blkpos.z) + ' '; }

inline bool sbsbcmp(const std::pair<Block*, long double>& x1, const std::pair<Block*, long double>& x2) {
    return x1.second < x2.second;
}
inline bool work(const std::string &chunkName, OreXYZ pos1, OreXYZ pos2) {
    Nlog.info("updating ores........");
    if (inChunkMap.find(chunkName) == inChunkMap.end())return false;
    Level::runcmd("tickingarea add " + strXYZ(pos1) + strXYZ(pos2) + "NIAOREPLUGIN" + chunkName + " true");
    std::mt19937 randmt(std::random_device{}());
    std::uniform_real_distribution<long double> unif(0, LDBL_MAX);
    vector<std::pair<Block*, long double> > chunkMap;
    auto& inckmap = inChunkMap[chunkName];
    for (auto& i : inckmap)
        chunkMap.push_back({ Level::getBlock({i.first.x, i.first.y, i.first.z}, 0), i.second });
    int x1 = pos1.x, y1 = pos1.y, z1 = pos1.z, x2 = pos2.x, y2 = pos2.y, z2 = pos2.z;
    !(x1 > x2) ? (void)0 : std::swap(x1, x2);
    !(y1 > y2) ? (void)0 : std::swap(y1, y2);
    !(z1 > z2) ? (void)0 : std::swap(z1, z2);
   for (int i = x1; i <= x2; i++)
        for (int j = y1; j <= y2; j++)
            for (int k = z1; k <= z2; k++)
                Level::setBlock({ i, j, k }, 0, 
                    chunkMap[(std::lower_bound(chunkMap.begin(), chunkMap.end(), std::make_pair( (Block*)0,unif(randmt)), sbsbcmp) - chunkMap.begin()) % chunkMap.size()].first);
    Nlog.info("done.");
    Level::runcmd("tickingarea remove NIAOREPLUGIN"+chunkName);
    return true;
}

void PluginInit() {

    Nlog.info("Run....."); initplugin();
    using ParamType = DynamicCommand::ParameterType;
    using Param = DynamicCommand::ParameterData;
    
    
    DynamicCommand::setup(
        "spawnores",
        "Respawn ores.(NIA Server Ore Plugin)",
        {
            {"ChunkName",{"Position1","Position2"}},
        },
        {
            Param("ChunkName", ParamType::String, false, "ChunkName"),
            Param("Position1", ParamType::BlockPos, false, "Position1"),
            Param("Position2", ParamType::BlockPos, false, "Position2"),
        },
        {
          {"ChunkName", "Position1", "Position2"},
        },
        [](
            DynamicCommand const& command,
            CommandOrigin const& origin,
            CommandOutput& output,
            std::unordered_map<std::string, DynamicCommand::Result>& results) {
                if (origin.getPermissionsLevel() != (CommandPermissionLevel)1 && origin.getPermissionsLevel() != (CommandPermissionLevel)4)
                output.success(std::string("You have no enough permission."));
                else {
                    if (!work(results["ChunkName"].get<std::string>(), OreXYZ(results["Position1"].get<BlockPos>()), OreXYZ(results["Position2"].get<BlockPos>()))) 
                        output.success("Something wrong!!!");
                    else output.success(std::string("Respawn successfully!"));
                }
            }, CommandPermissionLevel::GameMasters
    );

 
    return;
}
