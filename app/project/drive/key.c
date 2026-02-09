#include "key.h"

KEY_EVENT key_event_check(KEY *key, int interval)
{
    static KEY_EVENT key_event = KE_NONE;
    switch (key->status)
    {
    case KS_RELEASE:
    {
        key->count = 0;

        if (key->get() == K_PRESS)
        {
            key->status = KS_CHECK;
        }
        else
        {
            key_event = KE_NONE;
            key->status = KS_RELEASE;
        }
    }
    break;

    case KS_CHECK:
    {        
		
		if (key->get() == K_PRESS)
        {
            key->count += 1;
			
        }
		 if(key->get() == K_RELEASE)
		{
		    key->status = KS_PRESS;
		}
		key->key_cycle_time --;
         if (!key->key_cycle_time)
        {
            key->key_cycle_time = KEY_CYCLE_TIME;

            if (key->count >= interval)
            {
                key->status = KS_LONG_PRESS;
				
                break;
            }
            else
            {
                key->status = KS_PRESS;
                
                break;
            }
        }

    }
    break;

    case KS_LONG_PRESS:
    {
        if (key->get() == K_PRESS)
        {
            key->status = KS_LONG_PRESS;
            key_event = KE_LONG_PRESS;
        }
        else
        {
            key->status = KS_RELEASE;
        }
    }
    break;

    case KS_PRESS:
    {
		key->key_cycle_time = KEY_CYCLE_TIME;
        key->status = KS_RELEASE;
		key_event = KE_PRESS;
        
    }
    break;
    }

    return key_event;
}
