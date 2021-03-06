/*
 *   cxa2126.c - audio/video switch driver (dbox-II-project)
 *
 *   Homepage: http://www.tuxbox.org
 *
 *   Copyright (C) 2000-2002 Gillem gillem@berlios.de
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* ---------------------------------------------------------------------- */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/i2c.h>

#include "dbox/avs_core.h"
#include "cxa2126.h"

/* ---------------------------------------------------------------------- */

/*
 * cxa2126 data struct
 *
 */

typedef struct s_cxa2126_data {
 /* Data 1 */
 unsigned char evc : 3;
 unsigned char evf : 3;
 unsigned char tvmute1 : 1;
 unsigned char zcd : 1;
 /* Data 2 */
 unsigned char res1 : 2;
 unsigned char vsw1 : 3;
 unsigned char asw1 : 3;
 /* Data 3 */
 unsigned char vo5mute : 1;
 unsigned char res2 : 1;
 unsigned char vsw2 : 3;
 unsigned char asw2 : 3;
 /* Data 4 */
 unsigned char res3 : 8;
 /* Data 5 */
 unsigned char res4 : 2;
 unsigned char fblk : 3;
 unsigned char fnc  : 2;
 unsigned char log  : 1;
 /* Data 6 */
 unsigned char res5 : 2;
 unsigned char vo6on : 1;
 unsigned char vo5on : 1;
 unsigned char vo4on : 1;
 unsigned char vo3on : 1;
 unsigned char vo2on : 1;
 unsigned char vo1on : 1;
 /* Data 7 */
 unsigned char tvamute : 1;
 unsigned char tvagain : 1;
 unsigned char res6 : 1;
 unsigned char vcrmono : 1;
 unsigned char tvmono : 1;
 unsigned char res7 : 3;
} s_cxa2126_data;

#define CXA2126_DATA_SIZE sizeof(s_cxa2126_data)

/* ---------------------------------------------------------------------- */

#define dprintk     if (debug) printk

static struct s_cxa2126_data cxa2126_data;	/* current settings */

/* ---------------------------------------------------------------------- */

int cxa2126_set(struct i2c_client *client)
{
/*
int i;
dprintk("CXA2126: write ");
for(i=0; i<CXA2126_DATA_SIZE; i++)
  dprintk("%02x ", (unsigned)((unsigned char*)&cxa2126_data)[i]);
dprintk("\n");
*/
	if ( CXA2126_DATA_SIZE != i2c_master_send(client, (char*)&cxa2126_data, CXA2126_DATA_SIZE))
	{
		return -EFAULT;
    }

	return 0;
}

/* ---------------------------------------------------------------------- */

int cxa2126_set_volume( struct i2c_client *client, int vol )
{
	if ((vol > 63) || (vol < 0))
		return -EINVAL;
	
	cxa2126_data.evc = (vol >> 3) & 7;
	cxa2126_data.evf = vol & 7;

	return cxa2126_set(client);
}

inline int cxa2126_set_mute( struct i2c_client *client, int type )
{
	if ((type<0) || (type>1))
	{
		return -EINVAL;
	}

	// (Un-)mute immediately, 1 -> Mute
	cxa2126_data.tvmute1 = cxa2126_data.tvamute = type&1;

	return cxa2126_set(client);
}

inline int cxa2126_set_zcd( struct i2c_client *client, int type )
{
	if ((type<0) || (type>1))
	{
		return -EINVAL;
	}

	cxa2126_data.zcd = type&1;

	return cxa2126_set(client);
}

inline int cxa2126_set_fblk( struct i2c_client *client, int type )
{
	if (type<0 || type>3)
	{
		return -EINVAL;
	}

	cxa2126_data.fblk = type;

	return cxa2126_set(client);
}

inline int cxa2126_set_fnc( struct i2c_client *client, int type )
{
	if (type<0 || type>3)
	{
		return -EINVAL;
	}

	cxa2126_data.fnc = type;

	return cxa2126_set(client);
}

inline int cxa2126_set_vsw( struct i2c_client *client, int sw, int type )
{
	if (type<0 || type>7)
	{
		return -EINVAL;
	}

	switch(sw)
	{
		case 0:
			cxa2126_data.vsw1 = type;
			break;
		case 1:
			cxa2126_data.vsw2 = type;
			break;
//		case 2:
//			cxa2126_data.vsw3 = type;
//			break;
		default:
			return -EINVAL;
	}

	return cxa2126_set(client);
}

inline int cxa2126_set_asw( struct i2c_client *client, int sw, int type )
{
	if (type<0 || type>7)
	{
		return -EINVAL;
	}

	switch(sw)
	{
		case 0:
			cxa2126_data.asw1 = type;
			break;
		case 1:
			cxa2126_data.asw2 = type;
			break;
//		case 2:
//			cxa2126_data.asw3 = type;
//			break;
		default:
			return -EINVAL;
	}

	return cxa2126_set(client);
}

int cxa2126_get_volume(void)
{
	return ((cxa2126_data.evc*8)+cxa2126_data.evf);
}

inline int cxa2126_get_mute(void)
{
	return cxa2126_data.tvmute1;
}

inline int cxa2126_get_zcd(void)
{
	return cxa2126_data.zcd;
}

inline int cxa2126_get_fblk(void)
{
	return cxa2126_data.fblk;
}

inline int cxa2126_get_fnc(void)
{
	return cxa2126_data.fnc;
}

inline int cxa2126_get_vsw( int sw )
{
	switch(sw)
	{
		case 0:
			return cxa2126_data.vsw1;
			break;
		case 1:
			return cxa2126_data.vsw2;
			break;
//		case 2:
//			return cxa2126_data.vsw3;
//			break;
		default:
			return -EINVAL;
	}

	return -EINVAL;
}

inline int cxa2126_get_asw( int sw )
{
	switch(sw)
	{
		case 0:
			return cxa2126_data.asw1;
			break;
		case 1:
			return cxa2126_data.asw2;
			break;
//		case 2:
//			return cxa2126_data.asw3;
//			break;
		default:
			return -EINVAL;
	}

	return -EINVAL;
}

/* ---------------------------------------------------------------------- */

inline int cxa2126_set_logic( struct i2c_client *client, int type )
{
	if(type<0 || type>1)
		return -EINVAL;

	cxa2126_data.log = type;

	return cxa2126_set(client);
}

inline int cxa2126_get_logic(void)
{
	return cxa2126_data.log;
}

/* ---------------------------------------------------------------------- */

int cxa2126_get_status(struct i2c_client *client)
{
	unsigned char byte;

	if (1 != i2c_master_recv(client,&byte,1))
	{
		return -1;
	}

	return byte;
}

int cxa2126_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	int val;
	dprintk("[AVS]: IOCTL\n");
	
	if (cmd&AVSIOSET)
	{
		if ( get_user(val,(int*)arg) )
			return -EFAULT;

		switch (cmd)
		{
			/* set video */
			case AVSIOSVSW1:	return cxa2126_set_vsw(client,0,val);
			case AVSIOSVSW2:	return cxa2126_set_vsw(client,1,val);
//			case AVSIOSVSW3:	return cxa2126_set_vsw(2,val);
			/* set audio */
			case AVSIOSASW1:	return cxa2126_set_asw(client,0,val);
			case AVSIOSASW2:	return cxa2126_set_asw(client,1,val);
//			case AVSIOSASW3:	return cxa2126_set_asw(2,val);
			/* set vol & mute */
			case AVSIOSVOL:		return cxa2126_set_volume(client,val);
			case AVSIOSMUTE:	return cxa2126_set_mute(client,val);
			/* set video fast blanking */
			case AVSIOSFBLK:	return cxa2126_set_fblk(client,val);
			/* set video function switch control */
			case AVSIOSFNC:
			case AVSIOSSCARTPIN8:	return cxa2126_set_fnc(client,val);
			/* set output throgh vout 8 */
//			case AVSIOSYCM:		return cxa2126_set_ycm(val);
			/* set zero cross detector */
			case AVSIOSZCD:		return cxa2126_set_zcd(client,val);
			/* set logic outputs */
			case AVSIOSLOG1:	return cxa2126_set_logic(client,val);

			default:                return -EINVAL;
		}
	} else
	{
		switch (cmd)
		{
			/* get video */
			case AVSIOGVSW1:
                                val = cxa2126_get_vsw(0);
                                break;
			case AVSIOGVSW2:
                                val = cxa2126_get_vsw(1);
                                break;
			case AVSIOGVSW3:
                                val = cxa2126_get_vsw(2);
                                break;
			/* get audio */
			case AVSIOGASW1:
                                val = cxa2126_get_asw(0);
                                break;
			case AVSIOGASW2:
                                val = cxa2126_get_asw(1);
                                break;
			case AVSIOGASW3:
                                val = cxa2126_get_asw(2);
                                break;
			/* get vol & mute */
			case AVSIOGVOL:
                                val = cxa2126_get_volume();
                                break;
			case AVSIOGMUTE:
                                val = cxa2126_get_mute();
                                break;
			/* get video fast blanking */
			case AVSIOGFBLK:
                                val = cxa2126_get_fblk();
                                break;
			/* get video function switch control */
			case AVSIOGFNC:
			case AVSIOGSCARTPIN8:
                                val = cxa2126_get_fnc();
                                break;
			/* get output throgh vout 8 */
//			case AVSIOGYCM:
//                                val = cxa2126_get_ycm();
//                                break;
			/* get zero cross detector */
			case AVSIOGZCD:
                                val = cxa2126_get_zcd();
                                break;
			/* get logic outputs */
			case AVSIOGLOG1:
                                val = cxa2126_get_logic();
                                break;
			case AVSIOGSTATUS:
                                // TODO: error handling
                                val = cxa2126_get_status(client);
                                break;
			default:
                                return -EINVAL;
		}

        	return put_user(val,(int*)arg);
	}

	return 0;
}

/* ---------------------------------------------------------------------- */

int cxa2126_init(struct i2c_client *client)
{
    memset((void*)&cxa2126_data,0,CXA2126_DATA_SIZE);

    /* default values */
    cxa2126_data.vo6on = 1;
    cxa2126_data.vo5on = 1;
    cxa2126_data.vo4on = 1;
    cxa2126_data.vo3on = 1;
    cxa2126_data.vo2on = 1;
    cxa2126_data.vo1on = 1;

    cxa2126_data.asw1 = 0;
    cxa2126_data.vsw1 = 0;
    cxa2126_data.fnc  = 2;

    return cxa2126_set(client);
}

/* ---------------------------------------------------------------------- */
