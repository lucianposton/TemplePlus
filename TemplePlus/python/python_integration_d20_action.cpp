#include "stdafx.h"
#include "python_integration_d20_action.h"
#include <gamesystems/gamesystems.h>
#include <gamesystems/objects/objsystem.h>
#include "python_object.h"

#undef HAVE_ROUND
#define PYBIND11_EXPORT
#include <pybind11/pybind11.h>
#include <pybind11/common.h>
#include <pybind11/cast.h>

#include "d20.h"
#include "action_sequence.h"

namespace py = pybind11;
using namespace pybind11;
using namespace pybind11::detail;


PythonD20ActionIntegration pythonD20ActionIntegration;


PYBIND11_PLUGIN(tp_actions) {
	py::module m("tpactions", "Temple+ D20 Actions module, used for handling of D20 actions & action sequencing.");

	//py:class_<ActnSeq>(m, "");
	py::class_<ActnSeq>(m,"ActionSequence")
		.def_readwrite("cur_idx", &ActnSeq::d20aCurIdx)
		.def_readwrite("performer", &ActnSeq::performer)
		.def_readwrite("tb_status", &ActnSeq::tbStatus)
		.def_readwrite("target", &ActnSeq::targetObj)
		.def("add_action", [](ActnSeq & actSeq, D20Actn & d20a){
			actSeq.d20ActArray[actSeq.d20ActArrayNum++] = d20a;
		})
		;

	m.def("add_to_seq", [](D20Actn & d20a, ActnSeq & actSeq){
		actSeq.d20ActArray[actSeq.d20ActArrayNum++] = d20a;
	});

	m.def("get_new_spell_id", []()->int{
		return spellSys.GetNewSpellId();
	});

	return m.ptr();
}

PythonD20ActionIntegration::PythonD20ActionIntegration()
	:PythonIntegration("rules\\d20_actions\\action*.py", "(action(\\d{3,}).*)\\.py"){
}

void PythonD20ActionIntegration::GetActionEnums(std::vector<int>& actionEnums){

	for (auto it : mScripts){
		actionEnums.push_back(it.first);
	}
}

std::string PythonD20ActionIntegration::GetActionName(int actionEnum){
	auto actionSpecEntry = mScripts.find(actionEnum);
	if (actionSpecEntry == mScripts.end())
		return fmt::format(""); 

	return RunScriptStringResult(actionSpecEntry->second.id, (EventId)D20ActionSpecFunc::GetActionName, nullptr);
}



int PythonD20ActionIntegration::GetInt(int actionEnum, D20ActionSpecFunc specType, int defaultVal){
	auto actionSpecEntry = mScripts.find(actionEnum);
	if (actionSpecEntry == mScripts.end())
		return defaultVal; 

	return RunScriptDefault0(actionSpecEntry->second.id, (EventId)specType, nullptr);
}

int PythonD20ActionIntegration::GetActionDefinitionFlags(int actionEnum){
	return GetInt(actionEnum, D20ActionSpecFunc::GetActionDefinitionFlags);
}

int PythonD20ActionIntegration::GetTargetingClassification(int actionEnum){
	return GetInt(actionEnum, D20ActionSpecFunc::GetTargetingClassification);
}

ActionCostType PythonD20ActionIntegration::GetActionCostType(int actionEnum){
	return (ActionCostType)GetInt(actionEnum, D20ActionSpecFunc::GetActionCostType);
}

ActionErrorCode PythonD20ActionIntegration::PyAddToSeq(int actionEnum, D20Actn * d20a, ActnSeq * actSeq, TurnBasedStatus * tbStat){

	auto actionSpecEntry = mScripts.find(actionEnum);
	if (actionSpecEntry == mScripts.end())
		return AEC_INVALID_ACTION;

	py::object pbD20A = py::cast(d20a);
	py::object pbActSeq = py::cast(actSeq);
	py::object pbTbStat = py::cast(tbStat);

	
	auto dispPyArgs = Py_BuildValue("(OOO)", pbD20A.ptr(), pbActSeq.ptr(), pbTbStat.ptr());

	
	auto result = (ActionErrorCode)RunScriptDefault0(actionSpecEntry->second.id, (EventId)D20ActionSpecFunc::AddToSequence, dispPyArgs);

	Py_DECREF(dispPyArgs);

	return result;

}


static std::map<D20ActionSpecFunc, std::string> D20ActionSpecFunctions = {

	{ D20ActionSpecFunc::GetActionDefinitionFlags,"GetActionDefinitionFlags"},
	{ D20ActionSpecFunc::GetActionName,"GetActionName" },
	{ D20ActionSpecFunc::GetTargetingClassification,"GetTargetingClassification" },
	{ D20ActionSpecFunc::GetActionCostType,"GetActionCostType" },
	{ D20ActionSpecFunc::AddToSequence,"AddToSequence" },
	
	
};

const char* PythonD20ActionIntegration::GetFunctionName(EventId evt) {
	auto _evt = (D20ActionSpecFunc)evt;
	return D20ActionSpecFunctions[_evt].c_str();
}