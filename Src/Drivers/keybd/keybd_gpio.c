
#include <windows.h>
#include <nkintr.h>
#include <windev.h>
#include <winbase.h>
#include <winuser.h>
#include <pm.h>
#include "oal_intr.h"
#include "Pkfuncs.h"
#include <s3c6410.h>
#include <DrvLib.h>
#include <Sipapi.h>
//#include <aygshell.h>
volatile S3C6410_GPIO_REG *g_pGPIOReg = NULL;
UINT32 g_KeyIrq[10] = {IRQ_EINT0,IRQ_EINT1,IRQ_EINT2,IRQ_EINT5,IRQ_EINT6,IRQ_EINT9,IRQ_EINT10/*,IRQ_EINT11*/,IRQ_EINT16,IRQ_EINT17,IRQ_EINT21};	// Determined by SMDK2440 board layout.
UINT32 g_KeySysIntr[10] = {SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED,SYSINTR_UNDEFINED};


#define USEDKEY 11
const DWORD Key_EINT_Bit[USEDKEY] = {0,1,2,5,9,10,16,17,6,11,21};
BOOL LongPress=FALSE;
HANDLE KeyThread;
HANDLE KeyEvent;
#define KEYBTN_ERR(x)	RETAILMSG(TRUE, x)
void Virtual_Alloc();						// Virtual allocation
BOOL Fn_Key_Pressed();
DWORD UserKeyProcessThread(void);
BOOL InitInterruptThread();
void Change_Backlight(BOOL Level);
void Change_Volume(BOOL Level);
#define FILE_DEVICE_BAK 0x2330
#define IOCTL_BACKLIGHT_SET			CTL_CODE( FILE_DEVICE_BAK, 1, METHOD_NEITHER,FILE_ANY_ACCESS)
#define IOCTL_VOLUME_SET			CTL_CODE( FILE_DEVICE_BAK, 2, METHOD_NEITHER,FILE_ANY_ACCESS)
static HANDLE                        hBak;   // BackLight driver
static HANDLE                        hAudio;   // Audio driver
BOOL fnKey=FALSE;
unsigned char key_table[9]=
{
			0xc1/*Fn Key*/,0xc2/*G Key*/,0xc3/*KEYPAD KEY*/,VK_UP,0xc4/*CAMERA KEY*/,VK_DOWN,VK_LEFT,VK_RETURN,VK_RIGHT
};
#ifndef IMGULDR

CLSID g_SIpClsid[4];
int g_Index=0;

int SipEnumIMProc(IMENUMINFO* pIMInfo)
{
    if((g_Index<4)&&(g_Index>=0))
    {
        g_SIpClsid[g_Index]=pIMInfo->clsid;
        g_Index++;
    }
	RETAILMSG(1,(TEXT("Sip number is :%d \r\n"),g_Index));
    return g_Index;
}

//´úÂë 
void SwitchSIP()
{
    CLSID cls;
	int i;
    SipGetCurrentIM(&cls);
    for(i=0;i<g_Index;i++)
    {
        //if(g_SIpClsid[i]==cls)
		if(memcmp(&(g_SIpClsid[i]),&cls,sizeof(CLSID))==0)
        {
            if(i<g_Index-1)
            {
                SipSetCurrentIM(&g_SIpClsid[i+1]);
            }
            else
            {
                SipSetCurrentIM(&g_SIpClsid[0]);
            }
        }
    }
}

static void ChangeTheIME(void)
{
	DWORD Timecount=0;
	BOOL SipshowFlag=FALSE;
	SIPINFO si; 
	static BOOL flag=FALSE;
	/*memset( &si, 0, sizeof( si ) ); 
	si.dwImDataSize=0;
	si.cbSize = sizeof( si ); 
	if(!SipGetInfo(&si)) 
	{ 
		RETAILMSG(1,(TEXT("GetInfo failed \r\n")));
		return; 
	} */
	/*g_pGPIOReg->GPNCON &=~(0x3<<4);
	while((g_pGPIOReg->GPNDAT &(1<<2))==0)
	{
		Timecount++;
		Sleep(50);
		if(Timecount>10)
		{
			SipshowFlag=TRUE;
			break;
		}
	}
	Timecount=0;*/
	if(LongPress==TRUE)
		SipshowFlag=TRUE;
	if(SipshowFlag)
	{
	
		SipshowFlag = FALSE;
		//if((g_pGPIOReg->GPNDAT &(1<<2))==0)
		{
			if(flag==TRUE)  
				{
					SipShowIM(SIPF_OFF);
					flag=FALSE;
					RETAILMSG(1,(TEXT("Hide \r\n")));
				}
			else
				{
					SipShowIM(SIPF_ON);
					flag=TRUE;
					RETAILMSG(1,(TEXT("Display \r\n")));
				}
			//g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<4)) | (0x2<<4);

			return;
		}
		
	}
	else
	{
		flag=TRUE;
		RETAILMSG(1,(TEXT("Switch\r\n")));
		SwitchSIP();
	}
	//g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<4)) | (0x2<<4);
	
}
#else
static void ChangeTheIME(void)
{
	return ;
}
void keybd_event(int i,int j,int k,int l)
{
	return;
}
#endif

void KeyIOInit( void )
{
	DWORD i;

	
	
	//Mask Key EINT 
	
	for(i=0;i<USEDKEY;i++)
		g_pGPIOReg->EINT0MASK  |= (1<<Key_EINT_Bit[i]);
		
	//filter config	
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0xFF<<0)) | (0xFF<<0);  //EINT0 1
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0xFF<<8)) | (0xFF<<8);  //EINT2
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0xFF<<16)) | (0xFF<<16);  //EINT5
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0xFF<<24)) | (0xFF<<24);  //EINT6
	g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0xFF<<0)) | (0xFF<<0);  //EINT9	
	//g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0xFF<<8)) | (0xFF<<8);  //EINT10 11	
	g_pGPIOReg->EINT0FLTCON2 = (g_pGPIOReg->EINT0FLTCON2 & ~(0xFF<<0)) | (0xFF<<0);  //EINT16 17	
	//g_pGPIOReg->EINT0FLTCON2 = (g_pGPIOReg->EINT0FLTCON2 & ~(0xF<<16)) | (0xF<<16);  //EINT20	
	
	//Config Key int pin
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<0))|(2<<0);  // EINT0
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<2))|(2<<2);  //EINT1
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<4))|(2<<4);  //EINT2
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<10))|(2<<10); //EINT5
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<12))|(2<<12); //EINT6
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<18))|(2<<18); //EINT9
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<20))|(2<<20); //EINT10
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<22))|(2<<22); //EINT11
	g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1& ~(0xF<<0))|(3<<0);	//EINT16
	g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1& ~(0xF<<4))|(3<<4);	//EINT17
  //      g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1& ~(0xF<<16))|(3<<16);	//EINT20

 	g_pGPIOReg->GPLCON1 = g_pGPIOReg->GPLCON1&(~(0xf<<20))|(3<<20);   //¶ú»ú¼ì²âÖÐ¶Ï½Å
	//g_pGPIOReg->GPLPUD  = g_pGPIOReg->GPLPUD &(~(0x3<<26))|(2<<26);

	//config pull up
	
	//config falling triggle
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0& ~(0xF<<0))|(2<<0);// EINT0 1
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0& ~(0xF<<4))|(2<<4);// EINT2
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0& ~(0xF<<8))|(2<<8);// EINT5
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0& ~(0xF<<12))|(2<<12);// EINT6
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0& ~(0xF<<16))|(2<<16);//	EINT 9
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0& ~(0xF<<20))|(2<<20);// EINT10 11
	g_pGPIOReg->EINT0CON1 = (g_pGPIOReg->EINT0CON1& ~(0xF<<0))|(2<<0);//EINT 16 17
	//g_pGPIOReg->EINT0CON1 = (g_pGPIOReg->EINT0CON1& ~(0xF<<8))|(2<<0);//EINT 20	
	g_pGPIOReg->EINT0CON1=  (g_pGPIOReg->EINT0CON1& (~(0xF<<8)))|(0x6<<8);//EINT 21  BOTH EDGE
		
//Clear Pend
	for(i=0;i<USEDKEY;i++)
		g_pGPIOReg->EINT0PEND = (1<<Key_EINT_Bit[i]);

//Enable Key EINT
	for(i=0;i<USEDKEY;i++)
		g_pGPIOReg->EINT0MASK  &= ~(1<<Key_EINT_Bit[i]);
			
	
	

}


DWORD GetKeyValue( DWORD *whichkey ) 
{
	DWORD i;
	DWORD value;	
	DWORD keycount=0;
	
	value=0;
	
	//mask key eint
	for(i=0;i<USEDKEY;i++)
		g_pGPIOReg->EINT0MASK  |= (1<<Key_EINT_Bit[i]);
	
	//Config Key int pin
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<0))|(0<<0);  // EINT0
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<2))|(0<<2);  //EINT1
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<4))|(0<<4);  //EINT2
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<10))|(0<<10); //EINT5
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<12))|(0<<12); //EINT6
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<18))|(0<<18); //EINT9
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<20))|(0<<20); //EINT10
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON& ~(3<<22))|(0<<22); //EINT11
	g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1& ~(0xF<<0))|(0<<0);	//EINT16
	g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1& ~(0xF<<4))|(0<<4);	//EINT17
 	//pGPIOReg->GPLCON1 = (pGPIOReg->GPLCON1& ~(0xF<<16))|(0<<16);	//EINT20
 	g_pGPIOReg->GPLCON1 = g_pGPIOReg->GPLCON1&(~(0xf<<20))|(0<<20);   //¶ú»ú¼ì²âÖÐ¶Ï½Å
	
	if( (g_pGPIOReg->GPNDAT &(1<<0))==0)    //EINT0
	{
			value = 1 ;  //FN	
	}
	else if( (g_pGPIOReg->GPNDAT &(1<<1))==0)    //EINT1
			value =2 ; 		//app
	else if( (g_pGPIOReg->GPNDAT &(1<<2))==0)    //EINT2
			value= 3;    //sip
	else if( (g_pGPIOReg->GPNDAT &(1<<5))==0)    //EINT5
			value=4;   //up
	else if( (g_pGPIOReg->GPNDAT &(1<<9))==0)    //EINT9
			value=5;    //down
	else if( (g_pGPIOReg->GPNDAT &(1<<10))==0)    //EINT10
	        value=6;    //left
	else if( (g_pGPIOReg->GPLDAT &(1<<8))==0)    //EINT16
			value=7;    //enter
	else if( (g_pGPIOReg->GPLDAT &(1<<9))==0)    //EINT17
			value=8;    //RIGHT
	else if( (g_pGPIOReg->GPNDAT &(1<<6))==0)    //EINT6
			value=9;   //camera
	else if( (g_pGPIOReg->GPNDAT &(1<<11))==0)	//EINT 1£±
			value=10;
	
			
    
	if(value!=0)
		*whichkey = g_KeySysIntr[value-1];

	KeyIOInit();
	//pGPIOReg->EINT0PEND = (1<<Key_EINT_Bit[7]);
	//pGPIOReg->EINT0MASK  &= ~(1<<Key_EINT_Bit[7]);
	return value;
	
}



int Which_Key_Pressed()
{
	int key=-1;
	int i=0;
	PROCESS_INFORMATION AppProcess;
	//RETAILMSG(1,(TEXT("EINT0MASK %x \r\n"),g_pGPIOReg->EINT0MASK));
	if((g_pGPIOReg->EINT0MASK & (0x1<<0)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<0)) | (0x0<<0);	
			while((g_pGPIOReg->GPNDAT & (1<<0))==0)
				{
					Sleep(1);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=0;
						break;
					}
				}
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<0)) | (0x2<<0);	
						if(key==-1)
				InterruptDone(g_KeySysIntr[0]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<1)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<2)) | (0x0<<2);	
			while((g_pGPIOReg->GPNDAT & (1<<1))==0)
				{
					Sleep(1);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=1;
						CreateProcess(L"switch.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &AppProcess);
						break;
					}
				}
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<2)) | (0x2<<2);
									if(key==-1)
				InterruptDone(g_KeySysIntr[1]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<2)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<4)) | (0x0<<4);	
			while((g_pGPIOReg->GPNDAT & (1<<2))==0)
				{
					Sleep(40);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>=4 && i<16)
					{
						key=2;
						//break;
					}else if(i>16)
					{
						RETAILMSG(1,(TEXT("KeyPad Key long pressed i = %d\r\n"),i));
						LongPress=TRUE;
						//SipShowIM(SIPF_OFF);
						break;
					}
				}
				if(i<16)
				{
					LongPress=FALSE;
					RETAILMSG(1,(TEXT("KeyPad Key short pressed i = %d\r\n"),i));

				}
				g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<4)) | (0x2<<4);	
									if(key==-1)
				InterruptDone(g_KeySysIntr[2]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<5)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<10)) | (0x0<<10);	
			while((g_pGPIOReg->GPNDAT & (1<<5))==0)
				{
					Sleep(1);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=3;
						break;
					}
				}
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<10)) | (0x2<<10);	
									if(key==-1)
				InterruptDone(g_KeySysIntr[3]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<6)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<12)) | (0x0<<12);	
			while((g_pGPIOReg->GPNDAT & (1<<6))==0)
				{
					Sleep(1);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=4;
						break;
					}
				}
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<12)) | (0x2<<12);	
									if(key==-1)
				InterruptDone(g_KeySysIntr[4]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<9)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<18)) | (0x0<<18);	
			while((g_pGPIOReg->GPNDAT & (1<<9))==0)
				{
					Sleep(1);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=5;
						break;
					}
				}
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<18)) | (0x2<<18);	
									if(key==-1)
				InterruptDone(g_KeySysIntr[5]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<10)))
		{
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<20)) | (0x0<<20);	
			while((g_pGPIOReg->GPNDAT & (1<<10))==0)
				{
					Sleep(1);
					i++;
					//RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=6;
						break;
					}
				}
			g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<20)) | (0x2<<20);	
									if(key==-1)
				InterruptDone(g_KeySysIntr[6]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<16)))
		{
			g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1 & ~(0x7<<0)) | (0x0<<0);	
			while((g_pGPIOReg->GPLDAT & (1<<8))==0)
				{
					Sleep(1);
					i++;
					RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=7;
						break;
					}
				}
			g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1 & ~(0x7<<0)) | (0x3<<0);	
			if(key==-1)
			InterruptDone(g_KeySysIntr[7]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<17)))
		{
			g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1 & ~(0x7<<4)) | (0x0<<4);	
			while((g_pGPIOReg->GPLDAT & (1<<9))==0)
				{
					Sleep(1);
					i++;
					RETAILMSG(1,(TEXT("i = %d\r\n"),i));
					if(i>10)
					{
						key=8;
						break;
					}
				}
			g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1 & ~(0x7<<4)) | (0x3<<4);	
			if(key==-1)
			InterruptDone(g_KeySysIntr[8]);

		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<21)))
		{
			key=9;
			//g_pGPIOReg->EINT0MASK &= ~(0x1<<21);	
		}
	return key;
}
void Clear_Irq()
{
	if((g_pGPIOReg->EINT0MASK & (0x1<<0)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<0);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<1)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<1);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<2)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<2);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<5)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<5);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<6)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<6);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<9)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<9);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<10)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<10);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<16)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<16);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<17)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<17);	
		}
	else if((g_pGPIOReg->EINT0MASK & (0x1<<21)))
		{
			g_pGPIOReg->EINT0MASK &= ~(0x1<<21);	
		}
}
DWORD UserKeyProcessThread(void)
{
	int key_no;
	while(1)
	{
		WaitForSingleObject( KeyEvent, INFINITE);
		key_no=Which_Key_Pressed();
		if(key_no==-1)
			{
			Clear_Irq();
			//InterruptDone(g_KeySysIntr[key_no]);
			RETAILMSG(1,(TEXT("bad key pressed\r\n")));
			continue;
			}
		RETAILMSG(1,(TEXT("Key %x pressed,Fn is %x \r\n"),key_no,Fn_Key_Pressed()));
		if(key_no==9)//headphone event
			{
			Sleep(300);
			g_pGPIOReg->GPLCON1 = g_pGPIOReg->GPLCON1&(~(0xf<<20))|(0<<20);
			if ((g_pGPIOReg->GPLDAT & (1<<13) == (1<<13)))  //¶ú»ú²åÈë
					{ 
					
						g_pGPIOReg->GPADAT  |= (1<<2);	//¹Ø±ÕÀ®°ÈÊä³ö
						RETAILMSG(1,(TEXT("Headphone has been inserted in!!\r\n")));
					}
					else								//¶ú»ú°Î³ö
					{
						g_pGPIOReg->GPADAT  &= ~(1<<2);
						RETAILMSG(1,(TEXT("Headphone has been pulled out!\r\n")));
					}
			g_pGPIOReg->GPLCON1 = g_pGPIOReg->GPLCON1&(~(0xf<<20))|(3<<20);
			Clear_Irq();
			InterruptDone(g_KeySysIntr[key_no]);
			continue;
			}
		if(key_no==0)
			{
				fnKey=!fnKey;
				if(fnKey)
					g_pGPIOReg->GPKDAT &=~(1<<10);
				else
					g_pGPIOReg->GPKDAT |=(1<<10);
				Clear_Irq();
				InterruptDone(g_KeySysIntr[key_no]);
				continue;
			}
		if(fnKey)
		{
			if(key_no==3)
				Change_Backlight(TRUE);
			else if(key_no==5)
				Change_Backlight(FALSE);
			else if(key_no==6)
				Change_Volume(FALSE);
			else if(key_no==8)
				Change_Volume(TRUE);
			else
				{
					if(key_no==1 || key_no==4)
						{
							//keybd_event(key_table[key_no],0, 0, 0);
							//Sleep(10);
							//keybd_event(key_table[key_no],0, KEYEVENTF_KEYUP, 0);
							keybd_event(VK_LWIN,0,KEYEVENTF_SILENT,0);
							keybd_event(key_table[key_no],0, 0, 0);
							Sleep(10);
							keybd_event(key_table[key_no],0, KEYEVENTF_KEYUP, 0);
							//GenKeyEvent(key_table[key_no]);
							keybd_event(VK_LWIN,0,KEYEVENTF_SILENT|KEYEVENTF_KEYUP,0);
						}else{
						if(key_no==2)
							ChangeTheIME();
						else{
							keybd_event(key_table[key_no],0, 0, 0);
							Sleep(10);
							keybd_event(key_table[key_no],0, KEYEVENTF_KEYUP, 0);
							}
							}
				}
		}
		else
		{
			if(key_no==1 || key_no==4){
				keybd_event(VK_LWIN,0,KEYEVENTF_SILENT,0);
				keybd_event(key_table[key_no],0, 0, 0);
				Sleep(10);
				keybd_event(key_table[key_no],0, KEYEVENTF_KEYUP, 0);
				//GenKeyEvent(key_table[key_no]);
				keybd_event(VK_LWIN,0,KEYEVENTF_SILENT|KEYEVENTF_KEYUP,0);
				}else{
				if(key_no==2)
							ChangeTheIME();
						else{
				keybd_event(key_table[key_no],0, 0, 0);
				Sleep(10);
				keybd_event(key_table[key_no],0, KEYEVENTF_KEYUP, 0);
							}
					}
		}
		Clear_Irq();
		InterruptDone(g_KeySysIntr[key_no]);
		//ResetEvent(KeyEvent[key_no]);
	}
}


void Virtual_Alloc()
{
	int i;
	//------------------
	// GPIO Controller SFR
	//------------------
	g_pGPIOReg = (S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (g_pGPIOReg == NULL)
	{
		KEYBTN_ERR((_T("[KEY:ERR] PWR_AllocResources() : pGPIOReg DrvLib_MapIoSpace() Failed \n\r")));
		return ;
	}
	KeyEvent= CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == KeyEvent)
	{
		KEYBTN_ERR((_T("[KEY:ERR] PWR_AllocResources() : CreateEvent() keypad Button Failed \n\r")));
		return ;
	}

	for(i=0;i<10;i++)
	{
	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &(g_KeyIrq[i]), sizeof(DWORD), &(g_KeySysIntr[i]), sizeof(DWORD), NULL))
	{
		KEYBTN_ERR((_T("[KEY:ERR] PWR_AllocResources() : IOCTL_HAL_REQUEST_SYSINTR Power Button Failed \n\r")));
		g_KeySysIntr[i] = SYSINTR_UNDEFINED;
		return ;
	}


	if (!(InterruptInitialize(g_KeySysIntr[i], KeyEvent, NULL, 0)))
	{
		KEYBTN_ERR((_T("[KEY:ERR] PWR_AllocResources() : InterruptInitialize() keypad Button Failed \n\r")));
		return ;
	}
	}
	//EINT0 GPN0
	g_pGPIOReg->EINT0MASK |= (0x1<<0);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<0)) | (0x2<<0);	
	g_pGPIOReg->GPNPUD |= 0x3<<20;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<0)) | (0x2<<0);
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0x3<<0)) | (0x1<<0);
	g_pGPIOReg->EINT0PEND |= (0x1<<0);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<0);	
	//EINT1 GPN1
	g_pGPIOReg->EINT0MASK |= (0x1<<1);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<2)) | (0x2<<2);	
	g_pGPIOReg->GPNPUD |= 0x3<<2;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<0)) | (0x2<<0);
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0x3<<0)) | (0x1<<0);
	g_pGPIOReg->EINT0PEND |= (0x1<<1);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<1);	
	//EINT2 GPN2
	g_pGPIOReg->EINT0MASK |= (0x1<<2);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<4)) | (0x2<<4);	
	g_pGPIOReg->GPNPUD |= 0x3<<4;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<4)) | (0x2<<4);
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0x3<<8)) | (0x1<<8);
	g_pGPIOReg->EINT0PEND |= (0x1<<2);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<2);	
	//EINT5 GPN5
	g_pGPIOReg->EINT0MASK |= (0x1<<5);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<10)) | (0x2<<10);	
	g_pGPIOReg->GPNPUD |= 0x3<<10;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<8)) | (0x2<<8);
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0x3<<16)) | (0x1<<16);
	g_pGPIOReg->EINT0PEND |= (0x1<<5);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<5);	
	//EINT6 GPN6
	g_pGPIOReg->EINT0MASK |= (0x1<<6);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<12)) | (0x2<<12);	
	g_pGPIOReg->GPNPUD |= 0x3<<12;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<12)) | (0x2<<12);
	g_pGPIOReg->EINT0FLTCON0 = (g_pGPIOReg->EINT0FLTCON0 & ~(0x3<<24)) | (0x1<<24);
	g_pGPIOReg->EINT0PEND |= (0x1<<6);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<6);	
	//EINT9 GPN9
	g_pGPIOReg->EINT0MASK |= (0x1<<9);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<18)) | (0x2<<18);	
	g_pGPIOReg->GPNPUD |= 0x3<<18;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<16)) | (0x2<<16);
	g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0x3<<0)) | (0x1<<0);
	g_pGPIOReg->EINT0PEND |= (0x1<<9);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<9);	
	//EINT10 GPN10
	g_pGPIOReg->EINT0MASK |= (0x1<<10);	
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<20)) | (0x2<<20);	
	g_pGPIOReg->GPNPUD |= 0x3<<20;	
	g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<20)) | (0x2<<20);
	g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0x3<<8)) | (0x1<<8);
	g_pGPIOReg->EINT0PEND |= (0x1<<10);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<10);	
	//EINT16 GPL8
	g_pGPIOReg->EINT0MASK |= (0x1<<16);	
	g_pGPIOReg->GPLCON1 =(g_pGPIOReg->GPLCON1 & ~(0x7<<0)) | (0x3<<0);	
	g_pGPIOReg->GPLPUD |= 0x3<<16;	
	g_pGPIOReg->EINT0CON1 = (g_pGPIOReg->EINT0CON1 & ~(0x7<<0)) | (0x2<<0);
	g_pGPIOReg->EINT0FLTCON2 = (g_pGPIOReg->EINT0FLTCON2 & ~(0x3<<0)) | (0x1<<0);
	g_pGPIOReg->EINT0PEND |= (0x1<<16);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<16);	
	//EINT17 GPL9
	g_pGPIOReg->EINT0MASK |= (0x1<<17);	
	g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1 & ~(0x7<<4)) | (0x3<<4);	
	g_pGPIOReg->GPLPUD |= 0x3<<18;	
	g_pGPIOReg->EINT0CON1 = (g_pGPIOReg->EINT0CON1 & ~(0x7<<0)) | (0x2<<0);
	g_pGPIOReg->EINT0FLTCON2 = (g_pGPIOReg->EINT0FLTCON2 & ~(0x3<<0)) | (0x1<<0);
	g_pGPIOReg->EINT0PEND |= (0x1<<17);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<17);	
	//Fn Key GPK10
	g_pGPIOReg->GPKCON1 = g_pGPIOReg->GPKCON1 & ~(0x7<<8);	
	g_pGPIOReg->GPKCON1 = g_pGPIOReg->GPKCON1 | (0x1<<8);	
	g_pGPIOReg->GPKPUD |= 0x3<<20;	
	//EINT21 GPL13
	g_pGPIOReg->EINT0MASK |= (0x1<<21);	
	g_pGPIOReg->GPLCON1 = (g_pGPIOReg->GPLCON1 & ~(0x7<<20)) | (0x3<<20);	
	g_pGPIOReg->GPLPUD |= 0x3<<26;	
	g_pGPIOReg->EINT0CON1 = (g_pGPIOReg->EINT0CON1 & ~(0xf<<8)) | (0x6<<8);
	g_pGPIOReg->EINT0FLTCON2 = (g_pGPIOReg->EINT0FLTCON2 & ~(0x3<<16)) | (0x1<<16);
	g_pGPIOReg->EINT0PEND |= (0x1<<21);		
	g_pGPIOReg->EINT0MASK &= ~(0x1<<21);	

	if(Fn_Key_Pressed())
	{
		fnKey=!fnKey;
	}
	if(fnKey)
		g_pGPIOReg->GPKDAT &=~(1<<10);
	else
		g_pGPIOReg->GPKDAT |=(1<<10);

}


BOOL WINAPI  
DllEntry(HANDLE	hinstDLL, 
			DWORD dwReason, 
			LPVOID  Reserved/* lpvReserved */)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DEBUGREGISTER((HINSTANCE)hinstDLL);
		return TRUE;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
#ifdef UNDER_CE
	case DLL_PROCESS_EXITING:
		break;
	case DLL_SYSTEM_STARTED:
		break;
#endif
	}

	return TRUE;
}

BOOL Fn_Key_Pressed()
{
	//return FALSE;
	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<0)) | (0x0<<0);	
	if((g_pGPIOReg->GPNDAT & (1<<0))==0)
	{
		g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<0)) | (0x2<<0);	
		return TRUE;
	}
	else
	{
		g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(0x3<<0)) | (0x2<<0);	
		return FALSE;
	}
}
BOOL KEY_Deinit(DWORD hDeviceContext)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_Deinit\r\n")));
	return TRUE;
} 

BOOL InitInterruptThread()
{
	
	DWORD         threadID;                         // thread ID
       KeyThread = CreateThread(NULL,
                                 0,
                                 (LPTHREAD_START_ROUTINE)UserKeyProcessThread,
                                 0,
                                 0,
                                 &threadID);
    
    if (NULL == KeyThread ) {
    	RETAILMSG(1,(TEXT("Create Key Thread Fail\r\n")));
    }
	
	RETAILMSG(1,(_T("UserKey.DLL::InterruptThread Initialized.\r\n")));
	return TRUE;
}
void Change_Backlight(BOOL Level)
{
    DWORD bytes,dwErr;
    if ( !DeviceIoControl(hBak,
                      IOCTL_BACKLIGHT_SET, 
                      NULL, 0,
                      &Level, sizeof(BOOL), 
                      &bytes, NULL) )
    {
        dwErr = GetLastError();
        RETAILMSG(1,(TEXT("IOCTL_BACKLIGHT_SET ERROR: %u \r\n"), dwErr));
        return;
    }
    keybd_event(VK_F4,0, 0, 0);
    Sleep(10);
    keybd_event(VK_F4,0, KEYEVENTF_KEYUP, 0);
}
void Change_Volume(BOOL Level)
{
#if 0
    DWORD bytes,dwErr;
    if ( !DeviceIoControl(hAudio,
                      IOCTL_VOLUME_SET, 
                      NULL, 0,
                      &Level, sizeof(BOOL), 
                      &bytes, NULL) )
    {
        dwErr = GetLastError();
        RETAILMSG(1,(TEXT("IOCTL_VOLUME_SET ERROR: %u \r\n"), dwErr));
        return;
    }
    keybd_event(VK_F4,0, 0, 0);
    Sleep(10);
    keybd_event(VK_F4,0, KEYEVENTF_KEYUP, 0);
	#endif
}

DWORD KEY_Init(DWORD dwContext)
{
	DWORD dwErr;
	RETAILMSG(1,(TEXT("KEY_Init----\r\n")));
	// 1. Virtual Alloc
	Virtual_Alloc();

	if (!InitInterruptThread())
	{
        RETAILMSG(1,(TEXT("Fail to initialize userkey interrupt event\r\n")));
        return FALSE;
   	 }   	
    	hBak = CreateFile( L"BKL1:",
	                   GENERIC_READ|GENERIC_WRITE,
	                   FILE_SHARE_READ|FILE_SHARE_WRITE,
	                   NULL, OPEN_EXISTING, 0, 0);
                   
    if ( INVALID_HANDLE_VALUE == hBak ) 
	  {
        dwErr = GetLastError();
        RETAILMSG(1, (TEXT("Error %d opening device '%s' \r\n"), dwErr, L"BKL1:" ));
         return FALSE;
    }
	RETAILMSG(1,(TEXT("sizeof(ULONG)= %x\r\n"),sizeof(ULONG)));
    #if 0
    	hAudio = CreateFile( L"WAV1:",
	                   GENERIC_READ|GENERIC_WRITE,
	                   FILE_SHARE_READ|FILE_SHARE_WRITE,
	                   NULL, OPEN_EXISTING, 0, 0);
                   
    if ( INVALID_HANDLE_VALUE == hAudio ) 
	  {
        dwErr = GetLastError();
        RETAILMSG(1, (TEXT("Error %d opening device '%s' \r\n"), dwErr, L"WAV1:" ));
         return FALSE;
    }
	#endif
	#ifndef IMGULDR
	SipEnumIM(SipEnumIMProc); 
	#endif
	return TRUE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BOOL KEY_IOControl(DWORD hOpenContext, 
				   DWORD dwCode, 
				   PBYTE pBufIn, 
				   DWORD dwLenIn, 
				   PBYTE pBufOut, 
				   DWORD dwLenOut, 
				   PDWORD pdwActualOut)
{
    
	RETAILMSG(1,(TEXT("KEY:Ioctl code = 0x%x\r\n"), dwCode));
	return TRUE;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DWORD KEY_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_Open\r\n")));	
	return TRUE;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BOOL KEY_Close(DWORD hOpenContext)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_Close\r\n")));
	return TRUE;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void KEY_PowerDown(DWORD hDeviceContext)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_PowerDown\r\n")));
	
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void KEY_PowerUp(DWORD hDeviceContext)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_PowerUp\r\n")));
	KeyIOInit();
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DWORD KEY_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_Read\r\n")));
	return TRUE;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DWORD KEY_Seek(DWORD hOpenContext, long Amount, DWORD Type)
{
	RETAILMSG(1,(TEXT("USERKEY: KEY_Seek\r\n")));
	return 0;
} 

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DWORD KEY_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes)
{
	  RETAILMSG(1,(TEXT("USERKEY: KEY_Write\r\n")));
	return 0;
}




