#ifndef __USERKEY_H_
#define __USERKEY_H_

#ifdef __cplusplus
extern "C" {
#endif

BOOL KEY_Close(DWORD hOpenContext);
BOOL KEY_Deinit(DWORD hDeviceContext);
DWORD KEY_Init(DWORD dwContext);
DWORD KEY_Open(DWORD hDeviceContext, DWORD AccessCode, DWORD ShareMode);
void KEY_PowerUp(DWORD hDeviceContext);
void KEY_PowerDown(DWORD hDeviceContext);
DWORD KEY_Read(DWORD hOpenContext, LPVOID pBuffer, DWORD Count);
DWORD KEY_Seek(DWORD hOpenContext, long Amount, DWORD Type);
DWORD KEY_Write(DWORD hOpenContext, LPCVOID pSourceBytes, DWORD NumberOfBytes);

BOOL KEY_IOControl(DWORD hOpenContext, 
				   DWORD dwCode, 
				   PBYTE pBufIn, 
				   DWORD dwLenIn, 
				   PBYTE pBufOut, 
				   DWORD dwLenOut, 
				   PDWORD pdwActualOut);


#ifdef __cplusplus
}
#endif


#endif /* __CAMERA_H_ */