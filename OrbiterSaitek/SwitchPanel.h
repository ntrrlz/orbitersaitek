#pragma once

#include "OrbiterAPI.h"
#include "VesselAPI.h"
#include "hidapi.h"

class SwitchPanel
{
public:
	SwitchPanel(void);
	~SwitchPanel(void);

	void checkInputs();
	void updateDisplay();
	void clearDisplay();

private:
	hid_device *handle;

};

