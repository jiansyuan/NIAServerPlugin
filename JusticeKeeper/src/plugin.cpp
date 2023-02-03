/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <llapi/LoggerAPI.h>
#include <llapi/EventAPI.h>
#include <llapi/mc/Block.hpp>
#include <llapi/mc/BlockInstance.hpp>
#include <llapi/mc/BlockActor.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Scoreboard.hpp>
#include <llapi/ScheduleAPI.h>
#include <llapi/mc/Tag.hpp>
#include <llapi/DynamicCommandAPI.h>
#include <llapi/mc/Actor.hpp>
#include <llapi/mc/FarmBlock.hpp>

#include "version.h"
// We recommend using the global logger.

extern Logger logger;
Logger Nlog(PLUGIN_NAME);

inline int getDistance(Vec3 pos, int x, int y, int z) {
	return sqrt((pos.x - x) * (pos.x - x) + (pos.y - y) * (pos.y - y) + (pos.z - z) * (pos.z - z));
}
inline float getDistancef(Vec3 pos, int x, int y, int z) {
	return sqrt((pos.x - x) * (pos.x - x) + (pos.y - y) * (pos.y - y) + (pos.z - z) * (pos.z - z));
}
std::unordered_map<std::string, int> umap;
void PluginInit(){
	Nlog.info("Run...");
	DynamicCommand::setup(
		"setconvict",
		"Let someone you want go to the jail.",
		{}, {
			DynamicCommand::ParameterData("player", DynamicCommand::ParameterType::String, false),
			DynamicCommand::ParameterData("ticks", DynamicCommand::ParameterType::Int, false),
		}, { {"player","ticks"},
		},
		[](DynamicCommand const& command, CommandOrigin const& origin, CommandOutput& output,
		std::unordered_map<std::string, DynamicCommand::Result>& results) {
			auto player = results["player"].get<std::string>();
			auto ticks = results["ticks"].get<int>();
			umap[player] = ticks, Level::runcmd("tag "+player+" add convict");
		}, CommandPermissionLevel::GameMasters);

	Event::BlockChangedEvent::subscribe([](const Event::BlockChangedEvent& ev) {
		auto pre = ev.mPreviousBlockInstance; 
		if (pre.getBlock()->getTypeName().compare("minecraft:wheat")!=0 || !(getDistance(pre.getPosition().toVec3(), 698, 81, 554) < 150)) return true;
		auto[x, y, z] = pre.getPosition().toVec3();
		Player* player = (Level::setBlock({ x, y - 1, z }, 0, "minecraft:farmland", 0), nullptr);
		for (auto& i : Level::getAllPlayers())
			if (getDistancef(i->getPos(), x, y, z) <= 2.f) return Nlog.info(i->getName() + "破坏主城麦田罚款10000"), Scoreboard::reduceScore(i, "money", 10000),
				Level::runcmd("w " + i->getName() + " 你因为破坏主城麦田，被NIA服时空警察当场抓获。处以罚金10000元能源币，希望你能知错就改。"),
				(Scoreboard::getScore(i, "money") < 0) ? (umap[i->addTag("convict"), i->getName()] = 5 * 60 * 20) : 114514191810, false;
		return false;
	});
	Schedule::repeat([]() {
		for (auto& i : Level::getAllPlayers()) {
			(umap.find(i->getName()) == umap.end() || umap[i->getName()] <= 0) ? i->removeTag("convict") : 1145141919810;
			if (!i->hasTag("convict")) continue;
			umap[i->getName()] -= 5;
			if (getDistance(i->getPos(), 737, 86, 555) > 8) i->teleport({ 737, 86, 555 }, 0),
				Level::runcmd("w " + i->getName() + " 你还有" + std::to_string(umap[i->getName()] / 1200.f) + "分钟的刑期。");
		}
	}, 5);
}
