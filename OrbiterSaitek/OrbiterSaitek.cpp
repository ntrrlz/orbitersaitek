// OrbiterSaitek.cpp : Defines the entry point for the console application.
//


#define ORBITER_MODULE 
#include "orbitersdk.h" 
#include "OrbiterAPI.h"

#include "OrbiterSaitek.h"

#include "RadioPanel.h"
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

}

OrbiterSaitek::~OrbiterSaitek()
{
	delete radioPanel;

	hid_exit();
}


void OrbiterSaitek::clbkPreStep (double SimT, double SimDT, double mjd) // call every timestep
{
	radioPanel->checkInputs();
}

void OrbiterSaitek::clbkPostStep (double SimT, double SimDT, double mjd) // call every timestep
{
	radioPanel->updateDisplay();
}



