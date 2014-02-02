// OrbiterSaitek.cpp : Defines the entry point for the console application.
//


#define ORBITER_MODULE 
#include "orbitersdk.h" 
 

#include "hidapi.h"

HINSTANCE g_hDLL; 
 
hid_device *handle;

DLLCLBK void InitModule (HINSTANCE hModule) 
{ 
    g_hDLL = hModule; 
    // perform global module initialisation here 
	handle = hid_open(0x06a3, 0x0d05, NULL);
} 

DLLCLBK void ExitModule (HINSTANCE hModule) 
{ 
    // perform module cleanup here 
	hid_close(handle);
	hid_exit();
} 


DLLCLBK void opcPreStep (double SimT, double SimDT, double mjd) // call every timestep
{
	unsigned char buf[23];

	if (!handle)
	{
		return;
	}

	VESSEL *vessel = oapiGetFocusInterface(); // Get current vessel
	
	
	if (vessel!=NULL) // check if pointer is valid
	{
		
		DWORD navCount = vessel->GetNavCount();
		DWORD nav1 = 0;
		DWORD nav2 = 0;
		if (navCount >=1) {
			nav1 = vessel->GetNavChannel(0);
			DWORD freq = nav1 * 5 + 10800;
			buf[1]= (unsigned char) ((freq / 10000 ) % 10 );
			buf[2]= (unsigned char) ((freq /1000 )% 10 );
			buf[3]= (unsigned char) ((freq /100 ) % 10) + 0xd0 ; // add decimal point
			buf[4]= (unsigned char) ((freq /10 ) % 10 );
			buf[5]= (unsigned char) ((freq ) % 10 );
		}
		else 
		{
			buf[1] = 0x0f;
			buf[2] = 0x0f;
			buf[3] = 0x0f;
			buf[4] = 0x0f;
			buf[5] = 0x0f;
		}
		if (navCount >=2) {
			nav2 = vessel->GetNavChannel(1);
			DWORD freq = nav2 * 5 + 10800;
			buf[11]= (unsigned char) ((freq / 10000 ) % 10 );
			buf[12]= (unsigned char) ((freq /1000 )% 10 );
			buf[13]= (unsigned char) ((freq /100 ) % 10) + 0xd0 ; // add decimal point
			buf[14]= (unsigned char) ((freq /10 ) % 10 );
			buf[15]= (unsigned char) ((freq ) % 10 );
		}
		else 
		{
			buf[11] = 0x0f;
			buf[12] = 0x0f;
			buf[13] = 0x0f;
			buf[14] = 0x0f;
			buf[15] = 0x0f;
		}

		buf[0] = 0x00;
		buf[6] = 0x0f;
		buf[7] = 0x0f;
		buf[8] = 0x0f;
		buf[9] = 0x0f;
		buf[10] = 0x0f;
		buf[16] = 0x0f;
		buf[17] = 0x0f;
		buf[18] = 0x0f;
		buf[19] = 0x0f;
		buf[20] = 0x0f;
		buf[21] = 0x0f;
		buf[22] = 0x00;
		int res = hid_send_feature_report(handle, buf, 23);	

		
	}
}

