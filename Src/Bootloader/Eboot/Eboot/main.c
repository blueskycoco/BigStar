//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//

#include <windows.h>
#include <bsp.h>
#include <ethdbg.h>
#include <fmd.h>
#include <fmd_sb.h>
#include "loader.h"
#include "usb.h"

#include "s3c6410_ldi.h"
#include "s3c6410_display_con.h"
#if 0
#include "InitialImage_rgb16_480x272.h"
#else
//#include "samsung.c"
#endif

// For USB Download function.
extern BOOL UbootReadData (DWORD cbData, LPBYTE pbData);
extern BOOL InitUSB ();
extern void Isr_Init();

// To change Voltage for higher clock.
extern void LTC3714_Init();
extern void LTC3714_VoltageSet(UINT32,UINT32,UINT32);

//extern void OTGDEV_SetSoftDisconnect();
extern int HSMMCInit(void);
extern void ChooseImageFromSD();
extern bool SDReadData(DWORD cbData, LPBYTE pbData);

char *inet_ntoa(DWORD dwIP);
DWORD inet_addr( char *pszDottedD );
BOOL EbootInitEtherTransport (EDBG_ADDR *pEdbgAddr, LPDWORD pdwSubnetMask,
                              BOOL *pfJumpImg,
                              DWORD *pdwDHCPLeaseTime,
                              UCHAR VersionMajor, UCHAR VersionMinor,
                              char *szPlatformString, char *szDeviceName,
                              UCHAR CPUId, DWORD dwBootFlags);
BOOL EbootEtherReadData (DWORD cbData, LPBYTE pbData);
EDBG_OS_CONFIG_DATA *EbootWaitForHostConnect (EDBG_ADDR *pDevAddr, EDBG_ADDR *pHostAddr);
extern UINT32 BLFlashDownload();
// End   ***************************************

// Display Function
static void InitializeDisplay(void);
BOOL RAW_LB_ReadSector(UINT32 startSectorAddr, LPBYTE pSectorBuff, LPBYTE pSectorInfoBuff);


// Globals
//
DWORD			g_ImageType;
MultiBINInfo		g_BINRegionInfo;
PBOOT_CFG		g_pBootCfg;
UCHAR			g_TOC[LB_SECTOR_SIZE];
const PTOC 		g_pTOC = (PTOC)&g_TOC;
DWORD			g_dwImageStartBlock;
DWORD			g_dwTocEntry;
BOOL			g_bBootMediaExist = FALSE;
BOOL			g_bDownloadImage  = TRUE;
BOOLEAN			g_bUSBDownload = FALSE;
// jylee
BOOLEAN			g_bT32Download = FALSE;
BOOLEAN			g_bSDDownload = FALSE;
BOOLEAN         g_bAutoDownload = FALSE;

EDBG_ADDR 		g_DeviceAddr;	// NOTE: global used so it remains in scope throughout download process
								// since eboot library code keeps a global pointer to the variable provided.

DWORD			wNUM_BLOCKS;

// External definitions.
//
//extern const BYTE ScreenBitmap[];



/*
    @func   void | SpinForever | Halts execution (used in error conditions).
    @rdesc
    @comm
    @xref
*/
static void SpinForever(void)
{
    EdbgOutputDebugString("SpinForever...\r\n");

    while(1)
    {
        ;
    }
}


/*
    @func   void | main | Samsung bootloader C routine entry point.
    @rdesc  N/A.
    @comm
    @xref
*/
void main(void)
{

	// Clear LEDs.
	//
	//    OEMWriteDebugLED(0, 0xF);

	// Common boot loader (blcommon) main routine.
	//    
	//	OALMSG(1, (TEXT("\r\n============---------------\r\n")));
	volatile UINT32 uTemp;

	uTemp = Inp32SYSC(0x900);
	Outp32SYSC(0x900, uTemp|(1<<16)); // unmask usb signal
	//	OTGDEV_InitPhyCon();
	//	OTGDEV_SoftResetCore();
	//	OTGDEV_InitCore();


	//	OTGDEV_InitOtg(USB_HIGH);
	OTGDEV_SetSoftDisconnect();	

	BootloaderMain();

	// Should never get here.
	//
	SpinForever();
}

/*
    @func   void | SetDeviceID | Device ID from user input.
    @rdesc  N/A.
    @comm
    @xref
*/

static BOOL SetDeviceID(PBOOT_CFG pBootCfg)
{
	UCHAR   szDottedD[16 + 1];   // The string used to collect the dotted decimal IP address.
	UCHAR   szUserName[16+1];
	UCHAR   szPassword[16+1];
	UCHAR buffer[16];
	USHORT cwNumChars = 0;
	USHORT InChar = 0;
	int iIndex,j;
	CHAR bMask[64] = {0xA4, 0xAD, 0xBC, 0xE9, 0xC2, 0xBF, 0xBB, 0xE5,
					0xDC, 0xFF, 0xE5, 0x92, 0xFD, 0xF4, 0xEF, 0xAE,
					0xB6, 0xED, 0xB3, 0xD0, 0xE2, 0xDE, 0xA2, 0xC2,
					0xEB, 0xE0, 0xFE, 0xCD, 0x88, 0xA1, 0xB7, 0xE4,
					0xA4, 0xAD, 0xBC, 0xE9, 0xC2, 0xBF, 0xBB, 0xE5,
					0xDC, 0xFF, 0xE5, 0x92, 0xFD, 0xF4, 0xEF, 0xAE,
					0xB6, 0xED, 0xB3, 0xD0, 0xE2, 0xDE, 0xA2, 0xC2,
					0xEB, 0xE0, 0xFE, 0xCD, 0x88, 0xA1, 0xB7, 0xE4};
	CHAR bMask2[64] = {0x12, 0xF7, 0x3B, 0x6F, 0x7C, 0x24, 0x6F, 0x28,
					0x32, 0x60, 0x6C, 0x28, 0x66, 0x6F, 0x45, 0x03,
					0x6D, 0x0D, 0x51, 0x2B, 0x61, 0x2B, 0x37, 0x00,
					0x60, 0x20, 0x04, 0x04, 0x1F, 0x5B, 0x67, 0x14,
					0x12, 0xF7, 0x3B, 0x6F, 0x7C, 0x24, 0x6F, 0x28,
					0x32, 0x60, 0x6C, 0x28, 0x66, 0x6F, 0x45, 0x03,
					0x6D, 0x0D, 0x51, 0x2B, 0x61, 0x2B, 0x37, 0x00,
					0x60, 0x20, 0x04, 0x04, 0x1F, 0x5B, 0x67, 0x14};
#if 0
    EdbgOutputDebugString("\r\nEnter new IP address: ");
    //get username
    
    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
       //
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
        	 
        	if ((InChar >= '0' && InChar <= '9') || (InChar >= 'a' && InChar<='z')|| (InChar >= 'A' && InChar<='Z'))
            {
                if (cwNumChars < 16)
                {
                    szUserName[cwNumChars++] = (char)InChar;
                    //OEMWriteDebugByte((BYTE)szUserName[cwNumChars-1]);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    //OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }
   szUserName[9]='\0';
  // for(iIndex=0;iIndex<9;iIndex++)
   //	OEMWriteDebugByte((BYTE)szUserName[iIndex]);
	//if(strcmp(szUserName,L"K3f4WqeRt")!=0)
		//return;
	if(szUserName[0]!='K' 
		|| szUserName[1]!='3' 
		|| szUserName[2]!='f' 
		|| szUserName[3]!='4' 
		|| szUserName[4]!='W' 
		|| szUserName[5]!='q' 
		|| szUserName[6]!='e' 
		|| szUserName[7]!='R' 
		|| szUserName[8]!='t')
		{
		//	EdbgOutputDebugString("\r\nsome char is incorrect: ");
			return FALSE;
		}
		EdbgOutputDebugString("\r\ninput password: ");
	//get password
	cwNumChars=0;
	InChar=0;
    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
        	
        	if ((InChar >= '0' && InChar <= '9') || (InChar >= 'a' && InChar<='z')|| (InChar >= 'A' && InChar<='Z'))
            {
                if (cwNumChars < 16)
                {
                    szPassword[cwNumChars++] = (char)InChar;
                   // OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    //OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }
	//if(strcmp(szPassword,"U8i9He2B")!=0)
	//	return;
if(szPassword[0]!='U' 
		|| szPassword[1]!='8' 
		|| szPassword[2]!='i' 
		|| szPassword[3]!='9' 
		|| szPassword[4]!='H' 
		|| szPassword[5]!='e' 
		|| szPassword[6]!='2' 
		|| szPassword[7]!='B'
		)
		{
		//	EdbgOutputDebugString("\r\nsome char is incorrect: ");
			return FALSE;
		}
		#endif
		EdbgOutputDebugString("\r\ninput device id: ");
	//get deviceid
	cwNumChars=0;
	InChar=0;
    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
            // If it's a number or a period, add it to the string.
            //
            if ((InChar >= '0' && InChar <= '9') || (InChar >= 'a' && InChar<='z')|| (InChar >= 'A' && InChar<='Z'))
            {
                if (cwNumChars < 16)
                {
                    szDottedD[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }

    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars==16)
    {
	for(j=0;j<16;j++)
	{
		buffer[j]=szDottedD[j]^bMask[j]^bMask2[j];
	}
	pBootCfg->Device_ID[0]=(buffer[0]<<24)|(buffer[1]<<16)|(buffer[2]<<8)|buffer[3];
	pBootCfg->Device_ID[1]=(buffer[4]<<24)|(buffer[5]<<16)|(buffer[6]<<8)|buffer[7];
	pBootCfg->Device_ID[2]=(buffer[8]<<24)|(buffer[9]<<16)|(buffer[10]<<8)|buffer[11];
	pBootCfg->Device_ID[3]=(buffer[12]<<24)|(buffer[13]<<16)|(buffer[14]<<8)|buffer[15];

	pBSPArgs->Device_ID[0]=pBootCfg->Device_ID[0];
	pBSPArgs->Device_ID[1]=pBootCfg->Device_ID[1];
	pBSPArgs->Device_ID[2]=pBootCfg->Device_ID[2];
	pBSPArgs->Device_ID[3]=pBootCfg->Device_ID[3];	
	return TRUE;
    }
    else
   	{
				EdbgOutputDebugString("\r\nError  ");
   	}
	return FALSE;
}

/*
    @func   void | SetIP | Accepts IP address from user input.
    @rdesc  N/A.
    @comm
    @xref
*/

static void SetIP(PBOOT_CFG pBootCfg)
{
    CHAR   szDottedD[16 + 1];   // The string used to collect the dotted decimal IP address.
    USHORT cwNumChars = 0;
    USHORT InChar = 0;

    EdbgOutputDebugString("\r\nEnter new IP address: ");

    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
            // If it's a number or a period, add it to the string.
            //
            if (InChar == '.' || (InChar >= '0' && InChar <= '9'))
            {
                if (cwNumChars < 16)
                {
                    szDottedD[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }

    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars)
    {
        szDottedD[cwNumChars] = '\0';
        pBootCfg->EdbgAddr.dwIP = inet_addr(szDottedD);
    }
}


/*
    @func   void | SetMask | Accepts subnet mask from user input.
    @rdesc  N/A.
    @comm
    @xref
*/
static void SetMask(PBOOT_CFG pBootCfg)
{
    CHAR szDottedD[16 + 1]; // The string used to collect the dotted masks.
    USHORT cwNumChars = 0;
    USHORT InChar = 0;

    EdbgOutputDebugString("\r\nEnter new subnet mask: ");

    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
            // If it's a number or a period, add it to the string.
            //
            if (InChar == '.' || (InChar >= '0' && InChar <= '9'))
            {
                if (cwNumChars < 16)
                {
                    szDottedD[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }

    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars)
    {
        szDottedD[cwNumChars] = '\0';
        pBootCfg->SubnetMask = inet_addr(szDottedD);
    }
}


/*
    @func   void | SetDelay | Accepts an autoboot delay value from user input.
    @rdesc  N/A.
    @comm
    @xref
*/
static void SetDelay(PBOOT_CFG pBootCfg)
{
    CHAR szCount[16 + 1];
    USHORT cwNumChars = 0;
    USHORT InChar = 0;

    EdbgOutputDebugString("\r\nEnter maximum number of seconds to delay [1-255]: ");

    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
            // If it's a number or a period, add it to the string.
            //
            if ((InChar >= '0' && InChar <= '9'))
            {
                if (cwNumChars < 16)
                {
                    szCount[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }

    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars)
    {
        szCount[cwNumChars] = '\0';
        pBootCfg->BootDelay = atoi(szCount);
        if (pBootCfg->BootDelay > 255)
        {
            pBootCfg->BootDelay = 255;
        }
        else if (pBootCfg->BootDelay < 1)
        {
            pBootCfg->BootDelay = 1;
        }
    }
}


static ULONG mystrtoul(PUCHAR pStr, UCHAR nBase)
{
    UCHAR nPos=0;
    BYTE c;
    ULONG nVal = 0;
    UCHAR nCnt=0;
    ULONG n=0;

    // fulllibc doesn't implement isctype or iswctype, which are needed by
    // strtoul, rather than including coredll code, here's our own simple strtoul.

    if (pStr == NULL)
        return(0);

    for (nPos=0 ; nPos < strlen(pStr) ; nPos++)
    {
        c = tolower(*(pStr + strlen(pStr) - 1 - nPos));
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'a' && c <= 'f')
        {
            c -= 'a';
            c  = (0xa + c);
        }

        for (nCnt = 0, n = 1 ; nCnt < nPos ; nCnt++)
        {
            n *= nBase;
        }
        nVal += (n * c);
    }

    return(nVal);
}


static void CvtMAC(USHORT MacAddr[3], char *pszDottedD )
{
    DWORD cBytes;
    char *pszLastNum;
    int atoi (const char *s);
    int i=0;
    BYTE *p = (BYTE *)MacAddr;
    char InChar = 0;

    // Replace the dots with NULL terminators
    pszLastNum = pszDottedD;
    for(cBytes = 0 ; cBytes < 6 ; cBytes++)
    {
			InChar = *pszDottedD;
        while((InChar != '.') && (InChar != '\0'))
        {
            pszDottedD++;
        }
			InChar = *pszDottedD;
        if ((InChar == '\0') && cBytes != 5)
        {
            // zero out the rest of MAC address
            while(i++ < 6)
            {
                *p++ = 0;
            }
            break;
        }
        *pszDottedD = '\0';
        *p++ = (BYTE)(mystrtoul(pszLastNum, 16) & 0xFF);
        i++;
        pszLastNum = ++pszDottedD;
    }
}


static void SetCS8900MACAddress(PBOOT_CFG pBootCfg)
{
    CHAR szDottedD[24];
    USHORT cwNumChars = 0;
    USHORT InChar = 0;

    memset(szDottedD, 0x0 /*'0'*/, 24);

    EdbgOutputDebugString ( "\r\nEnter new MAC address in hexadecimal (hh.hh.hh.hh.hh.hh): ");

    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        InChar = tolower(InChar);
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
            // If it's a hex number or a period, add it to the string.
            //
            if (InChar == '.' || (InChar >= '0' && InChar <= '9') || (InChar >= 'a' && InChar <= 'f'))
            {
                if (cwNumChars < 17)
                {
                    szDottedD[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            else if (InChar == 8)       // If it's a backspace, back up.
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }

    EdbgOutputDebugString ( "\r\n");

    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars)
    {
        szDottedD[cwNumChars] = '\0';
        CvtMAC(pBootCfg->EdbgAddr.wMAC, szDottedD);

        EdbgOutputDebugString("INFO: MAC address set to: %x:%x:%x:%x:%x:%x\r\n",
                  pBootCfg->EdbgAddr.wMAC[0] & 0x00FF, pBootCfg->EdbgAddr.wMAC[0] >> 8,
                  pBootCfg->EdbgAddr.wMAC[1] & 0x00FF, pBootCfg->EdbgAddr.wMAC[1] >> 8,
                  pBootCfg->EdbgAddr.wMAC[2] & 0x00FF, pBootCfg->EdbgAddr.wMAC[2] >> 8);
    }
    else
    {
        EdbgOutputDebugString("WARNING: SetCS8900MACAddress: Invalid MAC address.\r\n");
    }
}

UINT32 SetBlockPage(void)
{
    CHAR szCount[16];
    USHORT cwNumChars = 0;
    USHORT InChar = 0;
    UINT32 block=0, page=0;
    UINT32 i;
 
    EdbgOutputDebugString("\r\nEnter Block # to read : ");
 
    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA) 
        {
            // If it's a number or a period, add it to the string.
            //
            if ((InChar >= '0' && InChar <= '9')) 
            {
                if (cwNumChars < 16) 
                {
                    szCount[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8) 
            {
                if (cwNumChars > 0) 
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }
 
    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars) 
    {
        szCount[cwNumChars] = '\0';
        block = atoi(szCount);
    }
 
    EdbgOutputDebugString("\r\nEnter Page # to read : ");
    InChar = 0;
    for (i=0; i<16; i++) szCount[i] = '0';
    
    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA) 
        {
            // If it's a number or a period, add it to the string.
            //
            if ((InChar >= '0' && InChar <= '9')) 
            {
                if (cwNumChars < 16) 
                {
                    szCount[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8) 
            {
                if (cwNumChars > 0) 
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }
 
    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars) 
    {
        szCount[cwNumChars] = '\0';
        page = atoi(szCount);
    }
 
    if (IS_LB) return ((block<<6)|page);
    else       return ((block<<5)|page);
}
 
void PrintPageData(DWORD nMData, DWORD nSData, UINT8* pMBuf, UINT8* pSBuf)
{
    DWORD i;
 
    OALMSG(TRUE, (TEXT("=========================================================")));
    for (i=0;i<nMData;i++)
    {
        if ((i%16)==0)
            OALMSG(TRUE, (TEXT("\r\n 0x%03x |"), i));
        OALMSG(TRUE, (TEXT(" %02x"), pMBuf[i]));
        if ((i%512)==511)
            OALMSG(TRUE, (TEXT("\r\n ------------------------------------------------------- ")));
    }
    for (i=0;i<nSData;i++)
    {
        if ((i%16)==0)
            OALMSG(TRUE, (TEXT("\r\n 0x%03x |"), i));
        OALMSG(TRUE, (TEXT(" %02x"), pSBuf[i]));
    }
    OALMSG(TRUE, (TEXT("\r\n=========================================================")));
}

 


/*
    @func   BOOL | MainMenu | Manages the Samsung bootloader main menu.
    @rdesc  TRUE == Success and FALSE == Failure.
    @comm
    @xref
*/

static BOOL MainMenu(PBOOT_CFG pBootCfg)
{
    BYTE KeySelect = 0;
    BOOL bConfigChanged = FALSE;
    BOOLEAN bDownload = TRUE;
    DWORD i=0;

    while(TRUE)
    {
        KeySelect = 0;

        EdbgOutputDebugString ( "\r\nEthernet Boot Loader Configuration:\r\n\r\n");
        EdbgOutputDebugString ( "0) IP address: %s\r\n",inet_ntoa(pBootCfg->EdbgAddr.dwIP));
        EdbgOutputDebugString ( "1) Subnet mask: %s\r\n", inet_ntoa(pBootCfg->SubnetMask));
        EdbgOutputDebugString ( "2) DHCP: %s\r\n", (pBootCfg->ConfigFlags & CONFIG_FLAGS_DHCP)?"Enabled":"Disabled");
        EdbgOutputDebugString ( "3) Boot delay: %d seconds\r\n", pBootCfg->BootDelay);
        EdbgOutputDebugString ( "4) Reset to factory default configuration\r\n");
        EdbgOutputDebugString ( "5) Startup image: %s\r\n", (g_pBootCfg->ConfigFlags & BOOT_TYPE_DIRECT) ? "LAUNCH EXISTING" : "DOWNLOAD NEW");
        EdbgOutputDebugString ( "6) Program disk image into NandFlash memory: %s\r\n", (pBootCfg->ConfigFlags & TARGET_TYPE_NAND)?"Enabled":"Disabled");
        EdbgOutputDebugString ( "7) Program CS8900 MAC address (%B:%B:%B:%B:%B:%B)\r\n",
                               g_pBootCfg->EdbgAddr.wMAC[0] & 0x00FF, g_pBootCfg->EdbgAddr.wMAC[0] >> 8,
                               g_pBootCfg->EdbgAddr.wMAC[1] & 0x00FF, g_pBootCfg->EdbgAddr.wMAC[1] >> 8,
                               g_pBootCfg->EdbgAddr.wMAC[2] & 0x00FF, g_pBootCfg->EdbgAddr.wMAC[2] >> 8);
        EdbgOutputDebugString ( "8) KITL Configuration: %s\r\n", (g_pBootCfg->ConfigFlags & CONFIG_FLAGS_KITL) ? "ENABLED" : "DISABLED");
        EdbgOutputDebugString ( "9) Format Boot Media for BinFS\r\n");

        // N.B: we need this option here since BinFS is really a RAM image, where you "format" the media
        // with an MBR. There is no way to parse the image to say it's ment to be BinFS enabled.
        EdbgOutputDebugString ( "E) Erase Block 0 \r\n");
        EdbgOutputDebugString ( "B) Mark Bad Block 0 \r\n");
        EdbgOutputDebugString ( "F) Low-level format the Smart Media card\r\n");
        EdbgOutputDebugString ( "D) Download image now\r\n");
        EdbgOutputDebugString ( "L) LAUNCH existing Boot Media image\r\n");
		EdbgOutputDebugString ( "R) Read Configuration \r\n");
#if (USE_TRACE32_DOWNLOAD)
        EdbgOutputDebugString ( "T) DOWNLOAD image now(Trace32)\r\n");
#endif
        EdbgOutputDebugString ( "S) DOWNLOAD image now(SDMMC card)\r\n");
		EdbgOutputDebugString ( "W) Write Configuration Right Now\r\n");
        EdbgOutputDebugString ( "A) Erase All Blocks\r\n");
        EdbgOutputDebugString ( "N) Read NAND Flash (Block/Page) \r\n"); 
        EdbgOutputDebugString ( "\r\nEnter your selection: ");

        while (! ( ( (KeySelect >= '0') && (KeySelect <= '9') ) ||
                   ( (KeySelect == 'A') || (KeySelect == 'a') ) ||
                   ( (KeySelect == 'C') || (KeySelect == 'c') ) ||
                   ( (KeySelect == 'D') || (KeySelect == 'd') ) ||
                   ( (KeySelect == 'E') || (KeySelect == 'e') ) ||
                   ( (KeySelect == 'B') || (KeySelect == 'b') ) ||
                   ( (KeySelect == 'F') || (KeySelect == 'f') ) ||
                   ( (KeySelect == 'L') || (KeySelect == 'l') ) ||
                   ( (KeySelect == 'R') || (KeySelect == 'r') ) ||
#if (USE_TRACE32_DOWNLOAD)
                   ( (KeySelect == 'T') || (KeySelect == 't') ) ||
#endif
#if 0                   
                   ( (KeySelect == 'M') || (KeySelect == 'm') ) ||
                   ( (KeySelect == 'N') || (KeySelect == 'n') ) ||
#endif                   
                   ( (KeySelect == 'S') || (KeySelect == 's') ) ||
                   ( (KeySelect == 'W') || (KeySelect == 'w') ) ||
                   ( (KeySelect == 'X') || (KeySelect == 'x') ) ))
        {
            KeySelect = OEMReadDebugByte();
        }

        EdbgOutputDebugString ( "%c\r\n", KeySelect);

        switch(KeySelect)
        {
        case '0':           // Change IP address.
            SetIP(pBootCfg);
            pBootCfg->ConfigFlags &= ~CONFIG_FLAGS_DHCP;   // clear DHCP flag
            bConfigChanged = TRUE;
            break;
        case '1':           // Change subnet mask.
            SetMask(pBootCfg);
            bConfigChanged = TRUE;
            break;
        case '2':           // Toggle static/DHCP mode.
            pBootCfg->ConfigFlags = (pBootCfg->ConfigFlags ^ CONFIG_FLAGS_DHCP);
            bConfigChanged = TRUE;
            break;
        case '3':           // Change autoboot delay.
            SetDelay(pBootCfg);
            bConfigChanged = TRUE;
            break;
        case '4':           // Reset the bootloader configuration to defaults.
            OALMSG(TRUE, (TEXT("Resetting default TOC...\r\n")));
            TOC_Init(DEFAULT_IMAGE_DESCRIPTOR, (IMAGE_TYPE_RAMIMAGE|IMAGE_TYPE_BINFS), 0, 0, 0);
            if ( !TOC_Write() ) {
                OALMSG(OAL_WARN, (TEXT("TOC_Write Failed!\r\n")));
            }
            OALMSG(TRUE, (TEXT("...TOC complete\r\n")));
            break;
        case '5':           // Toggle download/launch status.
            pBootCfg->ConfigFlags = (pBootCfg->ConfigFlags ^ BOOT_TYPE_DIRECT);
            bConfigChanged = TRUE;
            break;
        case '6':           // Toggle image storage to Smart Media.
            pBootCfg->ConfigFlags = (pBootCfg->ConfigFlags ^ TARGET_TYPE_NAND);
            bConfigChanged = TRUE;
            break;
        case '7':           // Configure Crystal CS8900 MAC address.
            SetCS8900MACAddress(pBootCfg);
            bConfigChanged = TRUE;
            break;
        case '8':           // Toggle KD
            g_pBootCfg->ConfigFlags = (g_pBootCfg->ConfigFlags ^ CONFIG_FLAGS_KITL);
            bConfigChanged = TRUE;
            continue;
            break;
        case '9':
            // format the boot media for BinFS
            // N.B: this does not destroy our OEM reserved sections (TOC, bootloaders, etc)
            if ( !g_bBootMediaExist ) {
				OALMSG(OAL_ERROR, (TEXT("ERROR: BootMonitor: boot media does not exist.\r\n")));
                continue;
            }
            // N.B: format offset by # of reserved blocks,
            // decrease the ttl # blocks available by that amount.
            if ( !BP_LowLevelFormat( g_dwImageStartBlock,
                                     wNUM_BLOCKS - g_dwImageStartBlock,
                                     FORMAT_SKIP_BLOCK_CHECK) )
            {
                OALMSG(OAL_ERROR, (TEXT("ERROR: BootMonitor: Low-level boot media format failed.\r\n")));
                continue;
            }
            break;
        case 'A':
        case 'a':
                OALMSG(TRUE, (TEXT("All block Erase...\r\n")));
                for (i = 0; i < wNUM_BLOCKS; i++) {
                    FMD_EraseBlock(i);
                }
        	break;
		case 'C':
            if(SetDeviceID(pBootCfg))
				TOC_Write();
			//bConfigChanged = TRUE;
	    	//TOC_Write();
			break;
#if 0     	
        case 'M':
        case 'm':
            {
                #define READ_LB_NAND_M_SIZE 2048
                #define READ_LB_NAND_S_SIZE 64
                #define READ_SB_NAND_M_SIZE 512
                #define READ_SB_NAND_S_SIZE 16
 
                UINT8  pMBuf[READ_LB_NAND_M_SIZE], pSBuf[READ_LB_NAND_S_SIZE];
                UINT32 rslt;
                UINT32 nMainloop, nSpareloop;
 
                rslt = SetBlockPage();
                OALMSG(TRUE, (TEXT("\r\n rlst : 0x%x\r\n"), rslt));
                for (i=0;i<512;i++)
                {
                memset(pMBuf+i, (UINT8)0x0, 1);
            	}
                for (i=512;i<1024;i++)
                {
                memset(pMBuf+i, (UINT8)0xff, 1);
            	}
                for (i=1024;i<1536;i++)
                {
                memset(pMBuf+i, (UINT8)0x5, 1);
            	}
                for (i=1536;i<2048;i++)
                {
                memset(pMBuf+i, (UINT8)i, 1);
            	}
                for (i=0;i<64;i++)
                {
                memset(pSBuf+i, (UINT8)i, 1);
            	}
 
                //if (IS_LB)
                if (true)  //c ksk
                {
                    nMainloop  = READ_LB_NAND_M_SIZE;
                    nSpareloop = READ_LB_NAND_S_SIZE;
 
					FMD_LB_WriteSector(rslt, pMBuf, pSBuf, 1, 0);
                }
                else
                {
                    nMainloop  = READ_SB_NAND_M_SIZE;
                    nSpareloop = READ_SB_NAND_S_SIZE;
 
                    //RAW_SB_ReadPage(rslt, pMBuf, pSBuf);
                }
 
                PrintPageData(nMainloop, nSpareloop, pMBuf, pSBuf);
            }
            break;
        case 'N':
        case 'n':
            {
                #define READ_LB_NAND_M_SIZE 2048
                #define READ_LB_NAND_S_SIZE 64
                #define READ_SB_NAND_M_SIZE 512
                #define READ_SB_NAND_S_SIZE 16
 
                UINT8  pMBuf[READ_LB_NAND_M_SIZE], pSBuf[READ_LB_NAND_S_SIZE];
                UINT32 rslt;
                UINT32 nMainloop, nSpareloop;
 
                rslt = SetBlockPage();
                OALMSG(TRUE, (TEXT("\r\n rlst : 0x%x\r\n"), rslt));
                memset(pMBuf, 0xff, READ_LB_NAND_M_SIZE);
                memset(pSBuf, 0xff, READ_LB_NAND_S_SIZE);
 
                if (IS_LB)
                //if (true)  //c ksk
                {
                    nMainloop  = READ_LB_NAND_M_SIZE;
                    nSpareloop = READ_LB_NAND_S_SIZE;

                    FMD_ReadSector(rslt, pMBuf, pSBuf, 1);
                    //RAW_LB_ReadSector(rslt, pMBuf, pSBuf);
					//FMD_LB_ReadSector(rslt, pMBuf, pSBuf, 1, 0);
                }
                else
                {
                    nMainloop  = READ_SB_NAND_M_SIZE;
                    nSpareloop = READ_SB_NAND_S_SIZE;
                    FMD_SB_ReadSector_Test(rslt, pMBuf, pSBuf, 1, 0);
                    //FMD_ReadSector(rslt, pMBuf, pSBuf, 1);
                    //FMD_SB_ReadSector(rslt, pMBuf, pSBuf, 1, 0);
                    //RAW_SB_ReadPage(rslt, pMBuf, pSBuf);
                }

                PrintPageData(nMainloop, nSpareloop, pMBuf, pSBuf);
            }
            break;
#endif            
        case 'F':
        case 'f':
            // low-level format
            // N.B: this erases images, BinFs, FATFS, user data, etc.
            // However, we don't format Bootloaders & TOC bolcks; use JTAG for this.
            if ( !g_bBootMediaExist ) {
				OALMSG(OAL_ERROR, (TEXT("ERROR: BootMonitor: boot media does not exist.\r\n")));
                continue;
            } else {
                #ifdef IROMBOOT
                ReserveLoaderArea();
                
                #else
                DWORD i;
                SectorInfo si;

                // to keep bootpart off of our reserved blocks we must mark it as bad, reserved & read-only
                si.bOEMReserved = OEM_BLOCK_RESERVED | OEM_BLOCK_READONLY;
                si.bBadBlock    = BADBLOCKMARK;
                si.dwReserved1  = 0xffffffff;
                si.wReserved2   = 0xffff;

                OALMSG(TRUE, (TEXT("Reserving Blocks [0x%x - 0x%x] ...\r\n"), 0, IMAGE_START_BLOCK-1));
                for (i = 0; i < (DWORD)IMAGE_START_SECTOR; i++) {
                    FMD_WriteSector(i, NULL, &si, 1);
                }
                OALMSG(TRUE, (TEXT("...reserve complete.\r\n")));

                #endif  // !IROMBOOT

                OALMSG(TRUE, (TEXT("Low-level format Blocks [0x%x - 0x%x] ...\r\n"), IMAGE_START_BLOCK, wNUM_BLOCKS-1));
                for (i = IMAGE_START_BLOCK; i < wNUM_BLOCKS; i++) {
                    FMD_EraseBlock(i);
                }
                OALMSG(TRUE, (TEXT("...erase complete.\r\n")));
            } break;
        case 'B':
        case 'b':
            // low-level format
            // N.B: this erases images, BinFs, FATFS, user data, etc.
            // However, we don't format Bootloaders & TOC bolcks; use JTAG for this.
            if ( !g_bBootMediaExist ) {
				OALMSG(OAL_ERROR, (TEXT("ERROR: BootMonitor: boot media does not exist.\r\n")));
                continue;
            } else {
                #ifdef IROMBOOT
                ReserveLoaderArea();

                #else
                DWORD i;
                SectorInfo si;

                // to keep bootpart off of our reserved blocks we must mark it as bad, reserved & read-only
                si.bOEMReserved = OEM_BLOCK_RESERVED | OEM_BLOCK_READONLY;
                si.bBadBlock    = BADBLOCKMARK;
                si.dwReserved1  = 0xffffffff;
                si.wReserved2   = 0xffff;

                OALMSG(TRUE, (TEXT("Reserving Blocks [0x%x - 0x%x] ...\r\n"), 0, IMAGE_START_BLOCK-1));
                for (i = 0; i < (DWORD)IMAGE_START_SECTOR; i++) {
                    FMD_WriteSector(i, NULL, &si, 1);
                }
                OALMSG(TRUE, (TEXT("...reserve complete.\r\n")));

                #endif  // !IROMBOOT
            } break;
        case 'E':
        case 'e':
            // low-level format
            // N.B: this erases images, BinFs, FATFS, user data, etc.
            // However, we don't format Bootloaders & TOC bolcks; use JTAG for this.
            if ( !g_bBootMediaExist ) {
				OALMSG(OAL_ERROR, (TEXT("ERROR: BootMonitor: boot media does not exist.\r\n")));
                continue;
            } else {
                OALMSG(TRUE, (TEXT("Low-level format Blocks [0x%x - 0x%x] ...\r\n"), 0, 0));
                for (i = 0; i < 1; i++) {
                    FMD_EraseBlock(i);
                }
                OALMSG(TRUE, (TEXT("...erase complete.\r\n")));
            } break;
        case 'D':           // Download? Yes.
        case 'd':
            bDownload = TRUE;
            goto MENU_DONE;
        case 'L':           // Download? No.
        case 'l':
            bDownload = FALSE;
            goto MENU_DONE;
        case 'R':
        case 'r':
			TOC_Read();
			TOC_Print();
            // TODO
            break;

#if (USE_TRACE32_DOWNLOAD)
        case 'T':           // Download? No.
        case 't':
            bConfigChanged = TRUE;
			g_bUSBDownload = FALSE;
            bDownload = TRUE;
            g_bT32Download = TRUE;  // jylee
            goto MENU_DONE;
#endif
		case 'S':           // Download? No.
        case 's':
            //bConfigChanged = TRUE;  // mask for using MLC type NAND flash device because of writing TOC frequently
			g_bSDDownload = TRUE;
			g_bUSBDownload = FALSE;
            bDownload = TRUE;
            goto MENU_DONE;
        case 'W':           // Configuration Write
        case 'w':
            if (!TOC_Write())
            {
                OALMSG(OAL_WARN, (TEXT("WARNING: MainMenu: Failed to store updated eboot configuration in flash.\r\n")));
            }
            else
            {
                OALMSG(OAL_INFO, (TEXT("Successfully Written\r\n")));
                bConfigChanged = FALSE;
            }
            break;
        default:
            break;
        }
    }

MENU_DONE:
    OALMSG(1,(TEXT("bConfigChanged %d\r\n"),bConfigChanged));
    // If eboot settings were changed by user, save them to flash.
    //
    if (bConfigChanged)
    {
	if(!TOC_Write())
        OALMSG(OAL_WARN, (TEXT("WARNING: MainMenu: Failed to store updated bootloader configuration to flash.\r\n")));
    }

    return(bDownload);
}


/*
    @func   BOOL | OEMPlatformInit | Initialize the Samsung SMD2440 platform hardware.
    @rdesc  TRUE = Success, FALSE = Failure.
    @comm
    @xref
*/
BOOL OEMPlatformInit(void)
{
    ULONG BootDelay;
    UINT8 KeySelect;
    UINT32 dwStartTime, dwPrevTime, dwCurrTime;
    BOOLEAN bResult = FALSE;
    FlashInfo flashInfo;

    // Check if Current ARM speed is not matched to Target Arm speed
    // then To get speed up, set Voltage
#if (APLL_CLK == CLK_1332MHz)
    LTC3714_Init();
    LTC3714_VoltageSet(1,1200,100);     // ARM
    LTC3714_VoltageSet(2,1300,100);     // INT
#elif (APLL_CLK == CLK_800MHz)
    LTC3714_Init();
    LTC3714_VoltageSet(1,1300,100);     // ARM
    LTC3714_VoltageSet(2,1200,100);     // INT
#endif

    OALMSG(OAL_FUNC, (TEXT("+OEMPlatformInit.\r\n")));

    EdbgOutputDebugString("<><>Microsoft Windows CE Bootloader for the Samsung SMDK6410 Version %d.%d Built %s\r\n\r\n",
                          EBOOT_VERSION_MAJOR, EBOOT_VERSION_MINOR, __DATE__);
	// Initialize the display.
	InitializeDisplay();

    // Initialize the BSP args structure.
    //
    memset(pBSPArgs, 0, sizeof(BSP_ARGS));
    pBSPArgs->header.signature       = OAL_ARGS_SIGNATURE;
    pBSPArgs->header.oalVersion      = OAL_ARGS_VERSION;
    pBSPArgs->header.bspVersion      = BSP_ARGS_VERSION;
    pBSPArgs->kitl.flags             = 0;
    pBSPArgs->kitl.devLoc.IfcType    = Internal;
    pBSPArgs->kitl.devLoc.BusNumber  = 0;
    pBSPArgs->kitl.devLoc.LogicalLoc = BSP_BASE_REG_PA_CS8900A_IOBASE;

    g_dwImageStartBlock = IMAGE_START_BLOCK;
	Isr_Init();
#if 1
	OALMSG(TRUE, (TEXT("HSMMC init\r\n")));
	HSMMCInit();
    // Try to initialize the boot media block driver and BinFS partition.
    //
    OALMSG(TRUE, (TEXT("BP_Init\r\n")));
  if ( !BP_Init((LPBYTE)BINFS_RAM_START, BINFS_RAM_LENGTH, NULL, NULL, NULL) )
    {
        OALMSG(OAL_WARN, (TEXT("WARNING: OEMPlatformInit failed to initialize Boot Media.\r\n")));
        g_bBootMediaExist = FALSE;
    }
    else
        g_bBootMediaExist = TRUE;

    // Get flash info
    if (!FMD_GetInfo(&flashInfo)) {
        OALMSG(OAL_ERROR, (L"ERROR: BLFlashDownload: "
            L"FMD_GetInfo call failed\r\n"
        ));
    }
	wNUM_BLOCKS = flashInfo.dwNumBlocks;
	RETAILMSG(1, (TEXT("wNUM_BLOCKS : %d(0x%x) \r\n"), wNUM_BLOCKS, wNUM_BLOCKS));
	stDeviceInfo = GetNandInfo();
	
//    OALMSG(TRUE, (TEXT("TOC_Read\r\n")));
    // Try to retrieve TOC (and Boot config) from boot media
    //
    if ( !TOC_Read( ) )
    {
        // use default settings
        TOC_Init(DEFAULT_IMAGE_DESCRIPTOR, (IMAGE_TYPE_RAMIMAGE), 0, 0, 0);
    }
#else

        g_bBootMediaExist = TRUE;
        TOC_Init(DEFAULT_IMAGE_DESCRIPTOR, (IMAGE_TYPE_RAMIMAGE), 0, 0, 0);

#endif

    // Display boot message - user can halt the autoboot by pressing any key on the serial terminal emulator.
    //
    BootDelay = g_pBootCfg->BootDelay;

	if(g_pBootCfg->ConfigFlags & CONFIG_FLAGS_KITL)
			pBSPArgs->kitl.flags = OAL_KITL_FLAGS_ENABLED | OAL_KITL_FLAGS_VMINI;
    RETAILMSG(1,(TEXT("\n\nFlags= %x, kitl.flags= %x \r\n"),g_pBootCfg->ConfigFlags,pBSPArgs->kitl.flags));
    if (g_pBootCfg->ConfigFlags & BOOT_TYPE_DIRECT)
    {
        OALMSG(TRUE, (TEXT("Press [ENTER] to launch image stored on boot media, or [SPACE] to enter boot monitor.\r\n")));
        OALMSG(TRUE, (TEXT("\r\nInitiating image launch in %d seconds. "),BootDelay--));
    }
    else
    {
        OALMSG(TRUE, (TEXT("Press [ENTER] to download image stored on boot media, or [SPACE] to enter boot monitor.\r\n")));
        OALMSG(TRUE, (TEXT("\r\nInitiating image download in %d seconds. "),BootDelay--));
    }

    dwStartTime = OEMEthGetSecs();
    dwPrevTime  = dwStartTime;
    dwCurrTime  = dwStartTime;
    KeySelect   = 0;

#if 1
    // Allow the user to break into the bootloader menu.
    //
    while((dwCurrTime - dwStartTime) < g_pBootCfg->BootDelay)
    {
        KeySelect = OEMReadDebugByte();
        if ((KeySelect == 0x20) || (KeySelect == 0x0d))
            break;
        dwCurrTime = OEMEthGetSecs();

        if (dwCurrTime > dwPrevTime)
        {
            int i = 0, j = 0;

            // 1 Second has elapsed - update the countdown timer.
            dwPrevTime = dwCurrTime;
            if (BootDelay < 9)
                i = 11;
            else if (BootDelay < 99)
                i = 12;
            else if (BootDelay < 999)
                i = 13;

            for(j = 0; j < i; j++)
                OEMWriteDebugByte((BYTE)0x08); // print back space
            EdbgOutputDebugString ( "%d seconds. ", BootDelay--);
        }
    }
	OALMSG(OAL_INFO, (TEXT("\r\n")));
#else
	KeySelect = 0x20;
#endif

    // Boot or enter bootloader menu.
    //
    switch(KeySelect)
    {
    case 0x20: // Boot menu.
        g_bDownloadImage = MainMenu(g_pBootCfg);
        break;
    case 0x00: // Fall through if no keys were pressed -or-
    case 0x0d: // the user cancelled the countdown.
    default:
		OALMSG(TRUE, (TEXT("\r\nStarting auto-download ... \r\n")));
        g_bDownloadImage = TRUE;
        g_bAutoDownload = TRUE;

		// erase all block 080828 hsjang
		{
			DWORD i;
			
	        OALMSG(TRUE, (TEXT("All block(%d) Erase...\r\n"), wNUM_BLOCKS));
	        for (i = 0; i < wNUM_BLOCKS; i++) {
	            FMD_EraseBlock(i);
	        }		
		}
        break;

    }

    if ( !g_bDownloadImage )
    {
        // User doesn't want to download image - load it from the boot media.
        // We could read an entire nk.bin or nk.nb0 into ram and jump.
        if ( !VALID_TOC(g_pTOC) ) {
			OALMSG(OAL_ERROR, (TEXT("OEMPlatformInit: ERROR_INVALID_TOC, can not autoboot.\r\n")));
            return FALSE;
        }
	}

    

    bResult = TRUE;

CleanUp:

    OALMSG(OAL_FUNC, (TEXT("_OEMPlatformInit.\r\n")));
    return(bResult);
}


/*
    @func   DWORD | OEMPreDownload | Complete pre-download tasks - get IP address, initialize TFTP, etc.
    @rdesc  BL_DOWNLOAD = Platform Builder is asking us to download an image, BL_JUMP = Platform Builder is requesting we jump to an existing image, BL_ERROR = Failure.
    @comm
    @xref
*/
DWORD OEMPreDownload(void)
{
    BOOL  bGotJump = TRUE;

	if ( (g_bDownloadImage != FALSE) && (g_bAutoDownload == FALSE) )
	{	
		OALMSG(TRUE, (TEXT("Please choose the Image on SD.\r\n")));
		bGotJump = FALSE;
		ChooseImageFromSD();
	}

    return(bGotJump ? BL_JUMP : BL_DOWNLOAD);
}


/*
    @func   BOOL | OEMReadData | Generically read download data (abstracts actual transport read call).
    @rdesc  TRUE = Success, FALSE = Failure.
    @comm
    @xref
*/
BOOL OEMReadData(DWORD dwData, PUCHAR pData)
{
	BOOL ret;
	//DWORD i;
   	OALMSG(OAL_FUNC, (TEXT("+OEMReadData.\r\n")));
	//OALMSG(TRUE, (TEXT("\r\nINFO: dwData = 0x%x, pData = 0x%x \r\n"), dwData, pData));

    if ( g_bAutoDownload == TRUE )
    {
		ret = SDReadData(dwData, pData);        
    }
	else if ( g_bUSBDownload == FALSE && g_bSDDownload == FALSE  )
	{
		ret = EbootEtherReadData(dwData, pData);
	}
	else if ( g_bSDDownload == TRUE )
	{
		// TODO :: This function have to be implemented.
		ret = SDReadData(dwData, pData);
	}
	return(ret);
}

void OEMReadDebugString(CHAR * szString)
{
    USHORT cwNumChars = 0;
    USHORT InChar = 0;

    while(!((InChar == 0x0d) || (InChar == 0x0a)))
    {
        InChar = OEMReadDebugByte();
        if (InChar != OEM_DEBUG_COM_ERROR && InChar != OEM_DEBUG_READ_NODATA)
        {
            if ((InChar >= 'a' && InChar <='z') || (InChar >= 'A' && InChar <= 'Z') || (InChar >= '0' && InChar <= '9'))
            {
                if (cwNumChars < 16)
                {
                    szString[cwNumChars++] = (char)InChar;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
            // If it's a backspace, back up.
            //
            else if (InChar == 8)
            {
                if (cwNumChars > 0)
                {
                    cwNumChars--;
                    OEMWriteDebugByte((BYTE)InChar);
                }
            }
        }
    }

    // If it's a carriage return with an empty string, don't change anything.
    //
    if (cwNumChars)
    {
        szString[cwNumChars] = '\0';
        EdbgOutputDebugString("\r\n");
    }
}


/*
    @func   void | OEMShowProgress | Displays download progress for the user.
    @rdesc  N/A.
    @comm
    @xref
*/
void OEMShowProgress(DWORD dwPacketNum)
{
    OALMSG(OAL_FUNC, (TEXT("+OEMShowProgress.\r\n")));
}


/*
    @func   void | OEMLaunch | Executes the stored/downloaded image.
    @rdesc  N/A.
    @comm
    @xref
*/
void OEMLaunch( DWORD dwImageStart, DWORD dwImageLength, DWORD dwLaunchAddr, const ROMHDR *pRomHdr )
{
    DWORD dwPhysLaunchAddr;

    OALMSG(1, (TEXT("+OEMLaunch.\r\n")));

    // If the user requested that a disk image (stored in RAM now) be written to the SmartMedia card, so it now.
    //
	if (g_bDownloadImage) // && (g_pBootCfg->ConfigFlags & TARGET_TYPE_NAND))
    {
        // Since this platform only supports RAM images, the image cache address is the same as the image RAM address.
        //

        switch (g_ImageType)
        {
        	case IMAGE_TYPE_STEPLDR:
		        if (!WriteRawImageToBootMedia(dwImageStart, dwImageLength, dwLaunchAddr))
		        {
            		OALMSG(OAL_ERROR, (TEXT("ERROR: OEMLaunch: Failed to store image to Smart Media.\r\n")));
            		goto CleanUp;
        		}
                #ifdef IROMBOOT
                ReserveLoaderArea();
                
                #else
				{
					DWORD i;
					SectorInfo si;

					// to keep bootpart off of our reserved blocks we must mark it as bad, reserved & read-only
					si.bOEMReserved = OEM_BLOCK_RESERVED | OEM_BLOCK_READONLY;
					si.bBadBlock    = BADBLOCKMARK;
//					si.bBadBlock    = 0xff;
					si.dwReserved1  = 0xffffffff;
					si.wReserved2   = 0xffff;

					OALMSG(TRUE, (TEXT("Reserving Blocks [0x%x - 0x%x] ...\r\n"), 0, IMAGE_START_BLOCK-1));
					for (i = 0; i < (DWORD)IMAGE_START_SECTOR; i++) {
						FMD_WriteSector(i, NULL, &si, 1);
					}
					OALMSG(TRUE, (TEXT("...reserve complete.\r\n")));
        		}

                #endif  // !IROMBOOT
		        OALMSG(TRUE, (TEXT("INFO: Step loader image stored to Smart Media.  Please Reboot.  Halting...\r\n")));
	            if (g_bAutoDownload == TRUE)
	            {
	                return ;
	            }				
	        	while(1)
	        	{
            		// Wait...
	        	}
        		break;

            case IMAGE_TYPE_LOADER:
				g_pTOC->id[0].dwLoadAddress = dwImageStart;
				g_pTOC->id[0].dwTtlSectors = FILE_TO_SECTOR_SIZE(dwImageLength);
		        if (!WriteRawImageToBootMedia(dwImageStart, dwImageLength, dwLaunchAddr))
		        {
            		OALMSG(OAL_ERROR, (TEXT("ERROR: OEMLaunch: Failed to store image to Smart Media.\r\n")));
            		goto CleanUp;
        		}
				if (dwLaunchAddr && (g_pTOC->id[0].dwJumpAddress != dwLaunchAddr))
				{
					g_pTOC->id[0].dwJumpAddress = dwLaunchAddr;
					if ( !TOC_Write() ) {
	            		EdbgOutputDebugString("*** OEMLaunch ERROR: TOC_Write failed! Next boot may not load from disk *** \r\n");
					}
	        		TOC_Print();
				}
                #ifdef IROMBOOT
                ReserveLoaderArea();
                
                #endif
		        OALMSG(TRUE, (TEXT("INFO: Eboot image stored to Smart Media.  Please Reboot.  Halting...\r\n")));
	            if (g_bAutoDownload == TRUE)
	            {
	                return ;
	            }				
		        while(1)
		        {
            		// Wait...
        		}

                break;

			case IMAGE_TYPE_RAMIMAGE:
				break;

			case IMAGE_TYPE_FLASHBIN:
		        if (!WriteRawImageToBootMedia(dwImageStart, dwImageLength, dwLaunchAddr))
			//if (!WriteOSImageToBootMedia(dwImageStart, dwImageLength, dwLaunchAddr))

				{
            		OALMSG(OAL_ERROR, (TEXT("ERROR: OEMLaunch: Failed to store image to Smart Media.\r\n")));
            		goto CleanUp;
        		}
        		break;


			default:
				break;
        }
    }


	if (BLFlashDownload() != BL_JUMP) {
		OALMSG(OAL_ERROR, (L"ERROR: OEMLaunch: "
			L"Image load from flash memory failed\r\n"
		));
		goto CleanUp;
	}

    OALMSG(1, (TEXT("waitforconnect\r\n")));
	/*
    // Wait for Platform Builder to connect after the download and send us IP and port settings for service
    // connections - also sends us KITL flags.  This information is used later by the OS (KITL).
    //
    EDBG_ADDR EshellHostAddr;
    EDBG_OS_CONFIG_DATA *pCfgData;

    if (g_bDownloadImage)
    {
        memset(&EshellHostAddr, 0, sizeof(EDBG_ADDR));

        g_DeviceAddr.dwIP  = pBSPArgs->kitl.ipAddress;
        memcpy(g_DeviceAddr.wMAC, pBSPArgs->kitl.mac, (3 * sizeof(UINT16)));
        g_DeviceAddr.wPort = 0;

        if (!(pCfgData = EbootWaitForHostConnect(&g_DeviceAddr, &EshellHostAddr)))
        {
            OALMSG(OAL_ERROR, (TEXT("ERROR: OEMLaunch: EbootWaitForHostConnect failed.\r\n")));
            goto CleanUp;
        }

        // If the user selected "passive" KITL (i.e., don't connect to the target at boot time), set the
        // flag in the args structure so the OS image can honor it when it boots.
        //
        if (pCfgData->KitlTransport & KTS_PASSIVE_MODE)
        {
            pBSPArgs->kitl.flags |= OAL_KITL_FLAGS_PASSIVE;
        }
	}
    */
    // If a launch address was provided, we must have downloaded the image, save the address in case we
    // want to jump to this image next time.  If no launch address was provided, retrieve the last one.
    //
	if (dwLaunchAddr && (g_pTOC->id[g_dwTocEntry].dwJumpAddress != dwLaunchAddr))
	{
		g_pTOC->id[g_dwTocEntry].dwJumpAddress = dwLaunchAddr;
	}
	else
	{
		dwLaunchAddr= g_pTOC->id[g_dwTocEntry].dwJumpAddress;
		OALMSG(OAL_INFO, (TEXT("INFO: using TOC[%d] dwJumpAddress: 0x%x\r\n"), g_dwTocEntry, dwLaunchAddr));
	}

    // Jump to downloaded image (use the physical address since we'll be turning the MMU off)...
    //
    dwPhysLaunchAddr = (DWORD)OALVAtoPA((void *)dwLaunchAddr);
    OALMSG(TRUE, (TEXT("INFO: OEMLaunch: Jumping to Physical Address 0x%Xh (Virtual Address 0x%Xh)...\r\n\r\n\r\n"), dwPhysLaunchAddr, dwLaunchAddr));

    // Jump...
    //
    Launch(dwPhysLaunchAddr);


CleanUp:

    OALMSG(TRUE, (TEXT("ERROR: OEMLaunch: Halting...\r\n")));
    SpinForever();
}


//------------------------------------------------------------------------------
//
//  Function Name:  OEMVerifyMemory( DWORD dwStartAddr, DWORD dwLength )
//  Description..:  This function verifies the passed address range lies
//                  within a valid region of memory. Additionally this function
//                  sets the g_ImageType if the image is a boot loader.
//  Inputs.......:  DWORD           Memory start address
//                  DWORD           Memory length
//  Outputs......:  BOOL - true if verified, false otherwise
//
//------------------------------------------------------------------------------

BOOL OEMVerifyMemory( DWORD dwStartAddr, DWORD dwLength )
{
	DWORD mStartAddr = 0;
	
    OALMSG(OAL_FUNC, (TEXT("+OEMVerifyMemory.\r\n")));
    OALMSG(TRUE, (TEXT("dwStartAddr = 0x%x \r\n"), dwStartAddr));
    OALMSG(TRUE, (TEXT("dwLength = 0x%x \r\n"), dwLength));

    // Is the image being downloaded the stepldr?
    mStartAddr = STEPLDR_RAM_IMAGE_BASE;
	if ((dwStartAddr >= mStartAddr) &&
        ((dwStartAddr + dwLength - 1) < (STEPLDR_RAM_IMAGE_BASE + STEPLDR_RAM_IMAGE_SIZE)))
    {
        OALMSG(OAL_INFO, (TEXT("Stepldr image\r\n")));
        g_ImageType = IMAGE_TYPE_STEPLDR;     // Stepldr image.
        return TRUE;
    }
    // Is the image being downloaded the superipl?
    mStartAddr = SUPERIPL_RAM_IMAGE_BASE;
	if ((dwStartAddr >= mStartAddr) &&
        ((dwStartAddr + dwLength - 1) < (SUPERIPL_RAM_IMAGE_BASE + SUPERIPL_RAM_IMAGE_SIZE)))
    {
        OALMSG(OAL_INFO, (TEXT("SuperIpl image\r\n")));
        g_ImageType = IMAGE_TYPE_STEPLDR;     // Stepldr image.
        return TRUE;
    }
    // Is the image being downloaded the dio.nb0?
//	else if (dwStartAddr == 0)
//    {
//        OALMSG(OAL_INFO, (TEXT("dio.nb0 image\r\n")));
//        g_ImageType = IMAGE_TYPE_DIONB0;     // dio.nb0 image.
//        return TRUE;
//    }
	// Is the image being downloaded the stepldr?
//	else if ((dwStartAddr >= UPLDR_RAM_IMAGE_BASE) &&
//		((dwStartAddr + dwLength - 1) < (UPLDR_RAM_IMAGE_BASE + UPLDR_RAM_IMAGE_SIZE)))
//	{
//		OALMSG(OAL_INFO, (TEXT("upldr image\r\n")));
//		g_ImageType = IMAGE_TYPE_UPLDR;     // Stepldr image.
//		return TRUE;
//	}
	// Is the image being downloaded the bootloader?
	else if ((dwStartAddr >= EBOOT_RAM_IMAGE_BASE) &&
		((dwStartAddr + dwLength - 1) < (EBOOT_RAM_IMAGE_BASE + EBOOT_RAM_IMAGE_SIZE)))
	{
		OALMSG(OAL_INFO, (TEXT("Eboot image\r\n")));
		g_ImageType = IMAGE_TYPE_LOADER;     // Eboot image.
		return TRUE;
	}
	// Is it a dio image?
//	else if ((dwStartAddr >= NAND_ROM_IMAGE_BASE) &&
//		((dwStartAddr + dwLength - 1) < (NAND_ROM_IMAGE_BASE + NAND_ROM_IMAGE_SIZE)))
//	{
//		OALMSG(OAL_INFO, (TEXT("DIO image\r\n")));
//		g_ImageType = IMAGE_TYPE_DIO;     // dio image.
//		return TRUE;
//	}
	// Is it a ram image?
//	else if ((dwStartAddr >= ROM_RAMIMAGE_START) &&
//		((dwStartAddr + dwLength - 1) < (ROM_RAMIMAGE_START + ROM_RAMIMAGE_SIZE)))
//	{
//		OALMSG(OAL_INFO, (TEXT("RAM image\r\n")));
//		g_ImageType = IMAGE_TYPE_RAMIMAGE;
//		return TRUE;
//	}
	else if (!dwStartAddr && !dwLength)
	{
		OALMSG(TRUE, (TEXT("Don't support raw image\r\n")));
		g_ImageType = IMAGE_TYPE_RAWBIN;
		return FALSE;
	}
	else
	{
		OALMSG(TRUE, (TEXT("Flash bin image\r\n")));
		g_ImageType = IMAGE_TYPE_FLASHBIN;
		return TRUE;
	}

    // HACKHACK: get around MXIP images with funky addresses
    OALMSG(TRUE, (TEXT("BIN image type unknow\r\n")));

    OALMSG(OAL_FUNC, (TEXT("_OEMVerifyMemory.\r\n")));

    return FALSE;
}

/*
    @func   void | OEMMultiBINNotify | Called by blcommon to nofity the OEM code of the number, size, and location of one or more BIN regions,
                                       this routine collects the information and uses it when temporarily caching a flash image in RAM prior to final storage.
    @rdesc  N/A.
    @comm
    @xref
*/
void OEMMultiBINNotify(const PMultiBINInfo pInfo)
{
    BYTE nCount;
	DWORD g_dwMinImageStart;

    OALMSG(OAL_FUNC, (TEXT("+OEMMultiBINNotify.\r\n")));

    if (!pInfo || !pInfo->dwNumRegions)
    {
        OALMSG(OAL_WARN, (TEXT("WARNING: OEMMultiBINNotify: Invalid BIN region descriptor(s).\r\n")));
        return;
    }

	if (!pInfo->Region[0].dwRegionStart && !pInfo->Region[0].dwRegionLength)
	{
    	return;
	}

    g_dwMinImageStart = pInfo->Region[0].dwRegionStart;

    OALMSG(TRUE, (TEXT("\r\nDownload BIN file information:\r\n")));
    OALMSG(TRUE, (TEXT("-----------------------------------------------------\r\n")));
    for (nCount = 0 ; nCount < pInfo->dwNumRegions ; nCount++)
    {
        OALMSG(TRUE, (TEXT("[%d]: Base Address=0x%x  Length=0x%x\r\n"),
            nCount, pInfo->Region[nCount].dwRegionStart, pInfo->Region[nCount].dwRegionLength));
        if (pInfo->Region[nCount].dwRegionStart < g_dwMinImageStart)
        {
            g_dwMinImageStart = pInfo->Region[nCount].dwRegionStart;
            if (g_dwMinImageStart == 0)
            {
                OALMSG(OAL_WARN, (TEXT("WARNING: OEMMultiBINNotify: Bad start address for region (%d).\r\n"), nCount));
                return;
            }
        }
    }

    memcpy((LPBYTE)&g_BINRegionInfo, (LPBYTE)pInfo, sizeof(MultiBINInfo));

    OALMSG(TRUE, (TEXT("-----------------------------------------------------\r\n")));
    OALMSG(OAL_FUNC, (TEXT("_OEMMultiBINNotify.\r\n")));
}

/////////////////////// START - Stubbed functions - START //////////////////////////////
/*
    @func   void | SC_WriteDebugLED | Write to debug LED.
    @rdesc  N/A.
    @comm
    @xref
*/

void SC_WriteDebugLED(USHORT wIndex, ULONG dwPattern)
{
    // Stub - needed by NE2000 EDBG driver...
    //
}


ULONG HalSetBusDataByOffset(IN BUS_DATA_TYPE BusDataType,
                            IN ULONG BusNumber,
                            IN ULONG SlotNumber,
                            IN PVOID Buffer,
                            IN ULONG Offset,
                            IN ULONG Length)
{
    return(0);
}


ULONG
HalGetBusDataByOffset(IN BUS_DATA_TYPE BusDataType,
                      IN ULONG BusNumber,
                      IN ULONG SlotNumber,
                      IN PVOID Buffer,
                      IN ULONG Offset,
                      IN ULONG Length)
{
    return(0);
}


BOOLEAN HalTranslateBusAddress(IN INTERFACE_TYPE  InterfaceType,
                               IN ULONG BusNumber,
                               IN PHYSICAL_ADDRESS BusAddress,
                               IN OUT PULONG AddressSpace,
                               OUT PPHYSICAL_ADDRESS TranslatedAddress)
{

    // All accesses on this platform are memory accesses...
    //
    if (AddressSpace)
        *AddressSpace = 0;

    // 1:1 mapping...
    //
    if (TranslatedAddress)
    {
        *TranslatedAddress = BusAddress;
        return(TRUE);
    }

    return(FALSE);
}


PVOID MmMapIoSpace(IN PHYSICAL_ADDRESS PhysicalAddress,
                   IN ULONG NumberOfBytes,
                   IN BOOLEAN CacheEnable)
{
    DWORD dwAddr = PhysicalAddress.LowPart;

    if (CacheEnable)
        dwAddr &= ~CACHED_TO_UNCACHED_OFFSET;
    else
        dwAddr |= CACHED_TO_UNCACHED_OFFSET;

    return((PVOID)dwAddr);
}


VOID MmUnmapIoSpace(IN PVOID BaseAddress,
                    IN ULONG NumberOfBytes)
{
}

VOID WINAPI SetLastError(DWORD dwErrCode)
{
}
/////////////////////// END - Stubbed functions - END //////////////////////////////


/*
    @func   PVOID | GetKernelExtPointer | Locates the kernel region's extension area pointer.
    @rdesc  Pointer to the kernel's extension area.
    @comm
    @xref
*/
PVOID GetKernelExtPointer(DWORD dwRegionStart, DWORD dwRegionLength)
{
    DWORD dwCacheAddress = 0;
    ROMHDR *pROMHeader;
    DWORD dwNumModules = 0;
    TOCentry *pTOC;

    if (dwRegionStart == 0 || dwRegionLength == 0)
        return(NULL);

    if (*(LPDWORD) OEMMapMemAddr (dwRegionStart, dwRegionStart + ROM_SIGNATURE_OFFSET) != ROM_SIGNATURE)
        return NULL;

    // A pointer to the ROMHDR structure lives just past the ROM_SIGNATURE (which is a longword value).  Note that
    // this pointer is remapped since it might be a flash address (image destined for flash), but is actually cached
    // in RAM.
    //
    dwCacheAddress = *(LPDWORD) OEMMapMemAddr (dwRegionStart, dwRegionStart + ROM_SIGNATURE_OFFSET + sizeof(ULONG));
    pROMHeader     = (ROMHDR *) OEMMapMemAddr (dwRegionStart, dwCacheAddress);

    // Make sure sure are some modules in the table of contents.
    //
    if ((dwNumModules = pROMHeader->nummods) == 0)
        return NULL;

    // Locate the table of contents and search for the kernel executable and the TOC immediately follows the ROMHDR.
    //
    pTOC = (TOCentry *)(pROMHeader + 1);

    while(dwNumModules--) {
        LPBYTE pFileName = OEMMapMemAddr(dwRegionStart, (DWORD)pTOC->lpszFileName);
        if (!strcmp((const char *)pFileName, "nk.exe")) {
            return ((PVOID)(pROMHeader->pExtensions));
        }
        ++pTOC;
    }
    return NULL;
}


/*
    @func   BOOL | OEMDebugInit | Initializes the serial port for debug output message.
    @rdesc  TRUE == Success and FALSE == Failure.
    @comm
    @xref
*/
BOOL OEMDebugInit(void)
{

    // Set up function callbacks used by blcommon.
    //
    g_pOEMVerifyMemory   = OEMVerifyMemory;      // Verify RAM.
    g_pOEMMultiBINNotify = OEMMultiBINNotify;

    // Call serial initialization routine (shared with the OAL).
    //
    OEMInitDebugSerial();

    return(TRUE);
}

static void InitializeDisplay(void)
{
	tDevInfo RGBDevInfo;

	volatile S3C6410_GPIO_REG *pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	volatile S3C6410_DISPLAY_REG *pDispReg = (S3C6410_DISPLAY_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_DISPLAY, FALSE);
	volatile S3C6410_SPI_REG *pSPIReg = (S3C6410_SPI_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SPI0, FALSE);
	volatile S3C6410_MSMIF_REG *pMSMIFReg = (S3C6410_MSMIF_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_MSMIF_SFR, FALSE);

	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	EdbgOutputDebugString("[Eboot] ++InitializeDisplay()\r\n");
	// Initialize Display Power Gating
	if(!(pSysConReg->BLK_PWR_STAT & (1<<4))) {
		pSysConReg->NORMAL_CFG |= (1<<14);
		while(!(pSysConReg->BLK_PWR_STAT & (1<<4)));
		}

	// Initialize Virtual Address
	LDI_initialize_register_address((void *)pSPIReg, (void *)pDispReg, (void *)pGPIOReg);
	Disp_initialize_register_address((void *)pDispReg, (void *)pMSMIFReg, (void *)pGPIOReg);

	// Set LCD Module Type
#if		(SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
	LDI_set_LCD_module_type(LDI_LTS222QV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_LTV350)
	LDI_set_LCD_module_type(LDI_LTV350QV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_LTE480)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_D1)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_QV)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_PQV)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_ML)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_MP)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_LTP700)
	LDI_set_LCD_module_type(LDI_LTP700WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_QC430)
	LDI_set_LCD_module_type(LDI_QC430QV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB)
	LDI_set_LCD_module_type(LDI_ZQ65_RGB);
#else
	EdbgOutputDebugString("[Eboot:ERR] InitializeDisplay() : Unknown Module Type [%d]\r\n", SMDK6410_LCD_MODULE);
#endif

	// Get RGB Interface Information from LDI Library
	LDI_fill_output_device_information(&RGBDevInfo);
	// Setup Output Device Information
	Disp_set_output_device_information(&RGBDevInfo);

	// Initialize Display Controller
	Disp_initialize_output_interface(DISP_VIDOUT_RGBIF);

#if		(LCD_BPP == 16)
	Disp_set_window_mode(DISP_WIN1_DMA, DISP_16BPP_565, LCD_WIDTH, LCD_HEIGHT, 0, 0);
#elif	(LCD_BPP == 32)	// XRGB format (RGB888)
	Disp_set_window_mode(DISP_WIN1_DMA, DISP_24BPP_888, LCD_WIDTH, LCD_HEIGHT, 0, 0);
#else
	EdbgOutputDebugString("[Eboot:ERR] InitializeDisplay() : Unknown Color Depth %d bpp\r\n", LCD_BPP);
#endif

	Disp_set_framebuffer(DISP_WIN1, IMAGE_FRAMEBUFFER_PA_START);
	Disp_window_onfoff(DISP_WIN1, DISP_WINDOW_ON);

#if	(SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
	// This type of LCD need MSM I/F Bypass Mode to be Disabled
	pMSMIFReg->MIFPCON &= ~(0x1<<3);	// SEL_BYPASS -> Normal Mode
#endif

	// Initialize LCD Module
	LDI_initialize_LCD_module();

	// Video Output Enable
	Disp_envid_onoff(DISP_ENVID_ON);

	// Fill Framebuffer
#if	(SMDK6410_LCD_MODULE == LCD_MODULE_LTV350 /*|| SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB*/)
	memcpy((void *)IMAGE_FRAMEBUFFER_UA_START, (void *)/*InitialImage_rgb16_320x240*/ScreenBitmap, 320*240*2);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMULQV)
	memcpy((void *)IMAGE_FRAMEBUFFER_UA_START, (void *)InitialImage_rgb16_320x240, 320*240*2);
#elif	(LCD_BPP == 16)
	{
		int i;
		unsigned short *pFB,*plogo;
		pFB = (unsigned short *)IMAGE_FRAMEBUFFER_UA_START;
        	//plogo = (unsigned short *)Logo;

		for (i=0; i<LCD_WIDTH*LCD_HEIGHT; i++)
			*pFB++ = 0x001f;//*plogo++;//0x001F;		// Blue
	}
#elif	(LCD_BPP == 32)
	{
		int i;
		unsigned int *pFB;
		pFB = (unsigned int *)IMAGE_FRAMEBUFFER_UA_START;

		for (i=0; i<LCD_WIDTH*LCD_HEIGHT; i++)
			*pFB++ = 0x000000FF;		// Blue
	}
#endif

	// TODO:
	// Backlight Power On
	//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin (GPF[15])
	pGPIOReg->GPFDAT |= (1<<15);
	pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<30)) | (1<<30);	// set GPF[15] as Output

	EdbgOutputDebugString("[Eboot] --InitializeDisplay()\r\n");
}

