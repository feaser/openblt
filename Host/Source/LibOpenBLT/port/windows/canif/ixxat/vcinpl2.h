/*****************************************************************************
 HMS Technology Center Ravensburg GmbH
******************************************************************************

 File    : VCINPL2.H
 Summary : Native VCI C Programming Library.

 Date    : 2016-03-31

 Compiler: MSVC

******************************************************************************
 all rights reserved
*****************************************************************************/

#ifndef _VCINPL2_H_
#define _VCINPL2_H_

/*##########################################################################*/
/*##                                                                      ##*/
/*##     common declarations                                              ##*/
/*##                                                                      ##*/
/*##########################################################################*/

#include <vcisdk.h>


/*##########################################################################*/
/*##                                                                      ##*/
/*##   exported API functions                                             ##*/
/*##                                                                      ##*/
/*##########################################################################*/

/*****************************************************************************
 * initialization
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  vciInitialize ( void );

typedef HRESULT
  (VCIAPI *PF_vciInitialize)( void );


/*****************************************************************************
 * version information
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  vciGetVersion( OUT PUINT32 pdwMajorVersion,
                 OUT PUINT32 pdwMinorVersion,
                 OUT PUINT32 pdwRevNumber,
                 OUT PUINT32 pdwBuildNumber );

typedef HRESULT
  (VCIAPI *PF_vciGetVersion)( OUT PUINT32 pdwMajorVersion,
                              OUT PUINT32 pdwMinorVersion,
                              OUT PUINT32 pdwRevNumber,
                              OUT PUINT32 pdwBuildNumber );


/*****************************************************************************
 * error handling
 ****************************************************************************/

EXTERN_C void VCIAPI
  vciFormatErrorA( IN  HRESULT hrError,
                   OUT PCHAR   pszText,
                   IN  UINT32  dwLength );

EXTERN_C void VCIAPI
  vciFormatErrorW( IN  HRESULT hrError,
                   OUT PWCHAR  pwszText,
                   IN  UINT32  dwLength );

typedef void
  (VCIAPI *PF_vciFormatErrorA)(IN  HRESULT hrError,
                               OUT PCHAR   pszText,
                               IN  UINT32  dwLength );

typedef void
  (VCIAPI *PF_vciFormatErrorW)(IN  HRESULT hrError,
                               OUT PWCHAR  pwszText,
                               IN  UINT32  dwLength );

#ifdef UNICODE
#define vciFormatError    vciFormatErrorW
#define PF_vciFormatError PF_vciFormatErrorW
#else
#define vciFormatError    vciFormatErrorA
#define PF_vciFormatError PF_vciFormatErrorA
#endif //UNICODE

EXTERN_C void VCIAPI
  vciDisplayErrorA( IN HWND    hwndParent,
                    IN PCHAR   pszCaption,
                    IN HRESULT hrError );

EXTERN_C void VCIAPI
  vciDisplayErrorW( IN HWND    hwndParent,
                    IN PWCHAR  pwszCaption,
                    IN HRESULT hrError );

typedef void
  (VCIAPI *PF_vciDisplayErrorA)( IN HWND    hwndParent,
                                 IN PCHAR   pszCaption,
                                 IN HRESULT hrError );

typedef void
  (VCIAPI *PF_vciDisplayErrorW)( IN HWND    hwndParent,
                                 IN PWCHAR  pwszCaption,
                                 IN HRESULT hrError );

#ifdef UNICODE
#define vciDisplayError    vciDisplayErrorW
#define PF_vciDisplayError PF_vciDisplayErrorW
#else
#define vciDisplayError    vciDisplayErrorA
#define PF_vciDisplayError PF_vciFormatErrorA
#endif //UNICODE


/*****************************************************************************
 * utility functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  vciCreateLuid( OUT PVCIID pVciid );

typedef HRESULT
  (VCIAPI *PF_vciCreateLuid)( OUT PVCIID pVciid );

EXTERN_C HRESULT VCIAPI
  vciLuidToCharA( IN  REFVCIID rVciid,
                  OUT PCHAR    pszLuid,
                  IN  LONG     cbSize );

EXTERN_C HRESULT VCIAPI
  vciLuidToCharW( IN  REFVCIID rVciid,
                  OUT PWCHAR   pwszLuid,
                  IN  LONG     cbSize );

typedef HRESULT
  (VCIAPI *PF_vciLuidToCharA)( IN  REFVCIID rVciid,
                               OUT PCHAR    pszLuid,
                               IN  LONG     cbSize );

typedef HRESULT
  (VCIAPI *PF_vciLuidToCharW)( IN  REFVCIID rVciid,
                               OUT PWCHAR   pwszLuid,
                               IN  LONG     cbSize );

#ifdef UNICODE
#define vciLuidToChar    vciLuidToCharW
#define PF_vciLuidToChar PF_vciLuidToCharW
#else
#define vciLuidToChar    vciLuidToCharA
#define PF_vciLuidToChar PF_vciLuidToCharA
#endif //UNICODE

EXTERN_C HRESULT VCIAPI
  vciCharToLuidA( IN  PCHAR  pszLuid,
                  OUT PVCIID pVciid );

EXTERN_C HRESULT VCIAPI
  vciCharToLuidW( IN  PWCHAR pszLuid,
                  OUT PVCIID pVciid );

typedef HRESULT
  (VCIAPI *PF_vciCharToLuidA)( IN  PCHAR  pszLuid,
                               OUT PVCIID pVciid );

typedef HRESULT
  (VCIAPI *PF_vciCharToLuidW)( IN  PWCHAR pszLuid,
                               OUT PVCIID pVciid );

#ifdef UNICODE
#define vciCharToLuid    vciCharToLuidW
#define PF_vciCharToLuid PF_vciCharToLuidW
#else
#define vciCharToLuid    vciCharToLuidA
#define PF_vciCharToLuid PF_vciCharToLuidA
#endif //UNICODE

EXTERN_C HRESULT VCIAPI
  vciGuidToCharA( IN  REFGUID rGuid,
                  OUT PCHAR   pszGuid,
                  IN  LONG    cbSize );

EXTERN_C HRESULT VCIAPI
  vciGuidToCharW( IN  REFGUID rGuid,
                  OUT PWCHAR  pszGuid,
                  IN  LONG    cbSize );

typedef HRESULT
  (VCIAPI *PF_vciGuidToCharA)( IN  REFGUID rGuid,
                               OUT PCHAR   pszGuid,
                               IN  LONG    cbSize );

typedef HRESULT
  (VCIAPI *PF_vciGuidToCharW)( IN  REFGUID rGuid,
                               OUT PWCHAR  pszGuid,
                               IN  LONG    cbSize );

#ifdef UNICODE
#define vciGuidToChar    vciGuidToCharW
#define PF_vciGuidToChar PF_vciGuidToCharW
#else
#define vciGuidToChar    vciGuidToCharA
#define PF_vciGuidToChar PF_vciGuidToCharA
#endif //UNICODE

EXTERN_C HRESULT VCIAPI
  vciCharToGuidA( IN  PCHAR pszGuid,
                  OUT PGUID pGuid );

EXTERN_C HRESULT VCIAPI
  vciCharToGuidW( IN  PWCHAR pszGuid,
                  OUT PGUID  pGuid );

typedef HRESULT
  (VCIAPI *PF_vciCharToGuidA)( IN  PCHAR pszGuid,
                               OUT PGUID pGuid );

typedef HRESULT
  (VCIAPI *PF_vciCharToGuidW)( IN  PWCHAR pszGuid,
                               OUT PGUID  pGuid );

#ifdef UNICODE
#define vciCharToGuid    vciCharToGuidW
#define PF_vciCharToGuid PF_vciCharToGuidW
#else
#define vciCharToGuid    vciCharToGuidA
#define PF_vciCharToGuid PF_vciCharToGuidA
#endif //UNICODE


/*****************************************************************************
 * event specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  vciEventCreate( IN  BOOL    fManReset,
                  IN  BOOL    fInitState,
                  OUT PHANDLE phEvent );

typedef HRESULT
  (VCIAPI *PF_vciEventCreate)( IN  BOOL    fManReset,
                               IN  BOOL    fInitState,
                               OUT PHANDLE phEvent );

EXTERN_C HRESULT VCIAPI
  vciEventDelete( IN HANDLE hEvent );

typedef HRESULT
  (VCIAPI *PF_vciEventDelete)( IN HANDLE hEvent );

EXTERN_C HRESULT VCIAPI
  vciEventSignal( IN HANDLE hEvent );

typedef HRESULT
  (VCIAPI *PF_vciEventSignal)( IN HANDLE hEvent );

EXTERN_C HRESULT VCIAPI
  vciEventReset( HANDLE hEvent );

typedef HRESULT
  (VCIAPI *PF_vciEventReset)( HANDLE hEvent );

EXTERN_C HRESULT VCIAPI
  vciEventWaitFor( IN HANDLE hEvent,
                   IN UINT32 dwTimeout );

typedef HRESULT
  (VCIAPI *PF_vciEventWaitFor)( IN HANDLE hEvent,
                                IN UINT32 dwTimeout );


/*****************************************************************************
 * device manager specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  vciEnumDeviceOpen( OUT PHANDLE hEnum );

typedef HRESULT
  (VCIAPI *PF_vciEnumDeviceOpen)( OUT PHANDLE hEnum );

EXTERN_C HRESULT VCIAPI
  vciEnumDeviceClose( IN HANDLE hEnum );

typedef HRESULT
  (VCIAPI *PF_vciEnumDeviceClose)( IN HANDLE hEnum );

EXTERN_C HRESULT VCIAPI
  vciEnumDeviceNext( IN  HANDLE         hEnum,
                     OUT PVCIDEVICEINFO pInfo );

typedef HRESULT
  (VCIAPI *PF_vciEnumDeviceNext)( IN  HANDLE         hEnum,
                                  OUT PVCIDEVICEINFO pInfo );

EXTERN_C HRESULT VCIAPI
  vciEnumDeviceReset( IN HANDLE hEnum );

typedef HRESULT
  (VCIAPI *PF_vciEnumDeviceReset)( IN HANDLE hEnum );

EXTERN_C HRESULT VCIAPI
  vciEnumDeviceWaitEvent( IN HANDLE hEnum,
                          IN UINT32 dwTimeout );

typedef HRESULT
  (VCIAPI *PF_vciEnumDeviceWaitEvent)( IN HANDLE hEnum,
                                       IN UINT32 dwTimeout );

EXTERN_C HRESULT VCIAPI
  vciFindDeviceByHwid( IN  REFGUID rHwid,
                       OUT PVCIID  pVciid );

typedef HRESULT
  (VCIAPI *PF_vciFindDeviceByHwid)( IN  REFGUID rHwid,
                                    OUT PVCIID  pVciid );

EXTERN_C HRESULT VCIAPI
  vciFindDeviceByClass( IN  REFGUID rClass,
                        IN  UINT32  dwInst,
                        OUT PVCIID  pVciid );

typedef HRESULT
  (VCIAPI *PF_vciFindDeviceByClass)( IN  REFGUID rClass,
                                     IN  UINT32  dwInst,
                                     OUT PVCIID  pVciid );

EXTERN_C HRESULT VCIAPI
  vciSelectDeviceDlg( IN  HWND   hwndParent,
                      OUT PVCIID pVciid );

typedef HRESULT
  (VCIAPI *PF_vciSelectDeviceDlg)( IN  HWND   hwndParent,
                                   OUT PVCIID pVciid );


/*****************************************************************************
 * device specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  vciDeviceOpen( IN  REFVCIID rVciid,
                 OUT PHANDLE  phDevice );

typedef HRESULT
  (VCIAPI *PF_vciDeviceOpen)( IN  REFVCIID rVciid,
                              OUT PHANDLE  phDevice );

EXTERN_C HRESULT VCIAPI
  vciDeviceOpenDlg( IN  HWND    hwndParent,
                    OUT PHANDLE phDevice );

typedef HRESULT
  (VCIAPI *PF_vciDeviceOpenDlg)( IN  HWND    hwndParent,
                                 OUT PHANDLE phDevice );

EXTERN_C HRESULT VCIAPI
  vciDeviceClose( IN HANDLE hDevice );

typedef HRESULT
  (VCIAPI *PF_vciDeviceClose)( IN HANDLE hDevice );

EXTERN_C HRESULT VCIAPI
  vciDeviceGetInfo( IN  HANDLE         hDevice,
                    OUT PVCIDEVICEINFO pInfo );

typedef HRESULT
  (VCIAPI *PF_vciDeviceGetInfo)( IN  HANDLE         hDevice,
                                 OUT PVCIDEVICEINFO pInfo );

EXTERN_C HRESULT VCIAPI
  vciDeviceGetCaps( IN  HANDLE         hDevice,
                    OUT PVCIDEVICECAPS pCaps );

typedef HRESULT
  (VCIAPI *PF_vciDeviceGetCaps)( IN  HANDLE         hDevice,
                                 OUT PVCIDEVICECAPS pCaps );


/*****************************************************************************
 * CAN controller specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  canControlOpen( IN  HANDLE  hDevice,
                  IN  UINT32  dwCanNo,
                  OUT PHANDLE phCanCtl );

typedef HRESULT
  (VCIAPI *PF_canControlOpen)( IN  HANDLE  hDevice,
                               IN  UINT32  dwCanNo,
                               OUT PHANDLE phCanCtl );

EXTERN_C HRESULT VCIAPI
  canControlClose( IN HANDLE hCanCtl );

typedef HRESULT
  (VCIAPI *PF_canControlClose)( IN HANDLE hCanCtl );

EXTERN_C HRESULT VCIAPI
  canControlGetCaps( IN  HANDLE            hCanCtl,
                     OUT PCANCAPABILITIES2 pCanCaps );

typedef HRESULT
  (VCIAPI *PF_canControlGetCaps)( IN  HANDLE            hCanCtl,
                                  OUT PCANCAPABILITIES2 pCanCaps );

EXTERN_C HRESULT VCIAPI
  canControlGetStatus( IN  HANDLE          hCanCtl,
                       OUT PCANLINESTATUS2 pStatus );

typedef HRESULT
  (VCIAPI *PF_canControlGetStatus)( IN  HANDLE          hCanCtl,
                                    OUT PCANLINESTATUS2 pStatus );

EXTERN_C HRESULT VCIAPI
  canControlDetectBitrate( IN  HANDLE  hCanCtl,
                           IN  UINT8   bOpMode,
                           IN  UINT8   bExMode,
                           IN  UINT16  wTimeout,
                           IN  UINT32  dwCount,
                           IN  PCANBTP paBtpSDR,
                           IN  PCANBTP paBtpFDR,
                           OUT PUINT32 pdwIndex );

typedef HRESULT
  (VCIAPI *PF_canControlDetectBitrate)( IN  HANDLE  hCanCtl,
                                        IN  UINT8   bOpMode,
                                        IN  UINT8   bExMode,
                                        IN  UINT16  wTimeout,
                                        IN  UINT32  dwCount,
                                        IN  PCANBTP paBtpSDR,
                                        IN  PCANBTP paBtpFDR,
                                        OUT PUINT32 pdwIndex );

EXTERN_C HRESULT VCIAPI
  canControlInitialize( IN HANDLE  hCanCtl,
                        IN UINT8   bOpMode,
                        IN UINT8   bExMode,
                        IN UINT8   bSFMode,
                        IN UINT8   bEFMode,
                        IN UINT32  dwSFIds,
                        IN UINT32  dwEFIds,
                        IN PCANBTP pBtpSDR,
                        IN PCANBTP pBtpFDR );

typedef HRESULT
  (VCIAPI *PF_canControlInitialize)( IN HANDLE  hCanCtl,
                                     IN UINT8   bOpMode,
                                     IN UINT8   bExMode,
                                     IN UINT8   bSFMode,
                                     IN UINT8   bEFMode,
                                     IN UINT32  dwSFIds,
                                     IN UINT32  dwEFIds,
                                     IN PCANBTP pBtpSDR,
                                     IN PCANBTP pBtpFDR );

EXTERN_C HRESULT VCIAPI
  canControlReset( IN HANDLE hCanCtl );

typedef HRESULT
  (VCIAPI *PF_canControlReset)( IN HANDLE hCanCtl );

EXTERN_C HRESULT VCIAPI
  canControlStart( IN HANDLE hCanCtl,
                   IN BOOL   fStart );

typedef HRESULT
  (VCIAPI *PF_canControlStart)( IN HANDLE hCanCtl,
                                IN BOOL   fStart );

EXTERN_C HRESULT VCIAPI
  canControlGetFilterMode( IN  HANDLE hCanCtl,
                           IN  BOOL   fExtend,
                           OUT PUINT8 pbMode );

typedef HRESULT
  (VCIAPI *PF_canControlGetFilterMode)( IN  HANDLE hCanCtl,
                                        IN  BOOL   fExtend,
                                        OUT PUINT8 pbMode );

EXTERN_C HRESULT VCIAPI
  canControlSetFilterMode( IN  HANDLE hCanCtl,
                           IN  BOOL   fExtend,
                           IN  UINT8  bMode,
                           OUT PUINT8 pbPrev );

typedef HRESULT
  (VCIAPI *PF_canControlSetFilterMode)( IN  HANDLE hCanCtl,
                                        IN  BOOL   fExtend,
                                        IN  UINT8  bMode,
                                        OUT PUINT8 pbPrev );

EXTERN_C HRESULT VCIAPI
  canControlSetAccFilter( IN HANDLE hCanCtl,
                          IN BOOL   fExtend,
                          IN UINT32 dwCode,
                          IN UINT32 dwMask );

typedef HRESULT
  (VCIAPI *PF_canControlSetAccFilter)( IN HANDLE hCanCtl,
                                       IN BOOL   fExtend,
                                       IN UINT32 dwCode,
                                       IN UINT32 dwMask );

EXTERN_C HRESULT VCIAPI
  canControlAddFilterIds( IN HANDLE hCanCtl,
                          IN BOOL   fExtend,
                          IN UINT32 dwCode,
                          IN UINT32 dwMask );

typedef HRESULT
  (VCIAPI *PF_canControlAddFilterIds)( IN HANDLE hCanCtl,
                                       IN BOOL   fExtend,
                                       IN UINT32 dwCode,
                                       IN UINT32 dwMask );

EXTERN_C HRESULT VCIAPI
  canControlRemFilterIds( IN HANDLE hCanCtl,
                          IN BOOL   fExtend,
                          IN UINT32 dwCode,
                          IN UINT32 dwMask );

typedef HRESULT
  (VCIAPI *PF_canControlRemFilterIds)( IN HANDLE hCanCtl,
                                       IN BOOL   fExtend,
                                       IN UINT32 dwCode,
                                       IN UINT32 dwMask );


/*****************************************************************************
 * CAN message channel specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  canChannelOpen( IN  HANDLE  hDevice,
                  IN  UINT32  dwCanNo,
                  IN  BOOL    fExclusive,
                  OUT PHANDLE phCanChn );

typedef HRESULT
  (VCIAPI *PF_canChannelOpen)( IN  HANDLE  hDevice,
                               IN  UINT32  dwCanNo,
                               IN  BOOL    fExclusive,
                               OUT PHANDLE phCanChn );

EXTERN_C HRESULT VCIAPI
  canChannelClose( IN HANDLE hCanChn );

typedef HRESULT
  (VCIAPI *PF_canChannelClose)( IN HANDLE hCanChn );

EXTERN_C HRESULT VCIAPI
  canChannelGetCaps( IN  HANDLE            hCanChn,
                     OUT PCANCAPABILITIES2 pCanCaps );

typedef HRESULT
  (VCIAPI *PF_canChannelGetCaps)( IN  HANDLE            hCanChn,
                                  OUT PCANCAPABILITIES2 pCanCaps );

EXTERN_C HRESULT VCIAPI
  canChannelGetStatus( IN  HANDLE          hCanChn,
                       OUT PCANCHANSTATUS2 pStatus );

typedef HRESULT
  (VCIAPI *PF_canChannelGetStatus)( IN  HANDLE          hCanChn,
                                    OUT PCANCHANSTATUS2 pStatus );

EXTERN_C HRESULT VCIAPI
  canChannelGetControl( IN  HANDLE  hCanChn,
                        OUT PHANDLE phCanCtl );

typedef HRESULT
  (VCIAPI *PF_canChannelGetControl)( IN  HANDLE  hCanChn,
                                     OUT PHANDLE phCanCtl );

EXTERN_C HRESULT VCIAPI
  canChannelInitialize( IN HANDLE hCanChn,
                        IN UINT16 wRxFifoSize,
                        IN UINT16 wRxThreshold,
                        IN UINT16 wTxFifoSize,
                        IN UINT16 wTxThreshold,
                        IN UINT32 dwFilterSize,
                        IN UINT8  bFilterMode );

typedef HRESULT
  (VCIAPI *PF_canChannelInitialize)( IN HANDLE hCanChn,
                                     IN UINT16 wRxFifoSize,
                                     IN UINT16 wRxThreshold,
                                     IN UINT16 wTxFifoSize,
                                     IN UINT16 wTxThreshold,
                                     IN UINT32 dwFilterSize,
                                     IN UINT8  bFilterMode );

EXTERN_C HRESULT VCIAPI
  canChannelGetFilterMode( IN  HANDLE hCanChn,
                           IN  BOOL   fExtend,
                           OUT PUINT8 pbMode );

typedef HRESULT
  (VCIAPI *PF_canChannelGetFilterMode)( IN  HANDLE hCanChn,
                                        IN  BOOL   fExtend,
                                        OUT PUINT8 pbMode );

EXTERN_C HRESULT VCIAPI
  canChannelSetFilterMode( IN  HANDLE hCanChn,
                           IN  BOOL   fExtend,
                           IN  UINT8  bMode,
                           OUT PUINT8 pbPrev );

typedef HRESULT
  (VCIAPI *PF_canChannelSetFilterMode)( IN  HANDLE hCanChn,
                                        IN  BOOL   fExtend,
                                        IN  UINT8  bMode,
                                        OUT PUINT8 pbPrev );

EXTERN_C HRESULT VCIAPI
  canChannelSetAccFilter( IN HANDLE hCanChn,
                          IN BOOL   fExtend,
                          IN UINT32 dwCode,
                          IN UINT32 dwMask );

typedef HRESULT
  (VCIAPI *PF_canChannelSetAccFilter)( IN HANDLE hCanChn,
                                       IN BOOL   fExtend,
                                       IN UINT32 dwCode,
                                       IN UINT32 dwMask );

EXTERN_C HRESULT VCIAPI
  canChannelAddFilterIds( IN HANDLE hCanChn,
                          IN BOOL   fExtend,
                          IN UINT32 dwCode,
                          IN UINT32 dwMask );

typedef HRESULT
  (VCIAPI *PF_canChannelAddFilterIds)( IN HANDLE hCanChn,
                                       IN BOOL   fExtend,
                                       IN UINT32 dwCode,
                                       IN UINT32 dwMask );

EXTERN_C HRESULT VCIAPI
  canChannelRemFilterIds( IN HANDLE hCanChn,
                          IN BOOL   fExtend,
                          IN UINT32 dwCode,
                          IN UINT32 dwMask );

typedef HRESULT
  (VCIAPI *PF_canChannelRemFilterIds)( IN HANDLE hCanChn,
                                       IN BOOL   fExtend,
                                       IN UINT32 dwCode,
                                       IN UINT32 dwMask );

EXTERN_C HRESULT VCIAPI
  canChannelActivate( IN HANDLE hCanChn,
                      IN BOOL   fEnable );

typedef HRESULT
  (VCIAPI *PF_canChannelActivate)( IN HANDLE hCanChn,
                                   IN BOOL   fEnable );

EXTERN_C HRESULT VCIAPI
  canChannelPeekMessage( IN  HANDLE   hCanChn,
                         OUT PCANMSG2 pCanMsg );

typedef HRESULT
  (VCIAPI *PF_canChannelPeekMessage)( IN  HANDLE   hCanChn,
                                      OUT PCANMSG2 pCanMsg );

EXTERN_C HRESULT VCIAPI
  canChannelPeekMsgMult( IN  HANDLE  hCanChn,
                         OUT CANMSG2 aCanMsg[],
                         IN  UINT32  dwCount,
                         OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_canChannelPeekMsgMult)( IN  HANDLE  hCanChn,
                                      OUT CANMSG2 aCanMsg[],
                                      IN  UINT32  dwCount,
                                      OUT PUINT32 pdwDone );

EXTERN_C HRESULT VCIAPI
  canChannelPostMessage( IN HANDLE   hCanChn,
                         IN PCANMSG2 pCanMsg );

typedef HRESULT
  (VCIAPI *PF_canChannelPostMessage)( IN HANDLE   hCanChn,
                                      IN PCANMSG2 pCanMsg );

EXTERN_C HRESULT VCIAPI
  canChannelPostMsgMult( IN  HANDLE  hCanChn,
                         IN  CANMSG2 aCanMsg[],
                         IN  UINT32  dwCount,
                         OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_canChannelPostMsgMult)( IN  HANDLE  hCanChn,
                                      IN  CANMSG2 aCanMsg[],
                                      IN  UINT32  dwCount,
                                      OUT PUINT32 pdwDone );

EXTERN_C HRESULT VCIAPI
  canChannelWaitRxEvent( IN HANDLE hCanChn,
                         IN UINT32 dwTimeout );

typedef HRESULT
  (VCIAPI *PF_canChannelWaitRxEvent)( IN HANDLE hCanChn,
                                      IN UINT32 dwTimeout );

EXTERN_C HRESULT VCIAPI
  canChannelWaitTxEvent( IN HANDLE hCanChn,
                         IN UINT32 dwTimeout );

typedef HRESULT
  (VCIAPI *PF_canChannelWaitTxEvent)( IN HANDLE hCanChn,
                                      IN UINT32 dwTimeout );

EXTERN_C HRESULT VCIAPI
  canChannelReadMessage( IN  HANDLE   hCanChn,
                         IN  UINT32   dwTimeout,
                         OUT PCANMSG2 pCanMsg );

typedef HRESULT
  (VCIAPI *PF_canChannelReadMessage)( IN  HANDLE   hCanChn,
                                      IN  UINT32   dwTimeout,
                                      OUT PCANMSG2 pCanMsg );

EXTERN_C HRESULT VCIAPI
  canChannelReadMsgMult( IN  HANDLE  hCanChn,
                         IN  UINT32  dwTimeout,
                         OUT CANMSG2 aCanMsg[],
                         IN  UINT32  dwCount,
                         OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_canChannelReadMsgMult)( IN  HANDLE  hCanChn,
                                      IN  UINT32  dwTimeout,
                                      OUT CANMSG2 aCanMsg[],
                                      IN  UINT32  dwCount,
                                      OUT PUINT32 pdwDone );

EXTERN_C HRESULT VCIAPI
  canChannelSendMessage( IN HANDLE   hCanChn,
                         IN UINT32   dwTimeout,
                         IN PCANMSG2 pCanMsg );

typedef HRESULT
  (VCIAPI *PF_canChannelSendMessage)( IN HANDLE   hCanChn,
                                      IN UINT32   dwTimeout,
                                      IN PCANMSG2 pCanMsg );

EXTERN_C HRESULT VCIAPI
  canChannelSendMsgMult( IN  HANDLE  hCanChn,
                         IN  UINT32  dwTimeout,
                         IN  CANMSG2 aCanMsg[],
                         IN  UINT32  dwCount,
                         OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_canChannelSendMsgMult)( IN  HANDLE  hCanChn,
                                      IN  UINT32  dwTimeout,
                                      IN  CANMSG2 aCanMsg[],
                                      IN  UINT32  dwCount,
                                      OUT PUINT32 pdwDone );


/*****************************************************************************
 * CAN message scheduler specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  canSchedulerOpen( IN  HANDLE  hDevice,
                    IN  UINT32  dwCanNo,
                    OUT PHANDLE phCanShd );

typedef HRESULT
  (VCIAPI *PF_canSchedulerOpen)( IN  HANDLE  hDevice,
                                 IN  UINT32  dwCanNo,
                                 OUT PHANDLE phCanShd );

EXTERN_C HRESULT VCIAPI
  canSchedulerClose( IN HANDLE hCanShd );

typedef HRESULT
  (VCIAPI *PF_canSchedulerClose)( IN HANDLE hCanShd );

EXTERN_C HRESULT VCIAPI
  canSchedulerGetCaps( IN  HANDLE            hCanShd,
                       OUT PCANCAPABILITIES2 pCanCaps );

typedef HRESULT
  (VCIAPI *PF_canSchedulerGetCaps)( IN  HANDLE            hCanShd,
                                    OUT PCANCAPABILITIES2 pCanCaps );

EXTERN_C HRESULT VCIAPI
  canSchedulerGetStatus( IN  HANDLE               hCanShd,
                         OUT PCANSCHEDULERSTATUS2 pStatus );

typedef HRESULT
  (VCIAPI *PF_canSchedulerGetStatus)( IN  HANDLE               hCanShd,
                                      OUT PCANSCHEDULERSTATUS2 pStatus );

EXTERN_C HRESULT VCIAPI
  canSchedulerGetControl( IN  HANDLE  hCanShd,
                          OUT PHANDLE phCanCtl );

typedef HRESULT
  (VCIAPI *PF_canSchedulerGetControl)( IN  HANDLE  hCanShd,
                                       OUT PHANDLE phCanCtl );

EXTERN_C HRESULT VCIAPI
  canSchedulerActivate( IN HANDLE hCanShd,
                        IN BOOL   fEnable );

typedef HRESULT
  (VCIAPI *PF_canSchedulerActivate)( IN HANDLE hCanShd,
                                     IN BOOL   fEnable );

EXTERN_C HRESULT VCIAPI
  canSchedulerReset( IN HANDLE hCanShd );

typedef HRESULT
  (VCIAPI *PF_canSchedulerReset)( IN HANDLE hCanShd );

EXTERN_C HRESULT VCIAPI
  canSchedulerAddMessage( IN  HANDLE           hCanShd,
                          IN  PCANCYCLICTXMSG2 pMessage,
                          OUT PUINT32          pdwIndex );

typedef HRESULT
  (VCIAPI *PF_canSchedulerAddMessage)( IN  HANDLE           hCanShd,
                                       IN  PCANCYCLICTXMSG2 pMessage,
                                       OUT PUINT32          pdwIndex );

EXTERN_C HRESULT VCIAPI
  canSchedulerRemMessage( IN HANDLE hCanShd,
                          IN UINT32 dwIndex );

typedef HRESULT
  (VCIAPI *PF_canSchedulerRemMessage)( IN HANDLE hCanShd,
                                       IN UINT32 dwIndex );

EXTERN_C HRESULT VCIAPI
  canSchedulerStartMessage( IN HANDLE hCanShd,
                            IN UINT32 dwIndex,
                            IN UINT16 wRepeat );

typedef HRESULT
  (VCIAPI *PF_canSchedulerStartMessage)( IN HANDLE hCanShd,
                                         IN UINT32 dwIndex,
                                         IN UINT16 wRepeat );

EXTERN_C HRESULT VCIAPI
  canSchedulerStopMessage( IN HANDLE hCanShd,
                           IN UINT32 dwIndex );

typedef HRESULT
  (VCIAPI *PF_canSchedulerStopMessage)( IN HANDLE hCanShd,
                                        IN UINT32 dwIndex );


/*****************************************************************************
 * CAN message switch specific functions
 ****************************************************************************/
#if defined(USE_VCIFSL)

#include <vcifsl.h>

EXTERN_C HRESULT VCIAPI
  canMsgSwitchOpen( IN  HANDLE  hDevice,
                    IN  UINT32  dwCanNo,
                    OUT PHANDLE phSwitch );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchOpen)( IN  HANDLE  hDevice,
                                 IN  UINT32  dwCanNo,
                                 OUT PHANDLE phSwitch );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchClose( IN HANDLE hSwitch );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchClose)( IN HANDLE hSwitch );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchGetCaps( IN  HANDLE            hSwitch,
                       OUT PCANCAPABILITIES2 pCanCaps );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchGetCaps)( IN  HANDLE            hSwitch,
                                    OUT PCANCAPABILITIES2 pCanCaps );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchGetStatus( IN  HANDLE              hSwitch,
                         OUT PCANMSGSWITCHSTATUS pStatus );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchGetStatus)( IN  HANDLE              hSwitch,
                                      OUT PCANMSGSWITCHSTATUS pStatus );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchGetControl( IN  HANDLE  hSwitch,
                          OUT PHANDLE phCanCtl );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchGetControl)( IN  HANDLE  hSwitch,
                                       OUT PHANDLE phCanCtl );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchInitialize( IN HANDLE hSwitch,
                          IN UINT32 dwTiming,
                          IN INT32  lPriority,
                          IN BOOL   fExclusive,
                          IN UINT16 wRxFifoSize,
                          IN UINT16 wTxFifoSize );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchInitialize)( IN HANDLE hSwitch,
                                       IN UINT32 dwTiming,
                                       IN INT32  lPriority,
                                       IN BOOL   fExclusive,
                                       IN UINT16 wRxFifoSize,
                                       IN UINT16 wTxFifoSize );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchActivate( IN HANDLE hSwitch,
                        IN BOOL   fEnable );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchActivate)( IN HANDLE hSwitch,
                                     IN BOOL   fEnable );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchForceReceive( IN HANDLE hSwitch );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchForceReceive)( IN HANDLE hSwitch );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchForceTransmit( IN HANDLE hSwitch );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchForceTransmit)( IN HANDLE hSwitch );


//
// CAN message switch client types
//
#define CANMSGSWITCH_CLIENT_NONE  0  // undefined
#define CANMSGSWITCH_CLIENT_RXMB  1  // receive message buffer
#define CANMSGSWITCH_CLIENT_TXMB  2  // transmit message buffer
#define CANMSGSWITCH_CLIENT_RXMQ  3  // receive message queue
#define CANMSGSWITCH_CLIENT_TXMQ  4  // transmit message queue
#define CANMSGSWITCH_CLIENT_RXMS  5  // receive message set
#define CANMSGSWITCH_CLIENT_TXMS  6  // transmit message set
#define CANMSGSWITCH_CLIENT_RXSS  7  // receive signal set
#define CANMSGSWITCH_CLIENT_TXSS  8  // transmit signal set

EXTERN_C HRESULT VCIAPI
  canMsgSwitchCreateClient( IN  HANDLE  hSwitch,
                            IN  UINT32  dwType,
                            OUT PHANDLE phClient );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchCreateClient)( IN  HANDLE  hSwitch,
                                         IN  UINT32  dwType,
                                         OUT PHANDLE phClient );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchDeleteClient( IN HANDLE hClient );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchDeleteClient)( IN HANDLE hClient );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchAttachClient( IN HANDLE    hSwitch,
                            IN IUnknown* pClient );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchAttachClient)( IN HANDLE    hSwitch,
                                         IN IUnknown* pClient );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchDetachClient( IN HANDLE    hSwitch,
                            IN IUnknown* pClient );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchDetachClient)( IN HANDLE    hSwitch,
                                         IN IUnknown* pClient );

//
// CAN message switch specific receive buffer client functions
//

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgBufferEnable( IN HANDLE hRBuffer,
                                IN UINT32 dwCanId );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgBufferEnable)( IN HANDLE hRBuffer,
                                             IN UINT32 dwCanId );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgBufferDisable( IN HANDLE hRBuffer );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgBufferDisable)( IN HANDLE hRBuffer );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgBufferRead( IN  HANDLE   hRBuffer,
                              OUT PCANMSG2 pCanMsg,
                              OUT PUINT32  pdwRxCnt );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgBufferRead)( IN  HANDLE   hRBuffer,
                                           OUT PCANMSG2 pCanMsg,
                                           OUT PUINT32  pdwRxCnt );

//
// CAN message switch specific transmit buffer client functions
//

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgBufferEnable(IN HANDLE hTBuffer,
                               IN UINT32 dwCanId,
                               IN UINT32 dwMode,
                               IN UINT32 dwTime );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgBufferEnable)(IN HANDLE hTBuffer,
                                            IN UINT32 dwCanId,
                                            IN UINT32 dwMode,
                                            IN UINT32 dwTime );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgBufferDisable( IN HANDLE hTBuffer );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgBufferDisable)( IN HANDLE hTBuffer );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgBufferWrite( IN HANDLE   hTBuffer,
                               IN PCANMSG2 pCanMsg );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgBufferWrite)( IN HANDLE   hTBuffer,
                                            IN PCANMSG2 pCanMsg );

//
// CAN message switch specific receive queue client functions
//

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgQueueEnable( IN HANDLE hRQueue,
                               IN UINT32 dwCanId, 
                               IN UINT16 wDepth );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgQueueEnable)( IN HANDLE hRQueue,
                                            IN UINT32 dwCanId, 
                                            IN UINT16 wDepth );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgQueueDisable( IN HANDLE hRQueue );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgQueueDisable)( IN HANDLE hRQueue );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgQueueRead( IN  HANDLE  hRQueue,
                             OUT CANMSG2 aCanMsg[],
                             IN  UINT32  dwCount,
                             OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgQueueRead)( IN  HANDLE  hRQueue,
                                          OUT CANMSG2 aCanMsg[],
                                          IN  UINT32  dwCount,
                                          OUT PUINT32 pdwDone );

//
// CAN message switch specific transmit queue client functions
//

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgQueueEnable( IN HANDLE hTQueue,
                               IN UINT32 dwCanId,
                               IN UINT16 wDepth,
                               IN UINT32 dwMode,
                               IN UINT32 dwTime );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgQueueEnable)( IN HANDLE hTQueue,
                                            IN UINT32 dwCanId,
                                            IN UINT16 wDepth,
                                            IN UINT32 dwMode,
                                            IN UINT32 dwTime );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgQueueDisable( IN HANDLE hTQueue );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgQueueDisable)( IN HANDLE hTQueue );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgQueueWrite( IN  HANDLE  hTQueue,
                              IN  CANMSG2 aCanMsg[],
                              IN  UINT32  dwCount,
                              OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgQueueWrite)( IN  HANDLE  hTQueue,
                                           IN  CANMSG2 aCanMsg[],
                                           IN  UINT32  dwCount,
                                           OUT PUINT32 pdwDone );

//
// CAN message switch specific receive message set client functions
//

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgSetEnable( IN HANDLE hRMsgSet,
                             IN UINT32 adwCanId[],
                             IN UINT16 awDepth[],
                             IN UINT32 dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgSetEnable)( IN HANDLE hRMsgSet,
                                          IN UINT32 adwCanId[],
                                          IN UINT16 awDepth[],
                                          IN UINT32 dwCount );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgSetDisable( IN HANDLE hRxMsgSet );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgSetDisable)( IN HANDLE hRxMsgSet );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchRMsgSetRead( IN  HANDLE  hRMsgSet,
                           OUT CANMSG2 aCanMsg[],
                           OUT UINT32  adwRxCnt[],
                           IN  UINT32  dwFirst,
                           IN  UINT32  dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchRMsgSetRead)( IN  HANDLE  hRMsgSet,
                                        OUT CANMSG2 aCanMsg[],
                                        OUT UINT32  adwRxCnt[],
                                        IN  UINT32  dwFirst,
                                        IN  UINT32  dwCount );

//
// CAN message switch specific transmit message set client functions
//

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgSetEnable( IN HANDLE hTMsgSet,
                             IN UINT32 adwCanId[],
                             IN UINT16 awDepth[],
                             IN UINT32 adwMode[],
                             IN UINT32 adwTime[],
                             IN UINT32 dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgSetEnable)( IN HANDLE hTMsgSet,
                                          IN UINT32 adwCanId[],
                                          IN UINT16 awDepth[],
                                          IN UINT32 adwMode[],
                                          IN UINT32 adwTime[],
                                          IN UINT32 dwCount );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgSetDisable( IN HANDLE hTMsgSet );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgSetDisable)( IN HANDLE hTMsgSet );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchTMsgSetWrite( IN  HANDLE  hTMsgSet,
                            IN  CANMSG2 aCanMsg[],
                            IN  BOOL8   afValid[],
                            OUT BOOL8   afDone[],
                            IN  UINT32  dwFirst,
                            IN  UINT32  dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchTMsgSetWrite)( IN  HANDLE  hTMsgSet,
                                         IN  CANMSG2 aCanMsg[],
                                         IN  BOOL8   afValid[],
                                         OUT BOOL8   afDone[],
                                         IN  UINT32  dwFirst,
                                         IN  UINT32  dwCount );

//
// CAN message switch specific signal set client functions
//
EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetLoadDBA( IN  HANDLE hSigSet,
                             IN  PCHAR  pszFile,
                             IN  PCHAR  pszPara,
                             IN  PCHAR  pszSigs,
                             IN  UINT16 awDepth[],
                             OUT HANDLE ahSigId[],
                             IN  UINT32 dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetLoadDBA)( IN  HANDLE hSigSet,
                                          IN  PCHAR  pszFile,
                                          IN  PCHAR  pszPara,
                                          IN  PCHAR  pszSigs,
                                          IN  UINT16 awDepth[],
                                          OUT HANDLE ahSigId[],
                                          IN  UINT32 dwCount );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetLoadDBW( IN  HANDLE hSigSet,
                             IN  PWCHAR pszFile,
                             IN  PWCHAR pszPara,
                             IN  PWCHAR pszSigs,
                             IN  UINT16 awDepth[],
                             OUT HANDLE ahSigId[],
                             IN  UINT32 dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetLoadDBW)( IN  HANDLE hSigSet,
                                          IN  PWCHAR pszFile,
                                          IN  PWCHAR pszPara,
                                          IN  PWCHAR pszSigs,
                                          IN  UINT16 awDepth[],
                                          OUT HANDLE ahSigId[],
                                          IN  UINT32 dwCount );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetSaveDBA( IN  HANDLE  hSigSet,
                             IN  PCHAR   pszFile,
                             IN  PCHAR   pszPara,
                             IN  HANDLE  ahSigId[],
                             IN  UINT32  dwCount,
                             OUT PUINT32 pdwSave );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetSaveDBA)( IN  HANDLE  hSigSet,
                                          IN  PCHAR   pszFile,
                                          IN  PCHAR   pszPara,
                                          IN  HANDLE  ahSigId[],
                                          IN  UINT32  dwCount,
                                          OUT PUINT32 pdwSave );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetSaveDBW( IN  HANDLE  hSigSet,
                             IN  PWCHAR  pszFile,
                             IN  PWCHAR  pszPara,
                             IN  HANDLE  ahSigId[],
                             IN  UINT32  dwCount,
                             OUT PUINT32 pdwSave );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetSaveDBW)( IN  HANDLE  hSigSet,
                                          IN  PWCHAR  pszFile,
                                          IN  PWCHAR  pszPara,
                                          IN  HANDLE  ahSigId[],
                                          IN  UINT32  dwCount,
                                          OUT PUINT32 pdwSave );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetCloseDB( HANDLE hSigSet );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetCloseDB)( HANDLE hSigSet );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetGetAttrA( IN  HANDLE  hSigSet,
                              IN  HANDLE  hSigId,
                              IN  UINT32  dwAttr,
                              OUT PVOID   pvData,
                              IN  UINT32  dwSize,
                              OUT PUINT32 pdwOut );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetGetAttrA)( IN  HANDLE  hSigSet,
                                           IN  HANDLE  hSigId,
                                           IN  UINT32  dwAttr,
                                           OUT PVOID   pvData,
                                           IN  UINT32  dwSize,
                                           OUT PUINT32 pdwOut );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetGetAttrW( IN  HANDLE  hSigSet,
                              IN  HANDLE  hSigId,
                              IN  UINT32  dwAttr,
                              OUT PVOID   pvData,
                              IN  UINT32  dwSize,
                              OUT PUINT32 pdwOut );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetGetAttrW)( IN  HANDLE  hSigSet,
                                           IN  HANDLE  hSigId,
                                           IN  UINT32  dwAttr,
                                           OUT PVOID   pvData,
                                           IN  UINT32  dwSize,
                                           OUT PUINT32 pdwOut );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetSetAttrA( IN HANDLE hSigSet,
                              IN HANDLE hSigId,
                              IN UINT32 dwAttr,
                              IN PVOID  pvData,
                              IN UINT32 dwSize );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetSetAttrA)( IN HANDLE hSigSet,
                                           IN HANDLE hSigId,
                                           IN UINT32 dwAttr,
                                           IN PVOID  pvData,
                                           IN UINT32 dwSize );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetSetAttrW( IN HANDLE hSigSet,
                              IN HANDLE hSigId,
                              IN UINT32 dwAttr,
                              IN PVOID  pvData,
                              IN UINT32 dwSize );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetSetAttrW)( IN HANDLE hSigSet,
                                           IN HANDLE hSigId,
                                           IN UINT32 dwAttr,
                                           IN PVOID  pvData,
                                           IN UINT32 dwSize );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetConvert( IN  HANDLE    hSigSet,
                             IN  UINT32    dwMode,
                             IN  FSLSIGNAL aInSig[],
                             OUT FSLSIGNAL aOutSig[],
                             IN  UINT32    dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetConvert)( IN  HANDLE    hSigSet,
                                          IN  UINT32    dwMode,
                                          IN  FSLSIGNAL aInSig[],
                                          OUT FSLSIGNAL aOutSig[],
                                          IN  UINT32    dwCount );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetEnable( IN HANDLE hSigSet );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetEnable)( IN HANDLE hSigSet );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetDisable( IN HANDLE hSigSet );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetDisable)( IN HANDLE hSigSet );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetRead( IN  HANDLE    hSigSet,
                          IN  BOOL      fConvert,
                          OUT FSLSIGNAL aSignal[],
                          OUT UINT32    adwRxCnt[] OPTIONAL,
                          IN  UINT32    dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetRead)( IN  HANDLE    hSigSet,
                                       IN  BOOL      fConvert,
                                       OUT FSLSIGNAL aSignal[],
                                       OUT UINT32    adwRxCnt[] OPTIONAL,
                                       IN  UINT32    dwCount );

EXTERN_C HRESULT VCIAPI
  canMsgSwitchSigSetWrite( IN  HANDLE    hSigSet,
                           IN  BOOL      fConvert,
                           IN  FSLSIGNAL aSignal[],
                           IN  BOOL8     afReady[],
                           OUT BOOL8     afDone[] OPTIONAL,
                           IN  UINT32    dwCount );

typedef HRESULT
  (VCIAPI *PF_canMsgSwitchSigSetWrite)( IN  HANDLE    hSigSet,
                                        IN  BOOL      fConvert,
                                        IN  FSLSIGNAL aSignal[],
                                        IN  BOOL8     afReady[],
                                        OUT BOOL8     afDone[] OPTIONAL,
                                        IN  UINT32    dwCount );

#endif // USE_VCIFSL

/*****************************************************************************
 * LIN controller specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  linControlOpen( IN  HANDLE  hDevice,
                  IN  UINT32  dwLinNo,
                  OUT PHANDLE phLinCtl );

typedef HRESULT
  (VCIAPI *PF_linControlOpen)( IN  HANDLE  hDevice,
                               IN  UINT32  dwLinNo,
                               OUT PHANDLE phLinCtl );

EXTERN_C HRESULT VCIAPI
  linControlClose( IN HANDLE hLinCtl );

typedef HRESULT
  (VCIAPI *PF_linControlClose)( IN HANDLE hLinCtl );

EXTERN_C HRESULT VCIAPI
  linControlGetCaps( IN  HANDLE           hLinCtl,
                     OUT PLINCAPABILITIES pLinCaps );

typedef HRESULT
  (VCIAPI *PF_linControlGetCaps)( IN  HANDLE           hLinCtl,
                                  OUT PLINCAPABILITIES pLinCaps );

EXTERN_C HRESULT VCIAPI
  linControlGetStatus( IN  HANDLE         hLinCtl,
                       OUT PLINLINESTATUS pStatus );

typedef HRESULT
  (VCIAPI *PF_linControlGetStatus)( IN  HANDLE         hLinCtl,
                                    OUT PLINLINESTATUS pStatus );

EXTERN_C HRESULT VCIAPI
  linControlInitialize( IN HANDLE hLinCtl,
                        IN UINT8  bMode,
                        IN UINT16 wBitrate );

typedef HRESULT
  (VCIAPI *PF_linControlInitialize)( IN HANDLE hLinCtl,
                                     IN UINT8  bMode,
                                     IN UINT16 wBitrate );

EXTERN_C HRESULT VCIAPI
  linControlReset( IN HANDLE hLinCtl );

typedef HRESULT
  (VCIAPI *PF_linControlReset)( IN HANDLE hLinCtl );

EXTERN_C HRESULT VCIAPI
  linControlStart( IN HANDLE hLinCtl,
                   IN BOOL   fStart );

typedef HRESULT
  (VCIAPI *PF_linControlStart)( IN HANDLE hLinCtl,
                                IN BOOL   fStart );

EXTERN_C HRESULT VCIAPI
  linControlWriteMessage( IN HANDLE  hLinCtl,
                          IN BOOL    fSend,
                          IN PLINMSG pLinMsg );

typedef HRESULT
  (VCIAPI *PF_linControlWriteMessage)( IN HANDLE  hLinCtl,
                                       IN BOOL    fSend,
                                       IN PLINMSG pLinMsg );


/*****************************************************************************
 * LIN message monitor specific functions
 ****************************************************************************/

EXTERN_C HRESULT VCIAPI
  linMonitorOpen( IN  HANDLE  hDevice,
                  IN  UINT32  dwLinNo,
                  IN  BOOL    fExclusive,
                  OUT PHANDLE phLinMon );

typedef HRESULT
  (VCIAPI *PF_linMonitorOpen)( IN  HANDLE  hDevice,
                               IN  UINT32  dwLinNo,
                               IN  BOOL    fExclusive,
                               OUT PHANDLE phLinMon );

EXTERN_C HRESULT VCIAPI
  linMonitorClose( IN HANDLE hLinMon );

typedef HRESULT
  (VCIAPI *PF_linMonitorClose)( IN HANDLE hLinMon );

EXTERN_C HRESULT VCIAPI
  linMonitorGetCaps( IN  HANDLE           hLinMon,
                     OUT PLINCAPABILITIES pLinCaps );

typedef HRESULT
  (VCIAPI *PF_linMonitorGetCaps)( IN  HANDLE           hLinMon,
                                  OUT PLINCAPABILITIES pLinCaps );

EXTERN_C HRESULT VCIAPI
  linMonitorGetStatus( IN  HANDLE            hLinMon,
                       OUT PLINMONITORSTATUS pStatus );

typedef HRESULT
  (VCIAPI *PF_linMonitorGetStatus)( IN  HANDLE            hLinMon,
                                    OUT PLINMONITORSTATUS pStatus );

EXTERN_C HRESULT VCIAPI
  linMonitorInitialize( IN HANDLE hLinMon,
                        IN UINT16 wFifoSize,
                        IN UINT16 wThreshold );

typedef HRESULT
  (VCIAPI *PF_linMonitorInitialize)( IN HANDLE hLinMon,
                                     IN UINT16 wFifoSize,
                                     IN UINT16 wThreshold );

EXTERN_C HRESULT VCIAPI
  linMonitorActivate( IN HANDLE hLinMon,
                      IN BOOL   fEnable );

typedef HRESULT
  (VCIAPI *PF_linMonitorActivate)( IN HANDLE hLinMon,
                                   IN BOOL   fEnable );

EXTERN_C HRESULT VCIAPI
  linMonitorPeekMessage( IN  HANDLE  hLinMon,
                         OUT PLINMSG pLinMsg );

typedef HRESULT
  (VCIAPI *PF_linMonitorPeekMessage)( IN  HANDLE  hLinMon,
                                      OUT PLINMSG pLinMsg );

EXTERN_C HRESULT VCIAPI
  linMonitorPeekMsgMult( IN  HANDLE  hLinMon,
                         OUT LINMSG  aLinMsg[],
                         IN  UINT32  dwCount,
                         OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_linMonitorPeekMsgMult)( IN  HANDLE  hLinMon,
                                      OUT LINMSG  aLinMsg[],
                                      IN  UINT32  dwCount,
                                      OUT PUINT32 pdwDone );

EXTERN_C HRESULT VCIAPI
  linMonitorWaitRxEvent( IN HANDLE hLinMon,
                         IN UINT32 dwTimeout );

typedef HRESULT
  (VCIAPI *PF_linMonitorWaitRxEvent)( IN HANDLE hLinMon,
                                      IN UINT32 dwTimeout );

EXTERN_C HRESULT VCIAPI
  linMonitorReadMessage( IN  HANDLE  hLinMon,
                         IN  UINT32  dwTimeout,
                         OUT PLINMSG pLinMsg );

typedef HRESULT
  (VCIAPI *PF_linMonitorReadMessage)( IN  HANDLE  hLinMon,
                                      IN  UINT32  dwTimeout,
                                      OUT PLINMSG pLinMsg );

EXTERN_C HRESULT VCIAPI
  linMonitorReadMsgMult( IN  HANDLE  hLinMon,
                         IN  UINT32  dwTimeout,
                         OUT LINMSG  aLinMsg[],
                         IN  UINT32  dwCount,
                         OUT PUINT32 pdwDone );

typedef HRESULT
  (VCIAPI *PF_linMonitorReadMsgMult)( IN  HANDLE  hLinMon,
                                      IN  UINT32  dwTimeout,
                                      OUT LINMSG  aLinMsg[],
                                      IN  UINT32  dwCount,
                                      OUT PUINT32 pdwDone );

#endif //_VCINPL2_H_
