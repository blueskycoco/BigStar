#if !defined(_Version_h)
#define _Version_h

#define DRIVER_NAME         "CSR Generic SDIO Driver"
#define COPYRIGHT_STRING    "Copyright (C) Cambridge Silicon Radio Ltd 2007"


#define _STRING2_(x)                #x
#define _STRING_(x)                 _STRING2_(x)

#ifdef UNIFI_MAJOR_VERSION
# define DRIVER_VERSION     UNIFI_MAJOR_VERSION,UNIFI_MINOR_VERSION,UNIFI_REVISION,UNIFI_BUILD_ID
#else
# define DRIVER_VERSION     7,0,0,68887
#endif

#define VERSION_STRING      _STRING_( DRIVER_VERSION )


#endif
