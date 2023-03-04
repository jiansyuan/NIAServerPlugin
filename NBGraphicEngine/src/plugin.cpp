/**
 * @file plugin.cpp
 * @brief The main file of the plugin
 */

#include <llapi/LoggerAPI.h>
#include <llapi/mc/Level.hpp>
#include <llapi/mc/Player.hpp>
#include <llapi/mc/Actor.hpp>
#include <llapi/ScheduleAPI.h>
#include <llapi/ParticleAPI.h>
#include <llapi/EventAPI.h>

#include <thread>
#include <cmath>
#include <random>
#include <bitset>
#include <Windows.h>

#include "version.h"
#include "OBJ_Loader.h"

// We recommend using the global logger.
extern Logger logger;

/**
 * @brief The entrypoint of the plugin. DO NOT remove or rename this function.
 *        
 */
#define updateTick 1
#define SPEED 6.f
#define ACTOR_R .5f
#define DIFF 120.f

ParticleCUI* CUI;
std::mt19937* MtRnd;
std::uniform_real_distribution<float>* UnfDbt;
std::unordered_map<std::string, std::bitset<30>> DisplayParticle;
objl::Loader objLoader, objLoader2;
float Qstatus = 0;
int roundticks = 0;

void AlwWork() {
	while(1) Qstatus += SPEED, Qstatus -= Qstatus > 360 ? 360 : 0, Sleep(updateTick*50);
}

void updateTag(const vector<Player*>& vec) {
	for (auto& i : vec) {
		auto uuid = i->getUuid();
		i->hasTag("Particle_HuiShuo")? DisplayParticle[uuid].set(0) : DisplayParticle[uuid].reset(0); //晦朔
		i->hasTag("Particle_WengYun")? DisplayParticle[uuid].set(1) : DisplayParticle[uuid].reset(1); //滃云
		i->hasTag("Particle_AiYun") ? DisplayParticle[uuid].set(2) : DisplayParticle[uuid].reset(2);//霭云
		i->hasTag("Particle_test")? DisplayParticle[uuid].set(3) : DisplayParticle[uuid].reset(3);//
		if(DisplayParticle[uuid].any()==0) DisplayParticle[uuid].set(1) ;
	}
}

void update() {
	auto AllPlayer = Level::getAllPlayers();
	if (roundticks == 1) updateTag(AllPlayer);
	for (auto& i : AllPlayer) {
		auto [posx, posy, posz] = i->getPos();
		auto uuid = i->getUuid();
		if(DisplayParticle[uuid].test(0)) {
			std::uniform_real_distribution<float> unif(-2.f, 2.f);
			if (!(roundticks%10)) 
				CUI->drawNumber(Vec3(posx + unif(*MtRnd), posy + unif(*MtRnd), posz + unif(*MtRnd)),
					i->getDimensionId(), ParticleCUI::NUM0, (*UnfDbt)(*MtRnd) > 0.0 ? mce::ColorPalette::BLACK : mce::ColorPalette::WHITE),
				CUI->drawNumber(Vec3(posx + unif(*MtRnd), posy + unif(*MtRnd), posz + unif(*MtRnd)),
					i->getDimensionId(), ParticleCUI::NUM1, (*UnfDbt)(*MtRnd) > 0.0 ? mce::ColorPalette::BLACK : mce::ColorPalette::WHITE);
		}
		if (DisplayParticle[uuid].test(1)) {
			float status = Qstatus, tmp = (*UnfDbt)(*MtRnd);
			CUI->spawnParticle(Vec3(
				posx + cos(status * (M_PI + 5 * tmp) / 180.f) * ACTOR_R,
				posy + cos(status * M_PI / 30.f) / 10.f - 1.55f,
				posz + sin(status * (M_PI + 5 * tmp) / 180.f) * ACTOR_R), std::string("minecraft:balloon_gas_particle"), i->getDimensionId());
			status += DIFF, tmp = (*UnfDbt)(*MtRnd), CUI->spawnParticle(Vec3(
				posx + cos(status * (M_PI + 5 * tmp) / 180.f) * ACTOR_R,
				posy + cos(status * M_PI / 30.f) / 10.f + tmp - 1.55f,
				posz + sin(status * (M_PI + 5 * tmp) / 180.f) * ACTOR_R), std::string("minecraft:balloon_gas_particle"), i->getDimensionId());
			status += DIFF, tmp = (*UnfDbt)(*MtRnd), CUI->spawnParticle(Vec3(
				posx + cos(status * (M_PI + 5 * tmp) / 180.f) * ACTOR_R,
				posy + cos(status * M_PI / 30.f) / 10.f + tmp - 1.55f,
				posz + sin(status * (M_PI + 5 * tmp) / 180.f) * ACTOR_R), std::string("minecraft:balloon_gas_particle"), i->getDimensionId());
		}
		if(DisplayParticle[uuid].test(2)) {
			std::uniform_real_distribution<float> unif(0.25f, ACTOR_R*2);
			std::uniform_real_distribution<float> unif2(0.f, 2 * M_PI);
			std::uniform_real_distribution<float> unif3(-1.5f, -.0f); 
			float r = unif(*MtRnd), rate = unif2(*MtRnd);
			CUI->spawnParticle(Vec3(
				posx + cos(rate)*r, posy - 1.55f, posz + sin(rate)*r), 
				std::string("minecraft:endrod"), i->getDimensionId());
			r = unif(*MtRnd), rate = unif2(*MtRnd), CUI->spawnParticle(Vec3(
				posx + cos(rate) * r, posy - 1.55f, posz + sin(rate) * r), 
				std::string("minecraft:endrod"), i->getDimensionId());
			r = unif(*MtRnd), rate = unif2(*MtRnd), CUI->spawnParticle(Vec3(
				posx + cos(rate) * r, posy - 1.55f, posz + sin(rate) * r), 
				std::string("minecraft:endrod"), i->getDimensionId());
		} 
		if (DisplayParticle[uuid].test(3)) {
			auto [viewx, viewy, viewz] = i->getViewVector(1.5f);

		}
		/*if ((roundticks % 10) == 0) {
			for (int i = 0; i < objLoader.LoadedMeshes[0].Vertices.size(); i++)
				CUI->spawnParticle(
					Vec3(objLoader.LoadedMeshes[0].Vertices[i].Position.X*80 + 100,
						objLoader.LoadedMeshes[0].Vertices[i].Position.Y*80 + 120,
						objLoader.LoadedMeshes[0].Vertices[i].Position.Z*80 + 100), "minecraft:balloon_gas_particle", 0);
			for(int i=0;i<objLoader2.LoadedMeshes[0].Vertices.size();i++)
				CUI->spawnParticle(
					Vec3(objLoader2.LoadedMeshes[0].Vertices[i].Position.X * 10 + 80,
						objLoader2.LoadedMeshes[0].Vertices[i].Position.Y * 10 + 120,
						objLoader2.LoadedMeshes[0].Vertices[i].Position.Z * 10 + 80), "minecraft:balloon_gas_particle", 0);
		}*/
	}
	roundticks++, roundticks -= roundticks > 20 ? 20 : 0;
}

void PluginInit() {
	std::thread AlwWorkThread(AlwWork);
	//objLoader.LoadFile("plugins/NiaServer/models/bunny.obj");
	//objLoader2.LoadFile("plugins/NiaServer/models/spot_control_mesh.obj");
	Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& ev) {
		return CUI = new ParticleCUI(), MtRnd = new std::mt19937(std::random_device{}()), 
			UnfDbt = new std::uniform_real_distribution<float>(-.08f, +.08f), true;
		});
	Schedule::repeat(update, updateTick), AlwWorkThread.detach();
}
