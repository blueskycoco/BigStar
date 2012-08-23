/**************************************************************************************
* 
*	Project Name : IIC Driver 
*
*	Copyright 2006 by Samsung Electronics, Inc.
*	All rights reserved.
*
*	Project Description :
*		This software is MDD layer for IIC Samsung driver. 
*  
*--------------------------------------------------------------------------------------
* 
*	File Name : iic_mdd.cpp
*  
*	File Description : This file implements MDD layer functions which is stream driver.
*
*	Author : JeGeon.Jung
*	Dept. : AP Development Team
*	Created Date : 2007/06/11
*	Version : 0.1 
* 
*	History
*	- Created(JeGeon.Jung 2007/06/11)
*   - Add Power Management (JeGeon.Jung 2007/06/25)
*
*	Todo
*
*
*	Note
*
**************************************************************************************/

#include <windows.h>
#include <types.h>
#include <linklist.h>
#include <nkintr.h>
#include <devload.h>
#include <pm.h>
#include <pmplatform.h>

#include <iic_mdd.h>
#include <iic_pdd.h>


#define DEFAULT_CE_THREAD_PRIORITY 103

#define	MSG_ERROR		1
#define	MSG_FUNCTION	0
#define	MSG_INFO		0
#define	MSG_WARN		0


CEDEVICE_POWER_STATE    g_Dx;

// Define some internally used functions
BOOL IIC_Close(PHW_OPEN_INFO    pOpenContext);
BOOL IIC_Deinit(PHW_INIT_INFO pInitContext);
#if DEBUG
#define ZONE_ERROR          DEBUGZONE(0)
#define ZONE_WARN           DEBUGZONE(1)
#define ZONE_FUNCTION       DEBUGZONE(2)
#define ZONE_INIT           DEBUGZONE(3)
#define ZONE_INFO           DEBUGZONE(4)
#define ZONE_IST            DEBUGZONE(5)

DBGPARAM dpCurSettings =
{
    TEXT("IIC"),
    {
         TEXT("Errors"),TEXT("Warnings"),TEXT("Function"),TEXT("Init"),
         TEXT("Info"),TEXT("Ist"),TEXT("Undefined"),TEXT("Undefined"),
         TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),
         TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined"),TEXT("Undefined")
    },
        (1 << 0)   // Errors
    |   (1 << 1)   // Warnings
};
#endif

//////////
// Function Name : DllEntry
// Function Description : Process attach/detach api.
// Input : HINSTANCE   hinstDll, DWORD   dwReason, LPVOID  lpReserved
// Output : The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v1.0
BOOL
DllEntry(
              HINSTANCE   hinstDll,             /*Instance pointer. */
              DWORD   dwReason,                 /*Reason routine is called. */
              LPVOID  lpReserved                /*system parameter. */
              )
{
    if ( dwReason == DLL_PROCESS_ATTACH ) {
        DEBUGREGISTER(hinstDll);
        DEBUGMSG (ZONE_INIT, (TEXT("serial port process attach\r\n")));
        DisableThreadLibraryCalls((HMODULE) hinstDll);
    }

    if ( dwReason == DLL_PROCESS_DETACH ) {
        DEBUGMSG (ZONE_INIT, (TEXT("process detach called\r\n")));
    }

    return(TRUE);
}

//////////
// Function Name : IIC_Init
// Function Description : IIC device initialization.
// Input : LPCTSTR pContext
// Output : Returns a pointer to the serial head which is passed into
//                              the IIC_OPEN and IIC_DEINIT entry points as a device handle.
// Version : v0.5
HANDLE
IIC_Init(
 		LPCTSTR pContext						/* Pointer to a string containing the registry path.*/
        )
{
	PHW_INIT_INFO	pInitContext = NULL;
    HKEY            hKey;
    ULONG           datasize = sizeof(ULONG);    
    ULONG           kvaluetype;    
    
    RETAILMSG(MSG_FUNCTION,(TEXT("+IIC_Init\r\n")));

    // Allocate our control structure.
    pInitContext  =  (PHW_INIT_INFO)LocalAlloc(LPTR, sizeof(HW_INIT_INFO));

    // Check that LocalAlloc did stuff ok too.
    if ( !pInitContext ) {
        RETAILMSG(MSG_ERROR,
                 (TEXT("Error allocating memory for pInitContext, IIC_Init failed\n\r")));
        return(NULL);
    }
    //memset(pInitContext,0,sizeof(HW_INIT_INFO));


    // Initially, open list is empty.
    InitializeListHead( &pInitContext->OpenList );
    InitializeCriticalSection(&(pInitContext->OpenCS));


    /* Initialize the critical sections that will guard the parts of
     * the receive and transmit action.
     */
    InitializeCriticalSection(&(pInitContext->CritSec));
    
    pInitContext->pAccessOwner = NULL;
    
	

    /* Want to use the Identifier to do RegOpenKey and RegQueryValue (?)
     * to get the index to be passed to GetHWObj.
     * The HWObj will also have a flag denoting whether to start the
     * listening thread or provide the callback.
     */
    RETAILMSG(MSG_INFO, (TEXT("Try to open %s\r\n"), pContext));
    hKey = OpenDeviceKey(pContext);
    if ( !hKey ) {
        RETAILMSG (MSG_ERROR,
                  (TEXT("Failed to open devkeypath, IIC_Init failed\r\n")));
        IIC_Deinit(pInitContext);
        return(NULL);
    }

    datasize = sizeof(DWORD);
    if ( RegQueryValueEx(hKey, L"Priority256", NULL, &kvaluetype,
                         (LPBYTE)&pInitContext->Priority256, &datasize) ) {
        pInitContext->Priority256 = DEFAULT_CE_THREAD_PRIORITY;
        RETAILMSG (MSG_WARN,
                  (TEXT("Failed to get Priority256 value, defaulting to %d\r\n"), pInitContext->Priority256));
    }
    
    if ( RegQueryValueEx(hKey, L"SlaveAddress", NULL, &kvaluetype,
                         (LPBYTE)&pInitContext->PDDCommonVal.SlaveAddress, &datasize) ) {
        pInitContext->PDDCommonVal.SlaveAddress = DEFAULT_SLAVE_ADDRESS;
        RETAILMSG (MSG_WARN,
                  (TEXT("Failed to get SlaveAddress value, defaulting to %d\r\n"), pInitContext->PDDCommonVal.SlaveAddress));
    }
    
    if ( RegQueryValueEx(hKey, L"Mode", NULL, &kvaluetype,
                         (LPBYTE)&pInitContext->PDDCommonVal.InterruptEnable, &datasize) ) {
        pInitContext->PDDCommonVal.InterruptEnable = DEFAULT_INTERRUPT_ENABLE;
        RETAILMSG (MSG_WARN,
                  (TEXT("Failed to get InterruptEnable value, defaulting to %d\r\n"), pInitContext->PDDCommonVal.InterruptEnable));
    }        

    RegCloseKey (hKey);



	pInitContext->State = IIC_RUN;
    /* Check that HW_Init did stuff ok.  From here on out, call Deinit function
     * when things fail.
     */
    if ( !HW_Init(pInitContext) ) {
        RETAILMSG (MSG_ERROR,
                  (TEXT("Hardware doesn't init correctly, IIC_Init failed\r\n")));
        IIC_Deinit(pInitContext);
        return(NULL);
    } 

	g_Dx = D0;
	
    RETAILMSG(MSG_FUNCTION, (TEXT("-IIC_Init\r\n")));
    return(pInitContext);
}

//////////
// Function Name : IIC_Deinit
// Function Description : IIC device De-initialization.
// Input : PHW_INIT_INFO	pInitContext
// Output : The return is a BOOL, representing success (TRUE) or failure (FALSE).
// Version : v0.5
BOOL
IIC_Deinit(
 		PHW_INIT_INFO	pInitContext						/* Context pointer returned from IIC_Init*/
        )
{
    RETAILMSG(MSG_FUNCTION, (TEXT("+IIC_Deinit\r\n")));	
    
    if ( !pInitContext ) {
        /* Can't do much without this */
        RETAILMSG (MSG_ERROR,
                  (TEXT("IIC_Deinit can't find pInitContext\r\n")));
        SetLastError(ERROR_INVALID_HANDLE);
        return(FALSE);
    }    
    
    /*
    ** Call close, if we have a user.  Note that this call will ensure that
    ** all users are out of the serial routines before it returns, so we can
    ** go ahead and free our internal memory.
    */
    EnterCriticalSection(&(pInitContext->OpenCS));
    if ( pInitContext->OpenCnt ) {
        PLIST_ENTRY     pEntry;
        PHW_OPEN_INFO   pOpenContext;

        pEntry = pInitContext->OpenList.Flink;
        while ( pEntry != &pInitContext->OpenList ) {
            pOpenContext = CONTAINING_RECORD( pEntry, HW_OPEN_INFO, llist);
            pEntry = pEntry->Flink;  // advance to next

            RETAILMSG (MSG_INFO, (TEXT(" Deinit - Closing Handle 0x%X\r\n"),
                                               pOpenContext ));
            IIC_Close(pOpenContext);
        }
    }
    LeaveCriticalSection(&(pInitContext->OpenCS));    
    
    /* Free our resources */


    DeleteCriticalSection(&(pInitContext->CritSec));
    DeleteCriticalSection(&(pInitContext->OpenCS));
        
	pInitContext->State = IIC_FINISH;        
	/* Now, call HW specific deinit function */        
	HW_Deinit(pInitContext);
	        
    LocalFree(pInitContext);    
    
    RETAILMSG(MSG_FUNCTION, (TEXT("-IIC_Deinit\r\n")));
    return(TRUE);	
}

//////////
// Function Name : IIC_Open
// Function Description : IIC device initialization.
/*		This routine must be called by the user to open the
 *      IIC device. The HANDLE returned must be used by the application in
 *      all subsequent calls to the IIC driver.
 *      Exported to users.
 */
// Input : HANDLE  pHead, DWORD   AccessCode, DWORD   ShareMode
// Output : This routine returns a HANDLE representing the device.
// Version : v0.1
HANDLE
IIC_Open(
        HANDLE  pHead,          // Handle returned by IIC_Init.
        DWORD   AccessCode,     // access code.
        DWORD   ShareMode       // share mode - Not used in this driver.
        )
{
    PHW_INIT_INFO  	pInitContext = (PHW_INIT_INFO)pHead;
    PHW_OPEN_INFO   pOpenContext;	
    
    RETAILMSG (MSG_FUNCTION, (TEXT("+IIC_Open handle x%X, access x%X, share x%X\r\n"),
                                        pHead, AccessCode, ShareMode));   
                                        
    // Return NULL if pInitContext failed.
    if ( !pInitContext ) {
        RETAILMSG (MSG_ERROR,
                  (TEXT("Open attempted on uninited device!\r\n")));
        SetLastError(ERROR_INVALID_HANDLE);
        return(NULL);
    }   
    
    if (AccessCode & DEVACCESS_BUSNAMESPACE ) {
        AccessCode &=~(GENERIC_READ |GENERIC_WRITE|GENERIC_EXECUTE|GENERIC_ALL);
    }
    
    // This driver has no access permission.
    /*
    // Return NULL if opening with access & someone else already has
    if ( (AccessCode & (GENERIC_READ | GENERIC_WRITE)) &&
         pInitContext->pAccessOwner ) {
        RETAILMSG (MSG_ERROR,
                  (TEXT("Open requested access %x, handle x%X already has x%X!\r\n"),
                   AccessCode, pInitContext->pAccessOwner,
                   pInitContext->pAccessOwner->AccessCode));
        SetLastError(ERROR_INVALID_ACCESS);
        return(NULL);
    }                       */
    
    // OK, lets allocate an open structure
    pOpenContext    =  (PHW_OPEN_INFO)LocalAlloc(LPTR, sizeof(HW_OPEN_INFO));
    if ( !pOpenContext ) {
        RETAILMSG (MSG_ERROR,
                 (TEXT("Error allocating memory for pOpenContext, IIC_Open failed\n\r")));
        return(NULL);
    }           
    
    // Init the structure
    pOpenContext->pInitContext = pInitContext;  // pointer back to our parent
    pOpenContext->StructUsers = 0;
    pOpenContext->AccessCode = AccessCode;
    pOpenContext->ShareMode = ShareMode;

    // if we have access permissions, note it in pInitContext
    if ( AccessCode & (GENERIC_READ | GENERIC_WRITE) ) {
        RETAILMSG(MSG_INFO,
                 (TEXT("IIC_Open: Access permission handle granted x%X\n\r"),
                  pOpenContext));
        pInitContext->pAccessOwner = pOpenContext;
    }
    
    // add this open entry to list of open entries.
    // Note that we hold the open CS for the duration of the routine since
    // all of our state info is in flux during this time.  In particular,
    // without the CS is would be possible for an open & close to be going on
    // simultaneously and have bad things happen like spinning a new event
    // thread before the old one was gone, etc.
    EnterCriticalSection(&(pInitContext->OpenCS));
    InsertHeadList(&pInitContext->OpenList,
                   &pOpenContext->llist);

    // We do special for Power Manger and Device Manager.
    if (pOpenContext->AccessCode &  DEVACCESS_BUSNAMESPACE ) {
        // OK, We do not need initialize pSerailHead and start any thread. return the handle now.
        LeaveCriticalSection(&(pInitContext->OpenCS));
        RETAILMSG(MSG_FUNCTION, (TEXT("-IIC_Open handle x%X, x%X, Ref x%X\r\n"),
                                        pOpenContext, pOpenContext->pInitContext, pInitContext->OpenCnt));
        return(pOpenContext);

    }
    
    if ( ! pInitContext->OpenCnt ) {
        RETAILMSG(MSG_INFO,
                 (TEXT("IIC_Open: First open : Do Init x%X\n\r"),
                  pOpenContext));
                  
		if ( !HW_OpenFirst(pOpenContext) ) {
            RETAILMSG (MSG_ERROR, (TEXT("HW Open First failed.\r\n")));
            goto OpenFail;
        }                  
        
        HW_PowerUp(pInitContext);
    }   
    
	if ( !HW_Open(pOpenContext) ) {
	    RETAILMSG (MSG_ERROR, (TEXT("HW Open failed.\r\n")));
	    goto OpenFail;
	}     

    ++(pInitContext->OpenCnt);

    // OK, we are finally back in a stable state.  Release the CS.
    LeaveCriticalSection(&(pInitContext->OpenCS));    
    
    
    RETAILMSG(MSG_FUNCTION, (TEXT("-IIC_Open handle x%X, x%X, Ref x%X\r\n"),
                                        pOpenContext, pOpenContext->pInitContext, pInitContext->OpenCnt));

    return(pOpenContext); 
    

OpenFail :
    RETAILMSG(MSG_FUNCTION, (TEXT("-IIC_Open handle x%X, x%X, Ref x%X\r\n"),
                                        NULL, pOpenContext->pInitContext, pInitContext->OpenCnt));

    SetLastError(ERROR_OPEN_FAILED);

    // If this was the handle with access permission, remove pointer
    if ( pOpenContext == pInitContext->pAccessOwner )
        pInitContext->pAccessOwner = NULL;

    // Remove the Open entry from the linked list
    RemoveEntryList(&pOpenContext->llist);

    // OK, everything is stable so release the critical section
    LeaveCriticalSection(&(pInitContext->OpenCS));

    // Free all data allocated in open
    LocalFree( pOpenContext );

    return(NULL);                       
}


//////////
// Function Name : IIC_Close
// Function Description : close the IIC device.
// Input : PHW_OPEN_INFO  pOpenContext
// Output : TRUE if success; FALSE if failure.
// Note : This routine is called by the device manager to close the device.
// Version : v0.5
BOOL
IIC_Close(PHW_OPEN_INFO pOpenContext)	//Context pointer returned from IIC_Open
{
    PHW_INIT_INFO  pInitContext = pOpenContext->pInitContext;

    BOOL            RetCode = TRUE;

    RETAILMSG (MSG_FUNCTION, (TEXT("+IIC_Close\r\n")));

    if ( !pInitContext ) {
        RETAILMSG (MSG_ERROR, (TEXT("!!IIC_Close: pInitContext == NULL!!\r\n")));
        SetLastError(ERROR_INVALID_HANDLE);
        return(FALSE);
    }

    // Use the OpenCS to make sure we don't collide with an in-progress open.
    EnterCriticalSection(&(pInitContext->OpenCS));
    // We do special for Power Manger and Device Manager.
    if (pOpenContext->AccessCode & DEVACCESS_BUSNAMESPACE) {


        // Remove the entry from the linked list
        RemoveEntryList(&pOpenContext->llist);

        LocalFree( pOpenContext );
    }
    else
    if ( pInitContext->OpenCnt ) {
        --(pInitContext->OpenCnt);

	    if ( ! pInitContext->OpenCnt ) {
	        RETAILMSG(MSG_INFO,
	                 (TEXT("IIC_Close: Last Close : Do Clost x%X\n\r"),
	                  pOpenContext));
	                  
			if ( !HW_CloseLast(pOpenContext) ) {
	            RETAILMSG (MSG_ERROR, (TEXT("HW_CloseLast failed.\r\n")));
	        }        
	        
	        HW_PowerDown(pInitContext);          
	    }   

        // If this was the handle with access permission, remove pointer
        if ( pOpenContext == pInitContext->pAccessOwner ) {
            RETAILMSG(MSG_INFO,
                     (TEXT("IIC_Close: Closed access owner handle\n\r"),
                      pOpenContext));

            pInitContext->pAccessOwner = NULL;
        }

        // Remove the entry from the linked list
        RemoveEntryList(&pOpenContext->llist);

        // Free all data allocated in open
        LocalFree( pOpenContext );
    } else {
        RETAILMSG (MSG_ERROR, (TEXT("!!Close of non-open serial port\r\n")));
        SetLastError(ERROR_INVALID_HANDLE);
        RetCode = FALSE;
    }

    // OK, other inits/opens can go ahead.
    LeaveCriticalSection(&(pInitContext->OpenCS));

    RETAILMSG (MSG_FUNCTION, (TEXT("-IIC_Close\r\n")));
    return(RetCode);
}


//////////
// Function Name : IIC_IOControl
// Function Description : Device IO control routine.
// Input : //      	DWORD | pOpenContext | value returned from IIC_Open call
//     				DWORD | dwCode | io control code to be performed
//      			PBYTE | pBufIn | input data to the device
//      			DWORD | dwLenIn | number of bytes being passed in
//      			PBYTE | pBufOut | output data from the device
//      			DWORD | dwLenOut |maximum number of bytes to receive from device
//      			PDWORD | pdwActualOut | actual number of bytes received from device
// Output : TRUE if success; FALSE if failure.
// Note : 
// Version : v0.1


BOOL
IIC_IOControl(PHW_OPEN_INFO pOpenContext,
              DWORD dwCode, PBYTE pBufIn,
              DWORD dwLenIn, PBYTE pBufOut, DWORD dwLenOut,
              PDWORD pdwActualOut)
{
    BOOL            RetVal           = TRUE;        // Initialize to success
    PHW_INIT_INFO  	pInitContext; //= pOpenContext->pInitContext;

    if (pOpenContext==NULL) {
        SetLastError (ERROR_INVALID_HANDLE);
        return(FALSE);
    }
    pInitContext = pOpenContext->pInitContext;
    if ( pInitContext ) {

    } else {
        SetLastError (ERROR_INVALID_HANDLE);
        return(FALSE);
    }

    RETAILMSG (MSG_FUNCTION,
              (TEXT("+IIC_IOControl(0x%X, %d, 0x%X, %d, 0x%X, %d, 0x%X)\r\n"),
               pOpenContext, dwCode, pBufIn, dwLenIn, pBufOut,
               dwLenOut, pdwActualOut));
   
    if ( !pInitContext->OpenCnt ) {
        RETAILMSG (MSG_ERROR,
                  (TEXT(" IIC_IOControl - device was closed\r\n")));
        SetLastError (ERROR_INVALID_HANDLE);
        return(FALSE);
    }   
    
    // Make sure the caller has access permissions
    // NOTE : Pay attention here.  I hate to make this check repeatedly
    // below, so I'll optimize it here.  But as you add new ioctl's be
    // sure to account for them in this if check.
    if ( !( (dwCode == IOCTL_IIC_READ) ||
            (dwCode == IOCTL_IIC_WRITE) ||
            (dwCode == IOCTL_IIC_SET_CLOCK) ||
            (dwCode == IOCTL_IIC_GET_CLOCK) ||
            (dwCode == IOCTL_IIC_SET_MODE) ||
            (dwCode == IOCTL_IIC_GET_MODE) ||
            (dwCode == IOCTL_IIC_SET_FILTER) ||
            (dwCode == IOCTL_IIC_GET_FILTER) ||   
            (dwCode == IOCTL_IIC_SET_DELAY) ||
            (dwCode == IOCTL_IIC_GET_DELAY) ||                                       
            (dwCode == IOCTL_POWER_CAPABILITIES) ||
            (dwCode == IOCTL_POWER_QUERY) ||
            (dwCode == IOCTL_POWER_SET)) ) {
        // If not one of the above operations, then read or write
        // access permissions are required.
        if ( !(pOpenContext->AccessCode & (GENERIC_READ | GENERIC_WRITE) ) ) {
            RETAILMSG(MSG_ERROR,
                     (TEXT("IIC_Ioctl: Ioctl %x access permission failure x%X\n\r"),
                      dwCode, pOpenContext->AccessCode));
            SetLastError (ERROR_INVALID_ACCESS);
            return(FALSE);
		}
	}

   
	switch ( dwCode ) {  
	case IOCTL_POWER_CAPABILITIES:
		{
			PPOWER_CAPABILITIES ppc;

			if ( !pdwActualOut || !pBufOut || (dwLenOut < sizeof(POWER_CAPABILITIES)) )
			{
				RetVal = FALSE;
       			SetLastError (ERROR_INVALID_PARAMETER);				
				break;
			}

			ppc = (PPOWER_CAPABILITIES)pBufOut;

			memset(ppc, 0, sizeof(POWER_CAPABILITIES));

			// support D0, D4
			ppc->DeviceDx = 0x11;

			// no wake
			// no inrush

			// Report our nominal power consumption in uAmps rather than mWatts.
			ppc->Flags = POWER_CAP_PREFIX_MICRO | POWER_CAP_UNIT_AMPS;

			//ppc->Power[D0] = 56000;

			*pdwActualOut = sizeof(POWER_CAPABILITIES);
		}
		break;	
	case IOCTL_POWER_QUERY:
		break;
	case IOCTL_POWER_SET:
	/*
		{
			CEDEVICE_POWER_STATE NewDx;
			RETAILMSG(1,(TEXT("[IIC]IOCTL_POWER_SET\n")));

			if ( !pdwActualOut || !pBufOut || (dwLenOut < sizeof(CEDEVICE_POWER_STATE)) )
			{
				RetVal = FALSE;
       			SetLastError (ERROR_INVALID_PARAMETER);				
				break;
			}

			NewDx = *(PCEDEVICE_POWER_STATE)pBufOut;

			if ( VALID_DX(NewDx) )
			{

				switch ( NewDx )
				{
				case D0:
					if (g_Dx != D0)
					{
						HW_PowerUp(pInitContext);
						g_Dx = D0;
					}
					break;
				default:
					if (g_Dx != (_CEDEVICE_POWER_STATE)D4)
					{
						HW_PowerDown(pInitContext);
						g_Dx = (_CEDEVICE_POWER_STATE)D4;
					}
					break;
				}

				// return our state
				*(PCEDEVICE_POWER_STATE)pBufOut = g_Dx;


				*pdwActualOut = sizeof(CEDEVICE_POWER_STATE);
			}
			else
			{
				RetVal = FALSE;
       			SetLastError (ERROR_INVALID_PARAMETER);		
			}
		}	
		*/	
		break;
	case IOCTL_IIC_WRITE:
        if ( (dwLenIn < sizeof(IIC_IO_DESC)) || (NULL == pBufIn) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }	
		RETAILMSG(0,(TEXT("IOCTL_IIC_WRITE %x is waiting\r\n"),pOpenContext));
    	EnterCriticalSection(&(pInitContext->CritSec));	
		RETAILMSG(0,(TEXT("IOCTL_IIC_WRITE %x is entered\r\n"),pOpenContext));
    	if(HW_Write(pOpenContext, (PIIC_IO_DESC)pBufIn))
    	{
    		// success
    	}
    	else
    	{
    		SetLastError(ERROR_TIMEOUT);
            RetVal = FALSE;	    		
    	}
    	LeaveCriticalSection(&(pInitContext->CritSec));	
		RETAILMSG(0,(TEXT("IOCTL_IIC_WRITE %x is leaving\r\n"),pOpenContext));
		break;
		
	case IOCTL_IIC_READ:	
        if ( (dwLenIn < sizeof(IIC_IO_DESC)) || (NULL == pBufIn) || (dwLenOut < sizeof(IIC_IO_DESC)) || (NULL == pBufOut) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }			
	    RETAILMSG(0,(TEXT("IOCTL_IIC_READ %x is waiting\r\n"),pOpenContext));
    	EnterCriticalSection(&(pInitContext->CritSec));	
		RETAILMSG(0,(TEXT("IOCTL_IIC_READ %x is entered\r\n"),pOpenContext));
	    if(HW_Read(pOpenContext, (PIIC_IO_DESC)pBufIn, (PIIC_IO_DESC)pBufOut))
	    {
	    	// success
			*pdwActualOut = sizeof(IIC_IO_DESC);		    	    	
	    }
	    else
	    {
	    	SetLastError(ERROR_TIMEOUT);
	    	*pdwActualOut = 0;
            RetVal = FALSE;	    	
	    }
	    LeaveCriticalSection(&(pInitContext->CritSec));	
        RETAILMSG(0,(TEXT("IOCTL_IIC_READ %x is leaving\r\n"),pOpenContext));
		break;
				
	case IOCTL_IIC_SET_CLOCK:	
        if ( (dwLenIn < sizeof(UINT32)) || (NULL == pBufIn) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }	
		pOpenContext->PDDContextVal.Clock = *(UINT32*)pBufIn;
		HW_SetClock(pOpenContext);
		pOpenContext->DirtyBit = 1;				
		break;
		
	case IOCTL_IIC_GET_CLOCK:	
        if ( (dwLenOut < sizeof(UINT32)) || (NULL == pBufOut) ||
             (NULL == pdwActualOut) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }

		*(UINT32*)pBufIn = pOpenContext->PDDContextVal.Clock;
        // Return the size
        *pdwActualOut = sizeof(UINT32);		
		break;
		
	case IOCTL_IIC_SET_MODE:
        if ( (dwLenIn < sizeof(UINT32)) || (NULL == pBufIn) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }	
		pOpenContext->PDDContextVal.ModeSel = (IIC_MODE)*(UINT32*)pBufIn;	
		pOpenContext->DirtyBit = 1;					
		break;
				
	case IOCTL_IIC_GET_MODE:		
        if ( (dwLenOut < sizeof(UINT32)) || (NULL == pBufOut) ||
             (NULL == pdwActualOut) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }

		*(UINT32*)pBufIn = pOpenContext->PDDContextVal.ModeSel;
        // Return the size
        *pdwActualOut = sizeof(UINT32);		
		break;
		
	case IOCTL_IIC_SET_FILTER:		
        if ( (dwLenIn < sizeof(UINT32)) || (NULL == pBufIn) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }	
		pOpenContext->PDDContextVal.FilterEnable = *(UINT32*)pBufIn;
		pOpenContext->DirtyBit = 1;						
		break;
		
	case IOCTL_IIC_GET_FILTER:	
        if ( (dwLenOut < sizeof(UINT32)) || (NULL == pBufOut) ||
             (NULL == pdwActualOut) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }

		*(UINT32*)pBufIn = pOpenContext->PDDContextVal.FilterEnable;
        // Return the size
        *pdwActualOut = sizeof(UINT32);		
		break;
		
	case IOCTL_IIC_SET_DELAY:		
        if ( (dwLenIn < sizeof(UINT32)) || (NULL == pBufIn) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }	
		pOpenContext->PDDContextVal.Delay = (IIC_DELAY)*(UINT32*)pBufIn;
		pOpenContext->DirtyBit = 1;						
		break;
																
	case IOCTL_IIC_GET_DELAY:		
        if ( (dwLenOut < sizeof(UINT32)) || (NULL == pBufOut) ||
             (NULL == pdwActualOut) ) {
            SetLastError (ERROR_INVALID_PARAMETER);
            RetVal = FALSE;
            RETAILMSG (MSG_ERROR, (TEXT(" Invalid parameter\r\n")));
            break;
        }

		*(UINT32*)pBufIn = pOpenContext->PDDContextVal.Delay;
        // Return the size
        *pdwActualOut = sizeof(UINT32);		
		break;							
	}
    
    RETAILMSG (MSG_FUNCTION|(RetVal == FALSE?MSG_ERROR:0),
              (TEXT("-IIC_IOControl %s Ecode=%d (len=%d)\r\n"),
               (RetVal == TRUE) ? TEXT("Success") : TEXT("Error"),
               GetLastError(), (NULL == pdwActualOut) ? 0 : *pdwActualOut));

    return(RetVal);    	
}

void IIC_PowerUp(
			PHW_INIT_INFO	pInitContext						/* Context pointer returned from IIC_Init*/
)
{
	HW_PowerUp(pInitContext);
}

void IIC_PowerDown(
			PHW_INIT_INFO	pInitContext						/* Context pointer returned from IIC_Init*/
)
{
	HW_PowerDown(pInitContext);
}