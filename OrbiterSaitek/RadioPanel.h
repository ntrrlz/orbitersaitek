#pragma once

#include "OrbiterAPI.h"
#include "VesselAPI.h"
#include "hidapi.h"

class RadioPanel
{
public:
	RadioPanel();
	~RadioPanel();

	void checkInputs();
	void updateDisplay();
	void clearDisplay();

private:
	void increaseNav(VESSEL *vessel, DWORD navNum , bool decimal, bool increase );
	void parseNavData(VESSEL *vessel, DWORD navNum, unsigned char * buffer);


	
	hid_device *handle;

};

