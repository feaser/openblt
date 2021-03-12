/*---------------------------------------------------------------------------------------
| File:
|   xlLoadlib.c
| Project:
|   xlCANcontrol, xlMOSTView, xlFlexDemo
|
| Description:
|   For the dynamically linking of the XL Driver Library.
|   
|
|-----------------------------------------------------------------------------
| $Author: vishsh $    $Date: 2016-05-19 08:45:09 +0200 (Thu, 19 May 2016) $   $Revision: 57918 $
| $Id: xlLoadlib.c 57918 2016-05-19 06:45:09Z vishsh $
|-----------------------------------------------------------------------------
| Copyright (c) 2014 by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/

#include <windows.h>
#ifndef _MFC_VER
# include <stdio.h>
#endif
#include <vxlapi.h>

///////////////////////////////////////////////////////////////////////////
//Global variables
////////////////////////////////////////////////////////////////////////////

// function pointers

  XLGETAPPLCONFIG                xlGetApplConfig = NULL;
  XLSETAPPLCONFIG                xlSetApplConfig = NULL;
  XLGETDRIVERCONFIG              xlGetDriverConfig = NULL;
  XLGETCHANNELINDEX              xlGetChannelIndex = NULL;
  XLGETCHANNELMASK               xlGetChannelMask = NULL;
  XLOPENPORT                     xlOpenPort = NULL;
  XLSETTIMERRATE                 xlSetTimerRate = NULL;
  XLRESETCLOCK                   xlResetClock = NULL;
  XLSETNOTIFICATION              xlSetNotification = NULL;
  XLFLUSHRECEIVEQUEUE            xlFlushReceiveQueue = NULL;
  XLGETRECEIVEQUEUELEVEL         xlGetReceiveQueueLevel = NULL;
  XLACTIVATECHANNEL              xlActivateChannel = NULL;
  XLRECEIVE                      xlReceive = NULL;                        
  XLGETEVENTSTRING               xlGetEventString = NULL;
  XLGETERRORSTRING               xlGetErrorString = NULL;
  XLGETSYNCTIME                  xlGetSyncTime = NULL;
  XLGENERATESYNCPULSE            xlGenerateSyncPulse = NULL;
  XLPOPUPHWCONFIG                xlPopupHwConfig = NULL;
  XLDEACTIVATECHANNEL            xlDeactivateChannel = NULL;
  XLCLOSEPORT                    xlClosePort = NULL;
  XLSETTIMERBASEDNOTIFY          xlSetTimerBasedNotify = NULL;  
  XLSETTIMERRATEANDCHANNEL       xlSetTimerRateAndChannel = NULL;
  XLGETLICENSEINFO               xlGetLicenseInfo = NULL;
  XLGETCHANNELTIME               xlGetChannelTime = NULL;
  XLGETREMOTEDRIVERCONFIG        xlGetRemoteDriverConfig = NULL;

  // CAN specific functions
  XLCANSETCHANNELOUTPUT          xlCanSetChannelOutput = NULL;    
  XLCANSETCHANNELMODE            xlCanSetChannelMode = NULL; 
  XLCANSETRECEIVEMODE            xlCanSetReceiveMode = NULL; 
  XLCANSETCHANNELTRANSCEIVER     xlCanSetChannelTransceiver = NULL;
  XLCANSETCHANNELPARAMS          xlCanSetChannelParams = NULL;           
  XLCANSETCHANNELPARAMSC200      xlCanSetChannelParamsC200 = NULL;        
  XLCANSETCHANNELBITRATE         xlCanSetChannelBitrate = NULL;   
  XLCANSETCHANNELACCEPTANCE      xlCanSetChannelAcceptance = NULL;       
  XLCANADDACCEPTANCERANGE        xlCanAddAcceptanceRange = NULL;    
  XLCANREMOVEACCEPTANCERANGE     xlCanRemoveAcceptanceRange = NULL; 
  XLCANRESETACCEPTANCE           xlCanResetAcceptance = NULL;   
  XLCANREQUESTCHIPSTATE          xlCanRequestChipState = NULL; 
  XLCANFLUSHTRANSMITQUEUE        xlCanFlushTransmitQueue = NULL;           
  XLCANTRANSMIT                  xlCanTransmit = NULL;

  // CAN-FD specific functions
  XLCANFDSETCONFIGURATION        xlCanFdSetConfiguration = NULL;
  XLCANTRANSMITEX                xlCanTransmitEx = NULL;
  XLCANRECEIVE                   xlCanReceive = NULL;
  XLCANGETEVENTSTRING            xlCanGetEventString = NULL;

  // LIN specific functions
  XLLINSETCHANNELPARAMS          xlLinSetChannelParams = NULL;
  XLLINSETDLC                    xlLinSetDLC = NULL;
  XLLINSETSLAVE                  xlLinSetSlave = NULL;
  XLLINSENDREQUEST               xlLinSendRequest = NULL;
  XLLINSETSLEEPMODE              xlLinSetSleepMode = NULL;
  XLLINWAKEUP                    xlLinWakeUp = NULL;
  XLLINSETCHECKSUM               xlLinSetChecksum = NULL;
  XLLINSWITCHSLAVE               xlLinSwitchSlave = NULL;


  // IOcab specific functions
  XLDAIOSETPWMOUTPUT             xlDAIOSetPWMOutput = NULL; 
  XLDAIOSETDIGITALOUTPUT         xlDAIOSetDigitalOutput = NULL;
  XLDAIOSETANALOGOUTPUT          xlDAIOSetAnalogOutput = NULL;
  XLDAIOREQUESTMEASUREMENT       xlDAIORequestMeasurement = NULL;
  XLDAIOSETDIGITALPARAMETERS     xlDAIOSetDigitalParameters = NULL;
  XLDAIOSETANALOGPARAMETERS      xlDAIOSetAnalogParameters = NULL;
  XLDAIOSETANALOGTRIGGER         xlDAIOSetAnalogTrigger = NULL;
  XLDAIOSETMEASUREMENTFREQUENCY  xlDAIOSetMeasurementFrequency = NULL;

  // MOST specific functions
  XLFP_MOSTRECEIVE               xlMostReceive = NULL; 
  XLFP_MOSTCTRLTRANSMIT          xlMostCtrlTransmit = NULL;
  XLFP_MOSTSWITCHEVENTSOURCES    xlMostSwitchEventSources = NULL;
  XLFP_MOSTSETALLBYPASS          xlMostSetAllBypass = NULL;
  XLFP_MOSTGETALLBYPASS          xlMostGetAllBypass = NULL;
  XLFP_MOSTSETTIMINGMODE         xlMostSetTimingMode = NULL;
  XLFP_MOSTGETTIMINGMODE         xlMostGetTimingMode = NULL;
  XLFP_MOSTSETFREQUENCY          xlMostSetFrequency = NULL;
  XLFP_MOSTGETFREQUENCY          xlMostGetFrequency = NULL;
  XLFP_MOSTWRITEREGISTER         xlMostWriteRegister = NULL;
  XLFP_MOSTREADREGISTER          xlMostReadRegister = NULL;
  XLFP_MOSTWRITEREGISTERBIT      xlMostWriteRegisterBit = NULL;
  XLFP_MOSTSYNCGETALLOCTABLE     xlMostSyncGetAllocTable = NULL;
  XLFP_MOSTCTRLSYNCAUDIO         xlMostCtrlSyncAudio = NULL;
  XLFP_MOSTCTRLSYNCAUDIOEX       xlMostCtrlSyncAudioEx = NULL;
  XLFP_MOSTSYNCVOLUME            xlMostSyncVolume = NULL;
  XLFP_MOSTSYNCMUTE              xlMostSyncMute = NULL;
  XLFP_MOSTSYNCGETVOLUMESTATUS   xlMostSyncGetVolumeStatus = NULL;
  XLFP_MOSTSYNCGETMUTESTATUS     xlMostSyncGetMuteStatus = NULL;
  XLFP_MOSTGETRXLIGHT            xlMostGetRxLight = NULL; 
  XLFP_MOSTSETTXLIGHT            xlMostSetTxLight = NULL; 
  XLFP_MOSTGETTXLIGHT            xlMostGetTxLight = NULL; 
  XLFP_MOSTSETLIGHTPOWER         xlMostSetLightPower = NULL; 
  XLFP_MOSTGETLOCKSTATUS         xlMostGetLockStatus = NULL; 
  XLFP_MOSTGENERATELIGHTERROR    xlMostGenerateLightError = NULL;
  XLFP_MOSTGENERATELOCKERROR     xlMostGenerateLockError = NULL;
  XLFP_MOSTCTRLRXBUFFER          xlMostCtrlRxBuffer = NULL;
  XLFP_MOSTTWINKLEPOWERLED       xlMostTwinklePowerLed = NULL;
  XLFP_MOSTASYNCTRANSMIT         xlMostAsyncTransmit = NULL; 
  XLFP_MOSTCTRLCONFIGUREBUSLOAD  xlMostCtrlConfigureBusload = NULL;
  XLFP_MOSTCTRLGENERATEBUSLOAD   xlMostCtrlGenerateBusload = NULL;
  XLFP_MOSTASYNCCONFIGUREBUSLOAD xlMostAsyncConfigureBusload = NULL;
  XLFP_MOSTASYNCGENERATEBUSLOAD  xlMostAsyncGenerateBusload = NULL;
  XLFP_MOSTSTREAMOPEN            xlMostStreamOpen = NULL;
  XLFP_MOSTSTREAMCLOSE           xlMostStreamClose = NULL;
  XLFP_MOSTSTREAMSTART           xlMostStreamStart = NULL;
  XLFP_MOSTSTREAMSTOP            xlMostStreamStop = NULL;
  XLFP_MOSTSTREAMBUFFERALLOCATE  xlMostStreamBufferAllocate = NULL;
  XLFP_MOSTSTREAMBUFFERDEALLOCATEALL xlMostStreamBufferDeallocateAll = NULL;
  XLFP_MOSTSTREAMBUFFERSETNEXT   xlMostStreamBufferSetNext = NULL;
  XLFP_MOSTSTREAMGETINFO         xlMostStreamGetInfo = NULL;
  XLFP_MOSTSTREAMBUFFERCLEARALL  xlMostStreamBufferClearAll = NULL;

  // FlexRay specific functions
  XLFP_FRSETCONFIGURATION        xlFrSetConfiguration = NULL; 
  XLFP_FRGETCHANNELCONFIGURATION xlFrGetChannelConfiguration = NULL;
  XLFP_FRSETMODE                 xlFrSetMode = NULL;               
  XLFP_FRINITSTARTUPANDSYNC      xlFrInitStartupAndSync = NULL;         
  XLFP_FRSETUPSYMBOLWINDOW       xlFrSetupSymbolWindow = NULL;     
  XLFP_FRRECEIVE                 xlFrReceive = NULL;               
  XLFP_FRTRANSMIT                xlFrTransmit = NULL;              
  XLFP_FRSETTRANSCEIVERMODE      xlFrSetTransceiverMode = NULL; 
  XLFP_FRSENDSYMBOLWINDOW        xlFrSendSymbolWindow = NULL; 
  XLFP_FRACTIVATESPY             xlFrActivateSpy = NULL;
  XLFP_FRSETACCEPTANCEFILTER     xlFrSetAcceptanceFilter = NULL;              

  // MOST150 specific functions
  XLFP_MOST150SWITCHEVENTSOURCES     xlMost150SwitchEventSources        = NULL;              
  XLFP_MOST150SETDEVICEMODE          xlMost150SetDeviceMode             = NULL;              
  XLFP_MOST150GETDEVICEMODE          xlMost150GetDeviceMode             = NULL;              
  XLFP_MOST150SETSPDIFMODE           xlMost150SetSPDIFMode              = NULL;              
  XLFP_MOST150GETSPDIFMODE           xlMost150GetSPDIFMode              = NULL;              
  XLFP_MOST150RECEIVE                xlMost150Receive                   = NULL;
  XLFP_MOST150SETSPECIALNODEINFO     xlMost150SetSpecialNodeInfo        = NULL;              
  XLFP_MOST150GETSPECIALNODEINFO     xlMost150GetSpecialNodeInfo        = NULL;              
  XLFP_MOST150SETFREQUENCY           xlMost150SetFrequency              = NULL;              
  XLFP_MOST150GETFREQUENCY           xlMost150GetFrequency              = NULL;              
  XLFP_MOST150CTRLTRANSMIT           xlMost150CtrlTransmit              = NULL;              
  XLFP_MOST150ASYNCTRANSMIT          xlMost150AsyncTransmit             = NULL;              
  XLFP_MOST150ETHTRANSMIT            xlMost150EthernetTransmit          = NULL;              
  XLFP_MOST150GETSYSTEMLOCK          xlMost150GetSystemLockFlag         = NULL;              
  XLFP_MOST150GETSHUTDOWN            xlMost150GetShutdownFlag           = NULL;              
  XLFP_MOST150SHUTDOWN               xlMost150Shutdown                  = NULL;              
  XLFP_MOST150STARTUP                xlMost150Startup                   = NULL;              
  XLFP_MOST150GETALLOCTABLE          xlMost150SyncGetAllocTable         = NULL;              
  XLFP_MOST150CTRLSYNCAUDIO          xlMost150CtrlSyncAudio             = NULL;              
  XLFP_MOST150SYNCSETVOLUME          xlMost150SyncSetVolume             = NULL;              
  XLFP_MOST150SYNCGETVOLUME          xlMost150SyncGetVolume             = NULL;              
  XLFP_MOST150SYNCSETMUTE            xlMost150SyncSetMute               = NULL;              
  XLFP_MOST150SYNCGETMUTE            xlMost150SyncGetMute               = NULL;              
  XLFP_MOST150GETLIGHTLOCKSTATUS     xlMost150GetRxLightLockStatus      = NULL;              
  XLFP_MOST150SETTXLIGHT             xlMost150SetTxLight                = NULL;              
  XLFP_MOST150GETTXLIGHT             xlMost150GetTxLight                = NULL;              
  XLFP_MOST150SETTXLIGHTPOWER        xlMost150SetTxLightPower           = NULL;              
  XLFP_MOST150GENLIGHTERROR          xlMost150GenerateLightError        = NULL;              
  XLFP_MOST150GENLOCKERROR           xlMost150GenerateLockError         = NULL; 
  XLFP_MOST150CONFIGURERXBUFFER      xlMost150ConfigureRxBuffer         = NULL;    
  XLFP_MOST150CTRLCONFIGLOAD         xlMost150CtrlConfigureBusload      = NULL;              
  XLFP_MOST150CTRLGENLOAD            xlMost150CtrlGenerateBusload       = NULL;              
  XLFP_MOST150ASYNCCONFIGLOAD        xlMost150AsyncConfigureBusload     = NULL;              
  XLFP_MOST150ASYNCGENLOAD           xlMost150AsyncGenerateBusload      = NULL;              
  XLFP_MOST150SETECLLINE             xlMost150SetECLLine                = NULL;              
  XLFP_MOST150SETECLTERMINATION      xlMost150SetECLTermination         = NULL;              
  XLFP_MOST150TWINKLEPOWERLED        xlMost150TwinklePowerLed           = NULL;
  XLFP_MOST150GETECLINFO             xlMost150GetECLInfo                = NULL;
  XLFP_MOST150STREAMOPEN             xlMost150StreamOpen                = NULL;
  XLFP_MOST150STREAMCLOSE            xlMost150StreamClose               = NULL;
  XLFP_MOST150STREAMSTART            xlMost150StreamStart               = NULL;
  XLFP_MOST150STREAMSTOP             xlMost150StreamStop                = NULL;
  XLFP_MOST150STREAMTRANSMITDATA     xlMost150StreamTransmitData        = NULL;
  XLFP_MOST150STREAMCLEARTXFIFO      xlMost150StreamClearTxFifo         = NULL;
  XLFP_MOST150STREAMGETINFO          xlMost150StreamGetInfo             = NULL;
  XLFP_MOST150STREAMINITRXFIFO       xlMost150StreamInitRxFifo          = NULL;
  XLFP_MOST150STREAMRECEIVEDATA      xlMost150StreamReceiveData         = NULL;
  XLFP_MOST150GENERATEBYPASSSTRESS   xlMost150GenerateBypassStress      = NULL;
  XLFP_MOST150ECLCONFIGURESEQ        xlMost150EclConfigureSeq           = NULL;
  XLFP_MOST150ECLGENERATESEQ         xlMost150EclGenerateSeq            = NULL;
  XLFP_MOST150SETECLGLITCHFILTER     xlMost150SetECLGlitchFilter        = NULL;
  XLFP_MOST150SETSSORESULT           xlMost150SetSSOResult              = NULL;
  XLFP_MOST150GETSSORESULT           xlMost150GetSSOResult              = NULL;

  // Ethernet specific funtions
  XLFP_ETHSETCONFIG                  xlEthSetConfig                     = NULL; 
  XLFP_ETHGETCONFIG                  xlEthGetConfig                     = NULL;
  XLFP_ETHRECEIVE                    xlEthReceive                       = NULL;
  XLFP_ETHSETBYPASS                  xlEthSetBypass                     = NULL;
  XLFP_ETHTWINKLESTATUSLED           xlEthTwinkleStatusLed              = NULL;
  XLFP_ETHTRANSMIT                   xlEthTransmit                      = NULL;

  // IOpiggy specific functions
  XLIOSETTRIGGERMODE                 xlIoSetTriggerMode                 = NULL; 
  XLIOSETDIGITALOUTPUT               xlIoSetDigitalOutput               = NULL; 
  XLIOCONFIGUREPORTS                 xlIoConfigurePorts                 = NULL; 
  XLIOSETDIGINTHRESHOLD              xlIoSetDigInThreshold              = NULL; 
  XLIOSETDIGOUTLEVEL                 xlIoSetDigOutLevel                 = NULL; 
  XLIOSETANALOGOUTPUT                xlIoSetAnalogOutput                = NULL; 
  XLIOSTARTSAMPLING                  xlIoStartSampling                  = NULL; 

  // Remote specific functions
  XLGETREMOTEDEVICEINFO              xlGetRemoteDeviceInfo              = NULL;
  XLRELEASEREMOTEDEVICEINFO          xlReleaseRemoteDeviceInfo          = NULL;
  XLADDREMOTEDEVICE                  xlAddRemoteDevice                  = NULL;
  XLREMOVEREMOTEDEVICE               xlRemoveRemoteDevice               = NULL;
  XLUPDATEREMOTEDEVICEINFO           xlUpdateRemoteDeviceInfo           = NULL;
  XLGETREMOTEHWINFO                  xlGetRemoteHwInfo                  = NULL;

  // A429 specific functions
  XLFP_A429RECEIVE                   xlA429Receive                      = NULL;
  XLFP_A429SETCHANNELPARAMS          xlA429SetChannelParams             = NULL;
  XLFP_A429TRANSMIT                  xlA429Transmit                     = NULL;

  // Keyman license dongle specific functions
  XLFP_GETKEYMANBOXES                xlGetKeymanBoxes                   = NULL;
  XLFP_GETKEYMANINFO                 xlGetKeymanInfo                    = NULL;

  //Local variables
  static XLCLOSEDRIVER               xlDllCloseDriver = NULL;
  static XLOPENDRIVER                xlDllOpenDriver = NULL;
  
  static HINSTANCE                   hxlDll;
 
////////////////////////////////////////////////////////////////////////////

//! xlLoadLibrary()

//! Loads API functions from DLL
//!
////////////////////////////////////////////////////////////////////////////

XLstatus xlLoadLibrary(char const * library){

  if (!hxlDll)
    hxlDll = LoadLibrary(library);
  
  if (!hxlDll){
#ifdef _MFC_VER
  #ifdef WIN64
      MessageBox(NULL,"Dynamic XL Driver Library - not found (vxlapi64.dll)","XL API",MB_OK|MB_ICONEXCLAMATION);
  #else
      MessageBox(NULL,"Dynamic XL Driver Library - not found (vxlapi.dll)","XL API",MB_OK|MB_ICONEXCLAMATION);
  #endif
#else
  #ifdef WIN64
    printf("Dynamic XL Driver Library - not found (vxlapi64.dll)\n");
  #else
    printf("Dynamic XL Driver Library - not found (vxlapi.dll)\n");
  #endif
#endif
    return XL_ERROR;
  } else {

    unsigned int NotFoundAll = 0;
    
    if ( (xlDllOpenDriver                 = (XLOPENDRIVER)GetProcAddress(hxlDll,"xlOpenDriver") )==NULL)                                              NotFoundAll= 1;
    if ( (xlDllCloseDriver                = (XLCLOSEDRIVER)GetProcAddress(hxlDll,"xlCloseDriver") )==NULL)                                            NotFoundAll= 2;
                                                                                                                                                       
    // bus independed functions                                                                                                                        
    if ( (xlGetApplConfig                 = (XLGETAPPLCONFIG)GetProcAddress(hxlDll,"xlGetApplConfig") )==NULL)                                        NotFoundAll= 3;
    if ( (xlSetApplConfig                 = (XLSETAPPLCONFIG)GetProcAddress(hxlDll,"xlSetApplConfig") )==NULL)                                        NotFoundAll= 4;
    if ( (xlGetDriverConfig               = (XLGETDRIVERCONFIG)GetProcAddress(hxlDll,"xlGetDriverConfig") )==NULL)                                    NotFoundAll= 5;
    if ( (xlGetChannelIndex               = (XLGETCHANNELINDEX)GetProcAddress(hxlDll,"xlGetChannelIndex") )==NULL)                                    NotFoundAll= 6;
    if ( (xlGetChannelMask                = (XLGETCHANNELMASK)GetProcAddress(hxlDll,"xlGetChannelMask") )==NULL)                                      NotFoundAll= 7;
    if ( (xlOpenPort                      = (XLOPENPORT)GetProcAddress(hxlDll,"xlOpenPort") )==NULL)                                                  NotFoundAll= 8;    
    if ( (xlSetTimerRate                  = (XLSETTIMERRATE)GetProcAddress(hxlDll,"xlSetTimerRate") )==NULL)                                          NotFoundAll= 9;    
    if ( (xlResetClock                    = (XLRESETCLOCK)GetProcAddress(hxlDll,"xlResetClock") )==NULL)                                              NotFoundAll=10;
    if ( (xlSetNotification               = (XLSETNOTIFICATION)GetProcAddress(hxlDll,"xlSetNotification") )==NULL)                                    NotFoundAll=11;
    if ( (xlFlushReceiveQueue             = (XLFLUSHRECEIVEQUEUE)GetProcAddress(hxlDll,"xlFlushReceiveQueue") )==NULL)                                NotFoundAll=12;
    if ( (xlGetReceiveQueueLevel          = (XLGETRECEIVEQUEUELEVEL)GetProcAddress(hxlDll,"xlGetReceiveQueueLevel") )==NULL)                          NotFoundAll=13;
    if ( (xlActivateChannel               = (XLACTIVATECHANNEL)GetProcAddress(hxlDll,"xlActivateChannel") )==NULL)                                    NotFoundAll=14;
    if ( (xlReceive                       = (XLRECEIVE)GetProcAddress(hxlDll,"xlReceive") )==NULL)                                                    NotFoundAll=15;
    if ( (xlGetEventString                = (XLGETEVENTSTRING)GetProcAddress(hxlDll,"xlGetEventString") )==NULL)                                      NotFoundAll=16;
    if ( (xlGetErrorString                = (XLGETERRORSTRING)GetProcAddress(hxlDll,"xlGetErrorString") )==NULL)                                      NotFoundAll=17;
    if ( (xlGenerateSyncPulse             = (XLGENERATESYNCPULSE)GetProcAddress(hxlDll,"xlGenerateSyncPulse") )==NULL)                                NotFoundAll=18;
    if ( (xlGetSyncTime                   = (XLGETSYNCTIME) GetProcAddress(hxlDll,"xlGetSyncTime") )==NULL)                                           NotFoundAll=19;
    if ( (xlPopupHwConfig                 = (XLPOPUPHWCONFIG)GetProcAddress(hxlDll,"xlPopupHwConfig") )==NULL)                                        NotFoundAll=20;
    if ( (xlDeactivateChannel             = (XLDEACTIVATECHANNEL)GetProcAddress(hxlDll,"xlDeactivateChannel") )==NULL)                                NotFoundAll=21;
    if ( (xlClosePort                     = (XLCLOSEPORT )GetProcAddress(hxlDll,"xlClosePort") )==NULL)                                               NotFoundAll=22;
    if ( (xlSetTimerBasedNotify           = (XLSETTIMERBASEDNOTIFY)GetProcAddress(hxlDll,"xlSetTimerBasedNotify") )==NULL)                            NotFoundAll=110;
    if ( (xlSetTimerRateAndChannel        = (XLSETTIMERRATEANDCHANNEL) GetProcAddress(hxlDll, "xlSetTimerRateAndChannel") )==NULL)                    NotFoundAll=111;
    if ( (xlGetLicenseInfo                = (XLGETLICENSEINFO) GetProcAddress(hxlDll, "xlGetLicenseInfo") )==NULL)                                    NotFoundAll=112;
    if ( (xlGetChannelTime                = (XLGETCHANNELTIME)GetProcAddress(hxlDll,"xlGetChannelTime") )==NULL)                                      NotFoundAll=113;
    if(  (xlGetRemoteDriverConfig         = (XLGETREMOTEDRIVERCONFIG) GetProcAddress(hxlDll, "xlGetRemoteDriverConfig") ) == NULL)                    NotFoundAll=114;
   
    // CAN specific functions
    if ( (xlCanSetChannelOutput           = (XLCANSETCHANNELOUTPUT)GetProcAddress(hxlDll,"xlCanSetChannelOutput") )==NULL)                            NotFoundAll=23;
    if ( (xlCanSetChannelMode             = (XLCANSETCHANNELMODE)GetProcAddress(hxlDll,"xlCanSetChannelMode") )==NULL)                                NotFoundAll=24;
    if ( (xlCanSetReceiveMode             = (XLCANSETRECEIVEMODE)GetProcAddress(hxlDll,"xlCanSetReceiveMode") ) == NULL)                              NotFoundAll=25; 
    if ( (xlCanSetChannelTransceiver      = (XLCANSETCHANNELTRANSCEIVER)GetProcAddress(hxlDll,"xlCanSetChannelTransceiver") ) == NULL)                NotFoundAll=26;
    if ( (xlCanSetChannelParams           = (XLCANSETCHANNELPARAMS)GetProcAddress(hxlDll,"xlCanSetChannelParams") ) == NULL)                          NotFoundAll=27;
    if ( (xlCanSetChannelParamsC200       = (XLCANSETCHANNELPARAMSC200)GetProcAddress(hxlDll,"xlCanSetChannelParamsC200") )==NULL)                    NotFoundAll=28;
    if ( (xlCanSetChannelBitrate          = (XLCANSETCHANNELBITRATE)GetProcAddress(hxlDll,"xlCanSetChannelBitrate") )==NULL)                          NotFoundAll=29; 
    if ( (xlCanSetChannelAcceptance       = (XLCANSETCHANNELACCEPTANCE)GetProcAddress(hxlDll,"xlCanSetChannelAcceptance") )==NULL)                    NotFoundAll=30;
    if ( (xlCanAddAcceptanceRange         = (XLCANADDACCEPTANCERANGE)GetProcAddress(hxlDll,"xlCanAddAcceptanceRange") )==NULL)                        NotFoundAll=31;
    if ( (xlCanRemoveAcceptanceRange      = (XLCANREMOVEACCEPTANCERANGE)GetProcAddress(hxlDll,"xlCanRemoveAcceptanceRange") )==NULL)                  NotFoundAll=32;
    if ( (xlCanResetAcceptance	          = (XLCANRESETACCEPTANCE)GetProcAddress(hxlDll,"xlCanResetAcceptance") )==NULL)                              NotFoundAll=33;
    if ( (xlCanRequestChipState           = (XLCANREQUESTCHIPSTATE)GetProcAddress(hxlDll,"xlCanRequestChipState") )==NULL)                            NotFoundAll=34;
    if ( (xlCanFlushTransmitQueue	        = (XLCANFLUSHTRANSMITQUEUE)GetProcAddress(hxlDll,"xlCanFlushTransmitQueue") )==NULL)                        NotFoundAll=35;  
    if ( (xlCanTransmit                   = (XLCANTRANSMIT)GetProcAddress(hxlDll,"xlCanTransmit") )==NULL)                                            NotFoundAll=36;                 
    
    // LIN specific functions
    if ( (xlLinSetChannelParams           = (XLLINSETCHANNELPARAMS)GetProcAddress(hxlDll,"xlLinSetChannelParams") )==NULL)                            NotFoundAll=37;
    if ( (xlLinSetDLC                     = (XLLINSETDLC)GetProcAddress(hxlDll,"xlLinSetDLC") )==NULL)                                                NotFoundAll=38;
    if ( (xlLinSetSlave                   = (XLLINSETSLAVE)GetProcAddress(hxlDll,"xlLinSetSlave") )==NULL)                                            NotFoundAll=39;
    if ( (xlLinSendRequest                = (XLLINSENDREQUEST)GetProcAddress(hxlDll,"xlLinSendRequest") )==NULL)                                      NotFoundAll=40;
    if ( (xlLinWakeUp                     = (XLLINWAKEUP) GetProcAddress(hxlDll,"xlLinWakeUp") )==NULL)                                               NotFoundAll=41;
    if ( (xlLinSetChecksum                = (XLLINSETCHECKSUM) GetProcAddress(hxlDll,"xlLinSetChecksum") )==NULL)                                     NotFoundAll=42;
    if ( (xlLinSwitchSlave                = (XLLINSWITCHSLAVE) GetProcAddress(hxlDll,"xlLinSwitchSlave") )==NULL)                                     NotFoundAll=43;
    if ( (xlLinSetSleepMode               = (XLLINSETSLEEPMODE) GetProcAddress(hxlDll,"xlLinSetSleepMode") )==NULL)                                   NotFoundAll=190;

    // IOcab specific functions
    if ( (xlDAIOSetPWMOutput              = (XLDAIOSETPWMOUTPUT)GetProcAddress(hxlDll,"xlDAIOSetPWMOutput") )==NULL)                                  NotFoundAll=44;
    if ( (xlDAIOSetDigitalOutput          = (XLDAIOSETDIGITALOUTPUT)GetProcAddress(hxlDll,"xlDAIOSetDigitalOutput") )==NULL)                          NotFoundAll=45;
    if ( (xlDAIOSetAnalogOutput           = (XLDAIOSETANALOGOUTPUT)GetProcAddress(hxlDll,"xlDAIOSetAnalogOutput") )==NULL)                            NotFoundAll=46;
    if ( (xlDAIORequestMeasurement        = (XLDAIOREQUESTMEASUREMENT)GetProcAddress(hxlDll,"xlDAIORequestMeasurement") )==NULL)                      NotFoundAll=47;
    if ( (xlDAIOSetDigitalParameters      = (XLDAIOSETDIGITALPARAMETERS) GetProcAddress(hxlDll,"xlDAIOSetDigitalParameters") )==NULL)                 NotFoundAll=48;
    if ( (xlDAIOSetAnalogParameters       = (XLDAIOSETANALOGPARAMETERS)GetProcAddress(hxlDll,"xlDAIOSetAnalogParameters") )==NULL)                    NotFoundAll=49;
    if ( (xlDAIOSetAnalogTrigger          = (XLDAIOSETANALOGTRIGGER) GetProcAddress(hxlDll,"xlDAIOSetAnalogTrigger") )==NULL)                         NotFoundAll=50;
    if ( (xlDAIOSetMeasurementFrequency   = (XLDAIOSETMEASUREMENTFREQUENCY)GetProcAddress(hxlDll,"xlDAIOSetMeasurementFrequency") )==NULL)            NotFoundAll=51;                        
    
    // MOST specific functions
    if ((xlMostReceive                    = (XLFP_MOSTRECEIVE) GetProcAddress(hxlDll,"xlMostReceive")) == NULL)                                       NotFoundAll=52;
    if ((xlMostCtrlTransmit               = (XLFP_MOSTCTRLTRANSMIT) GetProcAddress(hxlDll,"xlMostCtrlTransmit")) == NULL)                             NotFoundAll=53;
    if ((xlMostSwitchEventSources         = (XLFP_MOSTSWITCHEVENTSOURCES) GetProcAddress(hxlDll,"xlMostSwitchEventSources")) == NULL)                 NotFoundAll=54;
    if ((xlMostSetAllBypass               = (XLFP_MOSTSETALLBYPASS) GetProcAddress(hxlDll,"xlMostSetAllBypass")) == NULL)                             NotFoundAll=55;
    if ((xlMostGetAllBypass               = (XLFP_MOSTGETALLBYPASS) GetProcAddress(hxlDll,"xlMostGetAllBypass")) == NULL)                             NotFoundAll=56;
    if ((xlMostSetTimingMode              = (XLFP_MOSTSETTIMINGMODE) GetProcAddress(hxlDll,"xlMostSetTimingMode")) == NULL)                           NotFoundAll=57;
    if ((xlMostGetTimingMode              = (XLFP_MOSTGETTIMINGMODE) GetProcAddress(hxlDll,"xlMostGetTimingMode")) == NULL)                           NotFoundAll=58;
    if ((xlMostSetFrequency               = (XLFP_MOSTSETFREQUENCY) GetProcAddress(hxlDll,"xlMostSetFrequency")) == NULL)                             NotFoundAll=59;
    if ((xlMostGetFrequency               = (XLFP_MOSTGETFREQUENCY) GetProcAddress(hxlDll,"xlMostGetFrequency")) == NULL)                             NotFoundAll=60;
    if ((xlMostWriteRegister              = (XLFP_MOSTWRITEREGISTER) GetProcAddress(hxlDll,"xlMostWriteRegister")) == NULL)                           NotFoundAll=61;
    if ((xlMostReadRegister               = (XLFP_MOSTREADREGISTER) GetProcAddress(hxlDll,"xlMostReadRegister")) == NULL)                             NotFoundAll=62;
    if ((xlMostWriteRegisterBit           = (XLFP_MOSTWRITEREGISTERBIT) GetProcAddress(hxlDll,"xlMostWriteRegisterBit")) == NULL)                     NotFoundAll=63;
    if ((xlMostSyncGetAllocTable          = (XLFP_MOSTSYNCGETALLOCTABLE) GetProcAddress(hxlDll,"xlMostSyncGetAllocTable")) == NULL)                   NotFoundAll=64;
    if ((xlMostCtrlSyncAudio              = (XLFP_MOSTCTRLSYNCAUDIO) GetProcAddress(hxlDll,"xlMostCtrlSyncAudio")) == NULL)                           NotFoundAll=65;
    if ((xlMostCtrlSyncAudioEx            = (XLFP_MOSTCTRLSYNCAUDIOEX) GetProcAddress(hxlDll,"xlMostCtrlSyncAudioEx")) == NULL)                       NotFoundAll=66;
    if ((xlMostSyncVolume                 = (XLFP_MOSTSYNCVOLUME) GetProcAddress(hxlDll,"xlMostSyncVolume")) == NULL)                                 NotFoundAll=67;
    if ((xlMostSyncMute                   = (XLFP_MOSTSYNCMUTE) GetProcAddress(hxlDll,"xlMostSyncMute")) == NULL)                                     NotFoundAll=68;
    if ((xlMostSyncGetVolumeStatus        = (XLFP_MOSTSYNCGETVOLUMESTATUS) GetProcAddress(hxlDll,"xlMostSyncGetVolumeStatus")) == NULL)               NotFoundAll=69;
    if ((xlMostSyncGetMuteStatus          = (XLFP_MOSTSYNCGETMUTESTATUS) GetProcAddress(hxlDll,"xlMostSyncGetMuteStatus")) == NULL)                   NotFoundAll=70;
    if ((xlMostGetRxLight                 = (XLFP_MOSTGETRXLIGHT) GetProcAddress(hxlDll,"xlMostGetRxLight")) == NULL)                                 NotFoundAll=71;
    if ((xlMostSetTxLight                 = (XLFP_MOSTSETTXLIGHT) GetProcAddress(hxlDll,"xlMostSetTxLight")) == NULL)                                 NotFoundAll=72;
    if ((xlMostGetTxLight                 = (XLFP_MOSTGETTXLIGHT) GetProcAddress(hxlDll,"xlMostGetTxLight")) == NULL)                                 NotFoundAll=73;
    if ((xlMostSetLightPower              = (XLFP_MOSTSETLIGHTPOWER) GetProcAddress(hxlDll,"xlMostSetLightPower")) == NULL)                           NotFoundAll=74;
    if ((xlMostGetLockStatus              = (XLFP_MOSTGETLOCKSTATUS) GetProcAddress(hxlDll,"xlMostGetLockStatus")) == NULL)                           NotFoundAll=75;
    if ((xlMostGenerateLightError         = (XLFP_MOSTGENERATELIGHTERROR) GetProcAddress(hxlDll,"xlMostGenerateLightError")) == NULL)                 NotFoundAll=76;
    if ((xlMostGenerateLockError          = (XLFP_MOSTGENERATELOCKERROR) GetProcAddress(hxlDll,"xlMostGenerateLockError")) == NULL)                   NotFoundAll=77;
    if ((xlMostCtrlRxBuffer               = (XLFP_MOSTCTRLRXBUFFER) GetProcAddress(hxlDll,"xlMostCtrlRxBuffer")) == NULL)                             NotFoundAll=78;
    if ((xlMostTwinklePowerLed            = (XLFP_MOSTTWINKLEPOWERLED) GetProcAddress(hxlDll,"xlMostTwinklePowerLed")) == NULL)                       NotFoundAll=79;
    if ((xlMostAsyncTransmit              = (XLFP_MOSTASYNCTRANSMIT) GetProcAddress(hxlDll,"xlMostAsyncTransmit")) == NULL)                           NotFoundAll=80;
    if ((xlMostCtrlConfigureBusload       = (XLFP_MOSTCTRLCONFIGUREBUSLOAD) GetProcAddress(hxlDll,"xlMostCtrlConfigureBusload")) == NULL)             NotFoundAll=81;
    if ((xlMostCtrlGenerateBusload        = (XLFP_MOSTCTRLGENERATEBUSLOAD) GetProcAddress(hxlDll,"xlMostCtrlGenerateBusload")) == NULL)               NotFoundAll=82;
    if ((xlMostAsyncConfigureBusload      = (XLFP_MOSTASYNCCONFIGUREBUSLOAD) GetProcAddress(hxlDll,"xlMostAsyncConfigureBusload")) == NULL)           NotFoundAll=83;
    if ((xlMostAsyncGenerateBusload       = (XLFP_MOSTASYNCGENERATEBUSLOAD) GetProcAddress(hxlDll,"xlMostAsyncGenerateBusload")) == NULL)             NotFoundAll=84;   
    if ((xlMostStreamOpen                 = (XLFP_MOSTSTREAMOPEN) GetProcAddress(hxlDll,"xlMostStreamOpen")) == NULL)                                 NotFoundAll=85;   
    if ((xlMostStreamClose                = (XLFP_MOSTSTREAMCLOSE) GetProcAddress(hxlDll,"xlMostStreamClose")) == NULL)                               NotFoundAll=86;   
    if ((xlMostStreamStart                = (XLFP_MOSTSTREAMSTART) GetProcAddress(hxlDll,"xlMostStreamStart")) == NULL)                               NotFoundAll=87;   
    if ((xlMostStreamStop                 = (XLFP_MOSTSTREAMSTOP) GetProcAddress(hxlDll,"xlMostStreamStop")) == NULL)                                 NotFoundAll=88;
    if ((xlMostStreamBufferAllocate       = (XLFP_MOSTSTREAMBUFFERALLOCATE) GetProcAddress(hxlDll,"xlMostStreamBufferAllocate")) == NULL)             NotFoundAll=89;
    if ((xlMostStreamBufferDeallocateAll  = (XLFP_MOSTSTREAMBUFFERDEALLOCATEALL) GetProcAddress(hxlDll,"xlMostStreamBufferDeallocateAll")) == NULL)   NotFoundAll=90;
    if ((xlMostStreamBufferSetNext        = (XLFP_MOSTSTREAMBUFFERSETNEXT) GetProcAddress(hxlDll,"xlMostStreamBufferSetNext")) == NULL)               NotFoundAll=91;
    if ((xlMostStreamGetInfo              = (XLFP_MOSTSTREAMGETINFO) GetProcAddress(hxlDll,"xlMostStreamGetInfo")) == NULL)                           NotFoundAll=92;
    if ((xlMostStreamBufferClearAll       = (XLFP_MOSTSTREAMBUFFERCLEARALL) GetProcAddress(hxlDll,"xlMostStreamBufferClearAll")) == NULL)             NotFoundAll=93;
    
    // FlexRay specific functions
    if ((xlFrSetConfiguration             = (XLFP_FRSETCONFIGURATION) GetProcAddress(hxlDll,"xlFrSetConfiguration")) == NULL)                         NotFoundAll=100;  
    if ((xlFrGetChannelConfiguration      = (XLFP_FRGETCHANNELCONFIGURATION) GetProcAddress(hxlDll,"xlFrGetChannelConfiguration")) == NULL)           NotFoundAll=101;  
    if ((xlFrSetMode                      = (XLFP_FRSETMODE) GetProcAddress(hxlDll,"xlFrSetMode")) == NULL)                                           NotFoundAll=102;  
    if ((xlFrInitStartupAndSync           = (XLFP_FRINITSTARTUPANDSYNC) GetProcAddress(hxlDll,"xlFrInitStartupAndSync")) == NULL)                     NotFoundAll=103;  
    if ((xlFrSetupSymbolWindow            = (XLFP_FRSETUPSYMBOLWINDOW) GetProcAddress(hxlDll,"xlFrSetupSymbolWindow")) == NULL)                       NotFoundAll=104;  
    if ((xlFrReceive                      = (XLFP_FRRECEIVE) GetProcAddress(hxlDll,"xlFrReceive")) == NULL)                                           NotFoundAll=105;  
    if ((xlFrTransmit                     = (XLFP_FRTRANSMIT) GetProcAddress(hxlDll,"xlFrTransmit")) == NULL)                                         NotFoundAll=106;  
    if ((xlFrSetTransceiverMode           = (XLFP_FRSETTRANSCEIVERMODE) GetProcAddress(hxlDll,"xlFrSetTransceiverMode")) == NULL)                     NotFoundAll=107; 
    if ((xlFrSendSymbolWindow             = (XLFP_FRSENDSYMBOLWINDOW) GetProcAddress(hxlDll,"xlFrSendSymbolWindow")) == NULL)                         NotFoundAll=108; 
    if ((xlFrActivateSpy                  = (XLFP_FRACTIVATESPY) GetProcAddress(hxlDll,"xlFrActivateSpy")) == NULL)                                   NotFoundAll=109;
    if ((xlFrSetAcceptanceFilter          = (XLFP_FRSETACCEPTANCEFILTER) GetProcAddress(hxlDll,"xlFrSetAcceptanceFilter")) == NULL)                   NotFoundAll=120;

    // MOST150 specific functions
    if ((xlMost150SwitchEventSources      = (XLFP_MOST150SWITCHEVENTSOURCES) GetProcAddress(hxlDll,"xlMost150SwitchEventSources")) == NULL)           NotFoundAll=110; 
    if ((xlMost150SetDeviceMode           = (XLFP_MOST150SETDEVICEMODE) GetProcAddress(hxlDll,"xlMost150SetDeviceMode")) == NULL)                     NotFoundAll=112;
    if ((xlMost150GetDeviceMode           = (XLFP_MOST150GETDEVICEMODE) GetProcAddress(hxlDll,"xlMost150GetDeviceMode")) == NULL)                     NotFoundAll=113;
    if ((xlMost150SetSPDIFMode            = (XLFP_MOST150SETSPDIFMODE) GetProcAddress(hxlDll,"xlMost150SetSPDIFMode")) == NULL)                       NotFoundAll=114;
    if ((xlMost150GetSPDIFMode            = (XLFP_MOST150GETSPDIFMODE) GetProcAddress(hxlDll,"xlMost150GetSPDIFMode")) == NULL)                       NotFoundAll=115;
    if ((xlMost150Receive                 = (XLFP_MOST150RECEIVE) GetProcAddress(hxlDll,"xlMost150Receive")) == NULL)                                 NotFoundAll=130;
    if ((xlMost150SetSpecialNodeInfo      = (XLFP_MOST150SETSPECIALNODEINFO) GetProcAddress(hxlDll,"xlMost150SetSpecialNodeInfo")) == NULL)           NotFoundAll=131;
    if ((xlMost150GetSpecialNodeInfo      = (XLFP_MOST150GETSPECIALNODEINFO) GetProcAddress(hxlDll,"xlMost150GetSpecialNodeInfo")) == NULL)           NotFoundAll=132;
    if ((xlMost150SetFrequency            = (XLFP_MOST150SETFREQUENCY) GetProcAddress(hxlDll,"xlMost150SetFrequency")) == NULL)                       NotFoundAll=133;
    if ((xlMost150GetFrequency            = (XLFP_MOST150GETFREQUENCY) GetProcAddress(hxlDll,"xlMost150GetFrequency")) == NULL)                       NotFoundAll=134;
    if ((xlMost150CtrlTransmit            = (XLFP_MOST150CTRLTRANSMIT) GetProcAddress(hxlDll,"xlMost150CtrlTransmit")) == NULL)                       NotFoundAll=135;
    if ((xlMost150AsyncTransmit           = (XLFP_MOST150ASYNCTRANSMIT) GetProcAddress(hxlDll,"xlMost150AsyncTransmit")) == NULL)                     NotFoundAll=136;
    if ((xlMost150EthernetTransmit        = (XLFP_MOST150ETHTRANSMIT) GetProcAddress(hxlDll,"xlMost150EthernetTransmit")) == NULL)                    NotFoundAll=137;
    if ((xlMost150GetSystemLockFlag       = (XLFP_MOST150GETSYSTEMLOCK) GetProcAddress(hxlDll,"xlMost150GetSystemLockFlag")) == NULL)                 NotFoundAll=138;
    if ((xlMost150GetShutdownFlag         = (XLFP_MOST150GETSHUTDOWN) GetProcAddress(hxlDll,"xlMost150GetShutdownFlag")) == NULL)                     NotFoundAll=139;
    if ((xlMost150Shutdown                = (XLFP_MOST150SHUTDOWN) GetProcAddress(hxlDll,"xlMost150Shutdown")) == NULL)                               NotFoundAll=140;
    if ((xlMost150Startup                 = (XLFP_MOST150STARTUP) GetProcAddress(hxlDll,"xlMost150Startup")) == NULL)                                 NotFoundAll=141;
    if ((xlMost150SyncGetAllocTable       = (XLFP_MOST150GETALLOCTABLE) GetProcAddress(hxlDll,"xlMost150SyncGetAllocTable")) == NULL)                 NotFoundAll=142;
    if ((xlMost150CtrlSyncAudio           = (XLFP_MOST150CTRLSYNCAUDIO) GetProcAddress(hxlDll,"xlMost150CtrlSyncAudio")) == NULL)                     NotFoundAll=143;
    if ((xlMost150SyncSetVolume           = (XLFP_MOST150SYNCSETVOLUME) GetProcAddress(hxlDll,"xlMost150SyncSetVolume")) == NULL)                     NotFoundAll=144;
    if ((xlMost150SyncGetVolume           = (XLFP_MOST150SYNCGETVOLUME) GetProcAddress(hxlDll,"xlMost150SyncGetVolume")) == NULL)                     NotFoundAll=145;
    if ((xlMost150SyncSetMute             = (XLFP_MOST150SYNCSETMUTE) GetProcAddress(hxlDll,"xlMost150SyncSetMute")) == NULL)                         NotFoundAll=146;
    if ((xlMost150SyncGetMute             = (XLFP_MOST150SYNCGETMUTE) GetProcAddress(hxlDll,"xlMost150SyncGetMute")) == NULL)                         NotFoundAll=147;
    if ((xlMost150GetRxLightLockStatus    = (XLFP_MOST150GETLIGHTLOCKSTATUS) GetProcAddress(hxlDll,"xlMost150GetRxLightLockStatus")) == NULL)         NotFoundAll=148;
    if ((xlMost150SetTxLight              = (XLFP_MOST150SETTXLIGHT) GetProcAddress(hxlDll,"xlMost150SetTxLight")) == NULL)                           NotFoundAll=149;
    if ((xlMost150GetTxLight              = (XLFP_MOST150GETTXLIGHT) GetProcAddress(hxlDll,"xlMost150GetTxLight")) == NULL)                           NotFoundAll=150;
    if ((xlMost150SetTxLightPower         = (XLFP_MOST150SETTXLIGHTPOWER) GetProcAddress(hxlDll,"xlMost150SetTxLightPower")) == NULL)                 NotFoundAll=151;
    if ((xlMost150GenerateLightError      = (XLFP_MOST150GENLIGHTERROR) GetProcAddress(hxlDll,"xlMost150GenerateLightError")) == NULL)                NotFoundAll=152;
    if ((xlMost150GenerateLockError       = (XLFP_MOST150GENLOCKERROR) GetProcAddress(hxlDll,"xlMost150GenerateLockError")) == NULL)                  NotFoundAll=153;
    if ((xlMost150ConfigureRxBuffer       = (XLFP_MOST150CONFIGURERXBUFFER) GetProcAddress(hxlDll,"xlMost150ConfigureRxBuffer")) == NULL)             NotFoundAll=154;
    if ((xlMost150CtrlConfigureBusload    = (XLFP_MOST150CTRLCONFIGLOAD) GetProcAddress(hxlDll,"xlMost150CtrlConfigureBusload")) == NULL)             NotFoundAll=155;
    if ((xlMost150CtrlGenerateBusload     = (XLFP_MOST150CTRLGENLOAD) GetProcAddress(hxlDll,"xlMost150CtrlGenerateBusload")) == NULL)                 NotFoundAll=156;
    if ((xlMost150AsyncConfigureBusload   = (XLFP_MOST150ASYNCCONFIGLOAD) GetProcAddress(hxlDll,"xlMost150AsyncConfigureBusload")) == NULL)           NotFoundAll=157;
    if ((xlMost150AsyncGenerateBusload    = (XLFP_MOST150ASYNCGENLOAD) GetProcAddress(hxlDll,"xlMost150AsyncGenerateBusload")) == NULL)               NotFoundAll=158;
    if ((xlMost150SetECLLine              = (XLFP_MOST150SETECLLINE) GetProcAddress(hxlDll,"xlMost150SetECLLine")) == NULL)                           NotFoundAll=163;
    if ((xlMost150SetECLTermination       = (XLFP_MOST150SETECLTERMINATION) GetProcAddress(hxlDll,"xlMost150SetECLTermination")) == NULL)             NotFoundAll=164;
    if ((xlMost150TwinklePowerLed         = (XLFP_MOST150TWINKLEPOWERLED) GetProcAddress(hxlDll,"xlMost150TwinklePowerLed")) == NULL)                 NotFoundAll=165;
    if ((xlMost150GetECLInfo              = (XLFP_MOST150GETECLINFO) GetProcAddress(hxlDll,"xlMost150GetECLInfo")) == NULL)                           NotFoundAll=166;
    if ((xlMost150StreamOpen              = (XLFP_MOST150STREAMOPEN) GetProcAddress(hxlDll,"xlMost150StreamOpen")) == NULL)                           NotFoundAll=167;
    if ((xlMost150StreamClose             = (XLFP_MOST150STREAMCLOSE) GetProcAddress(hxlDll,"xlMost150StreamClose")) == NULL)                         NotFoundAll=168;
    if ((xlMost150StreamStart             = (XLFP_MOST150STREAMSTART) GetProcAddress(hxlDll,"xlMost150StreamStart")) == NULL)                         NotFoundAll=169;
    if ((xlMost150StreamStop              = (XLFP_MOST150STREAMSTOP) GetProcAddress(hxlDll,"xlMost150StreamStop")) == NULL)                           NotFoundAll=170;
    if ((xlMost150StreamTransmitData      = (XLFP_MOST150STREAMTRANSMITDATA) GetProcAddress(hxlDll,"xlMost150StreamTransmitData")) == NULL)           NotFoundAll=171;
    if ((xlMost150StreamClearTxFifo       = (XLFP_MOST150STREAMCLEARTXFIFO) GetProcAddress(hxlDll,"xlMost150StreamClearTxFifo")) == NULL)             NotFoundAll=172;
    if ((xlMost150StreamGetInfo           = (XLFP_MOST150STREAMGETINFO) GetProcAddress(hxlDll,"xlMost150StreamGetInfo")) == NULL)                     NotFoundAll=173;
    if ((xlMost150StreamInitRxFifo        = (XLFP_MOST150STREAMINITRXFIFO) GetProcAddress(hxlDll,"xlMost150StreamInitRxFifo")) == NULL)               NotFoundAll=174;
    if ((xlMost150StreamReceiveData       = (XLFP_MOST150STREAMRECEIVEDATA) GetProcAddress(hxlDll,"xlMost150StreamReceiveData")) == NULL)             NotFoundAll=175;
    if ((xlMost150GenerateBypassStress    = (XLFP_MOST150GENERATEBYPASSSTRESS) GetProcAddress(hxlDll,"xlMost150GenerateBypassStress")) == NULL)       NotFoundAll=176;
    if ((xlMost150EclConfigureSeq         = (XLFP_MOST150ECLCONFIGURESEQ) GetProcAddress(hxlDll,"xlMost150EclConfigureSeq")) == NULL)                 NotFoundAll=177;
    if ((xlMost150EclGenerateSeq          = (XLFP_MOST150ECLGENERATESEQ) GetProcAddress(hxlDll,"xlMost150EclGenerateSeq")) == NULL)                   NotFoundAll=178;
    if ((xlMost150SetECLGlitchFilter      = (XLFP_MOST150SETECLGLITCHFILTER) GetProcAddress(hxlDll,"xlMost150SetECLGlitchFilter")) == NULL)           NotFoundAll=179;
    if ((xlMost150SetSSOResult            = (XLFP_MOST150SETSSORESULT) GetProcAddress(hxlDll,"xlMost150SetSSOResult")) == NULL)                       NotFoundAll=180;
    if ((xlMost150GetSSOResult            = (XLFP_MOST150GETSSORESULT) GetProcAddress(hxlDll,"xlMost150GetSSOResult")) == NULL)                       NotFoundAll=181;

    // CAN-FD specific functions
    if ((xlCanFdSetConfiguration          = (XLCANFDSETCONFIGURATION) GetProcAddress(hxlDll,"xlCanFdSetConfiguration")) == NULL)                      NotFoundAll=200;
    if ((xlCanTransmitEx                  = (XLCANTRANSMITEX) GetProcAddress(hxlDll,"xlCanTransmitEx")) == NULL)                                      NotFoundAll=201;
    if ((xlCanReceive                     = (XLCANRECEIVE) GetProcAddress(hxlDll,"xlCanReceive")) == NULL)                                            NotFoundAll=202;
    if ((xlCanGetEventString              = (XLCANGETEVENTSTRING) GetProcAddress(hxlDll,"xlCanGetEventString")) == NULL)                              NotFoundAll=203;

    // IO piggy specific functions
    if ((xlIoSetTriggerMode               = (XLIOSETTRIGGERMODE) GetProcAddress(hxlDll,"xlIoSetTriggerMode")) == NULL)                                NotFoundAll=250;
    if ((xlIoSetDigitalOutput             = (XLIOSETDIGITALOUTPUT) GetProcAddress(hxlDll,"xlIoSetDigitalOutput")) == NULL)                            NotFoundAll=251;
    if ((xlIoConfigurePorts               = (XLIOCONFIGUREPORTS) GetProcAddress(hxlDll,"xlIoConfigurePorts")) == NULL)                                NotFoundAll=252;
    if ((xlIoSetDigInThreshold            = (XLIOSETDIGINTHRESHOLD) GetProcAddress(hxlDll,"xlIoSetDigInThreshold")) == NULL)                          NotFoundAll=253;
    if ((xlIoSetDigOutLevel               = (XLIOSETDIGOUTLEVEL) GetProcAddress(hxlDll,"xlIoSetDigOutLevel")) == NULL)                                NotFoundAll=254;
    if ((xlIoSetAnalogOutput              = (XLIOSETANALOGOUTPUT) GetProcAddress(hxlDll,"xlIoSetAnalogOutput")) == NULL)                              NotFoundAll=255;
    if ((xlIoStartSampling                = (XLIOSTARTSAMPLING) GetProcAddress(hxlDll,"xlIoStartSampling")) == NULL)                                  NotFoundAll=256;

    // Ethernet specific functions
    if ((xlEthSetConfig                   = (XLFP_ETHSETCONFIG) GetProcAddress(hxlDll,"xlEthSetConfig")) == NULL)                                     NotFoundAll=300;
    if ((xlEthGetConfig                   = (XLFP_ETHGETCONFIG) GetProcAddress(hxlDll,"xlEthGetConfig")) == NULL)                                     NotFoundAll=301;
    if ((xlEthReceive                     = (XLFP_ETHRECEIVE) GetProcAddress(hxlDll,"xlEthReceive")) == NULL)                                         NotFoundAll=302;
    if ((xlEthSetBypass                   = (XLFP_ETHSETBYPASS) GetProcAddress(hxlDll,"xlEthSetBypass")) == NULL)                                     NotFoundAll=303;
    if ((xlEthTwinkleStatusLed            = (XLFP_ETHTWINKLESTATUSLED) GetProcAddress(hxlDll,"xlEthTwinkleStatusLed")) == NULL)                       NotFoundAll=304;
    if ((xlEthTransmit                    = (XLFP_ETHTRANSMIT) GetProcAddress(hxlDll,"xlEthTransmit")) == NULL)                                       NotFoundAll=305;

    // Remove specifig functions
    if ((xlGetRemoteDeviceInfo            = (XLGETREMOTEDEVICEINFO) GetProcAddress(hxlDll,"xlGetRemoteDeviceInfo")) == NULL)                          NotFoundAll=400;
    if ((xlReleaseRemoteDeviceInfo        = (XLRELEASEREMOTEDEVICEINFO) GetProcAddress(hxlDll,"xlReleaseRemoteDeviceInfo")) == NULL)                  NotFoundAll=401;
    if ((xlAddRemoteDevice                = (XLADDREMOTEDEVICE) GetProcAddress(hxlDll,"xlAddRemoteDevice")) == NULL)                                  NotFoundAll=402;
    if ((xlRemoveRemoteDevice             = (XLREMOVEREMOTEDEVICE) GetProcAddress(hxlDll,"xlRemoveRemoteDevice")) == NULL)                            NotFoundAll=403;
    if ((xlUpdateRemoteDeviceInfo         = (XLUPDATEREMOTEDEVICEINFO) GetProcAddress(hxlDll,"xlUpdateRemoteDeviceInfo")) == NULL)                    NotFoundAll=405;
    if ((xlGetRemoteHwInfo                = (XLGETREMOTEHWINFO) GetProcAddress(hxlDll,"xlGetRemoteHwInfo")) == NULL)                                  NotFoundAll=406;

    // A429 specific functions
    if ((xlA429Receive                    = (XLFP_A429RECEIVE)GetProcAddress(hxlDll,"xlA429Receive") )==NULL)                                         NotFoundAll=500;      
    if ((xlA429SetChannelParams           = (XLFP_A429SETCHANNELPARAMS)GetProcAddress(hxlDll,"xlA429SetChannelParams") )==NULL)                       NotFoundAll=501;
    if ((xlA429Transmit                   = (XLFP_A429TRANSMIT)GetProcAddress(hxlDll,"xlA429Transmit") )==NULL)                                       NotFoundAll=502;

    // Keyman license dongle specific functions
    if ((xlGetKeymanBoxes                 = (XLFP_GETKEYMANBOXES)GetProcAddress(hxlDll,"xlGetKeymanBoxes") )==NULL)                                   NotFoundAll=600;      
    if ((xlGetKeymanInfo                  = (XLFP_GETKEYMANINFO)GetProcAddress(hxlDll,"xlGetKeymanInfo") )==NULL)                                     NotFoundAll=601;

    if (NotFoundAll) {
      return XL_ERROR;
    }
  }
  return XL_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

//! canUnloadLibrary()

//! Unload XL API DLL
//!
////////////////////////////////////////////////////////////////////////////

XLstatus xlUnloadLibrary(void){  
  if (hxlDll) {
    (void)FreeLibrary( hxlDll );
    hxlDll = 0;
  }
  return XL_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

//! xlOpenDriver()

//! Automatically loads DLL and then opens driver
//!
////////////////////////////////////////////////////////////////////////////

XLstatus xlOpenDriver(void){
  
#ifdef WIN64
  if (xlLoadLibrary("vxlapi64")!=XL_SUCCESS) return XL_ERR_CANNOT_OPEN_DRIVER;
#else
  if (xlLoadLibrary("vxlapi")!=XL_SUCCESS) return XL_ERR_CANNOT_OPEN_DRIVER;
#endif
  return xlDllOpenDriver();
}

////////////////////////////////////////////////////////////////////////////

//! xlCloseDriver()

//! Automatically close DLL
//!
////////////////////////////////////////////////////////////////////////////

XLstatus xlCloseDriver(void){
  (void)xlDllCloseDriver();
  return xlUnloadLibrary();
}

