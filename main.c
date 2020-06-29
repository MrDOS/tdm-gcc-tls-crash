#include <stdio.h>
#include <windows.h>

typedef void (FTls)(void);

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: main tls.dll\n");
        return 1;
    }

    HMODULE hTls = LoadLibrary(argv[1]);
    if (!hTls)
    {
        fprintf(stderr, "Failed to LoadLibrary: %lu\n", GetLastError());
        return 2;
    }

    FTls *tls = (FTls *) GetProcAddress(hTls, "tls");
    if (!tls)
    {
        fprintf(stderr, "Failed to GetProcAddress for tls: %lu\n", GetLastError());
        return 2;
    }

    tls();

    if (!FreeLibrary(hTls))
    {
        fprintf(stderr, "Failed to FreeLibrary: %lu\n", GetLastError());
        return 2;
    }

    return 0;
}
