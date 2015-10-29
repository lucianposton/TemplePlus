
#include "stdafx.h"

#include <infrastructure/exception.h>
#include <temple/dll.h>
#include "legacysystems.h"


//*****************************************************************************
//* Vagrant
//*****************************************************************************

VagrantSystem::VagrantSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10086ae0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Vagrant");
	}
}
VagrantSystem::~VagrantSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10086b10);
	shutdown();
}
void VagrantSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x10086cb0);
	advanceTime(time);
}
const std::string &VagrantSystem::GetName() const {
	static std::string name("Vagrant");
	return name;
}

//*****************************************************************************
//* Description
//*****************************************************************************

DescriptionSystem::DescriptionSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100865d0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Description");
	}
}
DescriptionSystem::~DescriptionSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10086670);
	shutdown();
}
void DescriptionSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10086710);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system Description");
	}
}
void DescriptionSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x10086780);
	unloadModule();
}
void DescriptionSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x100866c0);
	reset();
}
bool DescriptionSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x10086810);
	return save(file) == 1;
}
bool DescriptionSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100868b0);
	return load(saveFile) == 1;
}
const std::string &DescriptionSystem::GetName() const {
	static std::string name("Description");
	return name;
}

//*****************************************************************************
//* ItemEffect
//*****************************************************************************

ItemEffectSystem::ItemEffectSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100864d0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system ItemEffect");
	}
}
ItemEffectSystem::~ItemEffectSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10086550);
	shutdown();
}
void ItemEffectSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10086560);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system ItemEffect");
	}
}
void ItemEffectSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x100865c0);
	unloadModule();
}
const std::string &ItemEffectSystem::GetName() const {
	static std::string name("ItemEffect");
	return name;
}

//*****************************************************************************
//* Teleport
//*****************************************************************************

TeleportSystem::TeleportSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10084a20);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Teleport");
	}
}
TeleportSystem::~TeleportSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10084fa0);
	shutdown();
}
void TeleportSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10084f60);
	reset();
}
void TeleportSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x10086480);
	advanceTime(time);
}
const std::string &TeleportSystem::GetName() const {
	static std::string name("Teleport");
	return name;
}

//*****************************************************************************
//* Sector
//*****************************************************************************

SectorSystem::SectorSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10082db0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Sector");
	}
}
SectorSystem::~SectorSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10081bc0);
	shutdown();
}
void SectorSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10081bb0);
	reset();
}
bool SectorSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x10081be0);
	return save(file) == 1;
}
bool SectorSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x10081d20);
	return load(saveFile) == 1;
}
const std::string &SectorSystem::GetName() const {
	static std::string name("Sector");
	return name;
}

//*****************************************************************************
//* Random
//*****************************************************************************

RandomSystem::RandomSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10039040);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Random");
	}
}
const std::string &RandomSystem::GetName() const {
	static std::string name("Random");
	return name;
}

//*****************************************************************************
//* Critter
//*****************************************************************************

CritterSystem::CritterSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1007e310);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Critter");
	}
}
CritterSystem::~CritterSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1007e3f0);
	shutdown();
}
const std::string &CritterSystem::GetName() const {
	static std::string name("Critter");
	return name;
}

//*****************************************************************************
//* ScriptName
//*****************************************************************************

ScriptNameSystem::ScriptNameSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1007e000);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system ScriptName");
	}
}
ScriptNameSystem::~ScriptNameSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1007e0c0);
	shutdown();
}
void ScriptNameSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x1007e0e0);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system ScriptName");
	}
}
void ScriptNameSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x1007e1b0);
	unloadModule();
}
const std::string &ScriptNameSystem::GetName() const {
	static std::string name("ScriptName");
	return name;
}

//*****************************************************************************
//* Portrait
//*****************************************************************************

PortraitSystem::PortraitSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1007de10);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Portrait");
	}
}
PortraitSystem::~PortraitSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1007de30);
	shutdown();
}
const std::string &PortraitSystem::GetName() const {
	static std::string name("Portrait");
	return name;
}

//*****************************************************************************
//* Skill
//*****************************************************************************

SkillSystem::SkillSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1007cfa0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Skill");
	}
}
SkillSystem::~SkillSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1007d0c0);
	shutdown();
}
bool SkillSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1007d110);
	return save(file) == 1;
}
bool SkillSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1007d0e0);
	return load(saveFile) == 1;
}
const std::string &SkillSystem::GetName() const {
	static std::string name("Skill");
	return name;
}

//*****************************************************************************
//* Feat
//*****************************************************************************

FeatSystem::FeatSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1007bfa0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Feat");
	}
}
FeatSystem::~FeatSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1007b900);
	shutdown();
}
const std::string &FeatSystem::GetName() const {
	static std::string name("Feat");
	return name;
}

//*****************************************************************************
//* Spell
//*****************************************************************************

SpellSystem::SpellSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1007b740);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Spell");
	}
}
SpellSystem::~SpellSystem() {
	auto shutdown = temple::GetPointer<void()>(0x100791d0);
	shutdown();
}
void SpellSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x100750f0);
	reset();
}
const std::string &SpellSystem::GetName() const {
	static std::string name("Spell");
	return name;
}

//*****************************************************************************
//* Stat
//*****************************************************************************

StatSystem::StatSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10073680);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Stat");
	}
}
StatSystem::~StatSystem() {
	auto shutdown = temple::GetPointer<void()>(0x100739b0);
	shutdown();
}
const std::string &StatSystem::GetName() const {
	static std::string name("Stat");
	return name;
}

//*****************************************************************************
//* Script
//*****************************************************************************

ScriptSystem::ScriptSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10006580);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Script");
	}
}
ScriptSystem::~ScriptSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10007b60);
	shutdown();
}
void ScriptSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10006630);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system Script");
	}
}
void ScriptSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x10006650);
	unloadModule();
}
void ScriptSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10007ae0);
	reset();
}
bool ScriptSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x100066e0);
	return save(file) == 1;
}
bool ScriptSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x10006670);
	return load(saveFile) == 1;
}
const std::string &ScriptSystem::GetName() const {
	static std::string name("Script");
	return name;
}

//*****************************************************************************
//* Level
//*****************************************************************************

LevelSystem::LevelSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10072f50);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Level");
	}
}
LevelSystem::~LevelSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10073180);
	shutdown();
}
const std::string &LevelSystem::GetName() const {
	static std::string name("Level");
	return name;
}

//*****************************************************************************
//* D20
//*****************************************************************************

D20System::D20System(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1004c8a0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system D20");
	}
}
D20System::~D20System() {
	auto shutdown = temple::GetPointer<void()>(0x1004c950);
	shutdown();
}
void D20System::Reset() {
	auto reset = temple::GetPointer<void()>(0x1004c9b0);
	reset();
}
void D20System::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x1004fc40);
	advanceTime(time);
}
const std::string &D20System::GetName() const {
	static std::string name("D20");
	return name;
}

//*****************************************************************************
//* Map
//*****************************************************************************

MapSystem::MapSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1006f4d0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Map");
	}
}
MapSystem::~MapSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10071fd0);
	shutdown();
}
void MapSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10070ba0);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system Map");
	}
}
void MapSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x1006f880);
	unloadModule();
}
void MapSystem::ResetBuffers(const RebuildBufferInfo& rebuildInfo) {
	auto resetBuffers = temple::GetPointer<void(const RebuildBufferInfo*)>(0x1006f850);
	resetBuffers(&rebuildInfo);
}
void MapSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10071e40);
	reset();
}
bool MapSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x10072050);
	return save(file) == 1;
}
bool MapSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x10072c40);
	return load(saveFile) == 1;
}
void MapSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x1006f8c0);
	advanceTime(time);
}
const std::string &MapSystem::GetName() const {
	static std::string name("Map");
	return name;
}

//*****************************************************************************
//* LightScheme
//*****************************************************************************

LightSchemeSystem::LightSchemeSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1006ef30);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system LightScheme");
	}
}
LightSchemeSystem::~LightSchemeSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1006ef80);
	shutdown();
}
void LightSchemeSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x1006f440);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system LightScheme");
	}
}
void LightSchemeSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x1006ef50);
	unloadModule();
}
void LightSchemeSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1006f430);
	reset();
}
bool LightSchemeSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1006ef90);
	return save(file) == 1;
}
bool LightSchemeSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1006f470);
	return load(saveFile) == 1;
}
const std::string &LightSchemeSystem::GetName() const {
	static std::string name("LightScheme");
	return name;
}

//*****************************************************************************
//* Player
//*****************************************************************************

PlayerSystem::PlayerSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1006ede0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Player");
	}
}
PlayerSystem::~PlayerSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1006ee40);
	shutdown();
}
void PlayerSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1006ee00);
	reset();
}
bool PlayerSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x101f5850);
	return save(file) == 1;
}
bool PlayerSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x101f5850);
	return load(saveFile) == 1;
}
const std::string &PlayerSystem::GetName() const {
	static std::string name("Player");
	return name;
}

//*****************************************************************************
//* Area
//*****************************************************************************

AreaSystem::AreaSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1006e550);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Area");
	}
}
void AreaSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x1006e590);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system Area");
	}
}
void AreaSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x1006e860);
	unloadModule();
}
void AreaSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1006e560);
	reset();
}
bool AreaSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1006e920);
	return save(file) == 1;
}
bool AreaSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1006e8d0);
	return load(saveFile) == 1;
}
const std::string &AreaSystem::GetName() const {
	static std::string name("Area");
	return name;
}

//*****************************************************************************
//* Dialog
//*****************************************************************************

DialogSystem::DialogSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10036040);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Dialog");
	}
}
DialogSystem::~DialogSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10036080);
	shutdown();
}
const std::string &DialogSystem::GetName() const {
	static std::string name("Dialog");
	return name;
}

//*****************************************************************************
//* SoundMap
//*****************************************************************************

SoundMapSystem::SoundMapSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1006ded0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system SoundMap");
	}
}
const std::string &SoundMapSystem::GetName() const {
	static std::string name("SoundMap");
	return name;
}

//*****************************************************************************
//* SoundGame
//*****************************************************************************

SoundGameSystem::SoundGameSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1003d4a0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system SoundGame");
	}
}
SoundGameSystem::~SoundGameSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1003bb10);
	shutdown();
}
void SoundGameSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x1003bb80);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system SoundGame");
	}
}
void SoundGameSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x1003bbc0);
	unloadModule();
}
void SoundGameSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1003cb30);
	reset();
}
bool SoundGameSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1003bbd0);
	return save(file) == 1;
}
bool SoundGameSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1003cb70);
	return load(saveFile) == 1;
}
void SoundGameSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x1003dc50);
	advanceTime(time);
}
const std::string &SoundGameSystem::GetName() const {
	static std::string name("SoundGame");
	return name;
}

//*****************************************************************************
//* Item
//*****************************************************************************

ItemSystem::ItemSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10063c70);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Item");
	}
}
ItemSystem::~ItemSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10063dc0);
	shutdown();
}
void ItemSystem::ResetBuffers(const RebuildBufferInfo& rebuildInfo) {
	auto resetBuffers = temple::GetPointer<void(const RebuildBufferInfo*)>(0x10063df0);
	resetBuffers(&rebuildInfo);
}
const std::string &ItemSystem::GetName() const {
	static std::string name("Item");
	return name;
}

//*****************************************************************************
//* Combat
//*****************************************************************************

CombatSystem::CombatSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10063ba0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Combat");
	}
}
CombatSystem::~CombatSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10062eb0);
	shutdown();
}
void CombatSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10062ed0);
	reset();
}
bool CombatSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x10062440);
	return save(file) == 1;
}
bool CombatSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x10062470);
	return load(saveFile) == 1;
}
void CombatSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x10062e20);
	advanceTime(time);
}
const std::string &CombatSystem::GetName() const {
	static std::string name("Combat");
	return name;
}

//*****************************************************************************
//* TimeEvent
//*****************************************************************************

TimeEventSystem::TimeEventSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100616a0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system TimeEvent");
	}
}
TimeEventSystem::~TimeEventSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10061820);
	shutdown();
}
void TimeEventSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x100617a0);
	reset();
}
bool TimeEventSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x10061840);
	return save(file) == 1;
}
bool TimeEventSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x10061f90);
	return load(saveFile) == 1;
}
void TimeEventSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x100620c0);
	advanceTime(time);
}
const std::string &TimeEventSystem::GetName() const {
	static std::string name("TimeEvent");
	return name;
}

//*****************************************************************************
//* Rumor
//*****************************************************************************

RumorSystem::RumorSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1005f960);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Rumor");
	}
}
RumorSystem::~RumorSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1005f9d0);
	shutdown();
}
void RumorSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x101f5850);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system Rumor");
	}
}
bool RumorSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x101f5850);
	return save(file) == 1;
}
bool RumorSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x101f5850);
	return load(saveFile) == 1;
}
const std::string &RumorSystem::GetName() const {
	static std::string name("Rumor");
	return name;
}

//*****************************************************************************
//* Quest
//*****************************************************************************

QuestSystem::QuestSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1005f660);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Quest");
	}
}
QuestSystem::~QuestSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1005f2d0);
	shutdown();
}
void QuestSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x1005f310);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system Quest");
	}
}
void QuestSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1005f2a0);
	reset();
}
bool QuestSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1005f3a0);
	return save(file) == 1;
}
bool QuestSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1005f320);
	return load(saveFile) == 1;
}
const std::string &QuestSystem::GetName() const {
	static std::string name("Quest");
	return name;
}

//*****************************************************************************
//* AI
//*****************************************************************************

AISystem::AISystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10056d50);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system AI");
	}
}
void AISystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10056e30);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system AI");
	}
}
const std::string &AISystem::GetName() const {
	static std::string name("AI");
	return name;
}

//*****************************************************************************
//* Anim
//*****************************************************************************

AnimSystem::AnimSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10016bb0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Anim");
	}
}
AnimSystem::~AnimSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1000c110);
	shutdown();
}
void AnimSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1000c120);
	reset();
}
bool AnimSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1001cab0);
	return save(file) == 1;
}
bool AnimSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1001d250);
	return load(saveFile) == 1;
}
const std::string &AnimSystem::GetName() const {
	static std::string name("Anim");
	return name;
}

//*****************************************************************************
//* AnimPrivate
//*****************************************************************************

AnimPrivateSystem::AnimPrivateSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10055280);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system AnimPrivate");
	}
}
AnimPrivateSystem::~AnimPrivateSystem() {
	auto shutdown = temple::GetPointer<void()>(0x100552f0);
	shutdown();
}
void AnimPrivateSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10054dd0);
	reset();
}
const std::string &AnimPrivateSystem::GetName() const {
	static std::string name("AnimPrivate");
	return name;
}

//*****************************************************************************
//* Reputation
//*****************************************************************************

ReputationSystem::ReputationSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10054b00);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Reputation");
	}
}
ReputationSystem::~ReputationSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10054240);
	shutdown();
}
void ReputationSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x100542a0);
	reset();
}
bool ReputationSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x100542d0);
	return save(file) == 1;
}
bool ReputationSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100542f0);
	return load(saveFile) == 1;
}
const std::string &ReputationSystem::GetName() const {
	static std::string name("Reputation");
	return name;
}

//*****************************************************************************
//* Reaction
//*****************************************************************************

ReactionSystem::ReactionSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10053bd0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Reaction");
	}
}
ReactionSystem::~ReactionSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10053c50);
	shutdown();
}
const std::string &ReactionSystem::GetName() const {
	static std::string name("Reaction");
	return name;
}

//*****************************************************************************
//* TileScript
//*****************************************************************************

TileScriptSystem::TileScriptSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10053980);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system TileScript");
	}
}
TileScriptSystem::~TileScriptSystem() {
	auto shutdown = temple::GetPointer<void()>(0x100539d0);
	shutdown();
}
const std::string &TileScriptSystem::GetName() const {
	static std::string name("TileScript");
	return name;
}

//*****************************************************************************
//* SectorScript
//*****************************************************************************

SectorScriptSystem::SectorScriptSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x101f5850);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system SectorScript");
	}
}
const std::string &SectorScriptSystem::GetName() const {
	static std::string name("SectorScript");
	return name;
}

//*****************************************************************************
//* WP
//*****************************************************************************

WPSystem::WPSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100533c0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system WP");
	}
}
WPSystem::~WPSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10053410);
	shutdown();
}
void WPSystem::ResetBuffers(const RebuildBufferInfo& rebuildInfo) {
	auto resetBuffers = temple::GetPointer<void(const RebuildBufferInfo*)>(0x10053430);
	resetBuffers(&rebuildInfo);
}
const std::string &WPSystem::GetName() const {
	static std::string name("WP");
	return name;
}

//*****************************************************************************
//* InvenSource
//*****************************************************************************

InvenSourceSystem::InvenSourceSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10053220);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system InvenSource");
	}
}
InvenSourceSystem::~InvenSourceSystem() {
	auto shutdown = temple::GetPointer<void()>(0x100525f0);
	shutdown();
}
const std::string &InvenSourceSystem::GetName() const {
	static std::string name("InvenSource");
	return name;
}

//*****************************************************************************
//* TownMap
//*****************************************************************************

void TownMapSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10051cd0);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system TownMap");
	}
}
void TownMapSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x10052130);
	unloadModule();
}
void TownMapSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10052100);
	reset();
}
const std::string &TownMapSystem::GetName() const {
	static std::string name("TownMap");
	return name;
}

//*****************************************************************************
//* GMovie
//*****************************************************************************

void GMovieSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x10033d90);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system GMovie");
	}
}
void GMovieSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x10033dc0);
	unloadModule();
}
const std::string &GMovieSystem::GetName() const {
	static std::string name("GMovie");
	return name;
}

//*****************************************************************************
//* Brightness
//*****************************************************************************

BrightnessSystem::BrightnessSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10051ca0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Brightness");
	}
}
const std::string &BrightnessSystem::GetName() const {
	static std::string name("Brightness");
	return name;
}

//*****************************************************************************
//* GFade
//*****************************************************************************

GFadeSystem::GFadeSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100519e0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system GFade");
	}
}
void GFadeSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x10051a10);
	advanceTime(time);
}
const std::string &GFadeSystem::GetName() const {
	static std::string name("GFade");
	return name;
}

//*****************************************************************************
//* AntiTeleport
//*****************************************************************************

AntiTeleportSystem::AntiTeleportSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10051830);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system AntiTeleport");
	}
}
AntiTeleportSystem::~AntiTeleportSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10051870);
	shutdown();
}
void AntiTeleportSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x100518c0);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system AntiTeleport");
	}
}
void AntiTeleportSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x10051990);
	unloadModule();
}
const std::string &AntiTeleportSystem::GetName() const {
	static std::string name("AntiTeleport");
	return name;
}

//*****************************************************************************
//* Trap
//*****************************************************************************

TrapSystem::TrapSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10050da0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Trap");
	}
}
TrapSystem::~TrapSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10050940);
	shutdown();
}
const std::string &TrapSystem::GetName() const {
	static std::string name("Trap");
	return name;
}

//*****************************************************************************
//* MonsterGen
//*****************************************************************************

MonsterGenSystem::MonsterGenSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100500c0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system MonsterGen");
	}
}
MonsterGenSystem::~MonsterGenSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10050160);
	shutdown();
}
void MonsterGenSystem::ResetBuffers(const RebuildBufferInfo& rebuildInfo) {
	auto resetBuffers = temple::GetPointer<void(const RebuildBufferInfo*)>(0x10050170);
	resetBuffers(&rebuildInfo);
}
void MonsterGenSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10050140);
	reset();
}
bool MonsterGenSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x100501d0);
	return save(file) == 1;
}
bool MonsterGenSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100501a0);
	return load(saveFile) == 1;
}
const std::string &MonsterGenSystem::GetName() const {
	static std::string name("MonsterGen");
	return name;
}

//*****************************************************************************
//* Party
//*****************************************************************************

PartySystem::PartySystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1002b9d0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Party");
	}
}
void PartySystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1002ac00);
	reset();
}
bool PartySystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1002ac70);
	return save(file) == 1;
}
bool PartySystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1002ad80);
	return load(saveFile) == 1;
}
const std::string &PartySystem::GetName() const {
	static std::string name("Party");
	return name;
}

//*****************************************************************************
//* D20LoadSave
//*****************************************************************************

bool D20LoadSaveSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1004fb70);
	return save(file) == 1;
}
bool D20LoadSaveSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1004fbd0);
	return load(saveFile) == 1;
}
const std::string &D20LoadSaveSystem::GetName() const {
	static std::string name("D20LoadSave");
	return name;
}

//*****************************************************************************
//* GameInit
//*****************************************************************************

GameInitSystem::GameInitSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1004c610);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system GameInit");
	}
}
GameInitSystem::~GameInitSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1004c690);
	shutdown();
}
void GameInitSystem::LoadModule() {
	auto loadModule = temple::GetPointer<int()>(0x1004c6a0);
	if (!loadModule()) {
		throw TempleException("Unable to load module data for game system GameInit");
	}
}
void GameInitSystem::UnloadModule() {
	auto unloadModule = temple::GetPointer<void()>(0x1004c850);
	unloadModule();
}
void GameInitSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1004c660);
	reset();
}
const std::string &GameInitSystem::GetName() const {
	static std::string name("GameInit");
	return name;
}

//*****************************************************************************
//* Ground
//*****************************************************************************

GroundSystem::GroundSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1002c100);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Ground");
	}
}
GroundSystem::~GroundSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1002dc40);
	shutdown();
}
void GroundSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1002dbb0);
	reset();
}
void GroundSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x1002c290);
	advanceTime(time);
}
const std::string &GroundSystem::GetName() const {
	static std::string name("Ground");
	return name;
}

//*****************************************************************************
//* ObjFade
//*****************************************************************************

ObjFadeSystem::ObjFadeSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1004c130);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system ObjFade");
	}
}
ObjFadeSystem::~ObjFadeSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1004c170);
	shutdown();
}
void ObjFadeSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1004c190);
	reset();
}
bool ObjFadeSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x1004c1c0);
	return save(file) == 1;
}
bool ObjFadeSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x1004c220);
	return load(saveFile) == 1;
}
const std::string &ObjFadeSystem::GetName() const {
	static std::string name("ObjFade");
	return name;
}

//*****************************************************************************
//* Deity
//*****************************************************************************

DeitySystem::DeitySystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1004a760);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Deity");
	}
}
DeitySystem::~DeitySystem() {
	auto shutdown = temple::GetPointer<void()>(0x1004a800);
	shutdown();
}
const std::string &DeitySystem::GetName() const {
	static std::string name("Deity");
	return name;
}

//*****************************************************************************
//* UiArtManager
//*****************************************************************************

UiArtManagerSystem::UiArtManagerSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1004a610);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system UiArtManager");
	}
}
UiArtManagerSystem::~UiArtManagerSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1004a250);
	shutdown();
}
const std::string &UiArtManagerSystem::GetName() const {
	static std::string name("UiArtManager");
	return name;
}

//*****************************************************************************
//* ParticleSys
//*****************************************************************************

ParticleSysSystem::ParticleSysSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x1004a150);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system ParticleSys");
	}
}
ParticleSysSystem::~ParticleSysSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10049b60);
	shutdown();
}
void ParticleSysSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x10049af0);
	advanceTime(time);
}
const std::string &ParticleSysSystem::GetName() const {
	static std::string name("ParticleSys");
	return name;
}

//*****************************************************************************
//* Cheats
//*****************************************************************************

CheatsSystem::CheatsSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10048a60);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Cheats");
	}
}
const std::string &CheatsSystem::GetName() const {
	static std::string name("Cheats");
	return name;
}

//*****************************************************************************
//* D20Rolls
//*****************************************************************************

D20RollsSystem::D20RollsSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100475d0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system D20Rolls");
	}
}
D20RollsSystem::~D20RollsSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10047150);
	shutdown();
}
void D20RollsSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10047160);
	reset();
}
bool D20RollsSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x100471a0);
	return save(file) == 1;
}
bool D20RollsSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100471e0);
	return load(saveFile) == 1;
}
const std::string &D20RollsSystem::GetName() const {
	static std::string name("D20Rolls");
	return name;
}

//*****************************************************************************
//* Secretdoor
//*****************************************************************************

SecretdoorSystem::SecretdoorSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10046370);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Secretdoor");
	}
}
void SecretdoorSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10046390);
	reset();
}
bool SecretdoorSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x100463b0);
	return save(file) == 1;
}
bool SecretdoorSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x10046400);
	return load(saveFile) == 1;
}
const std::string &SecretdoorSystem::GetName() const {
	static std::string name("Secretdoor");
	return name;
}

//*****************************************************************************
//* MapFogging
//*****************************************************************************

MapFoggingSystem::MapFoggingSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10032020);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system MapFogging");
	}
}
MapFoggingSystem::~MapFoggingSystem() {
	auto shutdown = temple::GetPointer<void()>(0x1002eb80);
	shutdown();
}
void MapFoggingSystem::ResetBuffers(const RebuildBufferInfo& rebuildInfo) {
	auto resetBuffers = temple::GetPointer<void(const RebuildBufferInfo*)>(0x10032290);
	resetBuffers(&rebuildInfo);
}
void MapFoggingSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x1002ebd0);
	reset();
}
const std::string &MapFoggingSystem::GetName() const {
	static std::string name("MapFogging");
	return name;
}

//*****************************************************************************
//* RandomEncounter
//*****************************************************************************

RandomEncounterSystem::RandomEncounterSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100457b0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system RandomEncounter");
	}
}
RandomEncounterSystem::~RandomEncounterSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10045830);
	shutdown();
}
bool RandomEncounterSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x101f5850);
	return save(file) == 1;
}
bool RandomEncounterSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100458c0);
	return load(saveFile) == 1;
}
const std::string &RandomEncounterSystem::GetName() const {
	static std::string name("RandomEncounter");
	return name;
}

//*****************************************************************************
//* ObjectEvent
//*****************************************************************************

ObjectEventSystem::ObjectEventSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10045110);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system ObjectEvent");
	}
}
ObjectEventSystem::~ObjectEventSystem() {
	auto shutdown = temple::GetPointer<void()>(0x10045140);
	shutdown();
}
void ObjectEventSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10045160);
	reset();
}
bool ObjectEventSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x100456d0);
	return save(file) == 1;
}
bool ObjectEventSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100451b0);
	return load(saveFile) == 1;
}
void ObjectEventSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x10045740);
	advanceTime(time);
}
const std::string &ObjectEventSystem::GetName() const {
	static std::string name("ObjectEvent");
	return name;
}

//*****************************************************************************
//* Formation
//*****************************************************************************

FormationSystem::FormationSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100437c0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system Formation");
	}
}
void FormationSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10043250);
	reset();
}
bool FormationSystem::SaveGame(TioFile *file) {
	auto save = temple::GetPointer<int(TioFile*)>(0x10043270);
	return save(file) == 1;
}
bool FormationSystem::LoadGame(GameSystemSaveFile* saveFile) {
	auto load = temple::GetPointer<int(GameSystemSaveFile*)>(0x100432e0);
	return load(saveFile) == 1;
}
const std::string &FormationSystem::GetName() const {
	static std::string name("Formation");
	return name;
}

//*****************************************************************************
//* ItemHighlight
//*****************************************************************************

ItemHighlightSystem::ItemHighlightSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x100431b0);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system ItemHighlight");
	}
}
void ItemHighlightSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x100431d0);
	reset();
}
void ItemHighlightSystem::AdvanceTime(uint32_t time) {
	auto advanceTime = temple::GetPointer<void(uint32_t)>(0x100431f0);
	advanceTime(time);
}
const std::string &ItemHighlightSystem::GetName() const {
	static std::string name("ItemHighlight");
	return name;
}

//*****************************************************************************
//* PathX
//*****************************************************************************

PathXSystem::PathXSystem(const GameSystemConf &config) {
	auto startup = temple::GetPointer<int(const GameSystemConf*)>(0x10042a90);
	if (!startup(&config)) {
		throw TempleException("Unable to initialize game system PathX");
	}
}
void PathXSystem::Reset() {
	auto reset = temple::GetPointer<void()>(0x10042aa0);
	reset();
}
const std::string &PathXSystem::GetName() const {
	static std::string name("PathX");
	return name;
}