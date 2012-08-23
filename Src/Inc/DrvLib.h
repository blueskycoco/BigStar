#ifndef	__S3C6410_DEVICE_DRIVER_LIB_H__
#define __S3C6410_DEVICE_DRIVER_LIB_H__

#if __cplusplus
extern "C"
{
#endif

void *DrvLib_MapIoSpace(UINT32 PhysicalAddress, UINT32 NumberOfBytes, BOOL CacheEnable);
void DrvLib_UnmapIoSpace(void *MappedAddress);

void CPUStall_us(unsigned int us);
void CPUStall_ms(unsigned int ms);

#if __cplusplus
}
#endif

#endif	// __S3C6410_DEVICE_DRIVER_LIB_H__

