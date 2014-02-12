#ifndef ORBITERSAITEK_H
#define ORBITERSAITEK_H

#include <OrbiterAPI.h>
#include <ModuleAPI.h>


class RadioPanel;
class SwitchPanel;

class OrbiterSaitek : public oapi::Module
{
public:
	OrbiterSaitek(HINSTANCE hDLL);
	~OrbiterSaitek();

	virtual void clbkPreStep (double simt, double simdt, double mjd);
	virtual void clbkPostStep (double simt, double simdt, double mjd);
	virtual void clbkSimulationEnd ();
private:

	RadioPanel * radioPanel;
	SwitchPanel * switchPanel;
};


#endif