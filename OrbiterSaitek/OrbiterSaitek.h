#ifndef ORBITERSAITEK_H
#define ORBITERSAITEK_H

#include <OrbiterAPI.h>
#include <ModuleAPI.h>


#include "hidapi.h"

class OrbiterSaitek : public oapi::Module
{
public:
	OrbiterSaitek(HINSTANCE hDLL);
	~OrbiterSaitek();

	virtual void clbkPreStep (double simt, double simdt, double mjd);
	virtual void clbkPostStep (double simt, double simdt, double mjd);

private:

	void increaseNav(VESSEL *vessel, DWORD navNum , bool decimal, bool increase );
	void parseNavData(VESSEL *vessel, DWORD navNum, unsigned char * buffer);

	hid_device *handle;


};


#endif