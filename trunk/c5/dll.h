#ifndef CRYPTOPP_DLL_H
#define CRYPTOPP_DLL_H

#if !defined(CRYPTOPP_IMPORTS) && !defined(CRYPTOPP_EXPORTS) && !defined(CRYPTOPP_NO_DLL)
#ifdef CRYPTOPP_CONFIG_H
#error To use the DLL version of Crypto++, this file must be included before any other Crypto++ header files.
#endif
#define CRYPTOPP_IMPORTS
#endif

#include "aes.h"
#include "cbcmac.h"
#include "channels.h"
#include "des.h"
#include "dh.h"
#include "dsa.h"
#include "ec2n.h"
#include "eccrypto.h"
#include "ecp.h"
#include "files.h"
#include "fips140.h"
#include "hex.h"
#include "hmac.h"
#include "modes.h"
#include "mqueue.h"
#include "nbtheory.h"
#include "osrng.h"
#include "pkcspad.h"
#include "randpool.h"
#include "rsa.h"
#include "sha.h"
#include "skipjack.h"
#include "trdlocal.h"

#ifdef CRYPTOPP_IMPORTS

#ifdef _DLL
// cause CRT DLL to be initialized before Crypto++ so that we can use malloc and free during DllMain()
#ifdef NDEBUG
#pragma comment(lib, "msvcrt")
#else
#pragma comment(lib, "msvcrtd")
#endif
#endif

#pragma comment(lib, "cryptopp")

#endif		// #ifdef CRYPTOPP_IMPORTS

#include <new>	// for new_handler

NAMESPACE_BEGIN(CryptoPP)

#if !(defined(_MSC_VER) && (_MSC_VER < 1300))
using std::new_handler;
#endif

typedef void * (_cdecl * PNew)(size_t);
typedef void (_cdecl * PDelete)(void *);
typedef void (_cdecl * PGetNewAndDelete)(PNew &, PDelete &);
typedef new_handler (_cdecl * PSetNewHandler)(new_handler);
typedef void (_cdecl * PSetNewAndDelete)(PNew, PDelete, PSetNewHandler);

CRYPTOPP_DLL void DoDllPowerUpSelfTest();

NAMESPACE_END

#endif
