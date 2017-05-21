#include "ErrorOutputStream.h"
#define NOMINMAX
#include <Windows.h>

void ErrorOutputStream::reportError(NxErrorCode code, const char *message, const char *file, int line)
{
    //this should be routed to the application
    //specific error handling. If this gets hit
    //then you are in most cases using the SDK
    //wrong and you need to debug your code!
    //however, code may  just be a warning or
    //information.

    if (code < NXE_DB_INFO)
    {
        MessageBoxA(NULL, message, "Physics SDK Error", MB_OK),
        exit(1);
    }
}

NxAssertResponse ErrorOutputStream::reportAssertViolation(const char *message, const char *file, int line)
{
    //this should not get hit by
    // a properly debugged SDK!
    assert(0);
    return NX_AR_CONTINUE;
}

void ErrorOutputStream::print(const char *message)
{
	//printf("SDK says: %s\n", message);
}