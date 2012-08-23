
Wavedev device driver for IIS (WM8753) is implemented as below.
date : 2007/06/08
name : Lee Jae-Yong

1. Playback path : LINE OUT(L/R)
2. Record path : LINE IN(L/R)
3. Control path for external IIS Codec(WM8753) : IIC (using builtin IIC device driver)
4. Dinamic Power/Mute control for externel IIS Codec(WM8753) is not implemented, yet.
5. IIS library is implemented for general purpose.
6. DMA library is used, which is written by Lim Gab-Joo.
7. There's some problems to use IIC driver after os startup.
   So, we need to fix-up these problems, ASAP.
8. To change record path onto MIC in, the WM8753' SFR initial table on "wm8753.h" should be modified.
9. Jumper configurations for IIS audio path : 
    CFG1/CFG2 on base board, should be configured with all off configurations.