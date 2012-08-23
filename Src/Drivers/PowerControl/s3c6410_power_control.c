#include <windows.h>
#include <bsp.h>
#include "s3c6410_power_control.h"

#define PWRCON_MSG(x)
#define PWRCON_INF(x)
#define PWRCON_ERR(x)	RETAILMSG(TRUE, x)

static volatile S3C6410_SYSCON_REG *g_pSysConReg = NULL;

BOOL PwrCon_initialize_register_address(void *pSysConReg)
{
	PWRCON_MSG((_T("[PWRCON]++PwrCon_initialize_register_address(0x%08x)\n\r"), pSysConReg));

	if (pSysConReg == NULL)
	{
		PWRCON_ERR((_T("[PWRCON:ERR] PwrCon_initialize_register_address() : NULL pointer parameter\n\r")));
		return FALSE;
	}
	else
	{
		g_pSysConReg = (S3C6410_SYSCON_REG *)pSysConReg;
		PWRCON_INF((_T("[PWRCON:INF] g_pSysConReg    = 0x%08x\n\r"), g_pSysConReg));
	}

	PWRCON_MSG((_T("[PWRCON]--PwrCon_initialize_register_address()\n\r")));

	return TRUE;
}

BOOL PwrCon_set_block_power_on(BLKPWR_DOMAIN eDomain)
{
    UINT32 TimeOut;
	PWRCON_MSG((_T("[PWRCON]++PwrCon_set_block_power_on(%d)\n\r"), eDomain));

    TimeOut = 10000000L;   // This is big value.

	switch(eDomain)
	{
	case BLKPWR_DOMAIN_IROM:	// Internal ROM
		g_pSysConReg->NORMAL_CFG |= (1<<30);
		break;
	case BLKPWR_DOMAIN_ETM:	// ETM
		g_pSysConReg->NORMAL_CFG |= (1<<16);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<6)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;
		break;
	case BLKPWR_DOMAIN_S:		// SDMA, Security System
		g_pSysConReg->NORMAL_CFG |= (1<<15);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<5)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;
		break;
	case BLKPWR_DOMAIN_F:		// Display Controller, Post Processor, Rotator
		g_pSysConReg->NORMAL_CFG |= (1<<14);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<4)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;
		break;
	case BLKPWR_DOMAIN_P:		// FIMG-2D, TV Encoder, TV Scaler
		g_pSysConReg->NORMAL_CFG |= (1<<13);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<3)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;
		break;
	case BLKPWR_DOMAIN_I:		// Camera interface, Jpeg
		g_pSysConReg->NORMAL_CFG |= (1<<12);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<2)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;        
		break;
	case BLKPWR_DOMAIN_G:		// 3D
		g_pSysConReg->NORMAL_CFG |= (1<<10);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<7)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;        
		break;
	case BLKPWR_DOMAIN_V:		// MFC
		g_pSysConReg->NORMAL_CFG |= (1<<9);
        while(!(g_pSysConReg->BLK_PWR_STAT & (1<<1)) || (TimeOut-- == 0));
        if(TimeOut == 0) goto Block_Pwr_ON_FAIL;        
		break;
	default:
		PWRCON_ERR((_T("[PWRCON:ERR] PwrCon_set_block_power_on() : Unknown Domain = %d\n\r"), eDomain));
		return FALSE;
	}

	PWRCON_MSG((_T("[PWRCON]--PwrCon_set_block_power_on()\n\r")));

	return TRUE;

Block_Pwr_ON_FAIL:
    PWRCON_ERR((_T("[PWRCON:ERR] %s() : Time OUT!! Domain = %d\n\r"), _T(__FUNCTION__), eDomain));
    
    return FALSE;
}

BOOL PwrCon_set_block_power_off(BLKPWR_DOMAIN eDomain)
{
	PWRCON_MSG((_T("[PWRCON]++PwrCon_set_block_power_off(%d)\n\r"), eDomain));

	switch(eDomain)
	{
	case BLKPWR_DOMAIN_IROM:	// Internal ROM
		g_pSysConReg->NORMAL_CFG &= ~(1<<30);
		break;
	case BLKPWR_DOMAIN_ETM:	// ETM
		g_pSysConReg->NORMAL_CFG &= ~(1<<16);
		break;
	case BLKPWR_DOMAIN_S:		// SDMA, Security System
		g_pSysConReg->NORMAL_CFG &= ~(1<<15);
		break;
	case BLKPWR_DOMAIN_F:		// Display Controller, Post Processor, Rotator
		g_pSysConReg->NORMAL_CFG &= ~(1<<14);
		break;
	case BLKPWR_DOMAIN_P:		// FIMG-2D, TV Encoder, TV Scaler
		g_pSysConReg->NORMAL_CFG &= ~(1<<13);
		break;
	case BLKPWR_DOMAIN_I:		// Camera interface, Jpeg
		g_pSysConReg->NORMAL_CFG &= ~(1<<12);
		break;
	case BLKPWR_DOMAIN_G:		// 3D
		g_pSysConReg->NORMAL_CFG &= ~(1<<10);
		break;
	case BLKPWR_DOMAIN_V:		// MFC
		g_pSysConReg->NORMAL_CFG &= ~(1<<9);
		break;
	default:
		PWRCON_ERR((_T("[PWRCON:ERR] PwrCon_set_block_power_off() : Unknown Domain = %d\n\r"), eDomain));
		return FALSE;
	}

	PWRCON_MSG((_T("[PWRCON]--PwrCon_set_block_power_off()\n\r")));

	return TRUE;
}

void PwrCon_set_USB_phy(BOOL bOn)
{
	PWRCON_MSG((_T("[PWRCON]++PwrCon_set_USB_phy(%d)\n\r"), bOn));

	if (bOn)
	{
		g_pSysConReg->OTHERS |= (1<<16);
	}
	else
	{
		g_pSysConReg->OTHERS &= ~(1<<16);
	}

	PWRCON_MSG((_T("[PWRCON]--PwrCon_set_USB_phy()\n\r")));
}

RESET_STATUS PwrCon_get_reset_status(void)
{
	RESET_STATUS RstStatus = RST_UNKNOWN;

	switch(g_pSysConReg->RST_STAT)
	{
	case 0x01:
		PWRCON_INF((_T("[PWRCON:INF] PwrCon_get_reset_status() : H/W Reset [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		RstStatus = RST_HW_RESET;
		break;
	case 0x02:
		PWRCON_INF((_T("[PWRCON:INF] PwrCon_get_reset_status() : Warm Reset [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		RstStatus = RST_WARM_RESET;
		break;
	case 0x04:
		PWRCON_INF((_T("[PWRCON:INF] PwrCon_get_reset_status() : Watch Dog Timer Reset [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		RstStatus = RST_WDT_RESET;
		break;
	case 0x08:
		PWRCON_INF((_T("[PWRCON:INF] PwrCon_get_reset_status() : Sleep/Wakeup Reset [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		RstStatus = RST_SLEEP_WAKEUP;
		break;
	case 0x10:
		PWRCON_INF((_T("[PWRCON:INF] PwrCon_get_reset_status() : eSleep/Wakeup Reset [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		RstStatus = RST_ESLEEP_WAKEUP;
		break;
	case 0x20:
		PWRCON_INF((_T("[PWRCON:INF] PwrCon_get_reset_status() : S/W Reset [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		RstStatus = RST_SW_RESET;
		break;
	default:
		PWRCON_ERR((_T("[PWRCON:ERR] PwrCon_get_reset_status() : Unknown RST_STAT [0x%08x]\n\r"), g_pSysConReg->RST_STAT));
		break;
	}

	PWRCON_MSG((_T("[PWRCON] PwrCon_get_reset_status() = %d\n\r"), RstStatus));

	return RstStatus;
}

