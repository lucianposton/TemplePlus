#include "stdafx.h"
#include "ui_logbook.h"
#include <temple/dll.h>
#include <obj.h>
#include <gamesystems/objects/objsystem.h>
#include <gamesystems/objects/gameobject.h>
#include <party.h>


UiLogbook uiLogbook;

void UiLogbook::IncreaseAmount(PartyLogbookPacket & pkt, objHndl handle, int amount){

	if (pkt.HasObj(handle) || pkt.AddObj(handle)){
		for (auto i=0; i < LOGBOOK_MAX_PARTY_MEMBER_COUNT; i++){
			if (objSystem->GetHandleById(pkt.sub[i].id) == handle){
				pkt.sub[i].amount += amount;
				break;
			}
		}
	}
}

void UiLogbook::IncreaseCritHits(objHndl handle){
	if (!handle)
		return;

	if (objects.GetType(handle) == obj_t_pc && party.IsInParty(handle)){
		IncreaseAmount( temple::GetRef<PartyLogbookPacket>(0x10D24878), handle, 1);
	}
}

BOOL PartyLogbookPacket::HasObj(objHndl handle){
	for (auto i=0; i < LOGBOOK_MAX_PARTY_MEMBER_COUNT; i++){
		ObjectId id = this->sub[i].id;
	
		if (objSystem->GetHandleById(id) == handle)
			return TRUE;
	}

	return FALSE;
}

BOOL PartyLogbookPacket::AddObj(objHndl handle){
	return temple::GetRef<BOOL(__cdecl)(PartyLogbookPacket *, objHndl)>(0x10198C40)(this, handle);
}
