
//TSC2003 Header



//slave addr 
#define TSC_WRITE    0x90
#define TSC_READ     0x91

#define MAX_SAMPLE_NUM   8
#define DELTA_X_COORD_VARIANCE  20
#define DELTA_Y_COORD_VARIANCE  20
#define MAX_ADC_VAL  0xFFF
#define MAX_NO_Z   5//10


void TSC_Init(void);
DWORD TSC_ReadData( UCHAR cmd );
DWORD TSC_WriteCommand(UCHAR cmd);
BOOL TSC_GetXY(void);
BOOL GetTouchStatus(void);
BOOL PDDSampleTouchScreen(int *x,int *y);

void EnableTSC_Interrupt( void );
DWORD Lock(void);
DWORD Unlock(void);