#ifndef ERROROUTPUTSTREAM
#define ERROROUTPUTSTREAM

#define WIN32
#include "NxPhysics.h"

class ErrorOutputStream : public NxUserOutputStream
{
    void reportError (NxErrorCode code, const char *message, const char* file, int line);
    NxAssertResponse reportAssertViolation (const char *message, const char *file,int line);
	void print (const char *message);
};

#endif