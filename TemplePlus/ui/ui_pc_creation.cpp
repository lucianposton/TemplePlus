#include "stdafx.h"
#include "common.h"
#include <EASTL/hash_map.h>
#include <EASTL/fixed_string.h>
#include "config/config.h"
#include "d20.h"
#include "feat.h"
#include "ui_char_editor.h"
#include "obj.h"
#include "ui/ui.h"
#include "ui_render.h"
#include "util/fixes.h"
#include "gamesystems/gamesystems.h"
#include <tig/tig_texture.h>
#include <tig/tig_font.h>
#include "graphics/imgfile.h"
#include "graphics/render_hooks.h"

#include "party.h"
#include "tig/tig_msg.h"
#include <location.h>
#include <tio/tio.h>
#include <mod_support.h>
#include <gamesystems/d20/d20stats.h>
#include <gamesystems/objects/objsystem.h>
#include <gamesystems/d20/d20_help.h>
#include <critter.h>
#include <infrastructure/elfhash.h>
#include <condition.h>
#include <tig/tig_mouse.h>
#include <gamesystems/deity/legacydeitysystem.h>
#include <infrastructure/keyboard.h>
#include "combat.h"
#include "ui_assets.h"


enum ChargenStages : int {
	CG_Stage_Stats = 0,
	CG_Stage_Race,
	CG_Stage_Gender,
	CG_Stage_Height,
	CG_Stage_Hair,
	CG_Stage_Class,
	CG_Stage_Alignment,
	CG_Stage_Deity,
	CG_Stage_Abilities,
	CG_Stage_Feats,
	CG_Stage_Skills,
	CG_Stage_Spells,
	CG_Stage_Portrait,
	CG_Stage_Voice,

	CG_STAGE_COUNT
};

int PcCreationFeatUiPrereqCheckUsercallWrapper();
int(__cdecl * OrgFeatMultiselectSub_101822A0)();
int HookedFeatMultiselectSub_101822A0(feat_enums feat);
int HookedUsercallFeatMultiselectSub_101822A0();

struct PartyCreationPc
{
	int flags;
	int field4;
	int* field8;
	int fieldC;
	ObjectId objId;
	int field28;
	char* nameMaybe;
	char fileName[260];
	int field134;
	int field138;
	int objTypeMaybe;
	int field140;
	Alignment alignment;
	int field148;
	int field14C;
	objHndl handle;
};

const int testSizeOfCreationPc = sizeof(PartyCreationPc); // 344  0x158

struct ChargenSystem{ // incomplete
	const char* name;
	void(__cdecl *reset)(CharEditorSelectionPacket & charSpec);
	void(__cdecl *activate)();
	BOOL(__cdecl *systemInit)(GameSystemConf *);
	void(__cdecl*free)();
	int (__cdecl*resize)(UiResizeArgs &resizeArgs);
	void(__cdecl *hide)();
	void(__cdecl *show)();
	int(__cdecl *checkComplete)(); // checks if the char editing stage is complete (thus allowing you to move on to the next stage). This is checked at every render call.
	int field24;
	void(__cdecl *buttonExited)();
};


class UiPcCreation{
	friend class PcCreationHooks;

public:
	objHndl GetEditedChar();
	CharEditorSelectionPacket & GetCharEditorSelPacket();
	LgcyWindow &GetPcCreationWnd();
	LgcyWindow &GetStatsWnd();
	int &GetState();
	Alignment GetPartyAlignment();

	void PrepareNextStages();
	void BtnStatesUpdate(int systemId);
	void ResetNextStages(int systemId); // activates the reset callback for all the subsequent stages
	void ToggleClassRelatedStages(); // Spell Selection and Class Features

#pragma region Systems
	// Class
	BOOL ClassSystemInit(GameSystemConf & conf);
	BOOL ClassWidgetsInit();
	void ClassWidgetsFree();
	BOOL ClassShow();
	BOOL ClassHide();
	BOOL ClassWidgetsResize(UiResizeArgs & args);
	BOOL ClassCheckComplete();
	void ClassBtnEntered();
	void ClassActivate();
	void ClassFinalize(CharEditorSelectionPacket & selPkt, objHndl & handle);

	// Feats
	BOOL FeatsSystemInit(GameSystemConf & conf);
	BOOL FeatsWidgetsInit(int w, int h);
	void FeatsFree();
	void FeatWidgetsFree();
	BOOL FeatsWidgetsResize(UiResizeArgs &args);
	BOOL FeatsShow();
	BOOL FeatsHide();
	void FeatsActivate();
	BOOL FeatsCheckComplete();
	void FeatsFinalize(CharEditorSelectionPacket& selPkt, objHndl & handle);
	void FeatsReset(CharEditorSelectionPacket& selPkt);

	// Spells
	BOOL SpellsSystemInit(GameSystemConf & conf);
	void SpellsFree();
	BOOL SpellsWidgetsInit();
	void SpellsReset();
	void SpellsWidgetsFree();
	BOOL SpellsShow();
	BOOL SpellsHide();
	BOOL SpellsWidgetsResize(UiResizeArgs &args);
	void SpellsActivate();
	BOOL SpellsCheckComplete();
	void SpellsFinalize();
	void SpellsReset(CharEditorSelectionPacket& selPkt);
#pragma endregion

#pragma region Widget callbacks
	void StateTitleRender(int widId);

	// stats
	int GetRolledStatIdx(int x, int y, int *xyOut = nullptr); // gets the index of the Rolled Stats button according to the mouse position. Returns -1 if none.
	BOOL StatsWndMsg(int widId, TigMsg *msg);

	// class
	void ClassBtnRender(int widId);
	BOOL ClassBtnMsg(int widId, TigMsg* msg);
	BOOL ClassNextBtnMsg(int widId, TigMsg* msg);
	BOOL ClassPrevBtnMsg(int widId, TigMsg* msg);
	BOOL FinishBtnMsg(int widId, TigMsg* msg); // goes after the original FinishBtnMsg
	void ClassNextBtnRender(int widId);
	void ClassPrevBtnRender(int widId);

	// Feats
	BOOL FeatsWndMsg(int widId, TigMsg* msg);
	void FeatsWndRender(int widId);

	BOOL FeatsEntryBtnMsg(int widId, TigMsg* msg);
	void FeatsEntryBtnRender(int widId);
	BOOL FeatsExistingBtnMsg(int widId, TigMsg* msg);
	void FeatsExistingBtnRender(int widId);

	void FeatsMultiSelectWndRender(int widId);
	BOOL FeatsMultiSelectWndMsg(int widId, TigMsg* msg);
	void FeatsMultiOkBtnRender(int widId);
	BOOL FeatsMultiOkBtnMsg(int widId, TigMsg* msg);
	void FeatsMultiCancelBtnRender(int widId);
	BOOL FeatsMultiCancelBtnMsg(int widId, TigMsg* msg);
	void FeatsMultiBtnRender(int widId);
	BOOL FeatsMultiBtnMsg(int widId, TigMsg* msg);

	// spells
	void SpellsWndRender(int widId);
	BOOL SpellsWndMsg(int widId, TigMsg* msg);
	void SpellsPerDayUpdate();
	BOOL SpellsEntryBtnMsg(int widId, TigMsg* msg);
	void SpellsEntryBtnRender(int widId);

	BOOL SpellsAvailableEntryBtnMsg(int widId, TigMsg* msg);
	void SpellsAvailableEntryBtnRender(int widId);
#pragma endregion


	// state
	int classWndPage = 0;
	eastl::vector<int> classBtnMapping; // used as an index of choosable character classes
	int GetClassWndPage();
	Stat GetClassCodeFromWidgetAndPage(int idx, int page);
	int GetStatesComplete();

#pragma region logic 
	// class
	void ClassSetPermissibles();

	// feats
	bool IsSelectingNormalFeat(); // the normal feat you get every 3rd level in 3.5ed
	bool IsSelectingSecondFeat(); // currently racial bonus for humans
	bool IsSelectingBonusFeat(); // selecting a class bonus feat

	// deity
	void DeitySetPermissibles();
#pragma endregion 

	// utilities
	bool IsCastingStatSufficient(Stat classEnum);
	bool IsAlignmentOk(Stat classEnums); // checks if class is compatible with the selected party alignment
	void ClassScrollboxTextSet(Stat classEnum); // sets the chargen textbox to the class's short description from stat.mes
	void ButtonEnteredHandler(int helpId);
	int GetNewLvl(Stat classEnum = stat_level);


	// Feat Utilities
	std::string GetFeatName(feat_enums feat); // includes strings for Mutli-selection feat categories e.g. FEAT_WEAPON_FOCUS
	TigTextStyle & GetFeatStyle(feat_enums feat, bool allowMultiple = true);
	bool FeatAlreadyPicked(feat_enums feat);
	bool FeatCanPick(feat_enums feat);
	bool IsSelectingRangerSpec();
	bool IsClassBonusFeat(feat_enums feat);
	bool IsBonusFeatDisregardingPrereqs(feat_enums feat);

	void FeatsSanitize();
	void FeatsMultiSelectActivate(feat_enums feat);
	feat_enums FeatsMultiGetFirst(feat_enums feat); // first alphabetical

													// widget IDs
	int classWndId = 0;
	int classNextBtn = 0, classPrevBtn = 0;
	eastl::vector<int> classBtnIds;

	// geometry
	TigRect classNextBtnRect, classNextBtnFrameRect, classNextBtnTextRect,
		classPrevBtnRect, classPrevBtnFrameRect, classPrevBtnTextRect;
	TigRect spellsChosenTitleRect, spellsAvailTitleRect;
	TigRect spellsPerDayTitleRect;
	int featsMultiCenterX, featsMultiCenterY;
	TigRect featMultiOkRect, featMultiOkTextRect, featMultiCancelRect, featMultiCancelTextRect, featMultiTitleRect;
	TigRect featsAvailTitleRect, featsTitleRect, featsExistingTitleRect, featsClassBonusRect;
	TigRect featsSelectedBorderRect, featsSelected2BorderRect, featsClassBonusBorderRect, feat0TextRect, feat1TextRect, feat2TextRect;

	int featsMainWndId = 0, featsMultiSelectWndId = 0;
	int featsScrollbarId = 0, featsExistingScrollbarId = 0, featsMultiSelectScrollbarId = 0;
	int featsScrollbarY = 0, featsExistingScrollbarY = 0, featsMultiSelectScrollbarY = 0;
	LgcyWindow featsMainWnd, featsMultiSelectWnd;
	LgcyScrollBar featsScrollbar, featsExistingScrollbar, featsMultiSelectScrollbar;
	eastl::vector<int> featsAvailBtnIds, featsExistingBtnIds, featsMultiSelectBtnIds;
	int featsMultiOkBtnId = 0, featsMultiCancelBtnId = 0;
	const int FEATS_AVAIL_BTN_COUNT = 15; // vanilla 15
	const int FEATS_AVAIL_BTN_HEIGHT = 12; // vanilla 11
	const int FEATS_EXISTING_BTN_COUNT = 7; // vanilla 8
	const int FEATS_EXISTING_BTN_HEIGHT = 13; // vanilla 12
	const int FEATS_MULTI_BTN_COUNT = 15;
	const int FEATS_MULTI_BTN_HEIGHT = 12;
	std::string featsAvailTitleString, featsExistingTitleString;
	std::string featsTitleString;
	std::string featsClassBonusTitleString;

	const int DEITY_BTN_COUNT = 20;

	int spellsWndId = 0;
	LgcyWindow spellsWnd;
	LgcyScrollBar spellsScrollbar, spellsScrollbar2;
	int spellsScrollbarId = 0, spellsScrollbar2Id = 0;
	int spellsScrollbarY = 0, spellsScrollbar2Y = 0;
	eastl::vector<int> spellsAvailBtnIds, spellsChosenBtnIds;
	const int SPELLS_BTN_COUNT = 11; // vanilla had 12, decreasing this to increase the font
	const int SPELLS_BTN_HEIGHT = 13; // vanilla was 12 (so 13*11 = 143 ~= 144 vanilla)
	std::string spellsAvailLabel;
	std::string spellsChosenLabel;
	std::string spellsPerDayLabel;
	const int SPELLS_PER_DAY_BOXES_COUNT = 6;


	// caches
	eastl::hash_map<int, eastl::string> classNamesUppercase;
	eastl::vector<TigRect> classBtnFrameRects;
	eastl::vector<TigRect> classBtnRects;
	eastl::vector<TigRect> classTextRects;
	eastl::vector<TigRect> featsMultiBtnRects;
	eastl::vector<TigRect> featsBtnRects, featsExistingBtnRects;
	eastl::hash_map<int, std::string> featsMasterFeatStrings;
	eastl::vector<string> spellsPerDayTexts;
	eastl::vector<TigRect> spellsPerDayTextRects;
	eastl::vector<TigRect> spellsPerDayBorderRects;
	eastl::vector<TigRect> spellsLevelLabelRects;


	// art assets
	int buttonBox = 0;
	ColorRect genericShadowColor = ColorRect(0xFF000000);
	ColorRect whiteColorRect = ColorRect(0xFFFFffff);
	ColorRect blueColorRect = ColorRect(0xFF0000ff);
	ColorRect darkGreenColorRect = ColorRect(0xFF006003);
	ColorRect classBtnShadowColor = ColorRect(0xFF000000);
	ColorRect classBtnColorRect = ColorRect(0xFFFFffff);
	TigTextStyle whiteTextGenericStyle;
	TigTextStyle blueTextStyle;
	TigTextStyle classBtnTextStyle;
	TigTextStyle featsGreyedStyle, featsBonusTextStyle, featsNormalTextStyle, featsExistingTitleStyle, featsGoldenStyle, featsClassStyle, featsCenteredStyle;
	TigTextStyle spellsTextStyle;
	TigTextStyle spellsTitleStyle;
	TigTextStyle spellLevelLabelStyle;
	TigTextStyle spellsAvailBtnStyle;
	TigTextStyle spellsPerDayStyle;
	TigTextStyle spellsPerDayTitleStyle;
	CombinedImgFile* levelupSpellbar, *featsbackdrop;


	CharEditorClassSystem& GetClass() const {
		Expects(!!mClass);
		return *mClass;
	}

	UiPcCreation() {
		TigTextStyle baseStyle;
		baseStyle.flags = 0x4000;
		baseStyle.field2c = -1;
		baseStyle.shadowColor = &genericShadowColor;
		baseStyle.field0 = 0;
		baseStyle.kerning = 1;
		baseStyle.leading = 0;
		baseStyle.tracking = 3;
		baseStyle.textColor = baseStyle.colors2 = baseStyle.colors4 = &whiteColorRect;
		whiteTextGenericStyle = baseStyle;

		blueTextStyle = baseStyle;
		blueTextStyle.colors4 = blueTextStyle.colors2 = blueTextStyle.textColor = &blueColorRect;
	}

private:
	int mPageCount = 0;

	bool mFeatsActivated = false;
	bool mIsSelectingBonusFeat = false;
	bool mBonusFeatOk = false;
	feat_enums featsMultiSelected = FEAT_NONE, mFeatsMultiMasterFeat = FEAT_NONE;

	std::unique_ptr<CharEditorClassSystem> mClass;
	std::vector<FeatInfo> mExistingFeats, mSelectableFeats, mMultiSelectFeats, mMultiSelectMasterFeats;

	int &GetDeityBtnId(int deityId);

} uiPcCreation;

struct PcCreationUiAddresses : temple::AddressTable
{

	int * mainWindowWidgetId; // 10BF0ED4
	int * pcPortraitsWidgetIds; // 10BF0EBC  array of 5 entries


	int * dword_10C75F30;
	int * featsMultiselectNum_10C75F34;
	feat_enums * featMultiselect_10C75F38;
	int *dword_10C76AF0;
	LgcyWidget* widg_10C77CD0;
	int * dword_10C77D50;
	int * dword_10C77D54;
	int *widIdx_10C77D80;
	feat_enums * featsMultiselectList;
	feat_enums * feat_10C79344;
	int * widgId_10C7AE14;
	char* (__cdecl*sub_10182760)(feat_enums featEnums);
	int(__cdecl*sub_101F87B0)(int widIdx, LgcyWidget* widg);
	int(__cdecl*sub_101F8E40)(int);
	
	CharEditorSelectionPacket * charEdSelPkt;
	MesHandle* pcCreationMes;

	PcCreationUiAddresses()
	{
		rebase(dword_10C75F30, 0x10C75F30);
		rebase(featsMultiselectNum_10C75F34, 0x10C75F34);
		rebase(featMultiselect_10C75F38, 0x10C75F38);
		rebase(dword_10C76AF0, 0x10C76AF0);
		rebase(widg_10C77CD0, 0x10C77CD0);
		rebase(dword_10C77D50, 0x10C77D50);
		rebase(dword_10C77D54, 0x10C77D54);
		rebase(widIdx_10C77D80, 0x10C77D80);
		rebase(featsMultiselectList, 0x10C78920);
		rebase(feat_10C79344, 0x10C79344);
		rebase(widgId_10C7AE14, 0x10C7AE14);

		rebase(sub_10182760, 0x10182760);
		rebase(sub_101F87B0, 0x101F87B0);
		rebase(sub_101F8E40, 0x101F8E40);

		rebase(pcCreationMes, 0x11E72EF0);
		rebase(charEdSelPkt, 0x11E72F00);
	}

} addresses;


class PcCreationHooks : TempleFix
{
public:

	static CharEditorSelectionPacket &GetCharEdSelPkt();

	static void GetPartyPool(int fromIngame); //fromIngame is 0 when launching from main menu, 1 when launching from inn guestbook
	static int PartyPoolLoader();


	static BOOL StatsIncreaseBtnMsg(int widId, TigMsg* msg);
	static BOOL StatsDecreaseBtnMsg(int widId, TigMsg* msg);
	static BOOL StatsUpdateBtns();
	static void AutoAddSpellsMemorizedToGroup();

	void apply() override
	{

		replaceFunction(0x1011F290, AutoAddSpellsMemorizedToGroup);

		static BOOL(__cdecl*orgStatsWndMsg)(int, TigMsg*) = replaceFunction<BOOL(int, TigMsg*)>(0x1018BA50, [](int widId, TigMsg*msg){
			if (!uiPcCreation.StatsWndMsg(widId, msg))
				return orgStatsWndMsg(widId, msg);
			return TRUE;
		});

		// Chargen Class system
		replaceFunction<void(__cdecl)(GameSystemConf&)>(0x10188910, [](GameSystemConf& conf) {uiPcCreation.ClassSystemInit(conf); });
		replaceFunction<void(__cdecl)()>(0x101885E0, []() {uiPcCreation.ClassWidgetsFree(); });
		replaceFunction<void(__cdecl)()>(0x101885D0, []() {uiPcCreation.ClassActivate(); });
		replaceFunction<void(__cdecl)(UiResizeArgs&)>(0x101889F0, [](UiResizeArgs& args) {uiPcCreation.ClassWidgetsResize(args); });
		replaceFunction<void(__cdecl)()>(0x101880F0, []() {uiPcCreation.ClassShow(); });
		replaceFunction<void(__cdecl)()>(0x101880D0, []() {uiPcCreation.ClassHide(); });
		replaceFunction<void(__cdecl)(CharEditorSelectionPacket& , objHndl& )>(0x10188110, [](CharEditorSelectionPacket& selPkt, objHndl& handle) {uiPcCreation.ClassFinalize(selPkt, handle); });
		// replaceFunction<void(__cdecl)()>(0x101B0620, []() {uiPcCreation.ClassCheckComplete(); }); // same function as ui_char_editor, already replaced
		replaceFunction<void(__cdecl)()>(0x10188260, []() {uiPcCreation.ClassBtnEntered(); });
		
		// Skill

		// Hook for SkillIncreaseBtnMsg to raise 4 times when ctrl/alt is pressed
		static BOOL(__cdecl*orgSkillIncBtnMsg)(int, TigMsg*) = replaceFunction<BOOL(__cdecl)(int,TigMsg*)>(0x101815C0, [](int widId, TigMsg* msg){
			if (msg->type != TigMsgType::WIDGET)
				return FALSE;
			auto widMsg = (TigMsgWidget*)msg;
			if (widMsg->widgetEventType != TigMsgWidgetEvent::MouseReleased)
				return FALSE;

			if (infrastructure::gKeyboard.IsKeyPressed(VK_CONTROL) || infrastructure::gKeyboard.IsKeyPressed(VK_LCONTROL)
				|| infrastructure::gKeyboard.IsKeyPressed(VK_LMENU) || infrastructure::gKeyboard.IsKeyPressed(VK_RMENU)){
				orgSkillIncBtnMsg(widId, msg);
				int safetyCounter = 3;
				while (uiManager->GetButtonState(widId) != LgcyButtonState::Disabled && safetyCounter >= 0){
					orgSkillIncBtnMsg(widId, msg);
					safetyCounter--;
				}
				return TRUE;
			};
			return orgSkillIncBtnMsg(widId, msg);
		});

		// Feats
		replaceFunction<void(__cdecl)(GameSystemConf&)>(0x101847F0, [](GameSystemConf& conf) {uiPcCreation.FeatsSystemInit(conf); });
		replaceFunction<void(__cdecl)()>(0x10182D30, []() {uiPcCreation.FeatsFree(); });
		replaceFunction<void(__cdecl)()>(0x10182A30, []() {uiPcCreation.FeatsActivate(); });
		replaceFunction<void(__cdecl)(CharEditorSelectionPacket&)>(0x10181F40, [](CharEditorSelectionPacket& selPkt) {uiPcCreation.FeatsReset(selPkt); });
		replaceFunction<void(__cdecl)(UiResizeArgs&)>(0x10184B70, [](UiResizeArgs& args) {uiPcCreation.FeatsWidgetsResize(args); });
		replaceFunction<void(__cdecl)()>(0x10181F80, []() {uiPcCreation.FeatsShow(); });
		replaceFunction<void(__cdecl)()>(0x10181F60, []() {uiPcCreation.FeatsHide(); });
		replaceFunction<void(__cdecl)(CharEditorSelectionPacket&, objHndl&)>(0x10181FE0, [](CharEditorSelectionPacket& selPkt, objHndl& handle) {uiPcCreation.FeatsFinalize(selPkt, handle); });
		replaceFunction<void(__cdecl)()>(0x10181FA0, []() {uiPcCreation.FeatsCheckComplete(); });

		// Spell system
		replaceFunction<void(__cdecl)(GameSystemConf&)>(0x101800E0, [](GameSystemConf& conf) {uiPcCreation.SpellsSystemInit(conf); });
		replaceFunction<void(__cdecl)()>(0x1017F090, []() {uiPcCreation.SpellsFree(); });
		replaceFunction<void(__cdecl)()>(0x101804A0, []() {uiPcCreation.SpellsActivate(); });
		replaceFunction<void(__cdecl)()>(0x1017EAE0, []() {uiPcCreation.SpellsReset(); });
		replaceFunction<void(__cdecl)(UiResizeArgs&)>(0x10180390, [](UiResizeArgs& args) {uiPcCreation.SpellsWidgetsResize(args); });
		replaceFunction<void(__cdecl)()>(0x1017EB60, []() {uiPcCreation.SpellsShow(); });
		replaceFunction<void(__cdecl)()>(0x1017EB40, []() {uiPcCreation.SpellsHide(); });
		replaceFunction<void(__cdecl)(CharEditorSelectionPacket&, objHndl&)>(0x1017F0A0, [](CharEditorSelectionPacket& selPkt, objHndl& handle) {uiPcCreation.SpellsFinalize(); });
		replaceFunction<void(__cdecl)()>(0x1017EB80, []() {uiPcCreation.SpellsCheckComplete(); });

		// Deity
		replaceFunction<void(__cdecl)()>(0x10187340, []() {uiPcCreation.DeitySetPermissibles(); });

		// lax rules option for unbounded increase of stats & party member alignment
		replaceFunction(0x1018B940, StatsIncreaseBtnMsg);
		replaceFunction(0x1018B9B0, StatsDecreaseBtnMsg);
		replaceFunction(0x1018B570, StatsUpdateBtns);
		replaceFunction<BOOL(Alignment, Alignment)>(0x1011B880, [](Alignment a, Alignment b)->BOOL { 
			if (config.laxRules && config.disableAlignmentRestrictions){
				return TRUE;
			}
				
			return (BOOL)d20Stats.AlignmentsUnopposed(a, b); 
		});

		// PC Creation UI Fixes
		replaceFunction(0x10182E80, PcCreationFeatUiPrereqCheckUsercallWrapper);
		OrgFeatMultiselectSub_101822A0 = (int(__cdecl*)()) replaceFunction(0x101822A0, HookedUsercallFeatMultiselectSub_101822A0);
				
		
		// UiPartyCreationGetPartyPool
		static void(*orgGetPartyPool)(int) = replaceFunction<void(int)>(0x10165E60, GetPartyPool);
		// static int(*orgPartyPoolLoader)() = replaceFunction<int()>(0x10165790, PartyPoolLoader);
		

		if (temple::Dll::GetInstance().HasCo8Hooks() ) {
			writeNoops(0x1011D521); // disabling EXP draw call
		}

		// KotB alignment restrictions
		static BOOL (__cdecl*orgPartyAlignmentChoiceShow)() = replaceFunction<BOOL()>(0x1011E200, []()->BOOL
		{

			auto result = orgPartyAlignmentChoiceShow();
			if (modSupport.IsKotB()){
				auto alignmentBtnIds = temple::GetRef<int[9]>(0x10BDA73C);
				uiManager->SetButtonState(alignmentBtnIds[0], LgcyButtonState::Disabled); // LG
				uiManager->SetButtonState(alignmentBtnIds[2], LgcyButtonState::Disabled); // CG

				uiManager->SetButtonState(alignmentBtnIds[4], LgcyButtonState::Disabled); // TN
				uiManager->SetButtonState(alignmentBtnIds[5], LgcyButtonState::Disabled); // CN

				uiManager->SetButtonState(alignmentBtnIds[6], LgcyButtonState::Disabled); // LE
				uiManager->SetButtonState(alignmentBtnIds[8], LgcyButtonState::Disabled); // CE
			}
			return result;
		});
	}
} pcCreationHooks;

int __declspec(naked) HookedUsercallFeatMultiselectSub_101822A0()
{
	{ __asm push ecx __asm push esi __asm push ebx __asm push edi}
	__asm{
		push eax;
		call HookedFeatMultiselectSub_101822A0;
		add esp, 4;
	}
	{ __asm pop edi __asm pop ebx __asm pop esi __asm pop ecx}
	__asm retn;
}

CharEditorSelectionPacket & PcCreationHooks::GetCharEdSelPkt(){
	return temple::GetRef<CharEditorSelectionPacket>(0x11E72F00);
}

void PcCreationHooks::GetPartyPool(int fromIngame)
{
	int& uiPartypoolWidgetId = temple::GetRef<int>(0x10BF1764);
	int& uiPcCreationMainWndId = temple::GetRef<int>(0x10BDD690);
	int& uiPartyCreationNotFromShopmap = temple::GetRef<int>(0x10BF24E0);
	LocAndOffsets& locToCreatePcs = temple::GetRef<LocAndOffsets>(0x10BF17A8);


	uiManager->SetHidden(uiPartypoolWidgetId, false);
	uiManager->BringToFront(uiPartypoolWidgetId);
	uiManager->BringToFront(uiPcCreationMainWndId);
	uiPartyCreationNotFromShopmap = fromIngame;
	if (fromIngame)
	{
		auto dude = party.GroupListGetMemberN(0);
		if (dude)
		{
			locToCreatePcs = objects.GetLocationFull(dude);
		}
		if (uiPartyCreationNotFromShopmap)
		{
			int& pcCreationPartyAlignment = temple::GetRef<int>(0x11E741A8);
			pcCreationPartyAlignment = party.GetPartyAlignment();
		}
	}

	auto UiPartyCreationHidePcWidgets = temple::GetRef<void()>(0x10135080);
	UiPartyCreationHidePcWidgets();
	int& uiPartyPoolPcsIdx = temple::GetRef<int>(0x10BF1760);
	uiPartyPoolPcsIdx = -1;
	uiManager->SetButtonState(temple::GetRef<int>(0x10BF2408), LgcyButtonState::Disabled); // Add
	uiManager->SetButtonState(temple::GetRef<int>(0x10BF2538), LgcyButtonState::Disabled); // VIEW
	uiManager->SetButtonState(temple::GetRef<int>(0x10BF2410), LgcyButtonState::Disabled); // RENAME
	uiManager->SetButtonState(temple::GetRef<int>(0x10BF239C), LgcyButtonState::Disabled); // DELETE

	auto GetPcCreationPcBuffer = temple::GetRef<void()>(0x101631B0);
	GetPcCreationPcBuffer();
	auto PartyPoolLoader = temple::GetRef<int()>(0x10165790);
	PartyPoolLoader();
	auto AddPcsFromBuffer = temple::GetRef<void()>(0x10163210);
	AddPcsFromBuffer();
	auto UiPartyPoolRefreshTopButtons = temple::GetRef<void()>(0x10165150);
	UiPartyPoolRefreshTopButtons();
	auto PcPortraitsRefresh = temple::GetRef<void()>(0x10163440);
	PcPortraitsRefresh();
	auto UiPartyPoolScrollbox_10164620 = temple::GetRef<void()>(0x10164620);
	UiPartyPoolScrollbox_10164620();

	if (fromIngame || !party.GroupListGetLen())
	{
		uiManager->SetHidden(temple::GetRef<int>(0x10BDB8E0), true);
	}
	else
	{
		uiManager->SetHidden(temple::GetRef<int>(0x10BDB8E0), false);
	}
	auto UiUtilityBarHide = temple::GetRef<void()>(0x1010EEC0);
	UiUtilityBarHide();
}

int PcCreationHooks::PartyPoolLoader()
{
	int result = 1;

	/*auto PartyCreationClearPcs = temple::GetRef<void()>(0x10163E30);
	PartyCreationClearPcs();
	auto IsIronman = temple::GetRef<int()>(0x10003860);
	TioFileList filelist;
	if (IsIronman())
		tio_filelist_create(&filelist, "players\\ironman\\*.ToEEIMan");
	else
		tio_filelist_create(&filelist, "players\\*.ToEEPC");

	
	int& uiPartypoolNumPcs = temple::GetRef<int>(0x10BF237C);
	if (filelist.count)
	{
		auto uiPartyCreationPcs = temple::GetRef<PartyCreationPc*>(0x10BF253C);
		uiPartyCreationPcs = new PartyCreationPc[filelist.count];
		for (int i = 0; i < filelist.count; i++)
		{
			if (!PartyPoolGetPc(&uiPartyCreationPcs[uiPartypoolNumPcs], filelist.files[i].name))
			{
				result = 0;
				break;
			}
			logger->info("Successfully loaded PC ({}) {} ({})", 
				uiPartypoolNumPcs,
				uiPartyCreationPcs[uiPartypoolNumPcs].nameMaybe,
				uiPartyCreationPcs[uiPartypoolNumPcs].objId.ToString());
			uiPartypoolNumPcs++;
		}
	}
	tio_filelist_destroy(&filelist);

	if (result)
	{
		auto partyPoolPcIndices = temple::GetRef<int*>(0x10BF2378);
		if (partyPoolPcIndices)
			free(partyPoolPcIndices);
		partyPoolPcIndices = new int[uiPartypoolNumPcs];
		auto UiPartyPoolRefreshVisibles = temple::GetRef<void()>(0x10164B10);
		UiPartyPoolRefreshVisibles();
	}

	// not yet finished because pug found the bug by the time I got to it :P
	*/

	
	return result;
}
BOOL PcCreationHooks::StatsIncreaseBtnMsg(int widId, TigMsg * msg){

	if (msg->type != TigMsgType::WIDGET)
		return FALSE;

	auto msg_ = (TigMsgWidget*)msg;
	if (msg_->widgetEventType != TigMsgWidgetEvent::MouseReleased)
		return FALSE;

	auto idx = WidgetIdIndexOf(widId, temple::GetRef<int[]>(0x10C45310), 6);
	if (idx == -1)
		return TRUE;

	auto &selPkt = GetCharEdSelPkt();
	auto abilityLvl = selPkt.abilityStats[idx];
	auto cost = 1;
	if (abilityLvl >= 16)
		cost = 3;
	else if (abilityLvl >= 14)
		cost = 2;

	auto &pbPoints = temple::GetRef<int>(0x10C453F4);
	if (pbPoints >= cost && (abilityLvl < 18 || config.laxRules)){
		pbPoints -= cost;
		selPkt.abilityStats[idx]++;
		StatsUpdateBtns();
		temple::GetRef<void(__cdecl)(int)>(0x1011BC70)(0);
	}
	return TRUE;
}

BOOL PcCreationHooks::StatsDecreaseBtnMsg(int widId, TigMsg * msg){

	if (msg->type != TigMsgType::WIDGET)
		return FALSE;

	auto msg_ = (TigMsgWidget*)msg;
	if (msg_->widgetEventType != TigMsgWidgetEvent::MouseReleased)
		return FALSE;

	auto idx = WidgetIdIndexOf(widId, temple::GetRef<int[]>(0x10C44DA8), 6);
	if (idx == -1)
		return TRUE;

	auto &selPkt = GetCharEdSelPkt();
	auto abilityLvl = selPkt.abilityStats[idx];
	auto cost = 1;
	if (abilityLvl >= 17)
		cost = 3;
	else if (abilityLvl >= 15)
		cost = 2;

	auto &pbPoints = temple::GetRef<int>(0x10C453F4);
	if (pbPoints < config.pointBuyPoints && (abilityLvl > 8 || config.laxRules)) {
		pbPoints += cost;
		selPkt.abilityStats[idx]--;
		StatsUpdateBtns();
		temple::GetRef<void(__cdecl)(int)>(0x1011BC70)(0);
	}
	return TRUE;
}

BOOL PcCreationHooks::StatsUpdateBtns(){

	auto textBuf = temple::GetRef<char[]>(0x10C44C34);
	auto &pbPoints = temple::GetRef<int>(0x10C453F4);
	_snprintf(textBuf, 16, "%d@1/%d", pbPoints, config.pointBuyPoints);

	auto isIronman = temple::GetRef<BOOL(__cdecl)()>(0x10003860)();
	auto isPointBuyMode = temple::GetRef<BOOL(__cdecl)()>(0x1011B730)();


	// hide/show basic/advanced toggle button
	if (isIronman){
		if (isPointBuyMode){
			temple::GetRef<void(__cdecl)()>(0x1018B500)(); // pointbuy toggle
		}
		uiManager->SetHidden(temple::GetRef<int>(0x10C44C48), true); // hide toggle button
	}
	else{
		uiManager->SetHidden(temple::GetRef<int>(0x10C44C48), false); // show toggle button
	}

	auto &selPkt = GetCharEdSelPkt();
	for (auto i=0; i < 6; i++){
		auto abLvl = selPkt.abilityStats[i];

		// increase btn
		{
			auto incBtnId = temple::GetRef<int[6]>(0x10C45310)[i];
			auto cost = 1;
			if (abLvl >= 16)
				cost = 3;
			else if (abLvl >= 14)
				cost = 2;
			if (pbPoints < cost || (abLvl == 18 && !config.laxRules))
				uiManager->SetButtonState(incBtnId, LgcyButtonState::Disabled);
			else
				uiManager->SetButtonState(incBtnId, LgcyButtonState::Normal);
			uiManager->SetHidden(incBtnId, isPointBuyMode == 0);

		}

		// dec btn
		{
			auto decBtnId = temple::GetRef<int[6]>(0x10C44DA8)[i];
			auto cost = 1;
			if (abLvl >= 17)
				cost = 3;
			else if (abLvl >= 15)
				cost = 2;

			if (pbPoints >= config.pointBuyPoints || (abLvl == 8 && !config.laxRules) || abLvl <= 5)
				uiManager->SetButtonState(decBtnId, LgcyButtonState::Disabled);
			else
				uiManager->SetButtonState(decBtnId, LgcyButtonState::Normal);
			uiManager->SetHidden(decBtnId, isPointBuyMode == 0);
		}
		
	}

	auto rerollBtnId = temple::GetRef<int>(0x10C45460);
	uiManager->SetHidden(rerollBtnId, isPointBuyMode != FALSE);

	return isPointBuyMode;
}

void PcCreationHooks::AutoAddSpellsMemorizedToGroup(){
	auto N = party.GroupPCsLen();
	for (auto i=0u; i < N; i++){
		auto dude = party.GroupPCsGetMemberN(i);
		temple::GetRef<void(__cdecl)(objHndl)>(0x1011E920)(dude);
	}
}



int HookedFeatMultiselectSub_101822A0(feat_enums feat)
{
	if ((feat >= FEAT_EXOTIC_WEAPON_PROFICIENCY && feat <= FEAT_WEAPON_SPECIALIZATION) || feat == FEAT_WEAPON_FINESSE_DAGGER)
	{
		__asm mov eax, feat;
		return OrgFeatMultiselectSub_101822A0();
	}
	return 0;
}



int __cdecl PcCreationFeatUiPrereqCheck(feat_enums feat)
{
	int featArrayLen = 0;
	feat_enums featArray[10];
	if (addresses.charEdSelPkt->feat0 != FEAT_NONE)
		featArray[featArrayLen++] = addresses.charEdSelPkt->feat0;
	if (addresses.charEdSelPkt->feat1 != FEAT_NONE)
		featArray[featArrayLen++] = addresses.charEdSelPkt->feat1;
	if (addresses.charEdSelPkt->feat2 != FEAT_NONE)
		featArray[featArrayLen++] = addresses.charEdSelPkt->feat2;
	if (feat == FEAT_IMPROVED_TRIP)
	{
		if (objects.StatLevelGet(*feats.charEditorObjHnd, stat_level_monk) == 5
			&& addresses.charEdSelPkt->classCode == stat_level_monk)
			return 1;
	}

	if (feat == FEAT_WEAPON_FINESSE){
		if (addresses.charEdSelPkt->feat0 == FEAT_WEAPON_FINESSE_DAGGER
			|| addresses.charEdSelPkt->feat1 == FEAT_WEAPON_FINESSE_DAGGER
			|| addresses.charEdSelPkt->feat2 == FEAT_WEAPON_FINESSE_DAGGER
			)
			return 0;
	}

	if (feat <= FEAT_NONE || feat > FEAT_MONK_PERFECT_SELF)
		return feats.FeatPrereqsCheck(*feats.charEditorObjHnd, feat, featArray, featArrayLen, (Stat)0, addresses.charEdSelPkt->statBeingRaised);

	// the vanilla multiselect range

	return feats.FeatPrereqsCheck(*feats.charEditorObjHnd, feat, featArray, featArrayLen, (Stat)0, addresses.charEdSelPkt->statBeingRaised);
}


int __declspec(naked) PcCreationFeatUiPrereqCheckUsercallWrapper()
{
	{ __asm push ecx __asm push esi __asm push ebx __asm push edi}

	__asm
	{
		push ecx;
		call PcCreationFeatUiPrereqCheck;
		pop edi;
	}
	{ __asm pop edi __asm pop ebx __asm pop esi __asm pop ecx }
	__asm retn;
}

objHndl UiPcCreation::GetEditedChar(){
	return chargen.GetEditedChar();
}

CharEditorSelectionPacket & UiPcCreation::GetCharEditorSelPacket(){
	return chargen.GetCharEditorSelPacket();
}

LgcyWindow & UiPcCreation::GetPcCreationWnd(){
	return temple::GetRef<LgcyWindow>(0x11E73E40);
}

LgcyWindow & UiPcCreation::GetStatsWnd(){
	return temple::GetRef<LgcyWindow>(0x11E72BA0);
}

int & UiPcCreation::GetState(){
	return temple::GetRef<int>(0x102F7D68);
}

Alignment UiPcCreation::GetPartyAlignment(){
	return temple::GetRef<Alignment>(0x11E741A8);
}

void UiPcCreation::PrepareNextStages(){
}

void UiPcCreation::ResetNextStages(int systemId){
	temple::GetRef<void(__cdecl)(int)>(0x1011BC70)(systemId);
}

void UiPcCreation::ToggleClassRelatedStages(){
	auto handle = GetEditedChar();
	auto &selPkt = GetCharEditorSelPacket();
	auto classCode = selPkt.classCode;
	auto lvlNew = 1;
	auto &stateBtnIds = temple::GetRef<int[CG_STAGE_COUNT]>(0x10BDC434);


	uiManager->SetButtonState(stateBtnIds[CG_Stage_Abilities], LgcyButtonState::Disabled); // class features - off by default; todo expand this


	mIsSelectingBonusFeat = false;
	// feats and features
	if (classCode >= stat_level_barbarian) {

		auto classLvlNew = 1;

		if (d20ClassSys.IsSelectingFeatsOnLevelup(handle, classCode)) {
			mIsSelectingBonusFeat = true;
		}


		if (classCode == stat_level_cleric) {
			uiManager->SetButtonState(stateBtnIds[CG_Stage_Abilities], LgcyButtonState::Normal); // features
		}
		if (classCode == stat_level_ranger) {
			uiManager->SetButtonState(stateBtnIds[CG_Stage_Abilities], LgcyButtonState::Normal); // features
		}
		if (classCode == stat_level_wizard) {
			uiManager->SetButtonState(stateBtnIds[CG_Stage_Abilities], LgcyButtonState::Normal); // wizard special school
		}
	}

	// Spells
	if (d20ClassSys.IsSelectingSpellsOnLevelup(handle, classCode)) {
		uiManager->SetButtonState(stateBtnIds[CG_Stage_Spells], LgcyButtonState::Normal);
	}
	else
	{
		uiManager->SetButtonState(stateBtnIds[CG_Stage_Spells], LgcyButtonState::Disabled);
	};

}

BOOL UiPcCreation::ClassSystemInit(GameSystemConf & conf){
	if (textureFuncs.RegisterTexture("art\\interface\\pc_creation\\buttonbox.tga", &buttonBox))
		return 0;

	classBtnTextStyle.flags = 8;
	classBtnTextStyle.field2c = -1;
	classBtnTextStyle.textColor = &classBtnColorRect;
	classBtnTextStyle.shadowColor = &classBtnShadowColor;
	classBtnTextStyle.colors4 = &classBtnColorRect;
	classBtnTextStyle.colors2 = &classBtnColorRect;
	classBtnTextStyle.field0 = 0;
	classBtnTextStyle.kerning = 1;
	classBtnTextStyle.leading = 0;
	classBtnTextStyle.tracking = 3;

	for (auto it : d20ClassSys.baseClassEnums) {
		auto className = _strdup(d20Stats.GetStatName((Stat)it));
		classNamesUppercase[it] = className;
		for (auto &letter : classNamesUppercase[it]) {
			letter = toupper(letter);
		}
		classBtnMapping.push_back(it);
	}
	mPageCount = classBtnMapping.size() / 11;
	if (mPageCount * 11u < classBtnMapping.size())
		mPageCount++;

	return ClassWidgetsInit();
}

BOOL UiPcCreation::ClassWidgetsInit(){
	static LgcyWindow classWnd(219, 50, 431, 250);
	classWnd.x = GetPcCreationWnd().x + 219;
	classWnd.y = GetPcCreationWnd().y + 50;
	classWnd.flags = 1;
	classWnd.render = [](int widId) { uiPcCreation.StateTitleRender(widId); };
	classWndId = uiManager->AddWindow(classWnd);

	int coloff = 0, rowoff = 0;

	for (auto it : d20ClassSys.vanillaClassEnums) {
		// class buttons
		LgcyButton classBtn("Class btn", classWndId, 81 + coloff, 42 + rowoff, 130, 20);
		coloff = 139 - coloff;
		if (!coloff)
			rowoff += 29;
		if (rowoff == 5 * 29) // the bottom button
			coloff = 69;

		classBtnRects.push_back(TigRect(classBtn.x, classBtn.y, classBtn.width, classBtn.height));
		classBtn.x += classWnd.x; classBtn.y += classWnd.y;
		classBtn.render = [](int id) {uiPcCreation.ClassBtnRender(id); };
		classBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.ClassBtnMsg(id, msg); };
		classBtn.SetDefaultSounds();
		classBtnIds.push_back(uiManager->AddButton(classBtn, classWndId));

		//rects
		classBtnFrameRects.push_back(TigRect(classBtn.x - 5, classBtn.y - 5, classBtn.width + 10, classBtn.height + 10));


		UiRenderer::PushFont(PredefinedFont::PRIORY_12);
		auto classMeasure = UiRenderer::MeasureTextSize(classNamesUppercase[it].c_str(), classBtnTextStyle);
		TigRect rect(classBtn.x + (110 - classMeasure.width) / 2 - classWnd.x,
			classBtn.y + (20 - classMeasure.height) / 2 - classWnd.y,
			classMeasure.width, classMeasure.height);
		classTextRects.push_back(rect);
		UiRenderer::PopFont();
	}

	const int nextBtnXoffset = 329;
	const int nextBtnYoffset = 205;
	const int prevBtnXoffset = 38;
	classNextBtnTextRect = classNextBtnRect = TigRect(classWnd.x + nextBtnXoffset, classWnd.y + nextBtnYoffset, 55, 20);
	classPrevBtnTextRect = classPrevBtnRect = TigRect(classWnd.x + prevBtnXoffset, classWnd.y + nextBtnYoffset, 55, 20);
	classNextBtnFrameRect = TigRect(classWnd.x + nextBtnXoffset - 3, classWnd.y + nextBtnYoffset - 5, 55 + 6, 20 + 10);
	classPrevBtnFrameRect = TigRect(classWnd.x + prevBtnXoffset - 3, classWnd.y + nextBtnYoffset - 5, 55 + 6, 20 + 10);
	classNextBtnTextRect.x -= classWnd.x; classNextBtnTextRect.y -= classWnd.y;
	classPrevBtnTextRect.x -= classWnd.x; classPrevBtnTextRect.y -= classWnd.y;

	LgcyButton nextBtn("Class Next Button", classWndId, classWnd.x + nextBtnXoffset, classWnd.y + nextBtnYoffset-4, 55, 20);
	nextBtn.handleMessage = [](int widId, TigMsg*msg)->BOOL {
		if (uiPcCreation.classWndPage < uiPcCreation.mPageCount)
			uiPcCreation.classWndPage++;
		uiPcCreation.ClassSetPermissibles();
		return 1; };
	nextBtn.render = [](int id) { uiPcCreation.ClassNextBtnRender(id); };
	nextBtn.handleMessage = [](int widId, TigMsg*msg)->BOOL {	return uiPcCreation.ClassNextBtnMsg(widId, msg); };
	nextBtn.SetDefaultSounds();
	classNextBtn = uiManager->AddButton(nextBtn, classWndId);

	LgcyButton prevBtn("Class Prev. Button", classWndId, classWnd.x + prevBtnXoffset, classWnd.y + nextBtnYoffset - 4, 55, 20);
	prevBtn.render = [](int id) { uiPcCreation.ClassPrevBtnRender(id); };
	prevBtn.handleMessage = [](int widId, TigMsg*msg)->BOOL {	return uiPcCreation.ClassPrevBtnMsg(widId, msg); };
	prevBtn.SetDefaultSounds();
	classPrevBtn = uiManager->AddButton(prevBtn, classWndId);
	
	return TRUE;
	
}

void UiPcCreation::ClassWidgetsFree(){
	for (auto it : classBtnIds) {
		uiManager->RemoveChildWidget(it);
	}
	classBtnIds.clear();
	uiManager->RemoveChildWidget(classNextBtn);
	uiManager->RemoveChildWidget(classPrevBtn);
	uiManager->RemoveWidget(classWndId);
}

BOOL UiPcCreation::ClassShow(){
	uiManager->SetHidden(classWndId, false);
	uiManager->BringToFront(classWndId);
	return 1;
}

BOOL UiPcCreation::ClassHide(){
	uiManager->SetHidden(classWndId, true);
	return 0;
}

BOOL UiPcCreation::ClassWidgetsResize(UiResizeArgs & args){
	for (auto it : classBtnIds) {
		uiManager->RemoveChildWidget(it);
	}
	classBtnIds.clear();
	uiManager->RemoveChildWidget(classNextBtn);
	uiManager->RemoveChildWidget(classPrevBtn);
	uiManager->RemoveWidget(classWndId);
	classBtnFrameRects.clear();
	classBtnRects.clear();
	classTextRects.clear();
	return ClassWidgetsInit();
}

BOOL UiPcCreation::ClassCheckComplete(){
	auto &selPkt = GetCharEditorSelPacket();
	return (BOOL)(selPkt.classCode != 0);
}

void UiPcCreation::ClassBtnEntered(){
	auto &selPkt = GetCharEditorSelPacket();
	if (selPkt.classCode){
		ClassScrollboxTextSet(selPkt.classCode);
	}
	else{
		ButtonEnteredHandler(ElfHash::Hash("TAG_CHARGEN_CLASS"));
	}
}

void UiPcCreation::ClassActivate(){
	chargen.SetIsNewChar(true);
	ClassSetPermissibles();
}

void UiPcCreation::ClassFinalize(CharEditorSelectionPacket & selPkt, objHndl & handle){
	auto obj = objSystem->GetObject(handle);
	obj->ClearArray(obj_f_critter_level_idx);
	obj->SetInt32(obj_f_critter_level_idx, 0, selPkt.classCode);
	d20StatusSys.D20StatusRefresh(handle);
	critterSys.GenerateHp(handle);
}

BOOL UiPcCreation::FeatsSystemInit(GameSystemConf& conf){

	auto pcCreationMes = temple::GetRef<MesHandle>(0x11E72EF0);
	MesLine mesline;

	TigTextStyle baseStyle;
	baseStyle.flags = 0x4000;
	baseStyle.field2c = -1;
	baseStyle.shadowColor = &genericShadowColor;
	baseStyle.field0 = 0;
	baseStyle.kerning = 1;
	baseStyle.leading = 0;
	baseStyle.tracking = 3;
	baseStyle.textColor = baseStyle.colors2 = baseStyle.colors4 = &whiteColorRect;

	featsCenteredStyle = featsGreyedStyle = featsNormalTextStyle = featsExistingTitleStyle = featsGoldenStyle = featsClassStyle = baseStyle;

	featsClassStyle.textColor = featsClassStyle.colors2 = featsClassStyle.colors4 = &darkGreenColorRect;

	featsCenteredStyle.flags = 0x10;

	static ColorRect goldenColor(0xFFFFD919);
	featsGoldenStyle.colors2 = featsGoldenStyle.colors4 = featsGoldenStyle.textColor = &goldenColor;

	static ColorRect greyColor(0xFF5D5D5D);
	featsGreyedStyle.colors2 = featsGreyedStyle.colors4 = featsGreyedStyle.textColor = &greyColor;

#pragma region Titles and strings
	// Feats Available title
	mesline.key = 19000;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	featsAvailTitleString.append(mesline.value);

	// Class Feats title
	mesline.key = 19002;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	featsExistingTitleString.append(mesline.value);

	// Feats title
	mesline.key = 19001;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	featsTitleString.append(mesline.value);

	// Class Bonus title
	mesline.key = 19003;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	featsClassBonusTitleString.append(mesline.value);

	static auto prefabFeatsMasterMesPairs = eastl::hash_map<int, int>({
		{ FEAT_EXOTIC_WEAPON_PROFICIENCY, 19101 },
		{ FEAT_IMPROVED_CRITICAL , 19102 },
		{ FEAT_MARTIAL_WEAPON_PROFICIENCY , 19103 },
		{ FEAT_SKILL_FOCUS , 19104 },
		{ FEAT_WEAPON_FINESSE , 19105 },
		{ FEAT_WEAPON_FOCUS, 19106 },
		{ FEAT_WEAPON_SPECIALIZATION , 19107 },
		{ FEAT_GREATER_WEAPON_FOCUS , 19108 }
	});

	for (auto it : prefabFeatsMasterMesPairs) {
		mesline.key = it.second;
		mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
		featsMasterFeatStrings[it.first].append(mesline.value);
	}
	featsMasterFeatStrings[FEAT_GREATER_WEAPON_SPECIALIZATION].append(feats.GetFeatName(FEAT_GREATER_WEAPON_SPECIALIZATION));


#pragma endregion

	featsbackdrop = new CombinedImgFile("art\\interface\\pc_creation\\meta_backdrop.img");
	if (!featsbackdrop)
		return 0;
	return FeatsWidgetsInit(conf.width, conf.height);
}

BOOL UiPcCreation::FeatsWidgetsInit(int w, int h){

	auto &pcCreationWnd = temple::GetRef<LgcyWindow>(0x11E73E40);
	featsMainWnd = LgcyWindow(pcCreationWnd.x + 219, pcCreationWnd.y + 50, 431, 250);
	featsMainWnd.flags = 1;
	featsMainWnd.render = [](int widId) {uiPcCreation.FeatsWndRender(widId); };
	featsMainWnd.handleMessage = [](int widId, TigMsg*msg) { return uiPcCreation.FeatsWndMsg(widId, msg); };
	featsMainWndId = uiManager->AddWindow(featsMainWnd);

	// multi select wnd
	featsMultiCenterX = (w - 289) / 2;
	featsMultiCenterY = (h - 355) / 2;
	featsMultiSelectWnd = LgcyWindow(0, 0, w, h);
	auto featsMultiRefX = featsMultiCenterX + featsMultiSelectWnd.x;
	auto featsMultiRefY = featsMultiCenterY + featsMultiSelectWnd.y;
	featsMultiSelectWnd.flags = 1;
	featsMultiSelectWnd.render = [](int widId) {uiPcCreation.FeatsMultiSelectWndRender(widId); };
	featsMultiSelectWnd.handleMessage = [](int widId, TigMsg*msg) { return uiPcCreation.FeatsMultiSelectWndMsg(widId, msg); };
	featsMultiSelectWndId = uiManager->AddWindow(featsMultiSelectWnd);
	//scrollbar
	featsMultiSelectScrollbar.Init(256, 71, 219);
	featsMultiSelectScrollbar.parentId = featsMultiSelectWndId;
	featsMultiSelectScrollbar.x += featsMultiRefX;
	featsMultiSelectScrollbar.y += featsMultiRefY;
	featsMultiSelectScrollbarId = uiManager->AddScrollBar(featsMultiSelectScrollbar, featsMultiSelectWndId);

	//ok btn
	{
		LgcyButton multiOkBtn("Feats Multiselect Ok Btn", featsMultiSelectWndId, 29, 307, 110, 22);
		multiOkBtn.x += featsMultiRefX; multiOkBtn.y += featsMultiRefY;
		featMultiOkRect = TigRect(multiOkBtn.x, multiOkBtn.y, multiOkBtn.width, multiOkBtn.height);
		featMultiOkTextRect = TigRect(multiOkBtn.x, multiOkBtn.y + 4, multiOkBtn.width, multiOkBtn.height - 8);
		multiOkBtn.render = [](int id) {uiPcCreation.FeatsMultiOkBtnRender(id); };
		multiOkBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.FeatsMultiOkBtnMsg(id, msg); };
		multiOkBtn.renderTooltip = nullptr;
		multiOkBtn.SetDefaultSounds();
		featsMultiOkBtnId = uiManager->AddButton(multiOkBtn, featsMultiSelectWndId);
	}

	//cancel btn
	{
		LgcyButton multiCancelBtn("Feats Multiselect Cancel Btn", featsMultiSelectWndId, 153, 307, 110, 22);
		multiCancelBtn.x += featsMultiRefX; multiCancelBtn.y += featsMultiRefY;
		featMultiCancelRect = TigRect(multiCancelBtn.x, multiCancelBtn.y, multiCancelBtn.width, multiCancelBtn.height);
		featMultiCancelTextRect = TigRect(multiCancelBtn.x, multiCancelBtn.y + 4, multiCancelBtn.width, multiCancelBtn.height - 8);
		multiCancelBtn.render = [](int id) {uiPcCreation.FeatsMultiCancelBtnRender(id); };
		multiCancelBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.FeatsMultiCancelBtnMsg(id, msg); };
		multiCancelBtn.renderTooltip = nullptr;
		multiCancelBtn.SetDefaultSounds();
		featsMultiCancelBtnId = uiManager->AddButton(multiCancelBtn, featsMultiSelectWndId);
	}

	featMultiTitleRect = TigRect(featsMultiCenterX, featsMultiCenterY + 20, 289, 12);
	featsMultiSelectBtnIds.clear();
	featsMultiBtnRects.clear();
	auto rowOff = 75;
	for (auto i = 0; i < FEATS_MULTI_BTN_COUNT; i++) {
		LgcyButton featMultiBtn("Feats Multiselect btn", featsMultiSelectWndId, 23, 75 + i*(FEATS_MULTI_BTN_HEIGHT + 2), 233, FEATS_MULTI_BTN_HEIGHT);

		featMultiBtn.x += featsMultiRefX; featMultiBtn.y += featsMultiRefY;
		featMultiBtn.render = [](int id) {uiPcCreation.FeatsMultiBtnRender(id); };
		featMultiBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.FeatsMultiBtnMsg(id, msg); };
		featMultiBtn.renderTooltip = nullptr;
		featMultiBtn.SetDefaultSounds();
		featsMultiSelectBtnIds.push_back(uiManager->AddButton(featMultiBtn, featsMultiSelectWndId));
		featsMultiBtnRects.push_back(TigRect(featMultiBtn.x, featMultiBtn.y, featMultiBtn.width, featMultiBtn.height));
	}

	featsAvailTitleRect = TigRect(17, 17, 185, 10);
	featsTitleRect = TigRect(220, 19, 185, 10);
	featsExistingTitleRect = TigRect(220, 122, 185, 10);
	featsClassBonusRect = TigRect(220, 78, 185, 10);

	// Selectable feats
	featsAvailBtnIds.clear();
	featsBtnRects.clear();
	for (auto i = 0; i < FEATS_AVAIL_BTN_COUNT; i++) {
		LgcyButton featsAvailBtn("Feats Available btn", featsMainWndId, 20, 33 + i*(FEATS_AVAIL_BTN_HEIGHT + 1), 169, FEATS_AVAIL_BTN_HEIGHT);

		featsAvailBtn.x += featsMainWnd.x; featsAvailBtn.y += featsMainWnd.y;
		featsAvailBtn.render = [](int id) {uiPcCreation.FeatsEntryBtnRender(id); };
		featsAvailBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.FeatsEntryBtnMsg(id, msg); };
		featsAvailBtn.renderTooltip = nullptr;
		featsAvailBtn.SetDefaultSounds();
		featsAvailBtnIds.push_back(uiManager->AddButton(featsAvailBtn, featsMainWndId));
		featsBtnRects.push_back(TigRect(featsAvailBtn.x - featsMainWnd.x, featsAvailBtn.y - featsMainWnd.y, featsAvailBtn.width, featsAvailBtn.height));
	}
	//scrollbar
	featsScrollbar.Init(191, 31, 201);
	featsScrollbar.parentId = featsMainWndId;
	featsScrollbar.x += featsMainWnd.x;
	featsScrollbar.y += featsMainWnd.y;
	featsScrollbarId = uiManager->AddScrollBar(featsScrollbar, featsMainWndId);


	// Existing feats
	featsExistingBtnIds.clear();
	featsExistingBtnRects.clear();
	for (auto i = 0; i < FEATS_EXISTING_BTN_COUNT; i++) {
		LgcyButton featsExistingBtn("Feats Existing btn", featsMainWndId, 225, 140 + i*(FEATS_EXISTING_BTN_HEIGHT + 1), 175, FEATS_EXISTING_BTN_HEIGHT);

		featsExistingBtn.x += featsMainWnd.x; featsExistingBtn.y += featsMainWnd.y;
		featsExistingBtn.render = [](int id) {uiPcCreation.FeatsExistingBtnRender(id); };
		featsExistingBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.FeatsExistingBtnMsg(id, msg); };
		featsExistingBtn.renderTooltip = nullptr;
		featsExistingBtn.SetDefaultSounds();
		featsExistingBtnIds.push_back(uiManager->AddButton(featsExistingBtn, featsMainWndId));
		featsExistingBtnRects.push_back(TigRect(featsExistingBtn.x - featsMainWnd.x, featsExistingBtn.y - featsMainWnd.y, featsExistingBtn.width, featsExistingBtn.height));
	}
	//scrollbar
	featsExistingScrollbar.Init(395, 137, 95);
	featsExistingScrollbar.parentId = featsMainWndId;
	featsExistingScrollbar.x += featsMainWnd.x;
	featsExistingScrollbar.y += featsMainWnd.y;
	featsExistingScrollbarId = uiManager->AddScrollBar(featsExistingScrollbar, featsMainWndId);

	featsSelectedBorderRect = TigRect(featsMainWnd.x + 220, featsMainWnd.y + 34, 185, 19);
	featsSelected2BorderRect = TigRect(featsMainWnd.x + 220, featsMainWnd.y + 34 + 20, 185, 19);
	featsClassBonusBorderRect = TigRect(featsMainWnd.x + 220, featsMainWnd.y + 97, 185, 19);
	feat0TextRect = TigRect(223, 35, 185, 12);
	feat1TextRect = TigRect(223, 35 + 21, 185, 12);
	feat2TextRect = TigRect(223, 98, 185, 12);

	return 1;
}

void UiPcCreation::FeatsFree(){
	FeatWidgetsFree();
}

void UiPcCreation::FeatWidgetsFree(){
	for (auto i = 0; i < FEATS_MULTI_BTN_COUNT; i++) {
		uiManager->RemoveChildWidget(featsMultiSelectBtnIds[i]);
	}
	featsMultiSelectBtnIds.clear();

	for (auto i = 0; i < FEATS_AVAIL_BTN_COUNT; i++) {
		uiManager->RemoveChildWidget(featsAvailBtnIds[i]);
	}
	featsAvailBtnIds.clear();

	for (auto i = 0; i < FEATS_EXISTING_BTN_COUNT; i++) {
		uiManager->RemoveChildWidget(featsExistingBtnIds[i]);
	}
	featsExistingBtnIds.clear();

	uiManager->RemoveChildWidget(featsMultiOkBtnId);
	uiManager->RemoveChildWidget(featsMultiCancelBtnId);
	uiManager->RemoveChildWidget(featsMultiSelectScrollbarId);
	uiManager->RemoveChildWidget(featsScrollbarId);
	uiManager->RemoveChildWidget(featsExistingScrollbarId);

	auto wid = uiManager->GetWindow(featsMultiSelectWndId);
	auto wid2 = uiManager->GetWindow(featsMainWndId);

	uiManager->RemoveWidget(featsMultiSelectWndId);
	uiManager->RemoveWidget(featsMainWndId);
}

BOOL UiPcCreation::FeatsWidgetsResize(UiResizeArgs& args){
	FeatWidgetsFree();
	return FeatsWidgetsInit(args.rect1.width, args.rect1.height);
}

BOOL UiPcCreation::FeatsShow(){
	featsMultiSelected = FEAT_NONE;
	uiManager->SetHidden(featsMainWndId, false);
	uiManager->BringToFront(featsMainWndId);
	return 1;
}

BOOL UiPcCreation::FeatsHide()
{
	uiManager->SetHidden(featsMainWndId, true);
	return 0;
}

void UiPcCreation::FeatsActivate()
{
	mFeatsActivated = true;

	auto handle = GetEditedChar();
	auto &selPkt = GetCharEditorSelPacket();

	mIsSelectingBonusFeat = d20ClassSys.IsSelectingFeatsOnLevelup(handle, selPkt.classCode);
	chargen.BonusFeatsClear();
	if (mIsSelectingBonusFeat)
		d20ClassSys.LevelupGetBonusFeats(handle, selPkt.classCode); // can call set_bonus_feats

	feat_enums existingFeats[122];
	auto existingCount = feats.FeatListGet(handle, existingFeats, selPkt.classCode, FEAT_ACROBATIC);

	mExistingFeats.clear();
	for (auto i = 0u; i<existingCount; i++) {
		auto ftEnum = existingFeats[i];
		if (selPkt.feat0 != ftEnum && selPkt.feat1 != ftEnum && selPkt.feat2 != ftEnum)
			mExistingFeats.push_back(FeatInfo(ftEnum));
	}
	static auto featSorter = [](FeatInfo &first, FeatInfo &second) {

		auto firstEnum = (feat_enums)first.featEnum;
		auto secEnum = (feat_enums)second.featEnum;

		auto firstName = uiPcCreation.GetFeatName(firstEnum);
		auto secondName = uiPcCreation.GetFeatName(secEnum);

		return _stricmp(firstName.c_str(), secondName.c_str()) < 0;
	};

	std::sort(mExistingFeats.begin(), mExistingFeats.end(), featSorter);

	featsExistingScrollbar = *uiManager->GetScrollBar(featsExistingScrollbarId);
	featsExistingScrollbar.scrollbarY = 0;
	featsExistingScrollbarY = 0;
	featsExistingScrollbar.yMax = max((int)mExistingFeats.size() - FEATS_EXISTING_BTN_COUNT, 0);
	featsExistingScrollbar = *uiManager->GetScrollBar(featsExistingScrollbarId);

	// Available feats
	mSelectableFeats.clear();
	for (auto i = 0u; i < NUM_FEATS; i++) {
		auto feat = (feat_enums)i;
		if (!feats.IsFeatEnabled(feat) && !feats.IsFeatMultiSelectMaster(feat))
			continue;
		if (feats.IsFeatRacialOrClassAutomatic(feat))
			continue;
		if (feats.IsFeatPartOfMultiselect(feat))
			continue;
		if (feat == FEAT_NONE)
			continue;
		mSelectableFeats.push_back(FeatInfo(feat));
	}
	for (auto feat : feats.newFeats) {
		if (!feats.IsFeatEnabled(feat) && !feats.IsFeatMultiSelectMaster(feat))
			continue;
		if (!config.nonCoreMaterials && feats.IsNonCore(feat))
			continue;
		if (IsClassBonusFeat(feat)) {
			mSelectableFeats.push_back(FeatInfo(feat));
			continue;
		}
		if (feats.IsFeatRacialOrClassAutomatic(feat))
			continue;
		if (feats.IsFeatPartOfMultiselect(feat))
			continue;
		if (feat == FEAT_NONE)
			continue;

		mSelectableFeats.push_back(FeatInfo(feat));
	}
	std::sort(mSelectableFeats.begin(), mSelectableFeats.end(), featSorter);

	featsScrollbar = *uiManager->GetScrollBar(featsScrollbarId);
	featsScrollbar.scrollbarY = 0;
	featsScrollbarY = 0;
	featsScrollbar.yMax = max((int)mSelectableFeats.size() - FEATS_AVAIL_BTN_COUNT, 0);
	featsScrollbar = *uiManager->GetScrollBar(featsScrollbarId);
}

BOOL UiPcCreation::FeatsCheckComplete()
{
	auto handle = GetEditedChar();
	auto &selPkt = GetCharEditorSelPacket();

	// is a 3rd level and no feat chosen
	if (IsSelectingNormalFeat() && selPkt.feat0 == FEAT_NONE)
		return FALSE;

	if (IsSelectingSecondFeat() && selPkt.feat1 == FEAT_NONE)
		return FALSE;

	if (IsSelectingBonusFeat() && selPkt.feat2 == FEAT_NONE) // the logic will be handled in the msg callbacks & Python API now
		return FALSE;

	return TRUE;
}

void UiPcCreation::FeatsFinalize(CharEditorSelectionPacket& selPkt, objHndl & handle){

	feats.FeatAdd(handle, selPkt.feat0);
	d20StatusSys.D20StatusRefresh(handle);
	if (selPkt.feat1 != FEAT_NONE){
		feats.FeatAdd(handle, selPkt.feat1);
		d20StatusSys.D20StatusRefresh(handle);
	}
	if (selPkt.feat2 != FEAT_NONE) {
		feats.FeatAdd(handle, selPkt.feat2);
		d20StatusSys.D20StatusRefresh(handle);
	}

}

void UiPcCreation::FeatsReset(CharEditorSelectionPacket& selPkt)
{
	mFeatsActivated = false;
	//mIsSelectingBonusFeat = false; // should not do this here, since then if a user goes back to skills and decreases/increases them, it can cause problems

	selPkt.feat0 = FEAT_NONE;
	selPkt.feat1 = FEAT_NONE;
	if (selPkt.classCode != stat_level_ranger || objects.StatLevelGet(GetEditedChar(), stat_level_ranger) != 1)
		selPkt.feat2 = FEAT_NONE;

	mExistingFeats.clear();
	mSelectableFeats.clear();
	mMultiSelectFeats.clear();
}

BOOL UiPcCreation::SpellsSystemInit(GameSystemConf & conf)
{
	auto pcCreationMes = temple::GetRef<MesHandle>(0x11E72EF0);
	MesLine mesline;

	TigTextStyle baseStyle;
	baseStyle.flags = 0;
	baseStyle.field2c = -1;
	baseStyle.shadowColor = &genericShadowColor;
	baseStyle.field0 = 0;
	baseStyle.kerning = 1;
	baseStyle.leading = 0;
	baseStyle.tracking = 3;
	baseStyle.textColor = baseStyle.colors2 = baseStyle.colors4 = &whiteColorRect;


	spellsTitleStyle = baseStyle;

	// generic spells text style
	spellsTextStyle = baseStyle;

	// Spell Level Label Style
	spellLevelLabelStyle = baseStyle;
	static ColorRect spellLevelLabelColor(0xFF43586E);
	spellLevelLabelStyle.textColor = spellLevelLabelStyle.colors2 = spellLevelLabelStyle.colors4 = &spellLevelLabelColor;

	// Spells Available Btn Style
	spellsAvailBtnStyle = baseStyle;
	static ColorRect spellsAvailColor1(0x0FF5D5D5D);
	spellsAvailBtnStyle.textColor = &spellsAvailColor1;
	spellsAvailBtnStyle.colors2 = &spellsAvailColor1;
	spellsAvailBtnStyle.colors4 = &spellsAvailColor1;


	// Spells Per Day style
	spellsPerDayStyle = baseStyle;

	spellsPerDayTitleStyle = baseStyle;

	// Spells Available title
	mesline.key = 21000;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	spellsAvailLabel.append(mesline.value);

	// Spells Chosen title
	mesline.key = 21001;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	spellsChosenLabel.append(mesline.value);

	// Spells Per Day title
	mesline.key = 21002;
	mesFuncs.GetLine_Safe(pcCreationMes, &mesline);
	spellsPerDayLabel.append(mesline.value);

	// Spell Level label texts
	MesLine line(21100), line2(21200);
	mesFuncs.GetLine_Safe(pcCreationMes, &line);
	mesFuncs.GetLine_Safe(pcCreationMes, &line2);


	for (auto i = 0; i < NUM_SPELL_LEVELS; i++) {
		std::string text;
		text.append(line2.value);
		text[text.size() - 1] = '0' + i;
		chargen.levelLabels.push_back(text);

		text.clear();
		text.append(line.value);
		text[text.size() - 1] = '0' + i;

		chargen.spellLevelLabels.push_back(text);
	}

	for (auto i = 0; i < SPELLS_PER_DAY_BOXES_COUNT; i++) {
		spellsPerDayTextRects.push_back(TigRect());
	}

	levelupSpellbar = new CombinedImgFile("art\\interface\\pc_creation\\levelup_spellbar.img");
	if (!levelupSpellbar)
		return 0;

	// Widgets
	return SpellsWidgetsInit();
}

void UiPcCreation::SpellsFree(){
	SpellsWidgetsFree();
}

BOOL UiPcCreation::SpellsWidgetsInit(){

	auto &pcWnd = GetPcCreationWnd();

	const int spellsWndX = 219, spellsWndY = 50, spellsWndW = 431, spellsWndH = 250;
	spellsWnd = LgcyWindow(pcWnd.x + spellsWndX, pcWnd.y + spellsWndY, spellsWndW, spellsWndH);
	spellsWnd.flags = 1;
	spellsWnd.render = [](int widId) {uiPcCreation.SpellsWndRender(widId); };
	spellsWnd.handleMessage = [](int widId, TigMsg*msg) { return uiPcCreation.SpellsWndMsg(widId, msg); };
	spellsWndId = uiManager->AddWindow(spellsWnd);

	// Available Spells Scrollbar
	spellsScrollbar.Init(201, 34, 152);
	spellsScrollbar.parentId = spellsWndId;
	spellsScrollbar.x += spellsWnd.x;
	spellsScrollbar.y += spellsWnd.y;
	spellsScrollbarId = uiManager->AddScrollBar(spellsScrollbar, spellsWndId);

	// Spell selection scrollbar
	spellsScrollbar2.Init(415, 34, 152);
	spellsScrollbar2.parentId = spellsWndId;
	spellsScrollbar2.x += spellsWnd.x;
	spellsScrollbar2.y += spellsWnd.y;
	spellsScrollbar2Id = uiManager->AddScrollBar(spellsScrollbar2, spellsWndId);

	int rowOff = 38;
	for (auto i = 0; i < SPELLS_BTN_COUNT; i++, rowOff += SPELLS_BTN_HEIGHT) {

		LgcyButton spellAvailBtn("Spell Available btn", spellsWndId, 4, rowOff, 193, SPELLS_BTN_HEIGHT);

		spellAvailBtn.x += spellsWnd.x; spellAvailBtn.y += spellsWnd.y;
		spellAvailBtn.render = [](int id) {uiPcCreation.SpellsAvailableEntryBtnRender(id); };
		spellAvailBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.SpellsAvailableEntryBtnMsg(id, msg); };
		spellAvailBtn.renderTooltip = nullptr;
		spellAvailBtn.SetDefaultSounds();
		spellsAvailBtnIds.push_back(uiManager->AddButton(spellAvailBtn, spellsWndId));

		LgcyButton spellChosenBtn("Spell Chosen btn", spellsWndId, 221, rowOff, 193, SPELLS_BTN_HEIGHT);

		spellChosenBtn.x += spellsWnd.x; spellChosenBtn.y += spellsWnd.y;
		spellChosenBtn.render = [](int id) {uiPcCreation.SpellsEntryBtnRender(id); };
		spellChosenBtn.handleMessage = [](int id, TigMsg* msg) { return uiPcCreation.SpellsEntryBtnMsg(id, msg); };
		spellChosenBtn.renderTooltip = nullptr;
		spellChosenBtn.SetDefaultSounds();
		spellsChosenBtnIds.push_back(uiManager->AddButton(spellChosenBtn, spellsWndId));

	}

	// titles
	spellsAvailTitleRect.x = 5;
	spellsAvailTitleRect.y = 20;
	spellsChosenTitleRect.x = 219;
	spellsChosenTitleRect.y = 20;
	UiRenderer::PushFont("priory-12", 12);

	// Spells Per Day title
	auto spellsPerDayMeasure = UiRenderer::MeasureTextSize(spellsPerDayLabel, spellsTextStyle);
	spellsPerDayTitleRect = TigRect(5, 205, 99, 12);
	spellsPerDayTitleRect.x += (spellsPerDayTitleRect.width - spellsPerDayMeasure.width) / 2;
	spellsPerDayTitleRect.width = spellsPerDayMeasure.width;
	spellsPerDayTitleRect.height = spellsPerDayMeasure.height;

	// Spell Level labels
	spellsPerDayBorderRects.clear();
	spellsLevelLabelRects.clear();
	
	for (auto lvl = 0u; lvl < NUM_SPELL_LEVELS; lvl++) {
		auto textMeas = UiRenderer::MeasureTextSize(chargen.levelLabels[lvl].c_str(), spellLevelLabelStyle);
		spellsPerDayBorderRects.push_back(TigRect(105 + lvl * 51, 201, 29, 25));
		spellsLevelLabelRects.push_back(TigRect(spellsPerDayBorderRects[lvl].x + spellsPerDayBorderRects[lvl].width / 2 - textMeas.width / 2,
			spellsPerDayBorderRects[lvl].y - textMeas.height - 2, textMeas.width, textMeas.height));
	}
	UiRenderer::PopFont();
	return 1;
}

void UiPcCreation::SpellsReset(){
	chargen.SpellsNeedResetSet(true);
	chargen.GetKnownSpellInfo().clear();
}

void UiPcCreation::SpellsWidgetsFree(){
	for (auto i = 0; i < SPELLS_BTN_COUNT; i++) {
		uiManager->RemoveChildWidget(spellsChosenBtnIds[i]);
		uiManager->RemoveChildWidget(spellsAvailBtnIds[i]);
	}
	spellsChosenBtnIds.clear();
	spellsAvailBtnIds.clear();
	uiManager->RemoveWidget(spellsWndId);
}

BOOL UiPcCreation::SpellsShow()
{
	uiManager->SetHidden(spellsWndId, false);
	uiManager->BringToFront(spellsWndId);
	return 1;
}

BOOL UiPcCreation::SpellsHide()
{
	uiManager->SetHidden(spellsWndId, true);
	return 0;
}

BOOL UiPcCreation::SpellsWidgetsResize(UiResizeArgs & args)
{
	SpellsWidgetsFree();
	SpellsWidgetsInit();
	return 0;
}

void UiPcCreation::SpellsActivate()
{
	auto handle = chargen.GetEditedChar();
	auto obj = gameSystems->GetObj().GetObject(handle);
	auto &selPkt = chargen.GetCharEditorSelPacket();
	auto &avSpInfo = chargen.GetAvailableSpells();
	auto &knSpInfo = chargen.GetKnownSpellInfo();

	// get the new caster level for the levelled class (1 indicates a newly taken class)
	auto casterLvlNew = 1;
	auto classLeveled = selPkt.classCode;
	
	auto needsReset = chargen.SpellsNeedReset();

	static auto setScrollbars = []() {
		auto sbId = uiPcCreation.spellsScrollbarId;
		uiManager->ScrollbarSetY(sbId, 0);
		int numEntries = (int)chargen.GetAvailableSpells().size();
		uiManager->ScrollbarSetYmax(sbId, max(0, numEntries - uiPcCreation.SPELLS_BTN_COUNT));
		uiPcCreation.spellsScrollbar = *uiManager->GetScrollBar(sbId);
		uiPcCreation.spellsScrollbar.y = 0;
		uiPcCreation.spellsScrollbarY = 0;

		auto &charEdSelPkt = chargen.GetCharEditorSelPacket();
		auto sbAddedId = uiPcCreation.spellsScrollbar2Id;
		int numAdded = (int)chargen.GetKnownSpellInfo().size();
		uiManager->ScrollbarSetY(sbAddedId, 0);
		uiManager->ScrollbarSetYmax(sbAddedId, max(0, numAdded - uiPcCreation.SPELLS_BTN_COUNT));
		uiPcCreation.spellsScrollbar2 = *uiManager->GetScrollBar(sbAddedId);
		uiPcCreation.spellsScrollbar2.y = 0;
		uiPcCreation.spellsScrollbar2Y = 0;
	};

	if (!needsReset) {
		setScrollbars();
		return;
	}


	knSpInfo.clear();
	avSpInfo.clear();

	d20ClassSys.LevelupInitSpellSelection(handle, selPkt.classCode, 1);


	for (auto i = 0u; i < knSpInfo.size(); i++) {
		auto spEnum = knSpInfo[i].spEnum;
		if (spellSys.IsNewSlotDesignator(spEnum)) {
			knSpInfo[i].spEnum = 802;
			knSpInfo[i].spFlag = 3;
		}
	}

	SpellsPerDayUpdate();

	setScrollbars();
	chargen.SpellsNeedResetSet(false);
}

BOOL UiPcCreation::SpellsCheckComplete()
{
	auto selPkt = GetCharEditorSelPacket();
	auto handle = GetEditedChar();
	if (!d20ClassSys.IsSelectingSpellsOnLevelup(handle, selPkt.classCode))
		return true;

	if (chargen.SpellsNeedReset())
		return false;

	return d20ClassSys.LevelupSpellsCheckComplete(GetEditedChar(), selPkt.classCode);
}

void UiPcCreation::SpellsFinalize(){
	auto charEdited = GetEditedChar();
	auto &selPkt = GetCharEditorSelPacket();

	d20ClassSys.LevelupSpellsFinalize(charEdited, selPkt.classCode, 1);
}

void UiPcCreation::SpellsReset(CharEditorSelectionPacket & selPkt)
{
	temple::GetRef<int>(0x10C4D4C4) = 1; // needsPopulateEntries
	chargen.GetKnownSpellInfo().clear();
	chargen.GetAvailableSpells().clear();
}

void UiPcCreation::StateTitleRender(int widId){
	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	int state = GetState();
	auto &selPkt = GetCharEditorSelPacket();
	auto &stateTitles = temple::GetRef<const char*[14]>(0x10BDAE28);
	auto &altStateTitles = temple::GetRef<const char*[14]>(0x10BDAE60);


	auto &rect = temple::GetRef<TigRect>(0x10BDB8E8);
	auto &style = temple::GetRef<TigTextStyle>(0x10BDD638);
	if (!selPkt.isPointbuy)
		UiRenderer::DrawTextInWidget(widId, stateTitles[state], rect, style);
	else
		UiRenderer::DrawTextInWidget(widId, altStateTitles[state], rect, style);
	UiRenderer::PopFont();
}

int UiPcCreation::GetRolledStatIdx(int x, int y, int * xyOut){

	auto &statsWnd = GetStatsWnd();

	auto relX = x - statsWnd.x;
	auto relY = y - statsWnd.y;

	auto idx = 0;
	const int y0 = 71;
	const int x0 = 203, xMax = 241;
	const int yBtnSize = 31;

	if (relX < x0 || relX > xMax)
		return -1;

	auto btnY = y0;
	auto foundIt = false;
	for (auto i=0; i < 6; i++){

		if ( relY <= btnY && relY >= btnY - 27){
			foundIt = true;
			idx = i;
			break;
		}

		btnY += 31;
	}

	if (!foundIt)
		return -1;

	if (xyOut){
		xyOut[0] = relX - x0;
		xyOut[1] = relY - yBtnSize*idx - 44;
	}
	return idx;
}

BOOL UiPcCreation::StatsWndMsg(int widId, TigMsg * msg){
	// returning FALSE indicates it should pass the handling on to the original handler

	if (msg->type != TigMsgType::MOUSE)
		return FALSE;

	auto msgMouse = (TigMsgMouse*)msg;
	if (msgMouse->buttonStateFlags & MouseStateFlags::MSF_RMB_RELEASED){
		auto rolledStatIdx = GetRolledStatIdx(msgMouse->x, msgMouse->y);
		if (rolledStatIdx == -1)
			return FALSE;

		auto rolledStats = chargen.GetRolledStats();
		if (rolledStats[rolledStatIdx] <= 0)
			return FALSE;

		auto &selPkt = chargen.GetCharEditorSelPacket();
		for (auto i = 0; i < 6; i++) {
			if (selPkt.abilityStats[i] <= 0){
				selPkt.abilityStats[i] = rolledStats[rolledStatIdx];
				rolledStats[rolledStatIdx] = -1;
				return TRUE;
			}
		}
	}
		

	return FALSE;
}

void UiPcCreation::ClassBtnRender(int widId){
	auto idx = WidgetIdIndexOf(widId, &classBtnIds[0], classBtnIds.size());
	if (idx == -1)
		return;

	auto page = GetClassWndPage();
	auto classCode = GetClassCodeFromWidgetAndPage(idx, page);
	if (classCode == (Stat)-1)
		return;

	static TigRect srcRect(1, 1, 120, 30);
	UiRenderer::DrawTexture(buttonBox, classBtnFrameRects[idx], srcRect);

	auto btnState = uiManager->GetButtonState(widId);
	if (btnState != LgcyButtonState::Disabled && btnState != LgcyButtonState::Down)
	{
		auto &selPkt = GetCharEditorSelPacket();
		if (selPkt.classCode == classCode)
			btnState = LgcyButtonState::Released;
		else
			btnState = btnState == LgcyButtonState::Hovered ? LgcyButtonState::Hovered : LgcyButtonState::Normal;
	}

	auto texId = temple::GetRef<int[15]>(0x11E74140)[(int)btnState];
	static TigRect srcRect2(1, 1, 110, 20);
	auto &rect = classBtnRects[idx];
	UiRenderer::DrawTextureInWidget(classWndId, texId, rect, srcRect2);

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	auto textt = classNamesUppercase[classCode].c_str();

	auto textMeas = UiRenderer::MeasureTextSize(textt, classBtnTextStyle);
	TigRect classTextRect(rect.x + (rect.width - textMeas.width) / 2,
		rect.y + (rect.height - textMeas.height) / 2,
		textMeas.width, textMeas.height);

	UiRenderer::DrawTextInWidget(classWndId, textt, classTextRect, classBtnTextStyle);
	UiRenderer::PopFont();
}

BOOL UiPcCreation::ClassBtnMsg(int widId, TigMsg * msg){

	if (msg->type != TigMsgType::WIDGET)
		return 0;

	auto idx = WidgetIdIndexOf(widId, &classBtnIds[0], classBtnIds.size());
	if (idx == -1)
		return 0;

	auto _msg = (TigMsgWidget*)msg;
	auto classCode = GetClassCodeFromWidgetAndPage(idx, GetClassWndPage());
	if (classCode == (Stat)-1)
		return 0;

	auto handle = GetEditedChar();
	auto obj = objSystem->GetObject(handle);

	if (_msg->widgetEventType == TigMsgWidgetEvent::MouseReleased) {

		if (helpSys.IsClickForHelpActive()){
			helpSys.PresentWikiHelp(HELP_IDX_CLASSES + classCode - stat_level_barbarian, D20HelpType::Classes);
			return TRUE;
		}
		GetCharEditorSelPacket().classCode = classCode;
		obj->ClearArray(obj_f_critter_level_idx);
		obj->SetInt32(obj_f_critter_level_idx, 0, classCode);
		d20StatusSys.D20StatusRefresh(handle);
		critterSys.GenerateHp(handle);

		ToggleClassRelatedStages();
		ResetNextStages(CG_Stage_Class);
		return TRUE;
	}

	if (_msg->widgetEventType == TigMsgWidgetEvent::Exited) {
		ClassBtnEntered();
		return TRUE;
	}

	if (_msg->widgetEventType == TigMsgWidgetEvent::Entered) {

		auto isValid = true;
		if (!IsCastingStatSufficient(classCode)){
			isValid = false;
		}

		if (!IsAlignmentOk(classCode))
			isValid = false;
		
		if (!isValid){
			temple::GetRef<void(__cdecl)(Stat)>(0x1011B990)(classCode); // sets text in the scrollbox for why you can't pick the class 
			return TRUE;
		}
		

		ClassScrollboxTextSet(classCode); // ChargenClassScrollboxTextSet  (class short description)
		return TRUE;
	}


	return 0;
}

BOOL UiPcCreation::ClassNextBtnMsg(int widId, TigMsg * msg){

	if (!config.nonCoreMaterials)
		return FALSE;

	if (msg->type != TigMsgType::WIDGET)
		return FALSE;

	auto _msg = (TigMsgWidget*)msg;

	if (_msg->widgetEventType == TigMsgWidgetEvent::Clicked) {
		if (classWndPage < mPageCount - 1)
			classWndPage++;
		ClassSetPermissibles();
		return TRUE;
	}

	/*if (_msg->widgetEventType == TigMsgWidgetEvent::Exited) {
		temple::GetRef<void(__cdecl)(const char*)>(0x10162C00)("");
		return 1;
	}

	if (_msg->widgetEventType == TigMsgWidgetEvent::Entered) {
		auto textboxText = fmt::format("Prestige Classes");
		if (textboxText.size() >= 1024)
			textboxText[1023] = 0;
		strcpy(temple::GetRef<char[1024]>(0x10C80CC0), &textboxText[0]);
		temple::GetRef<void(__cdecl)(const char*)>(0x10162C00)(temple::GetRef<char[1024]>(0x10C80CC0));
		return 1;
	}*/

	return FALSE;
}

BOOL UiPcCreation::ClassPrevBtnMsg(int widId, TigMsg * msg)
{

	if (!config.nonCoreMaterials)
		return FALSE;

	if (msg->type != TigMsgType::WIDGET)
		return 0;

	auto _msg = (TigMsgWidget*)msg;

	if (_msg->widgetEventType == TigMsgWidgetEvent::Clicked) {
		if (classWndPage > 0)
			classWndPage--;
		ClassSetPermissibles();
		return TRUE;
	}

	return FALSE;
}

BOOL UiPcCreation::FinishBtnMsg(int widId, TigMsg * msg)
{
	if (msg->type == TigMsgType::MOUSE)
		return TRUE;

	if (msg->type != TigMsgType::WIDGET)
		return FALSE;

	auto _msg = (TigMsgWidget*)msg;

	if (_msg->widgetEventType != TigMsgWidgetEvent::MouseReleased)
		return TRUE;

	auto stComplete = GetStatesComplete();
	if (stComplete != CG_STAGE_COUNT)
		return TRUE;

	auto &selPkt = GetCharEditorSelPacket();
	auto charEdited = GetEditedChar();

	// add spell casting condition
	if (d20ClassSys.IsCastingClass(selPkt.classCode)) {
		auto spellcastCond = (std::string)d20ClassSys.GetSpellCastingCondition(selPkt.classCode);
		if (spellcastCond.size()) {
			conds.AddTo(charEdited, spellcastCond, { 0,0,0,0, 0,0,0,0 });
		}
	}
	return TRUE;
}

void UiPcCreation::ClassNextBtnRender(int widId){
	if (!config.nonCoreMaterials)
		return;

	static TigRect srcRect(1, 1, 120, 30);
	UiRenderer::DrawTexture(buttonBox, classNextBtnFrameRect, srcRect);

	auto btnState = uiManager->GetButtonState(widId);
	if (btnState != LgcyButtonState::Disabled && btnState != LgcyButtonState::Down) {
		btnState = btnState == LgcyButtonState::Hovered ? LgcyButtonState::Hovered : LgcyButtonState::Normal;
	}

	auto texId = temple::GetRef<int[15]>(0x11E74140)[(int)btnState];
	static TigRect srcRect2(1, 1, 110, 20);
	UiRenderer::DrawTexture(texId, classNextBtnRect, srcRect2);

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	auto textt = fmt::format("NEXT");
	auto textMeas = UiRenderer::MeasureTextSize(textt, classBtnTextStyle);
	TigRect textRect(classNextBtnTextRect.x + (classNextBtnTextRect.width - textMeas.width) / 2,
		classNextBtnTextRect.y + (classNextBtnTextRect.height - textMeas.height) / 2,
		textMeas.width, textMeas.height);
	UiRenderer::DrawTextInWidget(classWndId, textt, textRect, classBtnTextStyle);
	UiRenderer::PopFont();

}

void UiPcCreation::ClassPrevBtnRender(int widId){
	if (!config.nonCoreMaterials)
		return;

	static TigRect srcRect(1, 1, 120, 30);
	UiRenderer::DrawTexture(buttonBox, classPrevBtnFrameRect, srcRect);

	auto btnState = uiManager->GetButtonState(widId);
	if (btnState != LgcyButtonState::Disabled && btnState != LgcyButtonState::Down) {
		btnState = btnState == LgcyButtonState::Hovered ? LgcyButtonState::Hovered : LgcyButtonState::Normal;
	}

	auto texId = temple::GetRef<int[15]>(0x11E74140)[(int)btnState];
	static TigRect srcRect2(1, 1, 110, 20);
	UiRenderer::DrawTexture(texId, classPrevBtnRect, srcRect2);

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	auto textt = fmt::format("PREV");
	auto textMeas = UiRenderer::MeasureTextSize(textt, classBtnTextStyle);
	TigRect textRect(classPrevBtnTextRect.x + (classPrevBtnTextRect.width - textMeas.width) / 2,
		classPrevBtnTextRect.y + (classPrevBtnTextRect.height - textMeas.height) / 2,
		textMeas.width, textMeas.height);
	UiRenderer::DrawTextInWidget(classWndId, textt, textRect, classBtnTextStyle);
	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsWndMsg(int widId, TigMsg* msg){
	if (msg->type == TigMsgType::WIDGET) {
		auto msgW = (TigMsgWidget*)msg;
		if (msgW->widgetEventType == TigMsgWidgetEvent::Scrolled) {
			uiManager->ScrollbarGetY(featsScrollbarId, &featsScrollbarY);
			uiManager->ScrollbarGetY(featsExistingScrollbarId, &featsExistingScrollbarY);
		}
		return FALSE;
	}

	if (msg->type != TigMsgType::MOUSE)
		return FALSE;


	auto msgM = (TigMsgMouse*)msg;
	auto &selPkt = GetCharEditorSelPacket();

	if (msgM->buttonStateFlags & MouseStateFlags::MSF_RMB_RELEASED && uiManager->IsHidden(featsMultiSelectWndId)) {

		auto putFeat = false;
		feat_enums feat;

		// cycle thru widgets to find the one where the RMB happened
		for (auto i = 0; i < FEATS_AVAIL_BTN_COUNT; i++) {
			if (!featsBtnRects[i].ContainsPoint(msgM->x - featsMainWnd.x, msgM->y - featsMainWnd.y))
				continue;

			auto featIdx = i + featsScrollbarY;
			if (featIdx >= (int)mSelectableFeats.size())
				break;

			feat = (feat_enums)mSelectableFeats[featIdx].featEnum;


			if (IsSelectingNormalFeat() && selPkt.feat0 == FEAT_NONE) {
				selPkt.feat0 = feat;
				putFeat = true;
				break;
			}
			
			if(IsSelectingSecondFeat() && selPkt.feat1 == FEAT_NONE)
			{
				selPkt.feat1 = feat;
				putFeat = true;
				break;
			}
			
			if (IsSelectingBonusFeat() && IsClassBonusFeat(feat) && selPkt.feat2 == FEAT_NONE)
			{
				selPkt.feat2 = feat;
				putFeat = true;
				break;
			}
		}

		if (putFeat) {

			if (feats.IsFeatMultiSelectMaster(feat)) {
				FeatsMultiSelectActivate(feat);
			}
			FeatsSanitize();
			if (feat == FEAT_SKILL_MASTERY && selPkt.feat2 == feat) {
				auto skillMasteryActivate = temple::GetRef<void(__cdecl)(objHndl, int(__cdecl*)(int))>(0x1016C2B0);
				skillMasteryActivate(GetEditedChar(), temple::GetRef<int(__cdecl)(int)>(0x101A86D0));
			}
		}

		else if (featsSelectedBorderRect.ContainsPoint(msgM->x, msgM->y)) {
			selPkt.feat0 = FEAT_NONE;
		}
		else if (featsSelected2BorderRect.ContainsPoint(msgM->x, msgM->y)) {
			selPkt.feat1 = FEAT_NONE;
		}
		else if (featsClassBonusBorderRect.ContainsPoint(msgM->x, msgM->y) && IsSelectingBonusFeat()) {
			selPkt.feat2 = FEAT_NONE;
		}

	}

	if (!(msgM->buttonStateFlags & MouseStateFlags::MSF_SCROLLWHEEL_CHANGE))
		return TRUE;

	TigMsgMouse msgCopy = *msgM;
	msgCopy.buttonStateFlags = MouseStateFlags::MSF_SCROLLWHEEL_CHANGE;

	if ((int)msgM->x >= featsMainWnd.x + 3 && (int)msgM->x <= featsMainWnd.x + 188
		&& (int)msgM->y >= featsMainWnd.y + 36 && (int)msgM->y <= featsMainWnd.y + 263) {
		featsScrollbar = *uiManager->GetScrollBar(featsScrollbarId);
		if (featsScrollbar.handleMessage)
			return featsScrollbar.handleMessage(featsScrollbarId, (TigMsg*)&msgCopy);
	}

	if ((int)msgM->x >= featsMainWnd.x + 207 && (int)msgM->x <= featsMainWnd.x + 392
		&& (int)msgM->y >= featsMainWnd.y + 118 && (int)msgM->y <= featsMainWnd.y + 263) {
		featsExistingScrollbar = *uiManager->GetScrollBar(featsExistingScrollbarId);
		if (featsExistingScrollbar.handleMessage)
			return featsExistingScrollbar.handleMessage(featsExistingScrollbarId, (TigMsg*)&msgCopy);
	}

	return FALSE;
}

void UiPcCreation::FeatsWndRender(int widId)
{
	auto &selPkt = GetCharEditorSelPacket();

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);

	// Feats title
	RenderHooks::RenderRectInt(featsMainWnd.x + 17, featsMainWnd.y + 32, 185, 198, 0xFF5D5D5D);
	UiRenderer::DrawTextInWidget(widId, featsAvailTitleString, featsAvailTitleRect, whiteTextGenericStyle);

	// Feat Slot
	if (IsSelectingNormalFeat()) {
		RenderHooks::RenderRectInt(featsSelectedBorderRect.x, featsSelectedBorderRect.y, featsSelectedBorderRect.width, featsSelectedBorderRect.height, 0xFFFFffff);
		UiRenderer::DrawTextInWidget(widId, featsTitleString, featsTitleRect, featsNormalTextStyle);
		if (selPkt.feat0 != FEAT_NONE) {
			UiRenderer::DrawTextInWidget(widId, GetFeatName(selPkt.feat0), feat0TextRect, GetFeatStyle(selPkt.feat0));
		}
	}
	if (IsSelectingSecondFeat()){
		RenderHooks::RenderRectInt(featsSelected2BorderRect.x, featsSelected2BorderRect.y, featsSelected2BorderRect.width, featsSelected2BorderRect.height, 0xFFFFffff);
		if (selPkt.feat1 != FEAT_NONE) {
			UiRenderer::DrawTextInWidget(widId, GetFeatName(selPkt.feat1), feat1TextRect, GetFeatStyle(selPkt.feat1));
		}
	}

	// Class Bonus Feat slot
	if (IsSelectingBonusFeat()) {
		// title Class Bonus Feat
		RenderHooks::RenderRectInt(featsClassBonusBorderRect.x, featsClassBonusBorderRect.y, featsClassBonusBorderRect.width, featsClassBonusBorderRect.height, 0xFFFFD919);
		UiRenderer::DrawTextInWidget(widId, featsClassBonusTitleString, featsClassBonusRect, featsGoldenStyle);

		if (selPkt.feat2 != FEAT_NONE) {
			UiRenderer::DrawTextInWidget(widId, GetFeatName(selPkt.feat2), feat2TextRect, GetFeatStyle(selPkt.feat2));
		}
	}

	// Class Feats rect+title
	RenderHooks::RenderRectInt(featsMainWnd.x + 220, featsMainWnd.y + 138, 186, 92, 0xFF5D5D5D);
	UiRenderer::DrawTextInWidget(widId, featsExistingTitleString, featsExistingTitleRect, featsExistingTitleStyle);

	StateTitleRender(widId);

	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsEntryBtnMsg(int widId, TigMsg* msg)
{
	if (msg->type != TigMsgType::WIDGET)
		return 0;
	auto msgW = (TigMsgWidget*)msg;

	auto widIdx = WidgetIdIndexOf(widId, &featsAvailBtnIds[0], FEATS_AVAIL_BTN_COUNT);
	auto featIdx = widIdx + featsScrollbarY;
	if (widIdx == -1 || featIdx >= (int)mSelectableFeats.size())
		return FALSE;

	auto featInfo = mSelectableFeats[featIdx];
	auto feat = (feat_enums)featInfo.featEnum;

	auto &selPkt = GetCharEditorSelPacket();
	auto btn = uiManager->GetButton(widId);

	switch (msgW->widgetEventType) {
	case TigMsgWidgetEvent::Clicked:
		if (!FeatAlreadyPicked(feat) && FeatCanPick(feat)) {
			auto origX = msgW->x - btn->x, origY = msgW->y - btn->y;
			auto style = uiPcCreation.GetFeatStyle(feat);
			auto featCallback = [origX, origY, feat, style](int x, int y) {
				std::string text(uiPcCreation.GetFeatName(feat));
				UiRenderer::PushFont(PredefinedFont::PRIORY_12);
				TigRect rect(x - origX, y - origY, 180, uiPcCreation.FEATS_AVAIL_BTN_HEIGHT);
				tigFont.Draw(text.c_str(), rect, style);
				UiRenderer::PopFont();
			};
			mouseFuncs.SetCursorDrawCallback(featCallback, (uint32_t)&featCallback);

		}
		return TRUE;
	case TigMsgWidgetEvent::MouseReleased:
		if (helpSys.IsClickForHelpActive()) {
			mouseFuncs.SetCursorDrawCallback(nullptr, 0);
			helpSys.PresentWikiHelp(109 + feat);
			return TRUE;
		}
	case TigMsgWidgetEvent::MouseReleasedAtDifferentButton:
		if (FeatAlreadyPicked(feat) || !FeatCanPick(feat))
			return TRUE;
		mouseFuncs.SetCursorDrawCallback(nullptr, 0);

		// check if inserted into the normal slot
		if (featsSelectedBorderRect.ContainsPoint(msgW->x, msgW->y) && IsSelectingNormalFeat()) {
			selPkt.feat0 = feat;
			if (feats.IsFeatMultiSelectMaster(feat))
				FeatsMultiSelectActivate(feat);
		}
		else if (IsSelectingSecondFeat() && featsSelected2BorderRect.ContainsPoint(msgW->x, msgW->y)){
			selPkt.feat1 = feat;
			if (feats.IsFeatMultiSelectMaster(feat))
				FeatsMultiSelectActivate(feat);
		}
		// check if inserted into the bonus slot
		else if (IsSelectingBonusFeat()
			&& featsClassBonusBorderRect.ContainsPoint(msgW->x, msgW->y) && IsClassBonusFeat(feat)) {
			selPkt.feat2 = feat;
			if (feats.IsFeatMultiSelectMaster(feat))
				FeatsMultiSelectActivate(feat);
			else if (feat == FEAT_SKILL_MASTERY) {
				auto skillMasteryActivate = temple::GetRef<void(__cdecl)(objHndl, int(__cdecl*)(int))>(0x1016C2B0);
				skillMasteryActivate(GetEditedChar(), temple::GetRef<int(__cdecl)(int)>(0x101A86D0));
			}
		}
		FeatsSanitize();
		return TRUE;
	case TigMsgWidgetEvent::Entered:
		//temple::GetRef<void(int, char*, size_t)>(0x10162A10)(FeatsMultiGetFirst(feat), temple::GetRef<char[1024]>(0x10C76B48), 1024u); // UiTooltipSetForFeat
		//temple::GetRef<void(char*)>(0x10162C00)(temple::GetRef<char[1024]>(0x10C76B48)); // UiCharTextboxSet
		temple::GetRef<void(__cdecl)(feat_enums)>(0x1011BB50)(FeatsMultiGetFirst(feat));
		return TRUE;
	case TigMsgWidgetEvent::Exited:
		temple::GetRef<void(__cdecl)(char *)>(0x10162C00)(""); // UiCharTextboxSet
		return TRUE;
	default:
		return FALSE;

	}
	return TRUE;
}

void UiPcCreation::FeatsEntryBtnRender(int widId)
{
	auto widIdx = WidgetIdIndexOf(widId, &featsAvailBtnIds[0], FEATS_AVAIL_BTN_COUNT);
	auto featIdx = widIdx + featsScrollbarY;
	if (widIdx == -1 || featIdx >= (int)mSelectableFeats.size())
		return;

	auto featInfo = mSelectableFeats[featIdx];
	auto feat = (feat_enums)featInfo.featEnum;

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);

	UiRenderer::DrawTextInWidget(featsMainWndId, GetFeatName(feat), featsBtnRects[widIdx], GetFeatStyle(feat, false));

	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsExistingBtnMsg(int widId, TigMsg* msg)
{
	if (msg->type != TigMsgType::WIDGET)
		return 0;
	auto msgW = (TigMsgWidget*)msg;

	auto widIdx = WidgetIdIndexOf(widId, &featsExistingBtnIds[0], FEATS_EXISTING_BTN_COUNT);
	auto featIdx = widIdx + featsExistingScrollbarY;
	if (widIdx == -1 || featIdx >= (int)mExistingFeats.size())
		return FALSE;

	auto featInfo = mExistingFeats[featIdx];
	auto feat = (feat_enums)featInfo.featEnum;

	auto &selPkt = GetCharEditorSelPacket();
	auto btn = uiManager->GetButton(widId);

	switch (msgW->widgetEventType) {
	case TigMsgWidgetEvent::Entered:
		temple::GetRef<void(__cdecl)(feat_enums)>(0x1011BB50)(FeatsMultiGetFirst(feat));
		return TRUE;
	case TigMsgWidgetEvent::Exited:
		temple::GetRef<void(__cdecl)(char *)>(0x10162C00)(""); // UiCharTextboxSet
		return TRUE;
	default:
		return FALSE;

	}
	return TRUE;
}

void UiPcCreation::FeatsExistingBtnRender(int widId)
{
	auto widIdx = WidgetIdIndexOf(widId, &featsExistingBtnIds[0], FEATS_EXISTING_BTN_COUNT);
	auto featIdx = widIdx + featsExistingScrollbarY;
	if (widIdx == -1 || featIdx >= (int)mExistingFeats.size())
		return;

	auto featInfo = mExistingFeats[featIdx];
	auto feat = (feat_enums)featInfo.featEnum;

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);

	UiRenderer::DrawTextInWidget(featsMainWndId, GetFeatName(feat), featsExistingBtnRects[widIdx], featsClassStyle);

	UiRenderer::PopFont();
}

void UiPcCreation::FeatsMultiSelectWndRender(int widId)
{
	featsbackdrop->SetX(featsMultiSelectWnd.x + featsMultiCenterX);
	featsbackdrop->SetY(featsMultiSelectWnd.y + featsMultiCenterY);
	featsbackdrop->Render();

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);

	UiRenderer::DrawTextInWidget(widId, GetFeatName(mFeatsMultiMasterFeat), featMultiTitleRect, featsCenteredStyle);

	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsMultiSelectWndMsg(int widId, TigMsg* msg)
{
	if (msg->type != TigMsgType::WIDGET && msg->type != TigMsgType::KEYSTATECHANGE)
		return FALSE;

	uiManager->ScrollbarGetY(featsMultiSelectScrollbarId, &featsMultiSelectScrollbarY);

	return TRUE;
}

void UiPcCreation::FeatsMultiOkBtnRender(int widId)
{
	auto buttonState = uiManager->GetButtonState(widId);

	int texId;
	switch (buttonState) {
	case LgcyButtonState::Normal:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::AcceptNormal, texId);
		break;
	case LgcyButtonState::Hovered:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::AcceptHover, texId);
		break;
	case LgcyButtonState::Down:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::AcceptPressed, texId);
		break;
	case LgcyButtonState::Disabled:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::DisabledNormal, texId);
		break;
	default:
		break;
	}

	static TigRect srcRect(1, 1, 110, 22);
	UiRenderer::DrawTextureInWidget(featsMultiSelectWndId, texId, featMultiOkRect, srcRect);


	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	UiRenderer::DrawTextInWidget(featsMultiSelectWndId, combatSys.GetCombatMesLine(6009), featMultiOkTextRect, featsCenteredStyle);
	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsMultiOkBtnMsg(int widId, TigMsg* msg)
{
	if (msg->type != TigMsgType::WIDGET)
		return FALSE;
	auto msgW = (TigMsgWidget*)msg;
	if (msgW->widgetEventType != TigMsgWidgetEvent::MouseReleased)
		return FALSE;

	auto &selPkt = GetCharEditorSelPacket();

	if (featsMultiSelected == FEAT_NONE) {
		if (selPkt.feat0 == mFeatsMultiMasterFeat) {
			selPkt.feat0 = FEAT_NONE;
		}
		if (selPkt.feat1 == mFeatsMultiMasterFeat) {
			selPkt.feat1 = FEAT_NONE;
		}
		if (selPkt.feat2 == mFeatsMultiMasterFeat) {
			selPkt.feat2 = FEAT_NONE;
		}
	}
	else
	{
		if (selPkt.feat2 == mFeatsMultiMasterFeat) {
			selPkt.feat2 = featsMultiSelected;
		}
		else if (selPkt.feat0 == mFeatsMultiMasterFeat) {
			selPkt.feat0 = featsMultiSelected;
		}
		else if (selPkt.feat1 == mFeatsMultiMasterFeat) {
			selPkt.feat1 = featsMultiSelected;
		}
	}

	mFeatsMultiMasterFeat = FEAT_NONE;
	featsMultiSelected = FEAT_NONE;
	uiManager->SetHidden(featsMultiSelectWndId, true);

	return TRUE;
}

void UiPcCreation::FeatsMultiCancelBtnRender(int widId)
{
	auto buttonState = uiManager->GetButtonState(widId);

	int texId;
	switch (buttonState) {
	case LgcyButtonState::Normal:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::DeclineNormal, texId);
		break;
	case LgcyButtonState::Hovered:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::DeclineHover, texId);
		break;
	case LgcyButtonState::Down:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::DeclinePressed, texId);
		break;
	case LgcyButtonState::Disabled:
		uiAssets->GetAsset(UiAssetType::Generic, UiGenericAsset::DisabledNormal, texId);
		break;
	default:
		break;
	}

	static TigRect srcRect(1, 1, 110, 22);
	UiRenderer::DrawTextureInWidget(featsMultiSelectWndId, texId, featMultiCancelRect, srcRect);


	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	UiRenderer::DrawTextInWidget(featsMultiSelectWndId, combatSys.GetCombatMesLine(6010), featMultiCancelTextRect, featsCenteredStyle);
	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsMultiCancelBtnMsg(int widId, TigMsg* msg)
{
	if (msg->type != TigMsgType::WIDGET)
		return FALSE;
	auto msgW = (TigMsgWidget*)msg;
	if (msgW->widgetEventType != TigMsgWidgetEvent::MouseReleased)
		return FALSE;

	auto &selPkt = GetCharEditorSelPacket();

	if (selPkt.feat0 == mFeatsMultiMasterFeat) {
		selPkt.feat0 = FEAT_NONE;
	}
	if (selPkt.feat1 == mFeatsMultiMasterFeat) {
		selPkt.feat1 = FEAT_NONE;
	}
	if (selPkt.feat2 == mFeatsMultiMasterFeat) {
		selPkt.feat2 = FEAT_NONE;
	}

	mFeatsMultiMasterFeat = FEAT_NONE;
	featsMultiSelected = FEAT_NONE;
	uiManager->SetHidden(featsMultiSelectWndId, true);

	return TRUE;
}

void UiPcCreation::FeatsMultiBtnRender(int widId)
{
	auto widIdx = WidgetIdIndexOf(widId, &featsMultiSelectBtnIds[0], FEATS_MULTI_BTN_COUNT);
	auto featIdx = widIdx + featsMultiSelectScrollbarY;
	if (widIdx == -1 || featIdx >= (int)mMultiSelectFeats.size())
		return;

	auto featInfo = mMultiSelectFeats[featIdx];
	auto feat = (feat_enums)featInfo.featEnum;



	auto getFeatShortName = [](feat_enums ft) {

		if (ft > NUM_FEATS)
		{
			auto dummy = 1;
		}

		if (feats.IsFeatMultiSelectMaster(ft))
			return uiPcCreation.GetFeatName(ft);


		auto mesKey = 50000 + ft;

		if (feats.IsFeatPropertySet(ft, FPF_GREAT_WEAP_SPEC_ITEM)) {
			mesKey = 50000 + (ft - FEAT_GREATER_WEAPON_SPECIALIZATION_GAUNTLET + FEAT_WEAPON_SPECIALIZATION_GAUNTLET);
		}

		MesLine line(mesKey);
		auto pcCreationMes = temple::GetRef<MesHandle>(0x11E72EF0);
		auto text = mesFuncs.GetLineById(pcCreationMes, mesKey);
		if (text) {
			return std::string(text);
		}
		else
			return uiPcCreation.GetFeatName(ft);
	};

	auto ftName = getFeatShortName(feat);

	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	UiRenderer::DrawTextInWidget(featsMultiSelectWndId, ftName, featsMultiBtnRects[widIdx], GetFeatStyle(feat, false));

	UiRenderer::PopFont();
}

BOOL UiPcCreation::FeatsMultiBtnMsg(int widId, TigMsg* msg)
{
	if (msg->type == TigMsgType::MOUSE)
		return TRUE;

	if (msg->type != TigMsgType::WIDGET)
		return FALSE;


	auto msgW = (TigMsgWidget*)msg;

	auto widIdx = WidgetIdIndexOf(widId, &featsMultiSelectBtnIds[0], FEATS_MULTI_BTN_COUNT);
	auto featIdx = widIdx + featsMultiSelectScrollbarY;
	if (widIdx == -1 || featIdx >= (int)mMultiSelectFeats.size())
		return FALSE;

	auto featInfo = mMultiSelectFeats[featIdx];
	auto feat = (feat_enums)featInfo.featEnum;

	auto &selPkt = GetCharEditorSelPacket();
	auto btn = uiManager->GetButton(widId);

	switch (msgW->widgetEventType) {
	case TigMsgWidgetEvent::MouseReleased:
		if (helpSys.IsClickForHelpActive()) {
			helpSys.PresentWikiHelp(109 + feat);
			return TRUE;
		}
		if (FeatCanPick(feat) && !FeatAlreadyPicked(feat)) {
			featsMultiSelected = feat;
			uiManager->SetButtonState(featsMultiOkBtnId, LgcyButtonState::Normal);
		}
		else
		{
			featsMultiSelected = FEAT_NONE;
			uiManager->SetButtonState(featsMultiOkBtnId, LgcyButtonState::Disabled);
		}
		return TRUE;
	default:
		return FALSE;

	}

	return FALSE;
}

void UiPcCreation::SpellsWndRender(int widId)
{
	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	UiRenderer::DrawTextInWidget(widId, spellsAvailLabel, spellsAvailTitleRect, spellsTitleStyle);
	UiRenderer::DrawTextInWidget(widId, spellsChosenLabel, spellsChosenTitleRect, spellsTitleStyle);
	for (auto i = 0; i < SPELLS_PER_DAY_BOXES_COUNT; i++) {
		UiRenderer::DrawTextInWidget(widId, chargen.levelLabels[i], spellsLevelLabelRects[i], spellLevelLabelStyle);
	}


	// RenderSpellsPerDay
	
	UiRenderer::DrawTextInWidget(widId, spellsPerDayLabel, spellsPerDayTitleRect, spellsTextStyle);
	UiRenderer::PopFont();

	UiRenderer::PushFont(PredefinedFont::ARIAL_BOLD_24);
	for (auto i = 0; i < SPELLS_PER_DAY_BOXES_COUNT; i++) {
		RenderHooks::RenderRectInt(spellsWnd.x + spellsPerDayBorderRects[i].x, spellsWnd.y + spellsPerDayBorderRects[i].y, spellsPerDayBorderRects[i].width, spellsPerDayBorderRects[i].height, 0xFF43586E);
		UiRenderer::DrawTextInWidget(widId, spellsPerDayTexts[i], spellsPerDayTextRects[i], spellsPerDayStyle);
		
	}
	UiRenderer::PopFont();

	// Rects
	RenderHooks::RenderRectInt(spellsWnd.x + 5  , spellsWnd.y + 35, 207, 149, 0xFF5D5D5D);
	RenderHooks::RenderRectInt(spellsWnd.x + 219, spellsWnd.y + 35, 207, 149, 0xFF5D5D5D);

	StateTitleRender(widId);
}

BOOL UiPcCreation::SpellsWndMsg(int widId, TigMsg * msg)
{
	if (msg->type == TigMsgType::WIDGET) {
		auto msgW = (TigMsgWidget*)msg;
		if (msgW->widgetEventType == TigMsgWidgetEvent::Scrolled) {
			uiManager->ScrollbarGetY(spellsScrollbarId, &spellsScrollbarY);
			uiManager->ScrollbarGetY(spellsScrollbar2Id, &spellsScrollbar2Y);
			SpellsPerDayUpdate();
			return 1;
		}
		return 0;
	}

	if (msg->type == TigMsgType::MOUSE) {
		auto msgM = (TigMsgMouse*)msg;
		if ((msgM->buttonStateFlags & MouseStateFlags::MSF_LMB_RELEASED) && helpSys.IsClickForHelpActive()) {
			// LMB handler - present help for spell

			auto &knSpInfo = chargen.GetKnownSpellInfo();
			for (auto i = 0; i < SPELLS_BTN_COUNT; i++) {
				// check if mouse within button
				if (!uiManager->DoesWidgetContain(spellsChosenBtnIds[i], msgM->x, msgM->y))
					continue;

				auto spellIdx = i + spellsScrollbar2Y;
				if ((uint32_t)spellIdx >= knSpInfo.size())
					break;

				auto spEnum = knSpInfo[spellIdx].spEnum;
				// ensure is not label
				if (spellSys.IsLabel(spEnum))
					break;

				helpSys.PresentWikiHelp(HELP_IDX_SPELLS + spEnum);
				return 1;
			}
		}
		if (msgM->buttonStateFlags & MouseStateFlags::MSF_RMB_RELEASED) {
			// RMB handler - add to known spells

			auto &knSpInfo = chargen.GetKnownSpellInfo();
			auto &avSpInfo = chargen.GetAvailableSpells();

			for (auto i = 0; i < SPELLS_BTN_COUNT; i++) {
				// get spell btn
				if (!uiManager->DoesWidgetContain(spellsAvailBtnIds[i], msgM->x, msgM->y))
					continue;
				auto spellAvailIdx = i + spellsScrollbarY;
				if ((uint32_t)spellAvailIdx >= avSpInfo.size())
					break;

				// got the avail btn, now search for suitable vacant slot
				auto spEnum = avSpInfo[spellAvailIdx].spEnum;
				auto spClass = avSpInfo[spellAvailIdx].spellClass;
				auto spLevel = avSpInfo[spellAvailIdx].spellLevel;

				if (spellSys.IsLabel(spEnum))
					break;

				if (chargen.SpellIsAlreadyKnown(spEnum, spClass) || chargen.SpellIsForbidden(spEnum))
					break;

				auto curSpellLvl = -1;
				auto foundSlot = false;
				for (auto j = 0u; j < knSpInfo.size(); j++) {
					auto spInfo = knSpInfo[j];
					if (spInfo.spellClass != spClass)
						continue;
					if (spellSys.IsLabel(spInfo.spEnum)) {
						curSpellLvl = spInfo.spellLevel;
						if (curSpellLvl > spLevel)
							break;
						continue;
					}

					if (spInfo.spEnum != SPELL_ENUM_VACANT)
						continue;

					// ensure spell slot is of correct level
					if (spInfo.spellLevel == -1 // for "wildcard" empty slots (e.g. Wizard)
						|| curSpellLvl == spLevel) {
						knSpInfo[j].spEnum = spEnum; // spell level might still be -1 so be careful when adding to spellbook later on!
						break;
					}
				}
			}
		}
		if (!(msgM->buttonStateFlags & MouseStateFlags::MSF_SCROLLWHEEL_CHANGE))
			return 1;

		TigMsgMouse msgCopy = *msgM;
		msgCopy.buttonStateFlags = MouseStateFlags::MSF_SCROLLWHEEL_CHANGE;

		if ((int)msgM->x >= spellsWnd.x + 4 && (int)msgM->x <= spellsWnd.x + 184
			&& (int)msgM->y >= spellsWnd.y && (int)msgM->y <= spellsWnd.y + 259) {
			spellsScrollbar = *uiManager->GetScrollBar(spellsScrollbarId);
			if (spellsScrollbar.handleMessage)
				return spellsScrollbar.handleMessage(spellsScrollbarId, (TigMsg*)&msgCopy);
		}

		if ((int)msgM->x >= spellsWnd.x + 206 && (int)msgM->x <= spellsWnd.x + 376
			&& (int)msgM->y >= spellsWnd.y && (int)msgM->y <= spellsWnd.y + 259) {
			spellsScrollbar2 = *uiManager->GetScrollBar(spellsScrollbar2Id);
			if (spellsScrollbar2.handleMessage)
				return spellsScrollbar2.handleMessage(spellsScrollbar2Id, (TigMsg*)&msgCopy);
		}
		return 1;

	}

	return 0;
}

void UiPcCreation::SpellsPerDayUpdate()
{
	UiRenderer::PushFont(PredefinedFont::ARIAL_BOLD_24);
	auto &selPkt = GetCharEditorSelPacket();

	spellsPerDayTexts.clear();
	for (auto i = 0; i < SPELLS_PER_DAY_BOXES_COUNT; i++) {
		auto &handle = GetEditedChar();
		auto casterLvl = objects.StatLevelGet(handle, selPkt.classCode);
		auto numSpells = d20ClassSys.GetNumSpellsFromClass(handle, selPkt.classCode, i, casterLvl);
		if (numSpells < 0)
			numSpells = 0;
		std::string text(fmt::format("{}", numSpells));
		spellsPerDayTexts.push_back(text);

		auto textMeas = UiRenderer::MeasureTextSize(text, spellsPerDayStyle);
		spellsPerDayTextRects[i].x = spellsPerDayBorderRects[i].x +
			(spellsPerDayBorderRects[i].width - textMeas.width) / 2;
		spellsPerDayTextRects[i].y = spellsPerDayBorderRects[i].y +
			(spellsPerDayBorderRects[i].height - textMeas.height) / 2;
		spellsPerDayTextRects[i].width = textMeas.width;
		spellsPerDayTextRects[i].height = textMeas.height;
	}
	UiRenderer::PopFont();
}

BOOL UiPcCreation::SpellsEntryBtnMsg(int widId, TigMsg * msg)
{
	if (msg->type != TigMsgType::WIDGET)
		return FALSE;
	auto widMsg = (TigMsgWidget*)msg;
	if (widMsg->widgetEventType != TigMsgWidgetEvent::MouseReleased)
		return FALSE;

	auto widIdx = WidgetIdIndexOf(widId, &spellsChosenBtnIds[0], SPELLS_BTN_COUNT);
	if (widIdx == -1)
		return FALSE;

	auto &knSpInfo = chargen.GetKnownSpellInfo();

	auto spellIdx = widIdx + spellsScrollbar2Y;
	if (spellIdx >= (int)knSpInfo.size())
		return FALSE;

	auto &spInfo = knSpInfo[spellIdx];
	auto spEnum = spInfo.spEnum;
	
	if (spellSys.IsLabel(spEnum))
		return FALSE;

	spInfo.spEnum = SPELL_ENUM_VACANT;

	return FALSE;
}

void UiPcCreation::SpellsEntryBtnRender(int widId)
{
	auto widIdx = WidgetIdIndexOf(widId, &spellsChosenBtnIds[0], SPELLS_BTN_COUNT);
	if (widIdx == -1)
		return;

	auto &knSpInfo = chargen.GetKnownSpellInfo();

	auto spellIdx = widIdx + spellsScrollbar2Y;
	if (spellIdx >= (int)knSpInfo.size())
		return;

	auto spInfo = knSpInfo[spellIdx];
	auto spFlag = spInfo.spFlag;
	auto spEnum = spInfo.spEnum;
	auto spLvl = spInfo.spellLevel;

	auto btn = uiManager->GetButton(widId);

	auto &selPkt = GetCharEditorSelPacket();
	if (spFlag && (!selPkt.spellEnumToRemove || spFlag != 1)) {
		RenderHooks::RenderRectInt(btn->x + 11, btn->y, btn->width - 11, btn->height, 0xFF222C37);
	}

	std::string text;
	TigRect rect(btn->x - spellsWnd.x, btn->y - spellsWnd.y, btn->width, btn->height);
	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	if (spEnum == SPELL_ENUM_VACANT) {
		// don't draw text (will only draw the frame)
	}
	else if (spellSys.IsLabel(spEnum)) {
		if (spLvl >= 0 && spLvl < NUM_SPELL_LEVELS) {
			text.append(fmt::format("{}", chargen.spellLevelLabels[spLvl]));
			UiRenderer::DrawTextInWidget(spellsWndId, text, rect, spellLevelLabelStyle);
		}
	}
	else
	{
		text.append(fmt::format("{}", spellSys.GetSpellMesline(spEnum)));
		rect.x += 11;
		//rect.width -= 11;
		UiRenderer::DrawTextInWidget(spellsWndId, text, rect, spellsTextStyle);
	}
	UiRenderer::PopFont();
}

BOOL UiPcCreation::SpellsAvailableEntryBtnMsg(int widId, TigMsg * msg)
{
	if (msg->type != TigMsgType::WIDGET)
		return 0;
	auto msgW = (TigMsgWidget*)msg;

	auto widIdx = WidgetIdIndexOf(widId, &spellsAvailBtnIds[0], SPELLS_BTN_COUNT);
	if (widIdx == -1)
		return 0;

	auto &avSpInfo = chargen.GetAvailableSpells();
	auto spellIdx = widIdx + spellsScrollbarY;
	if (spellIdx >= (int)avSpInfo.size())
		return 0;

	auto spInfo = avSpInfo[spellIdx];
	auto spFlag = spInfo.spFlag;
	auto spEnum = spInfo.spEnum;
	auto spLvl = spInfo.spellLevel;
	auto spClass = spInfo.spellClass;

	if (!spellSys.IsLabel(spEnum)) {

		auto btn = uiManager->GetButton(widId);
		auto curSpellLvl = -1;
		auto &selPkt = GetCharEditorSelPacket();
		auto &knSpInfo = chargen.GetKnownSpellInfo();

		switch (msgW->widgetEventType) {
		case TigMsgWidgetEvent::Clicked: // button down - initiate drag
			if (!chargen.SpellIsAlreadyKnown(spEnum, spClass) && !chargen.SpellIsForbidden(spEnum)) {
				auto origX = msgW->x - btn->x, origY = msgW->y - btn->y;
				auto spellCallback = [origX, origY, spEnum](int x, int y) {
					std::string text(spellSys.GetSpellMesline(spEnum));
					UiRenderer::PushFont(PredefinedFont::PRIORY_12);
					TigRect rect(x - origX, y - origY, 180, uiPcCreation.SPELLS_BTN_HEIGHT);
					tigFont.Draw(text.c_str(), rect, uiPcCreation.spellsTextStyle);
					UiRenderer::PopFont();
				};
				mouseFuncs.SetCursorDrawCallback(spellCallback, (uint32_t)&spellCallback);
			}
			return 1;
		case TigMsgWidgetEvent::MouseReleased:
			if (helpSys.IsClickForHelpActive()) {
				mouseFuncs.SetCursorDrawCallback(nullptr, 0);
				helpSys.PresentWikiHelp(spEnum + HELP_IDX_SPELLS);
				return 1;
			}
		case TigMsgWidgetEvent::MouseReleasedAtDifferentButton:
			mouseFuncs.SetCursorDrawCallback(nullptr, 0);
			if (chargen.SpellIsAlreadyKnown(spEnum, spClass)
				|| chargen.SpellIsForbidden(spEnum))
				return 1;


			for (auto i = 0u; i < knSpInfo.size(); i++) {
				auto rhsSpInfo = knSpInfo[i];

				// make sure the spell class is ok
				if (rhsSpInfo.spellClass != spClass)
					continue;

				// if encountered label - go on
				if (spellSys.IsLabel(rhsSpInfo.spEnum)) {
					curSpellLvl = rhsSpInfo.spellLevel;
					continue;
				}

				// else - make sure is visible slot
				if ((int)i < spellsScrollbar2Y)
					continue;
				if ((int)i >= spellsScrollbar2Y + SPELLS_BTN_COUNT)
					break;

				auto chosenWidIdx = (int)i - spellsScrollbar2Y;
				if (!uiManager->DoesWidgetContain(spellsChosenBtnIds[chosenWidIdx], msgW->x, msgW->y))
					continue;

				if (rhsSpInfo.spellLevel == -1 // wildcard slot
					|| rhsSpInfo.spellLevel == spLvl
					&& rhsSpInfo.spFlag != 0
					&& (rhsSpInfo.spFlag != 1 || !selPkt.spellEnumToRemove)
					) {

					if (rhsSpInfo.spFlag == 1) { // replaceable spell
						knSpInfo[i].spFlag = 2;
						selPkt.spellEnumToRemove = rhsSpInfo.spEnum;
					}
					else if (rhsSpInfo.spFlag == 2 && selPkt.spellEnumToRemove == spEnum) { // was already replaced, and now restoring
						knSpInfo[i].spFlag = 1;
						selPkt.spellEnumToRemove = 0;
					}
					knSpInfo[i].spEnum = spEnum;
					return 1;
				}

			}

			return 1;
		case TigMsgWidgetEvent::Entered:
			temple::GetRef<void(int, char*, size_t)>(0x10162AB0)(spEnum, temple::GetRef<char[1024]>(0x10C732B0), 1024u); // UiTooltipSetForSpell
			temple::GetRef<void(char*)>(0x10162C00)(temple::GetRef<char[1024]>(0x10C732B0)); // UiCharTextboxSet
			return 1;
		case TigMsgWidgetEvent::Exited:
			temple::GetRef<void(__cdecl)(char *)>(0x10162C00)(""); // UiCharTextboxSet
			return 1;
		default:
			return 0;
		}
	}

	/*if (msgW->widgetEventType == TigMsgWidgetEvent::Entered){
	std::string text;
	text.append(fmt::format(""));
	auto helpTopicId = ElfHash::Hash(text);
	temple::GetRef<void(__cdecl)(uint32_t)>(0x)(helpTopicId);
	return 1;
	}*/

	if (msgW->widgetEventType == TigMsgWidgetEvent::Exited) {
		temple::GetRef<void(__cdecl)(char *)>(0x10162C00)(""); // UiCharTextboxSet
		return 1;
	}

	return 0;

}

void UiPcCreation::SpellsAvailableEntryBtnRender(int widId)
{
	auto widIdx = WidgetIdIndexOf(widId, &spellsAvailBtnIds[0], SPELLS_BTN_COUNT);
	if (widIdx == -1)
		return;

	auto &avSpInfo = chargen.GetAvailableSpells();

	auto spellIdx = widIdx + spellsScrollbarY;
	if (spellIdx >= (int)avSpInfo.size())
		return;

	auto btn = uiManager->GetButton(widId);
	auto spEnum = avSpInfo[spellIdx].spEnum;

	std::string text;
	TigRect rect(btn->x - spellsWnd.x, btn->y - spellsWnd.y, btn->width, btn->height);
	UiRenderer::PushFont(PredefinedFont::PRIORY_12);
	if (spellSys.IsLabel(spEnum)) {
		rect.x += 2;
		auto spLvl = avSpInfo[spellIdx].spellLevel;
		if (spLvl >= 0 && spLvl < NUM_SPELL_LEVELS)
		{
			text.append(fmt::format("{}", chargen.spellLevelLabels[spLvl]));
			UiRenderer::DrawTextInWidget(spellsWndId, text, rect, spellLevelLabelStyle);
		}

	}
	else
	{
		text.append(fmt::format("{}", spellSys.GetSpellMesline(spEnum)));
		rect.x += 12;
		//rect.width -= 11;
		if (chargen.SpellIsAlreadyKnown(spEnum, avSpInfo[spellIdx].spellClass)
			|| chargen.SpellIsForbidden(spEnum))
			UiRenderer::DrawTextInWidget(spellsWndId, text, rect, spellsAvailBtnStyle);
		else
			UiRenderer::DrawTextInWidget(spellsWndId, text, rect, spellsTextStyle);
	}
	UiRenderer::PopFont();
}

int UiPcCreation::GetClassWndPage(){
	return classWndPage;
}

Stat UiPcCreation::GetClassCodeFromWidgetAndPage(int idx, int page){
	if (page == 0)
		return (Stat)(stat_level_barbarian + idx);

	auto idx2 = idx + page * 11u;
	if (idx2 >= classBtnMapping.size())
		return (Stat)-1;
	return (Stat)classBtnMapping[idx2];
}

int UiPcCreation::GetStatesComplete(){
	return temple::GetRef<int>(0x10BDD5D4);
}

void UiPcCreation::ClassSetPermissibles(){

	auto page = GetClassWndPage();
	auto idx = 0;
	auto handle = GetEditedChar();

	for (auto it : classBtnIds) {
		auto classCode = GetClassCodeFromWidgetAndPage(idx++, page);
		if (classCode == (Stat)-1)
			uiManager->SetButtonState(it, LgcyButtonState::Disabled);

		auto isValid = true;

		if (!d20ClassSys.ReqsMet(handle, classCode)){
			isValid = false;
		}
		if (!IsCastingStatSufficient(classCode) || !IsAlignmentOk(classCode))
			isValid = false;
		
		if (isValid){
			uiManager->SetButtonState(it, LgcyButtonState::Normal);
		}
		else {
			uiManager->SetButtonState(it, LgcyButtonState::Disabled);
		}

	}

	if (!config.newClasses) {
		uiManager->SetButtonState(classNextBtn, LgcyButtonState::Disabled);
		uiManager->SetButtonState(classPrevBtn, LgcyButtonState::Disabled);
		return;
	}

	if (page > 0)
		uiManager->SetButtonState(classPrevBtn, LgcyButtonState::Normal);
	else
		uiManager->SetButtonState(classPrevBtn, LgcyButtonState::Disabled);

	if (page < mPageCount - 1)
		uiManager->SetButtonState(classNextBtn, LgcyButtonState::Normal);
	else
		uiManager->SetButtonState(classNextBtn, LgcyButtonState::Disabled);
}

bool UiPcCreation::IsSelectingNormalFeat(){
	return true;
}

bool UiPcCreation::IsSelectingSecondFeat()
{
	auto &selPkt = GetCharEditorSelPacket();
	return selPkt.raceId == race_human;
}

bool UiPcCreation::IsSelectingBonusFeat()
{
	return mIsSelectingBonusFeat;
}

void UiPcCreation::DeitySetPermissibles(){
	for (auto i = 0; i < DEITY_BTN_COUNT; i++){
		if (deitySys.CanPickDeity(GetEditedChar(), i)){
			uiManager->SetButtonState(GetDeityBtnId(i), LgcyButtonState::Normal);
		} else
			uiManager->SetButtonState(GetDeityBtnId(i), LgcyButtonState::Disabled);	
	}
}

bool UiPcCreation::IsCastingStatSufficient(Stat classEnum){

	auto handle = GetEditedChar();

	if (d20ClassSys.IsCastingClass(classEnum) 
		&& !d20ClassSys.IsLateCastingClass(classEnum)){
		return objects.StatLevelGet(handle, d20ClassSys.GetSpellStat(classEnum)) > 10;
	}
	return true;
}

bool UiPcCreation::IsAlignmentOk(Stat classCode){

	if (!(config.laxRules && config.disableAlignmentRestrictions)) {
		auto hasOkAlignment = false;;
		static std::vector<Alignment> alignments{
			ALIGNMENT_LAWFUL_GOOD, ALIGNMENT_LAWFUL, ALIGNMENT_LAWFUL_EVIL,
			ALIGNMENT_GOOD, ALIGNMENT_NEUTRAL, ALIGNMENT_EVIL,
			ALIGNMENT_CHAOTIC_GOOD, ALIGNMENT_CHAOTIC, ALIGNMENT_CHAOTIC_EVIL };

		auto partyAlignment = GetPartyAlignment();

		// make sure class is compatible with at least one alignment that is compatible with the party alignment
		for (auto al : alignments) {

			if (d20Stats.AlignmentsUnopposed(al, partyAlignment)) {
				if (d20ClassSys.IsCompatibleWithAlignment(classCode, al)) {
					hasOkAlignment = true;
					break;
				}
			}
		}
		if (!hasOkAlignment)
			return false;
	}
	return true;
}

void UiPcCreation::ClassScrollboxTextSet(Stat classEnum){
	temple::GetRef<void(__cdecl)(Stat)>(0x1011B920)(classEnum);
}

void UiPcCreation::ButtonEnteredHandler(int helpId){
	temple::GetRef<void(__cdecl)(int)>(0x1011B890)(helpId);
}

int UiPcCreation::GetNewLvl(Stat classEnum)
{
	return 1;
	/*
	 *
	 
	auto handle = GetEditedChar();
	return objects.StatLevelGet(handle, classEnum) + 1;
	*/
}

std::string UiPcCreation::GetFeatName(feat_enums feat)
{
	if (feat >= FEAT_EXOTIC_WEAPON_PROFICIENCY && feat <= FEAT_GREATER_WEAPON_FOCUS)
		return featsMasterFeatStrings[feat];

	return std::string(feats.GetFeatName(feat));
}

TigTextStyle& UiPcCreation::GetFeatStyle(feat_enums feat, bool allowMultiple)
{
	auto &selPkt = GetCharEditorSelPacket();
	auto newLvl = 1;

	if ((allowMultiple || !uiPcCreation.FeatAlreadyPicked(feat))
		&& uiPcCreation.FeatCanPick(feat))
	{
		if (uiPcCreation.featsMultiSelected == feat) {
			return uiPcCreation.blueTextStyle;
		}

		if (uiPcCreation.IsClassBonusFeat(feat)) {  // is choosing class bonus right now 
			return uiPcCreation.featsGoldenStyle;
		}
		else if (feats.IsClassFeat(feat))// class Specific feat
		{
			return uiPcCreation.featsClassStyle;
		}
		else
			return uiPcCreation.featsNormalTextStyle;
	}

	return uiPcCreation.featsGreyedStyle;
}

bool UiPcCreation::FeatAlreadyPicked(feat_enums feat){
	if (feats.IsFeatPropertySet(feat, 0x1)  // can be gained multiple times
		|| feats.IsFeatMultiSelectMaster(feat))
		return false;
	auto &selPkt = GetCharEditorSelPacket();
	if (selPkt.feat0 == feat || selPkt.feat1 == feat || selPkt.feat2 == feat)
		return true;

	auto handle = GetEditedChar();

	auto isRangerSpecial = IsSelectingRangerSpec();
	return feats.HasFeatCountByClass(handle, feat, selPkt.classCode, isRangerSpecial ? selPkt.feat2 : FEAT_ACROBATIC) != 0;
}

bool UiPcCreation::FeatCanPick(feat_enums feat)
{
	std::vector<feat_enums> featsPicked;
	auto &selPkt = GetCharEditorSelPacket();
	auto handle = GetEditedChar();

	if (selPkt.feat0 != FEAT_NONE) {
		featsPicked.push_back(selPkt.feat0);
	}
	if (selPkt.feat1 != FEAT_NONE) {
		featsPicked.push_back(selPkt.feat1);
	}
	if (selPkt.feat2 != FEAT_NONE) {
		featsPicked.push_back(selPkt.feat2);
	}

	if (IsSelectingBonusFeat() && IsClassBonusFeat(feat)) {
		if (feats.IsFeatPropertySet(feat, FPF_ROGUE_BONUS))
			return true;
		if (uiPcCreation.IsBonusFeatDisregardingPrereqs(feat))
			return true;
	}


	if (!feats.IsFeatMultiSelectMaster(feat)) {
		return feats.FeatPrereqsCheck(handle, feat, featsPicked.size() > 0 ? &featsPicked[0] : nullptr, featsPicked.size(), (Stat)0, selPkt.statBeingRaised) != FALSE;
	}


	// Multiselect Master feats

	auto ftrLvl = objects.StatLevelGet(handle, stat_level_fighter);
	

	bool hasFocus = false;
	switch (feat) {
	case FEAT_EXOTIC_WEAPON_PROFICIENCY:
		return critterSys.GetBaseAttackBonus(handle) >= 1;
	case FEAT_IMPROVED_CRITICAL:
		return critterSys.GetBaseAttackBonus(handle) >= 8;

	case FEAT_MARTIAL_WEAPON_PROFICIENCY:
	case FEAT_SKILL_FOCUS:
		return true;

	case FEAT_WEAPON_FINESSE:
		if (critterSys.GetBaseAttackBonus(handle) < 1)
			return false;
		for (auto i = (int)FEAT_WEAPON_FINESSE_GAUNTLET; i <= FEAT_WEAPON_FINESSE_NET; i++) {
			if (feats.HasFeatCountByClass(handle, (feat_enums)i, (Stat)0, 0))
				return false;
		}
		for (auto it : featsPicked) {
			if (feats.IsFeatPropertySet(it, FPF_WEAP_FINESSE_ITEM))
				return false;
		}
		return true;

	case FEAT_WEAPON_FOCUS:
		return critterSys.GetBaseAttackBonus(handle) >= 1;

	case FEAT_WEAPON_SPECIALIZATION:

		return (ftrLvl >= 4);


	case FEAT_GREATER_WEAPON_FOCUS:
		if (ftrLvl < 8)
			return false;


		// check if has weapon focus

		for (auto i = (int)FEAT_WEAPON_FOCUS_GAUNTLET; i <= FEAT_WEAPON_FOCUS_RAY; i++) {
			if (feats.HasFeatCountByClass(handle, (feat_enums)i, (Stat)0, 0)) {
				return true;
			}
			// if not, check if it's one of the picked ones
			for (auto it : featsPicked) {
				if (it == (feat_enums)i)
					return true;
			}
		}
		return false;

	case FEAT_GREATER_WEAPON_SPECIALIZATION:
		if (ftrLvl < 12)
			return false;

		for (auto i = (int)FEAT_GREATER_WEAPON_FOCUS_GAUNTLET; i <= FEAT_GREATER_WEAPON_FOCUS_RAY; i++) {
			hasFocus = false;
			if (feats.HasFeatCountByClass(handle, (feat_enums)i, (Stat)0, 0)) {
				hasFocus = true;
			}
			// if not, check if it's one of the picked ones
			for (auto it : featsPicked) {
				if (it == (feat_enums)i)
					hasFocus = true;
				break;
			}
			// if has Greater Weapon Focus, check for Weapon Specialization
			if (hasFocus) {

				for (auto j = (int)FEAT_WEAPON_SPECIALIZATION_GAUNTLET; j <= FEAT_WEAPON_SPECIALIZATION_GRAPPLE; j++) {
					if (feats.HasFeatCountByClass(handle, (feat_enums)j, (Stat)0, 0))
						return true;
				}
			}
		}

	default:
		return true;
	}
}

bool UiPcCreation::IsSelectingRangerSpec()
{
	return false;
	/*auto &selPkt = GetCharEditorSelPacket();
	auto handle = GetEditedChar();
	auto isRangerSpecial = selPkt.classCode == stat_level_ranger && (objects.StatLevelGet(handle, stat_level_ranger) + 1) == 2;
	return isRangerSpecial;
	*/
}

bool UiPcCreation::IsClassBonusFeat(feat_enums feat)
{
	return chargen.IsClassBonusFeat(feat);
}

bool UiPcCreation::IsBonusFeatDisregardingPrereqs(feat_enums feat)
{
	return chargen.IsBonusFeatDisregardingPrereqs(feat);
}


void UiPcCreation::FeatsSanitize()
{
	auto &selPkt = GetCharEditorSelPacket();

	for (auto i = 0; i < 3; i++) { // check if any of the feat now lack the prereq (due to user removal). loop three times to ensure up-to-date state.
		if (selPkt.feat0 != FEAT_NONE && !FeatCanPick(selPkt.feat0))
			selPkt.feat0 = FEAT_NONE;
		if (selPkt.feat1 != FEAT_NONE && !FeatCanPick(selPkt.feat1)) {
			selPkt.feat1 = FEAT_NONE;
		}
		if (selPkt.feat2 != FEAT_NONE && !FeatCanPick(selPkt.feat2) && !IsSelectingRangerSpec())
			selPkt.feat2 = FEAT_NONE;
	}
}

void UiPcCreation::FeatsMultiSelectActivate(feat_enums feat)
{
	if (!FeatCanPick(feat))
		return;

	auto &selPkt = GetCharEditorSelPacket();
	if (feat == FEAT_WEAPON_FINESSE) {
		if (selPkt.feat0 == FEAT_WEAPON_FINESSE)
			selPkt.feat0 = FEAT_WEAPON_FINESSE_DAGGER;
		if (selPkt.feat1 == FEAT_WEAPON_FINESSE)
			selPkt.feat1 = FEAT_WEAPON_FINESSE_DAGGER;
		if (selPkt.feat2 == FEAT_WEAPON_FINESSE)
			selPkt.feat2 = FEAT_WEAPON_FINESSE_DAGGER;
		return;
	}

	mFeatsMultiMasterFeat = feat;
	featsMultiSelected = FEAT_NONE;

	// populate list
	mMultiSelectFeats.clear();

	if (feat >NUM_FEATS) {
		std::vector<feat_enums> tmp;
		feats.MultiselectGetChildren(feat, tmp);
		for (auto it : tmp) {
			mMultiSelectFeats.push_back(FeatInfo(it));
		}
	}
	else {
		auto featIt = FEAT_ACROBATIC;
		auto featProp = 0x100;
		switch (feat) {
		case FEAT_EXOTIC_WEAPON_PROFICIENCY:
			featProp = FPF_EXOTIC_WEAP_ITEM;
			break;
		case FEAT_IMPROVED_CRITICAL:
			featProp = FPF_IMPR_CRIT_ITEM;
			break;
		case FEAT_MARTIAL_WEAPON_PROFICIENCY:
			featProp = FPF_MARTIAL_WEAP_ITEM;
			break;
		case FEAT_SKILL_FOCUS:
			featProp = FPF_SKILL_FOCUS_ITEM;
			break;
		case FEAT_WEAPON_FINESSE:
			featProp = FPF_WEAP_FINESSE_ITEM;
			break;
		case FEAT_WEAPON_FOCUS:
			featProp = FPF_WEAP_FOCUS_ITEM;
			break;
		case FEAT_WEAPON_SPECIALIZATION:
			featProp = FPF_WEAP_SPEC_ITEM;
			break;
		case FEAT_GREATER_WEAPON_FOCUS:
			featProp = FPF_GREATER_WEAP_FOCUS_ITEM;
			break;
		case FEAT_GREATER_WEAPON_SPECIALIZATION:
			featProp = FPF_GREAT_WEAP_SPEC_ITEM;
			break;
		default:
			break;
		}

		for (auto ft = 0; ft < NUM_FEATS; ft++) {
			featIt = (feat_enums)ft;
			if (feats.IsFeatPropertySet(featIt, featProp) && feats.IsFeatEnabled(featIt)) {
				mMultiSelectFeats.push_back(FeatInfo(ft));
			}
		}
	}



	featsMultiSelectScrollbar = *uiManager->GetScrollBar(featsMultiSelectScrollbarId);
	featsMultiSelectScrollbar.scrollbarY = 0;
	featsMultiSelectScrollbarY = 0;
	featsMultiSelectScrollbar.yMax = max(0, (int)mMultiSelectFeats.size() - FEATS_MULTI_BTN_COUNT);
	featsMultiSelectScrollbar = *uiManager->GetScrollBar(featsMultiSelectScrollbarId);
	uiManager->SetButtonState(featsMultiOkBtnId, LgcyButtonState::Disabled);

	uiManager->SetHidden(featsMultiSelectWndId, false);
	uiManager->BringToFront(featsMultiSelectWndId);
}

feat_enums UiPcCreation::FeatsMultiGetFirst(feat_enums feat)
{
	return feats.MultiselectGetFirst(feat);
}

int &UiPcCreation::GetDeityBtnId(int deityId){
	return temple::GetRef<int[20]>(0x10C3EE80)[deityId];
}
