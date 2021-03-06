

#include "stdafx.h"
#include <iostream>

#include "obj_structs.h"
#include "description.h"
#include "gamesystems/objects/objsystem.h"

const objHndl objHndl::null{ 0 };

std::ostream &operator <<(std::ostream &out, const objHndl &handle) {
	if (!handle || !objSystem->IsValidHandle(handle)) {
		out << "OBJ_HANDLE_NULL";
	} else {
		auto d = description.getDisplayName(handle);
		if (d != nullptr){
			out << d
				<< " [0x" << std::hex << handle.handle << std::dec << "]";
		}
		else{
			out << "DESCRIPTION MISSING"
				<< " [0x" << std::hex << handle.handle << std::dec << "]";
		}
		
	}
	return out;
}
