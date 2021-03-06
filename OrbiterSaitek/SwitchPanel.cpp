#include "SwitchPanel.h"

#include "XRVesselCtrl.h"
#include "../samples/DeltaGlider/DeltaGlider.h"

SwitchPanel::SwitchPanel()
{
	handle = hid_open(0x06a3, 0x0d67, NULL);
	oapiWriteLog("Device opened");
	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);
}


SwitchPanel::~SwitchPanel(void)
{
	// perform module cleanup here 
	hid_close(handle);
}

void SwitchPanel::checkInputs()
{
	static unsigned char inBuffer[5];


	if (!handle)
	{
		return;
	}

	VESSEL *vessel = oapiGetFocusInterface(); // Get current vessel


	int bytesRead = hid_read(handle, inBuffer, sizeof(inBuffer));
	if (vessel!=NULL) // check if pointer is valid
	{

		//0x04 >> gear up
		//0x08 >> gear down

		if (bytesRead==3)
		{
			char keyByte = inBuffer[2];
			if (keyByte & 0x04) 
			{
				// gear up
				setGear(vessel, true);
			}

			if (keyByte & 0x08) 
			{
				// gear down
				setGear(vessel, false);
			}


		}
	}


}

void SwitchPanel::setGear(VESSEL * vessel, bool up)
{
	if (XRVesselCtrl::IsXRVesselCtrl(vessel))
	{
		// is XR vessel
		XRVesselCtrl * xrVessel = static_cast<XRVesselCtrl *>(vessel);
		if (xrVessel->GetCtrlAPIVersion() >= THIS_XRVESSELCTRL_API_VERSION)
		{
			XRDoorState doorState = xrVessel->GetDoorState(XRD_Gear,NULL);
			if (up && (doorState== XRDS_Open || doorState== XRDS_Opening))
			{
				xrVessel->SetDoorState(XRD_Gear,XRDS_Closing);
			}
			else if (!up && (doorState== XRDS_Closed || doorState== XRDS_Closing))
			{
				xrVessel->SetDoorState(XRD_Gear,XRDS_Opening);
			}
		}
	}
}

void SwitchPanel::updateDisplay()
{
	// 0x00 off
	// 0x07 green
	// 0x38 red

	static unsigned char buf[2];

	if (!handle)
	{
		return;
	}

	buf[0]=0x00;
	buf[1]=0x00;

	VESSEL *vessel = oapiGetFocusInterface(); // Get current vessel


	if (vessel!=NULL) // check if pointer is valid
	{

		if (XRVesselCtrl::IsXRVesselCtrl(vessel))
		{
			// is XR vessel
			XRVesselCtrl * xrVessel = static_cast<XRVesselCtrl *>(vessel);
			if (xrVessel->GetCtrlAPIVersion() >= THIS_XRVESSELCTRL_API_VERSION)
			{
				XRDoorState doorState = xrVessel->GetDoorState(XRD_Gear,NULL);
				switch (doorState)
				{
				case XRDS_Open:
					// green
					buf[1]=0x07;
					break;
				case XRDS_Opening:
				case XRDS_Closing:
					// red
					buf[1]=0x38;
					break;
				default:
					// off
					buf[1]=0x00;
					break;
				}
			}
		}
		else if(strcmp(vessel->GetClassNameA(),"DeltaGlider")==0 || strcmp(vessel->GetClassNameA(),"DG-S")==0)
		{
			DeltaGlider * dgVessel = static_cast<DeltaGlider *>(vessel);
			switch (dgVessel->gear_status)
			{
			case DeltaGlider::DOOR_OPEN:
				// green
				buf[1]=0x07;
				break;
			case DeltaGlider::DOOR_OPENING:
			case DeltaGlider::DOOR_CLOSING:
				// red
				buf[1]=0x38;
				break;
			default:
				// off
				buf[1]=0x00;
				break;
			}
		}
	}

	int res = hid_send_feature_report(handle, buf, 2);	

}

void SwitchPanel::clearDisplay()
{
	static unsigned char buf[2];

	if (!handle)
	{
		return;
	}

	buf[0]=0x00;
	buf[1]=0x00;


	int res = hid_send_feature_report(handle, buf, 2);	

}
