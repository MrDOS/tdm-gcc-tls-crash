#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
/* MSVC only includes objects in a DLL which have been tagged with
 * `__declspec(dllexport)`: https://docs.microsoft.com/en-us/cpp/build/
 * exporting-from-a-dll-using-declspec-dllexport?view=vs-2019. */
#define EXPORT_SYMBOL __declspec(dllexport)
/* MSVC doesn't support the C11 `_Thread_local` keyword yet, relying on another
 * `__declspec`. */
#define TLS_SYMBOL __declspec(thread)
#else
/* Shared libraries built by GCC/Mingw-w64 will export all functions not marked
 * as `static`. */
#define EXPORT_SYMBOL
#define TLS_SYMBOL _Thread_local
#endif

static TLS_SYMBOL int tls_value = 0;

EXPORT_SYMBOL void tls(void)
{
    printf("Now running library code.\n");
    printf("The TLS value is %d.\n", tls_value);
    printf("Returning from the library code.\n");
}

#ifdef __cplusplus
}
#endif
