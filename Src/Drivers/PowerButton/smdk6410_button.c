#include <windows.h>
#include <bsp_cfg.h>
#include <s3c6410.h>
#include "smdk6410_button.h"

#ifdef	REMOVE_BEFORE_RELEASE
#define BTN_MSG(x)
#define BTN_INF(x)
#define BTN_ERR(x)	RETAILMSG(TRUE, x)
#else
//#define BTN_MSG(x)	RETAILMSG(TRUE, x)
#define BTN_MSG(x)
#define BTN_INF(x)	RETAILMSG(TRUE, x)
//#define BTN_INF(...)
#define BTN_ERR(x)	RETAILMSG(TRUE, x)
//#define BTN_ERR(x)
#endif

static volatile S3C6410_GPIO_REG *g_pGPIOReg = NULL;

BOOL Button_initialize_register_address(void *pGPIOReg)
{
	BTN_MSG((_T("[BTN]++Button_initialize_register_address(0x%08x)\n\r"), pGPIOReg));

	if (pGPIOReg == NULL)
	{
		BTN_ERR((_T("[BTN:ERR] Button_initialize_register_address() : NULL pointer parameter\n\r")));
		return FALSE;
	}
	else
	{
		g_pGPIOReg = (S3C6410_GPIO_REG *)pGPIOReg;
		BTN_INF((_T("[BTN:INF] g_pGPIOReg    = 0x%08x\n\r"), g_pGPIOReg));
	}

	BTN_MSG((_T("[BTN]--Button_initialize_register_address()\n\r")));

	return TRUE;
}

void Button_port_initialize(void)
{
	BTN_MSG((_T("[BTN] Button_port_initialize()\n\r")));

	// Power Button -> GPN[11] : EINT11
	// Reset Button -> GPN[9] : EINT9 / ADDR_CF[1]

	g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & ~(/*(0x3<<18)|*/(0x3<<22))) | (/*(0x2<<18)|*/(0x2<<22));	// GPN[9] to EINT9, GPN[11] to EINT11
	g_pGPIOReg->GPNPUD = (g_pGPIOReg->GPNPUD & ~(/*(0x3<<18)|*/(0x3<<22))) | (/*(0x2<<18)|*/(0x2<<22));	// GPN[9], GPN[11] set Pull-up Enable
}

BOOL Button_pwrbtn_set_interrupt_method(EINT_SIGNAL_METHOD eMethod)
{
	BOOL bRet =TRUE;

	BTN_MSG((_T("[BTN] Button_pwrbtn_set_interrupt_method(%d)\n\r"), eMethod));

	// Power Button -> GPN[11] : EINT11

	switch(eMethod)
	{
	case EINT_SIGNAL_LOW_LEVEL:
		g_pGPIOReg->EINT0CON0 &= ~(0x7<<20);
		break;
	case EINT_SIGNAL_HIGH_LEVEL:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<20)) | (0x1<<20);
		break;
	case EINT_SIGNAL_FALL_EDGE:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<20)) | (0x2<<20);
		break;
	case EINT_SIGNAL_RISE_EDGE:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<20)) | (0x4<<20);
		break;
	case EINT_SIGNAL_BOTH_EDGE:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<20)) | (0x6<<20);
		break;
	default:
		BTN_ERR((_T("[BTN:ERR] Button_pwrbtn_set_interrupt_method() : Unknown Method = %d\n\r"), eMethod));
		bRet = FALSE;
		break;
	}

	BTN_MSG((_T("[BTN] --Button_pwrbtn_set_interrupt_method() = %d\n\r"), bRet));

	return bRet;
}

BOOL Button_pwrbtn_set_filter_method(EINT_FILTER_METHOD eMethod, unsigned int uiFilterWidth)
{
	BOOL bRet =TRUE;

	BTN_MSG((_T("[BTN] Button_pwrbtn_set_filter_method(%d, %d)\n\r"), eMethod, uiFilterWidth));

	// Power Button -> GPN[11] : EINT11

	switch(eMethod)
	{
	case EINT_FILTER_DISABLE:
		g_pGPIOReg->EINT0FLTCON1 &= ~(0x1<<15);
		break;
	case EINT_FILTER_DELAY:
		g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0x3<<14)) | (0x1<<15);
		break;
	case EINT_FILTER_DIGITAL:
		g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0xff<<8)) | ((0x3<<14) | ((uiFilterWidth&0x3f)<<8));
		break;
	default:
		BTN_ERR((_T("[BTN:ERR] Button_pwrbtn_set_filter_method() : Unknown Method = %d\n\r"), eMethod));
		bRet = FALSE;
		break;
	}

	BTN_MSG((_T("[BTN] --Button_pwrbtn_set_filter_method() = %d\n\r"), bRet));

	return bRet;
}

void Button_pwrbtn_enable_interrupt(void)
{
	BTN_MSG((_T("[BTN] Button_pwrbtn_enable_interrupt()\n\r")));
	// Power Button -> GPN[11] : EINT11

	g_pGPIOReg->EINT0MASK &= ~(0x1<<11);	// Unmask EINT11
}

void Button_pwrbtn_disable_interrupt(void)
{
	BTN_MSG((_T("[BTN] Button_pwrbtn_disable_interrupt()\n\r")));
	// Power Button -> GPN[11] : EINT11

	g_pGPIOReg->EINT0MASK |= (0x1<<11);	// Mask EINT11
}

void Button_pwrbtn_clear_interrupt_pending(void)
{
	BTN_MSG((_T("[BTN] Button_pwrbtn_clear_interrupt_pending()\n\r")));
	// Power Button -> GPN[11] : EINT11

	g_pGPIOReg->EINT0PEND = (0x1<<11);		// Clear pending EINT11
}

BOOL Button_pwrbtn_is_pushed(void)
{
	BTN_MSG((_T("[BTN] Button_pwrbtn_is_pushed()\n\r")));

	// Power Button -> GPN[11] : EINT11

	if (g_pGPIOReg->GPNDAT & (0x1<<11))		// We can read GPDAT pin level when configured as EINT
	{
		return FALSE;	// Low Active Switch (Pull-up switch)
	}
	else
	{
		return TRUE;
	}
}

BOOL Button_rstbtn_set_interrupt_method(EINT_SIGNAL_METHOD eMethod)
{
	BOOL bRet =TRUE;

	BTN_MSG((_T("[BTN] Button_rstbtn_set_interrupt_method(%d)\n\r"), eMethod));

	// Reset Button -> GPN[9] : EINT9

	switch(eMethod)
	{
	case EINT_SIGNAL_LOW_LEVEL:
		g_pGPIOReg->EINT0CON0 &= ~(0x7<<16);
		break;
	case EINT_SIGNAL_HIGH_LEVEL:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<16)) | (0x1<<16);
		break;
	case EINT_SIGNAL_FALL_EDGE:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<16)) | (0x2<<16);
		break;
	case EINT_SIGNAL_RISE_EDGE:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<16)) | (0x4<<16);
		break;
	case EINT_SIGNAL_BOTH_EDGE:
		g_pGPIOReg->EINT0CON0 = (g_pGPIOReg->EINT0CON0 & ~(0x7<<16)) | (0x6<<16);
		break;
	default:
		BTN_ERR((_T("[BTN:ERR] Button_rstbtn_set_interrupt_method() : Unknown Method = %d\n\r"), eMethod));
		bRet = FALSE;
		break;
	}

	BTN_MSG((_T("[BTN] --Button_rstbtn_set_interrupt_method() = %d\n\r"), bRet));

	return bRet;
}

BOOL Button_rstbtn_set_filter_method(EINT_FILTER_METHOD eMethod, unsigned int uiFilterWidth)
{
	BOOL bRet =TRUE;

	BTN_MSG((_T("[BTN] Button_rstbtn_set_filter_method(%d, %d)\n\r"), eMethod, uiFilterWidth));

	// Reset Button -> GPN[9] : EINT9

	switch(eMethod)
	{
	case EINT_FILTER_DISABLE:
		g_pGPIOReg->EINT0FLTCON1 &= ~(0x1<<6);
		break;
	case EINT_FILTER_DELAY:
		g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~(0x3<<6)) | (0x1<<7);
		break;
	case EINT_FILTER_DIGITAL:
		g_pGPIOReg->EINT0FLTCON1 = (g_pGPIOReg->EINT0FLTCON1 & ~0xff) | ((0x3<<6) | (uiFilterWidth&0x3f));
		break;
	default:
		BTN_ERR((_T("[BTN:ERR] Button_rstbtn_set_filter_method() : Unknown Method = %d\n\r"), eMethod));
		bRet = FALSE;
		break;
	}

	BTN_MSG((_T("[BTN] --Button_rstbtn_set_filter_method() = %d\n\r"), bRet));

	return bRet;
}

void Button_rstbtn_enable_interrupt(void)
{
	BTN_MSG((_T("[BTN] Button_rstbtn_enable_interrupt()\n\r")));
	// Reset Button -> GPN[9] : EINT9

	g_pGPIOReg->EINT0MASK &= ~(0x1<<9);	// Unmask EINT9
}

void Button_rstbtn_disable_interrupt(void)
{
	BTN_MSG((_T("[BTN] Button_rstbtn_disable_interrupt()\n\r")));
	// Reset Button -> GPN[9] : EINT9

	g_pGPIOReg->EINT0MASK |= (0x1<<9);		// Mask EINT9
}

void Button_rstbtn_clear_interrupt_pending(void)
{
	BTN_MSG((_T("[BTN] Button_rstbtn_clear_interrupt_pending()\n\r")));
	// Reset Button -> GPN[9] : EINT9

	g_pGPIOReg->EINT0PEND = (0x1<<9);		// Clear pending EINT9
}

BOOL Button_rstbtn_is_pushed(void)
{
	BTN_MSG((_T("[BTN] Button_rstbtn_is_pushed()\n\r")));
	// Reset Button -> GPN[9] : EINT9

	if (g_pGPIOReg->GPNDAT & (0x1<<9))		// We can read GPDAT pin level when configured as EINT
	{
		return FALSE;	// Low Active Switch (Pull-up switch)
	}
	else
	{
		return TRUE;
	}
}

