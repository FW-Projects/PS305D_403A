#include "beep_handle.h"
#include "PS305D_handle.h"

void beep_handle(void)
{
	static uint8_t ocp_time = 0;
	static uint8_t ocp_number = 0;
	static uint8_t beep_warning_time = 10;;
	if(ps305d.speak_gate == SPEAK_OPEN)
		BeepProc(&sbeep);
	else if(ps305d.speak_gate == SPEAK_CLOSE)
		sbeep.off();
	
	if(ps305d.General_parameters.ocp_triggered_flag == true)
	{
		ocp_time++;
		if(ocp_time > 15)
		{
			ocp_time = 0;
			if(ocp_number < 6)
			{
				ocp_number++;
				sbeep.cmd = BEEP_LONG;
			}
			else
			{
				sbeep.cmd = BEEP_STOP;
			}
				
		}	
	}
	else
	{
		ocp_time = 0;
		ocp_number = 0;
	}
	
	if(ps305d.error_state != VOLTAGE_NORMAL)
	{
		beep_warning_time--;

		if (!beep_warning_time)
		{
			beep_warning_time = 40;
			sbeep.cmd = BEEP_LONG;
		}
	}
}


