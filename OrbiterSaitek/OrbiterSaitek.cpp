// OrbiterSaitek.cpp : Defines the entry point for the console application.
//


#define ORBITER_MODULE 
#include "orbitersdk.h" 
#include "OrbiterAPI.h"

#include "OrbiterSaitek.h"

#include "RadioPanel.h"
#include "SwitchPanel.h"
#include "hidapi.h"

HINSTANCE g_hDLL; 

DLLCLBK void InitModule (HINSTANCE hModule) 
{ 
	g_hDLL = hModule; 
	// perform global module initialisation here 
	OrbiterSaitek * mod = new OrbiterSaitek(hModule);
	oapiRegisterModule(mod);
} 

DLLCLBK void ExitModule (HINSTANCE hModule) 
{ 
} 


OrbiterSaitek::OrbiterSaitek(HINSTANCE hDLL)
	: oapi::Module(hDLL)
{
	radioPanel = new RadioPanel();
	switchPanel = new SwitchPanel();
}

OrbiterSaitek::~OrbiterSaitek()
{
	delete radioPanel;
	delete switchPanel;
	hid_exit();
}


void OrbiterSaitek::clbkPreStep (double SimT, double SimDT, double mjd) // call every timestep
{
	radioPanel->checkInputs();
	switchPanel->checkInputs();
}

void OrbiterSaitek::clbkPostStep (double SimT, double SimDT, double mjd) // call every timestep
{
	radioPanel->updateDisplay();
	switchPanel->updateDisplay();
}

void OrbiterSaitek::clbkSimulationEnd()
{
	radioPanel->clearDisplay();
	switchPanel->clearDisplay();

}


