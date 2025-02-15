////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, //
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 		        //
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.		    //
// 																				                                                //
//							                        Copyright (c) UIROBOT. 								            //
// 							                            All rights reserved.								                //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __LOADSIMCANFUNC_H__
#define __LOADSIMCANFUNC_H__
#if defined(_WIN32)
#include <windows.h>
#include "UISimCanFunc_RT.h"

extern GETLASTERR    GetLastErr;
extern SETDEVINDEX SetDevIndex;
extern GETUIMDEVIDLIST GetUimDevIdList;
extern SEARCHDEVICE SearchDevice;
extern GETUIMDEVINFO GetUimDevInfo;
extern OPENUIMDEV OpenUimDev;
extern CLOSEUIMDEV CloseUimDev;
extern UIMGROBREG UimGrobReg;
extern GETMDL GetMDL;
extern UIMENA UimEna;
extern UIMOFF UimOff;
extern SETSPD SetSPD;
extern UIMACK UimAck;
extern UIMFBK UimFbk;
extern UIMSFBK UimSfbk;
extern SETPOS SetPOS;
extern SETSTP SetSTP;
extern SETGOFF SetgOFF;
extern SETGCUR SetgCUR;
extern SETGORG SetgORG;
extern SETGDIR SetgDIR;
extern SETGSPD SetgSPD;
extern SETGSTP SetgSTP;
extern SETGPOS SetgPOS;
extern SETGQEC SetgQEC;
extern GETUIMMCFG GetUimMCFG;
extern SETUIMMCFG SetUimMCFG;
extern SETMACC SetmACC;
extern GETMACC GetmACC;
extern SETMDEC SetmDEC;
extern GETMDEC GetmDEC;
extern SETMMDS SetmMDS;
extern GETMMDS GetmMDS;
extern SETMMSS SetmMSS;
extern GETMMSS GetmMSS;
extern GETSTG GetSTG;
extern SETSTG SetSTG;
extern SETQEC SetQEC;
extern SETQER SetQER;
extern SETORG SetORG;
extern SETSQT SetSQT;
extern GETSQT GetSQT;
extern SETBDR SetBDR;
extern SETADR SetADR;
extern SETGMCS SetgMCS;
extern SETGDOUT SetgDOUT;
extern SETGACR SetgACR;
extern UIMSTORE UimStore;
extern SETACR SetACR;
extern GETACR GetACR;
extern SETMCS SetMCS;
extern GETMCS GetMCS;
extern SETCUR SetCUR;
extern GETCUR GetCUR;
extern GETSPD GetSPD;
extern GETSTP GetSTP;
extern GETQEC GetQEC;
extern GETPOS GetPOS;
/*
extern SETRPT SetRPT;
extern GETRPT GetRPT;
extern SETRPC SetRPC;
extern GETRPC GetRPC;
*/
extern SETATCONH SetATCONH;
extern GETATCONH GetATCONH;
extern SETATCONL SetATCONL;
extern GETATCONL GetATCONL;
extern SETBTR SetBTR;
extern GETBTR GetBTR;
extern GETSCFG GetSCFG;
extern SETS12CON SetS12CON;
extern GETS12CON GetS12CON;
extern SETS34CON SetS34CON;
extern GETS34CON GetS34CON;
extern SETDOUT SetDOUT;
extern GETDOUT GetDOUT;
extern UIMREGRTCNCALLBACK UIMRegRtcnCallBack;
extern SETUIDMCFG SetUIDMCFG;
extern GETUIDMCFG GetUIDMCFG;
extern SETIOCONFIG SetIOconfig;
extern GETIOCONFIG GetIOconfig;
extern SETDVAINFO SetDVAInfo;
extern GETDVAINFO GetDVAInfo;
extern GETAVAINFO GetAVAInfo;
extern SETPMB SetPMB;
extern GETPMB GetPMB;
extern SETPMD SetPMD;
extern GETPMD GetPMD;
extern SETPBR SetPBR;
extern GETPBR GetPBR;
extern SETDMA SetDMA;
extern GETDMA GetDMA;
extern SETOPR SetOPR;
extern OPRREAD OPRRead;
extern OPRREADPTR OPRReadPtr;

extern GETUIMICFG GetUimICFG;
extern SETUIMICFG SetUimICFG;
extern UIMADVENA UimAdvENA;
extern UIMGETADVENA UimGetAdvENA;
extern UIMADVSTORE UimAdvStore;
extern GETBLC GetBLC;
extern SETBLC SetBLC;
extern GETQER GetQER;
extern SETNRD SetNRD;
extern OPR_OPENANTENNA OPR_OpenAntenna;
extern OPR_CLOSEANTENNA OPR_CloseAntenna;
extern OPR_FINDRFID OPR_FindRFID;
extern OPR_CLOSERFID OPR_CloseRFID;
extern OPR_RFIDREAD OPR_RFIDRead;
extern OPR_RFIDWRITE OPR_RFIDWrite;
extern OPR_SETRFIDKEY OPR_SetRFIDKey;
extern OPR_GETRFIDBLKDATA OPR_GetRFIDBlkData;
extern OPR_SETRFIDBLKDATA OPR_SetRFIDBlkData;
extern OPR_GETRFIDCARDNUM OPR_GetRFIDCardNum;

extern FORCESTOPPROGRAM ForceStopProgram;



extern SEARCHGATEWAY SearchGateway;
extern OPENGATEWAY OpenGateway;
extern CLOSEGATEWAY CloseGateway;
extern GETLASTERR   GetSDKLastErr;


BOOL LoadSimCanFunc();
VOID FreeSimCanFunc();
#endif // end of _WIN32

#endif //__LOADSIMCANFUNC_H__