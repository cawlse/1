///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifndef __SCIINTERFACE_H__
#define __SCIINTERFACE_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "sci.h"
#include "sciWebServices.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SDK instance
typedef struct
{
	SCWebServices mWebServices;
	gsi_u32       mGameId;
	//gsi_u32	  mOptionsFlags;
	gsi_bool      mInit;
	const char *  mServiceURL;
	GSCoreMgr *   mSdkCore;
	gsi_u8        mSessionId[SC_SESSION_GUID_SIZE];
	gsi_u8        mConnectionId[SC_CONNECTION_GUID_SIZE];
} SCInterface;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SCResult sciInterfaceCreate (SCInterface** theInterfaceOut);
SCResult sciInterfaceInit   (SCInterface*  theInterface);
void     sciInterfaceDestroy(SCInterface*  theInterface);

void     sciInterfaceSetSessionId(SCInterface* theInterface, const char * theSessionId);
void     sciInterfaceSetConnectionId(SCInterface* theInterface, const char * theConnectionId);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __SCINTERFACE_H__
