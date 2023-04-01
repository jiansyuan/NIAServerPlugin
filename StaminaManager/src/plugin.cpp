/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Scoreboard.hpp>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include "version.h"

// We recommend using the global logger.
extern Logger logger;
Logger Nlog(PLUGIN_NAME);

inline string readfile(string filename) {
    FILE* fp = fopen(filename.c_str(), "rb");
    char buf[1024];
    string result;
    while (int n = fgets(buf, sizeof buf, fp) != NULL) result.append(buf);
    return fclose(fp), result;
}
inline void writefile(string filename, string str2) {
    std::fstream file;
    file.open(filename, std::fstream::out), file << str2.c_str(), file.close();
}
inline bool isFileExists_ifstream(string name) {
    std::ifstream f(name.c_str());
    return f.good();
}
std::unordered_map<std::string, long long> staminaDB;
void checkDB() {
    if (!isFileExists_ifstream("plugins/NiaServer/staminaDB.txt")) {
        rapidjson::StringBuffer buf;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> wrt(buf);
        wrt.SetIndent('\t', 1), wrt.SetFormatOptions(rapidjson::kFormatSingleLineArray);
        wrt.StartObject();
        wrt.Key("DB"), wrt.StartArray();
        wrt.EndArray();
        wrt.EndObject();
        writefile("plugins\\NiaServer\\staminaDB.txt", buf.GetString());
    }
}
inline void loadDB() {
    rapidjson::Document doc;
    auto str = readfile("plugins\\NiaServer\\staminaDB.txt");
    doc.Parse(str.c_str());
    for (auto& i : doc["DB"].GetArray()) {
        auto ary = i.GetArray();
        staminaDB.insert({ ary[0].GetString(), ary[1].GetInt64()});
    }

}
inline void writeDB() {
    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> wrt(buf);
    wrt.SetIndent('\t', 1), wrt.SetFormatOptions(rapidjson::kFormatSingleLineArray);
    wrt.StartObject();
    wrt.Key("DB"), wrt.StartArray();
    for (auto& i : staminaDB)
        wrt.StartArray(), wrt.String(i.first.c_str()), wrt.Int64(i.second), wrt.EndArray();
    wrt.EndArray();
    wrt.EndObject();
    writefile("plugins\\NiaServer\\staminaDB.txt", buf.GetString());
}
inline void playerLeft(const std::string& uuid) {
    staminaDB[uuid] = std::time(0);
    writeDB();
}
void PluginInit() {
	Nlog.info("Run...");
    if (!std::filesystem::exists("plugins/NiaServer/")) std::filesystem::create_directories("plugins/NiaServer/");
	checkDB(), loadDB();
    Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& ev) {
        int lstScore=0;
        auto pluuid = ev.mPlayer->getUuid();
        try { lstScore = Scoreboard::getScore(ev.mPlayer, "stamina"); }
        catch (std::invalid_argument) { Level::runcmd("scoreboard players set " + ev.mPlayer->getName() + " stamina 160"), lstScore = 160; }
        if (staminaDB.find(pluuid) == staminaDB.end()) return  Scoreboard::setScore(ev.mPlayer, "stamina", 160), true;
        return Scoreboard::setScore(ev.mPlayer, "stamina", std::min((int)(lstScore+floor((std::time(0) - staminaDB[pluuid]) / (long double)600)),160))
            , true;
	});
    Event::PlayerLeftEvent::subscribe([](const Event::PlayerLeftEvent& ev) {
        return playerLeft(ev.mPlayer->getUuid()), true;
    });
    Event::ServerStoppedEvent::subscribe([](const Event::ServerStoppedEvent& ev) {
        for (auto& i : Level::getAllPlayers())  playerLeft(i->getUuid());
        return true;
    });
}
