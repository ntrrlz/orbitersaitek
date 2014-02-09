// OrbiterSaitek.cpp : Defines the entry point for the console application.
//


#define ORBITER_MODULE 
#include "orbitersdk.h" 
#include "OrbiterAPI.h"

#include "OrbiterSaitek.h"



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
	handle = hid_open(0x06a3, 0x0d05, NULL);
	oapiWriteLog("Device opened");
	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);

}

OrbiterSaitek::~OrbiterSaitek()
{
	// perform module cleanup here 
	hid_close(handle);
	hid_exit();

}


void OrbiterSaitek::clbkPreStep (double SimT, double SimDT, double mjd) // call every timestep
{
	static unsigned char inBuffer[5];


	if (!handle)
	{
		return;
	}

	VESSEL *vessel = oapiGetFocusInterface(); // Get current vessel


	if (vessel!=NULL) // check if pointer is valid
	{

		//0x01 >> 1 i u
		//0x02 >> 1 i d
		//0x04 >> 1 o u
		//0x08 >> 1 o d

		//0x10 >> 2 i u
		//0x20 >> 2 i d
		//0x40 >> 2 o u
		//0x80 >> 2 o d
		int bytesRead = hid_read(handle, inBuffer, sizeof(inBuffer));
		if (bytesRead==3)
		{
			char keyByte = inBuffer[2];
			if (keyByte & 0x01) 
			{
				increaseNav(vessel,0,true,true);
			}
			if (keyByte & 0x02)
			{
				increaseNav(vessel,0,true,false);
			}
			if (keyByte & 0x04) 
			{
				increaseNav(vessel,0,false,true);
			}
			if (keyByte & 0x08)
			{
				increaseNav(vessel,0,false,false);
			}
			if (keyByte & 0x10) 
			{
				increaseNav(vessel,1,true,true);
			}
			if (keyByte & 0x20)
			{
				increaseNav(vessel,1,true,false);
			}
			if (keyByte & 0x40) 
			{
				increaseNav(vessel,1,false,true);
			}
			if (keyByte & 0x80)
			{
				increaseNav(vessel,1,false,false);
			}
		}
	}


}

void OrbiterSaitek::clbkPostStep (double SimT, double SimDT, double mjd) // call every timestep
{
	static unsigned char buf[23];

	if (!handle)
	{
		return;
	}

	VESSEL *vessel = oapiGetFocusInterface(); // Get current vessel


	if (vessel!=NULL) // check if pointer is valid
	{

		buf[0]=0x00;
		// parse nav1
		parseNavData(vessel, 0, &(buf[1]));
		// parse nav2
		parseNavData(vessel, 1, &(buf[11]));

		int res = hid_send_feature_report(handle, buf, 23);	
	}


}

void OrbiterSaitek::parseNavData(VESSEL *vessel, DWORD navNum, unsigned char * buffer)
{
	if (vessel->GetNavCount() > navNum) {
		// vessel has nav
		DWORD navValue = vessel->GetNavChannel(navNum);
		DWORD freq = navValue * 5 + 10800;
		buffer[0]= (unsigned char) ((freq / 10000 ) % 10 );
		buffer[1]= (unsigned char) ((freq /1000 )% 10 );
		buffer[2]= (unsigned char) ((freq /100 ) % 10) + 0xd0 ; // add decimal point
		buffer[3]= (unsigned char) ((freq /10 ) % 10 );
		buffer[4]= (unsigned char) ((freq ) % 10 );

		NAVHANDLE navHandle = vessel->GetNavSource(navNum);
		if (navHandle != NULL) 
		{
			// nav has signal
			VECTOR3 gpos;
			vessel->GetGlobalPos(gpos);

			double signal = oapiGetNavSignal(navHandle, gpos);
			int bars = max(0,min(5,1+(int)(0.5*log(signal))));
			buffer[5] = (bars >= 1) ? 0xe0 : 0x0f;
			buffer[6] = (bars >= 2) ? 0xe0 : 0x0f;
			buffer[7] = (bars >= 3) ? 0xe0 : 0x0f;
			buffer[8] = (bars >= 4) ? 0xe0 : 0x0f;
			buffer[9] = (bars >= 5) ? 0xe0 : 0x0f;

		}
		else 
		{
			// no signal -> blank indicator
			buffer[5] = 0x0f;
			buffer[6] = 0x0f;
			buffer[7] = 0x0f;
			buffer[8] = 0x0f;
			buffer[9] = 0x0f;
		}

	}
	else 
	{
		// no nav radio -> all blank
		buffer[0] = 0x0f;
		buffer[1] = 0x0f;
		buffer[2] = 0x0f;
		buffer[3] = 0x0f;
		buffer[4] = 0x0f;
		buffer[5] = 0x0f;
		buffer[6] = 0x0f;
		buffer[7] = 0x0f;
		buffer[8] = 0x0f;
		buffer[9] = 0x0f;

	}

}

void OrbiterSaitek::increaseNav(VESSEL *vessel, DWORD navNum , bool decimal, bool increase )
{
	DWORD navCount = vessel->GetNavCount();
	if (navCount <= navNum)
		return;

	DWORD nav = vessel->GetNavChannel(navNum);
	DWORD upper = nav / 20;
	DWORD lower = nav % 20;
	if (decimal && increase)
	{
		if (lower==19) 
		{
			lower = 0;
		}
		else 
		{
			lower++;
		}
	}
	if (decimal && !increase)
	{
		if (lower==0) 
		{
			lower = 19;
		}
		else 
		{
			lower--;
		}
	}
	if (!decimal && increase)
	{
		if (upper==31) 
		{
			upper = 0;
		}
		else 
		{
			upper++;
		}
	}
	if (!decimal && !increase)
	{
		if (upper==0) 
		{
			upper = 31;
		}
		else 
		{
			upper--;
		}
	}
	nav=upper * 20 + lower;
	vessel->SetNavChannel(navNum,nav);

}

