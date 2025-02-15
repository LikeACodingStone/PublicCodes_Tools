#ifndef _CMD_REQUEST_H_
#define _CMD_REQUEST_H_ 
typedef long long int SemcParseHandle;

extern SemcParseHandle semcParseHandInit();
extern void semcParseResponse(char *quesCmd,SemcParseHandle semcPrHand,char **smdRlt);


#endif

