////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, //
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 		        //
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.		    //
// 																				                                                //
//							                        Copyright (c) UIROBOT. 								            //
// 							                            All rights reserved.								                //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadSimCanFunc.h"
#if defined(_WIN32)

#ifdef UNICODE
#define _T(x) L##x
#else
#define _T(x) x
#endif

#undef UNICODE
GETLASTERR    GetLastErr;
SETDEVINDEX SetDevIndex;
SEARCHDEVICE SearchDevice;
GETUIMDEVIDLIST GetUimDevIdList;
GETUIMDEVINFO GetUimDevInfo;
OPENUIMDEV OpenUimDev;
CLOSEUIMDEV CloseUimDev;
UIMGROBREG UimGrobReg;
GETMDL GetMDL;
UIMENA UimEna;
UIMOFF UimOff;
SETSPD SetSPD;
UIMACK UimAck;
UIMFBK UimFbk;
UIMSFBK UimSfbk;
SETPOS SetPOS;
SETSTP SetSTP;
SETGOFF SetgOFF;
SETGCUR SetgCUR;
SETGORG SetgORG;
SETGDIR SetgDIR;
SETGSPD SetgSPD;
SETGSTP SetgSTP;
SETGPOS SetgPOS;
SETGQEC SetgQEC;
GETUIMMCFG GetUimMCFG;
SETUIMMCFG SetUimMCFG;
SETMACC SetmACC;
GETMACC GetmACC;
SETMDEC SetmDEC;
GETMDEC GetmDEC;
SETMMDS SetmMDS;
GETMMDS GetmMDS;
SETMMSS SetmMSS;
GETMMSS GetmMSS;
GETSTG GetSTG;
SETSTG SetSTG;
SETQEC SetQEC;
SETQER SetQER;
SETORG SetORG;
SETSQT SetSQT;
GETSQT GetSQT;
SETBDR SetBDR;
SETADR SetADR;
SETGMCS SetgMCS;
SETGDOUT SetgDOUT;
SETGACR SetgACR;
UIMSTORE UimStore;
SETACR SetACR;
GETACR GetACR;
SETMCS SetMCS;
GETMCS GetMCS;
SETCUR SetCUR;
GETCUR GetCUR;
GETSPD GetSPD;
GETSTP GetSTP;
GETQEC GetQEC;
GETPOS GetPOS;
/*
SETRPT SetRPT;
GETRPT GetRPT;
SETRPC SetRPC;
GETRPC GetRPC;
*/
SETATCONH SetATCONH;
GETATCONH GetATCONH;
SETATCONL SetATCONL;
GETATCONL GetATCONL;
SETBTR SetBTR;
GETBTR GetBTR;
GETSCFG GetSCFG;
SETS12CON SetS12CON;
GETS12CON GetS12CON;
SETS34CON SetS34CON;
GETS34CON GetS34CON;
SETDOUT SetDOUT;
GETDOUT GetDOUT;
SETUIDMCFG SetUIDMCFG;
GETUIDMCFG GetUIDMCFG;
SETIOCONFIG SetIOconfig;
GETIOCONFIG GetIOconfig;
SETDVAINFO SetDVAInfo;
GETDVAINFO GetDVAInfo;
GETAVAINFO GetAVAInfo;
SETPMB SetPMB;
GETPMB GetPMB;
SETPMD SetPMD;
GETPMD GetPMD;
SETPBR SetPBR;
GETPBR GetPBR;
SETDMA SetDMA;
GETDMA GetDMA;
SETOPR SetOPR;
OPRREAD OPRRead;
OPRREADPTR OPRReadPtr;

GETUIMICFG GetUimICFG;
SETUIMICFG SetUimICFG;
UIMADVENA UimAdvENA;
UIMGETADVENA UimGetAdvENA;
UIMADVSTORE UimAdvStore;
GETBLC GetBLC;
SETBLC SetBLC;
GETQER GetQER;
SETNRD SetNRD;

OPR_OPENANTENNA OPR_OpenAntenna;
OPR_CLOSEANTENNA OPR_CloseAntenna;
OPR_FINDRFID OPR_FindRFID;
OPR_CLOSERFID OPR_CloseRFID;
OPR_RFIDREAD OPR_RFIDRead;
OPR_RFIDWRITE OPR_RFIDWrite;
OPR_SETRFIDKEY OPR_SetRFIDKey;
OPR_GETRFIDBLKDATA OPR_GetRFIDBlkData;
OPR_SETRFIDBLKDATA OPR_SetRFIDBlkData;
OPR_GETRFIDCARDNUM OPR_GetRFIDCardNum;

UIMREGRTCNCALLBACK UIMRegRtcnCallBack;
FORCESTOPPROGRAM ForceStopProgram;


SEARCHGATEWAY SearchGateway;
OPENGATEWAY OpenGateway;
CLOSEGATEWAY CloseGateway;
GETLASTERR   GetSDKLastErr;


HINSTANCE m_hInstDevFunc;
BOOL LoadSimCanFunc()
{
	m_hInstDevFunc= NULL;

	GetLastErr = NULL;

	SearchDevice = NULL;
	GetUimDevIdList = NULL;
	GetUimDevInfo = NULL;
	OpenUimDev = NULL;
	CloseUimDev = NULL;
	UimGrobReg = NULL;
	GetMDL = NULL;
	UimEna = NULL;
	UimOff = NULL;
	SetSPD = NULL;
	UimAck = NULL;
	UimFbk = NULL;
	UimSfbk = NULL;
	SetPOS = NULL;
	SetSTP = NULL;
	SetgOFF = NULL;
	SetgCUR = NULL;
	SetgORG = NULL;
	SetgDIR = NULL;
	SetgSPD = NULL;
	SetgSTP = NULL;
	SetgPOS = NULL;
	SetgQEC = NULL;
	GetUimMCFG = NULL;
	SetUimMCFG = NULL;
	SetmACC = NULL;
	GetmACC = NULL;
	SetmDEC = NULL;
	GetmDEC = NULL;
	SetmMDS = NULL;
	GetmMDS = NULL;
	SetmMSS = NULL;
	GetmMSS = NULL;
	GetSTG = NULL;
	SetSTG = NULL;
	SetQEC = NULL;
	SetQER = NULL;
	SetORG = NULL;
	SetSQT = NULL;
	GetSQT = NULL;
	SetBDR = NULL;
	SetADR = NULL;
	SetgMCS = NULL;
	SetgDOUT = NULL;
	SetgACR = NULL;
	UimStore = NULL;
	SetACR = NULL;
	GetACR = NULL;
	SetMCS = NULL;
	GetMCS = NULL;
	SetCUR = NULL;
	GetCUR = NULL;
	GetSPD = NULL;
	GetSTP = NULL;
	GetQEC = NULL;
	GetPOS = NULL;
	/*
	SetRPT= NULL;
	GetRPT= NULL;
	SetRPC= NULL;
	GetRPC= NULL;
	*/
	SetATCONH = NULL;
	GetATCONH = NULL;
	SetATCONL = NULL;
	GetATCONL = NULL;
	SetBTR = NULL;
	GetBTR = NULL;
	GetSCFG = NULL;
	SetS12CON = NULL;
	GetS12CON = NULL;
	SetS34CON = NULL;
	GetS34CON = NULL;
	SetDOUT = NULL;
	GetDOUT = NULL;
	SetUIDMCFG = NULL;
	GetUIDMCFG = NULL;
	SetIOconfig = NULL;
	GetIOconfig = NULL;
	SetDVAInfo = NULL;
	GetDVAInfo = NULL;
	GetAVAInfo = NULL;
	SetPMB = NULL;
	GetPMB = NULL;
	SetPMD = NULL;
	GetPMD = NULL;
	SetPBR = NULL;
	GetPBR = NULL;
	SetDMA = NULL;
	GetDMA = NULL;
	SetOPR = NULL;
	OPRRead = NULL;
	OPRReadPtr = NULL;
    SetDevIndex = NULL;
	SetNRD = NULL;

	GetUimICFG = NULL;
	SetUimICFG = NULL;
	UimAdvENA = NULL;
	UimGetAdvENA = NULL;
	UimAdvStore = NULL;
	GetBLC = NULL;
	SetBLC = NULL;
	GetQER = NULL;


	OPR_OpenAntenna = NULL;
	OPR_CloseAntenna = NULL;
	OPR_FindRFID = NULL;
	OPR_CloseRFID = NULL;
	OPR_RFIDRead = NULL;
	OPR_RFIDWrite = NULL;
	OPR_SetRFIDKey = NULL;
	OPR_GetRFIDBlkData = NULL;
	OPR_SetRFIDBlkData = NULL;
	OPR_GetRFIDCardNum = NULL;

	UIMRegRtcnCallBack = NULL;
	ForceStopProgram = NULL;



	 SearchGateway = NULL;
	 OpenGateway= NULL;
	 CloseGateway= NULL;
	   GetSDKLastErr= NULL;

	//装载UIROBOT提供的CAN网络驱动动态库
	m_hInstDevFunc = LoadLibrary(_T("UISimCanFunc.dll"));
	if (m_hInstDevFunc == NULL)
	{
		FreeLibrary(m_hInstDevFunc);
		return FALSE;
	}

	//装载库函数
	GetLastErr = (GETLASTERR)GetProcAddress(m_hInstDevFunc, "GetLastErr");
	SearchDevice = (SEARCHDEVICE)GetProcAddress(m_hInstDevFunc, "SearchDevice");
	GetUimDevIdList = (GETUIMDEVIDLIST)GetProcAddress(m_hInstDevFunc, "GetUimDevIdList");
	GetUimDevInfo = (GETUIMDEVINFO)GetProcAddress(m_hInstDevFunc, "GetUimDevInfo");
	OpenUimDev = (OPENUIMDEV)GetProcAddress(m_hInstDevFunc, "OpenUimDev");
	CloseUimDev = (CLOSEUIMDEV)GetProcAddress(m_hInstDevFunc, "CloseUimDev");
	UimGrobReg = (UIMGROBREG)GetProcAddress(m_hInstDevFunc, "UimGrobReg");
	GetMDL = (GETMDL)GetProcAddress(m_hInstDevFunc, "GetMDL");
	UimEna = (UIMENA)GetProcAddress(m_hInstDevFunc, "UimENA");
	UimOff = (UIMOFF)GetProcAddress(m_hInstDevFunc, "UimOFF");
	SetSPD = (SETSPD)GetProcAddress(m_hInstDevFunc, "SetSPD");
	UimAck = (UIMACK)GetProcAddress(m_hInstDevFunc, "UimACK");
	UimFbk = (UIMFBK)GetProcAddress(m_hInstDevFunc, "UimFBK");
	UimSfbk = (UIMSFBK)GetProcAddress(m_hInstDevFunc, "UimSFBK");
	SetPOS = (SETPOS)GetProcAddress(m_hInstDevFunc, "SetPOS");
	SetSTP = (SETSTP)GetProcAddress(m_hInstDevFunc, "SetSTP");
	SetgOFF = (SETGOFF)GetProcAddress(m_hInstDevFunc, "SetgOFF");
	SetgCUR = (SETGCUR)GetProcAddress(m_hInstDevFunc, "SetgCUR");
	SetgORG = (SETGORG)GetProcAddress(m_hInstDevFunc, "SetgORG");
	SetgDIR = (SETGDIR)GetProcAddress(m_hInstDevFunc, "SetgDIR");
	SetgSPD = (SETGSPD)GetProcAddress(m_hInstDevFunc, "SetgSPD");
	SetgSTP = (SETGSTP)GetProcAddress(m_hInstDevFunc, "SetgSTP");
	SetgPOS = (SETGPOS)GetProcAddress(m_hInstDevFunc, "SetgPOS");
	SetgQEC = (SETGQEC)GetProcAddress(m_hInstDevFunc, "SetgQEC");
	GetUimMCFG = (GETUIMMCFG)GetProcAddress(m_hInstDevFunc, "GetUimMCFG");
	SetUimMCFG = (SETUIMMCFG)GetProcAddress(m_hInstDevFunc, "SetUimMCFG");
	SetmACC = (SETMACC)GetProcAddress(m_hInstDevFunc, "SetmACC");
	GetmACC = (GETMACC)GetProcAddress(m_hInstDevFunc, "GetmACC");
	SetmDEC = (SETMDEC)GetProcAddress(m_hInstDevFunc, "SetmDEC");
	GetmDEC = (GETMDEC)GetProcAddress(m_hInstDevFunc, "GetmDEC");
	SetmMDS = (SETMMDS)GetProcAddress(m_hInstDevFunc, "SetmMDS");
	GetmMDS = (GETMMDS)GetProcAddress(m_hInstDevFunc, "GetmMDS");
	SetmMSS = (SETMMSS)GetProcAddress(m_hInstDevFunc, "SetmMSS");
	GetmMSS = (GETMMSS)GetProcAddress(m_hInstDevFunc, "GetmMSS");
	GetSTG = (GETSTG)GetProcAddress(m_hInstDevFunc, "GetSTG");
	SetSTG = (SETSTG)GetProcAddress(m_hInstDevFunc, "SetSTG");
	SetQEC = (SETQEC)GetProcAddress(m_hInstDevFunc, "SetQEC");
	SetQER = (SETQER)GetProcAddress(m_hInstDevFunc, "SetQER");
	SetORG = (SETORG)GetProcAddress(m_hInstDevFunc, "SetORG");
	SetSQT = (SETSQT)GetProcAddress(m_hInstDevFunc, "SetSQT");
	GetSQT = (GETSQT)GetProcAddress(m_hInstDevFunc, "GetSQT");
	SetBDR = (SETBDR)GetProcAddress(m_hInstDevFunc, "SetBDR");
	SetADR = (SETADR)GetProcAddress(m_hInstDevFunc, "SetADR");
	SetgMCS = (SETGMCS)GetProcAddress(m_hInstDevFunc, "SetgMCS");
	SetgDOUT = (SETGDOUT)GetProcAddress(m_hInstDevFunc, "SetgDOUT");
	SetgACR = (SETGACR)GetProcAddress(m_hInstDevFunc, "SetgACR");
	UimStore = (UIMSTORE)GetProcAddress(m_hInstDevFunc, "UimSTORE");
	SetACR = (SETACR)GetProcAddress(m_hInstDevFunc, "SetACR");
	GetACR = (GETACR)GetProcAddress(m_hInstDevFunc, "GetACR");
	SetMCS = (SETMCS)GetProcAddress(m_hInstDevFunc, "SetMCS");
	GetMCS = (GETMCS)GetProcAddress(m_hInstDevFunc, "GetMCS");
	SetCUR = (SETCUR)GetProcAddress(m_hInstDevFunc, "SetCUR");
	GetCUR = (GETCUR)GetProcAddress(m_hInstDevFunc, "GetCUR");
	GetSPD = (GETSPD)GetProcAddress(m_hInstDevFunc, "GetSPD");
	GetSTP = (GETSTP)GetProcAddress(m_hInstDevFunc, "GetSTP");
	GetQEC = (GETQEC)GetProcAddress(m_hInstDevFunc, "GetQEC");
	GetPOS = (GETPOS)GetProcAddress(m_hInstDevFunc, "GetPOS");

/*
	SetRPT= (SETRPT)GetProcAddress(m_hInstDevFunc, "SetRPT");
	GetRPT= (GETRPT)GetProcAddress(m_hInstDevFunc, "GetRPT");
	SetRPC= (SETRPC)GetProcAddress(m_hInstDevFunc, "SetRPC");
	GetRPC= (GETRPC)GetProcAddress(m_hInstDevFunc, "GetRPC");

*/

	SetATCONH = (SETATCONH)GetProcAddress(m_hInstDevFunc, "SetATCONH");
	GetATCONH = (GETATCONH)GetProcAddress(m_hInstDevFunc, "GetATCONH");
	SetATCONL = (SETATCONL)GetProcAddress(m_hInstDevFunc, "SetATCONL");
	GetATCONL = (GETATCONL)GetProcAddress(m_hInstDevFunc, "GetATCONL");
	SetBTR = (SETBTR)GetProcAddress(m_hInstDevFunc, "SetBTR");
	GetBTR = (GETBTR)GetProcAddress(m_hInstDevFunc, "GetBTR");
	GetSCFG = (GETSCFG)GetProcAddress(m_hInstDevFunc, "GetSCFG");
	SetS12CON = (SETS12CON)GetProcAddress(m_hInstDevFunc, "SetS12CON");
	GetS12CON = (GETS12CON)GetProcAddress(m_hInstDevFunc, "GetS12CON");
	SetS34CON = (SETS34CON)GetProcAddress(m_hInstDevFunc, "SetS34CON");
	GetS34CON = (GETS34CON)GetProcAddress(m_hInstDevFunc, "GetS34CON");
	SetDOUT =(SETDOUT)GetProcAddress(m_hInstDevFunc, "SetDOUT");
	GetDOUT =(GETDOUT)GetProcAddress(m_hInstDevFunc, "GetDOUT");
	SetUIDMCFG = (SETUIDMCFG)GetProcAddress(m_hInstDevFunc, "SetUIDMCFG");
	GetUIDMCFG = (GETUIDMCFG)GetProcAddress(m_hInstDevFunc, "GetUIDMCFG");
	SetIOconfig = (SETIOCONFIG)GetProcAddress(m_hInstDevFunc, "SetIOconfig");
	GetIOconfig = (GETIOCONFIG)GetProcAddress(m_hInstDevFunc, "GetIOconfig");
	SetDVAInfo = (SETDVAINFO)GetProcAddress(m_hInstDevFunc, "SetDVAInfo");
	GetDVAInfo = (GETDVAINFO)GetProcAddress(m_hInstDevFunc, "GetDVAInfo");
	GetAVAInfo = (GETAVAINFO)GetProcAddress(m_hInstDevFunc, "GetAVAInfo");
	SetPMB = (SETPMB)GetProcAddress(m_hInstDevFunc, "SetPMB");
	GetPMB = (GETPMB)GetProcAddress(m_hInstDevFunc, "GetPMB");



	SetPMD = (SETPMD)GetProcAddress(m_hInstDevFunc, "SetPMD");
	GetPMD = (GETPMD)GetProcAddress(m_hInstDevFunc, "GetPMD");
	SetPBR = (SETPBR)GetProcAddress(m_hInstDevFunc, "SetPBR");
    GetPBR = (GETPBR)GetProcAddress(m_hInstDevFunc, "GetPBR");
	SetDMA = (SETDMA)GetProcAddress(m_hInstDevFunc, "SetDMA");
	GetDMA = (GETDMA)GetProcAddress(m_hInstDevFunc, "GetDMA");
	SetOPR = (SETOPR)GetProcAddress(m_hInstDevFunc, "SetOPR");
	OPRRead = (OPRREAD)GetProcAddress(m_hInstDevFunc, "OPRRead");
	OPRReadPtr =  (OPRREADPTR)GetProcAddress(m_hInstDevFunc, "OPRReadPtr");
    SetDevIndex = (SETDEVINDEX)GetProcAddress(m_hInstDevFunc, "SetDevIndex");

	GetUimICFG = (GETUIMICFG)GetProcAddress(m_hInstDevFunc, "GetUimICFG");
	SetUimICFG = (SETUIMICFG)GetProcAddress(m_hInstDevFunc, "SetUimICFG");
	UimAdvENA = (UIMADVENA)GetProcAddress(m_hInstDevFunc, "UimAdvENA");
	UimGetAdvENA = (UIMGETADVENA)GetProcAddress(m_hInstDevFunc, "UimGetAdvENA");
	UimAdvStore = (UIMADVSTORE)GetProcAddress(m_hInstDevFunc, "UimAdvSTORE");
	GetBLC = (GETBLC)GetProcAddress(m_hInstDevFunc, "GetBLC");
	SetBLC = (SETBLC)GetProcAddress(m_hInstDevFunc, "SetBLC");
	GetQER = (GETQER)GetProcAddress(m_hInstDevFunc, "GetQER");
	SetNRD = (SETNRD)GetProcAddress(m_hInstDevFunc, "SetNRD");

	OPR_OpenAntenna = (OPR_OPENANTENNA)GetProcAddress(m_hInstDevFunc, "OPR_OpenAntenna");
	OPR_CloseAntenna = (OPR_CLOSEANTENNA)GetProcAddress(m_hInstDevFunc, "OPR_CloseAntenna");
	OPR_FindRFID = (OPR_FINDRFID)GetProcAddress(m_hInstDevFunc, "OPR_FindRFID");
	OPR_CloseRFID = (OPR_CLOSERFID)GetProcAddress(m_hInstDevFunc, "OPR_CloseRFID");
	OPR_RFIDRead = (OPR_RFIDREAD)GetProcAddress(m_hInstDevFunc, "OPR_RFIDRead");
	OPR_RFIDWrite = (OPR_RFIDWRITE)GetProcAddress(m_hInstDevFunc, "OPR_RFIDWrite");
	OPR_SetRFIDKey = (OPR_SETRFIDKEY)GetProcAddress(m_hInstDevFunc, "OPR_SetRFIDKey");
	OPR_GetRFIDBlkData = (OPR_GETRFIDBLKDATA)GetProcAddress(m_hInstDevFunc, "OPR_GetRFIDBlkData");
	OPR_SetRFIDBlkData = (OPR_SETRFIDBLKDATA)GetProcAddress(m_hInstDevFunc, "OPR_SetRFIDBlkData");
	OPR_GetRFIDCardNum = (OPR_GETRFIDCARDNUM)GetProcAddress(m_hInstDevFunc, "OPR_GetRFIDCardNum");

	UIMRegRtcnCallBack = (UIMREGRTCNCALLBACK)GetProcAddress(m_hInstDevFunc, "UIMRegRtcnCallBack");
	ForceStopProgram = (FORCESTOPPROGRAM)GetProcAddress(m_hInstDevFunc, "ForceStopProgram");
	SearchGateway =  (SEARCHGATEWAY)GetProcAddress(m_hInstDevFunc, "SearchGateway"); 
	OpenGateway = (OPENGATEWAY)GetProcAddress(m_hInstDevFunc, "OpenGateway"); 
	CloseGateway = (CLOSEGATEWAY)GetProcAddress(m_hInstDevFunc, "CloseGateway"); 
	GetSDKLastErr = (GETLASTERR)GetProcAddress(m_hInstDevFunc, "GetLastErr"); 


	if (SearchDevice == NULL || GetUimDevIdList == NULL|| GetUimDevInfo == NULL //|| GetLastErr == NULL  
        || OpenUimDev == NULL || CloseUimDev == NULL || UimGrobReg == NULL 
        || GetMDL == NULL || UimEna == NULL || UimOff == NULL 
        || SetSPD == NULL || UimAck == NULL || UimFbk == NULL 
        || UimSfbk == NULL || SetPOS == NULL || SetSTP == NULL 
        || SetgOFF == NULL || SetgCUR == NULL || SetgORG == NULL 
        || SetgDIR == NULL || SetgSPD == NULL || SetgSTP == NULL 
        || SetgPOS == NULL || SetgQEC == NULL || GetUimMCFG == NULL 
        || SetUimMCFG == NULL || SetmACC == NULL || GetmACC == NULL 
        || SetmDEC == NULL || GetmDEC == NULL || SetmMDS == NULL 
        || GetmMDS == NULL || SetmMSS == NULL || GetmMSS == NULL 
        || GetSTG == NULL || SetSTG == NULL || SetQEC == NULL 
        || SetQER == NULL || SetORG == NULL || SetSQT == NULL || GetSQT == NULL
        || SetBDR == NULL || SetADR == NULL || SetgMCS == NULL 
        || SetgDOUT == NULL || SetgACR == NULL || UimStore == NULL 
        || SetACR == NULL || GetACR == NULL || SetMCS == NULL 
        || SetCUR == NULL  
        || GetSPD == NULL || GetSTP == NULL || GetQEC == NULL || GetPOS == NULL 
		//|| SetRPT == NULL || GetRPT == NULL || SetRPC== NULL || GetRPC == NULL
		|| SetATCONH == NULL || GetATCONH == NULL 
        || SetATCONL == NULL || GetATCONL == NULL || SetBTR == NULL 
        || GetBTR == NULL || GetSCFG == NULL || SetS12CON == NULL 
        || GetS12CON == NULL  || SetS34CON == NULL  || GetS34CON == NULL  || SetDOUT == NULL
        || SetUIDMCFG == NULL || GetUIDMCFG == NULL || SetIOconfig == NULL 
        || GetIOconfig == NULL || SetDVAInfo == NULL || GetDVAInfo == NULL || GetDVAInfo == NULL 
        || SetPMB == NULL || GetPMB == NULL || SetPMD == NULL 
        || GetPMD == NULL || SetPBR == NULL || GetPBR == NULL 
		|| SetDMA == NULL || GetDMA == NULL
        || UIMRegRtcnCallBack == NULL || SetDevIndex == NULL
		|| GetUimICFG == NULL || SetUimICFG == NULL
		|| UimAdvENA == NULL || UimGetAdvENA == NULL || UimAdvStore == NULL
		|| GetBLC == NULL || SetBLC == NULL || GetQER == NULL || SetNRD== NULL 
		|| OPR_OpenAntenna == NULL || OPR_CloseAntenna == NULL || OPR_FindRFID == NULL || OPR_CloseRFID== NULL
		|| OPR_RFIDRead == NULL || OPR_RFIDWrite == NULL || OPR_SetRFIDKey == NULL 
		|| OPR_GetRFIDBlkData == NULL || OPR_SetRFIDBlkData == NULL || OPR_GetRFIDCardNum == NULL || ForceStopProgram == NULL)
    {
		return FALSE;
    }
	return TRUE;
}


VOID FreeSimCanFunc()
{
     if (m_hInstDevFunc)
     {
         FreeLibrary(m_hInstDevFunc);
         m_hInstDevFunc = NULL;
     }
}

#endif // end of _WIN32