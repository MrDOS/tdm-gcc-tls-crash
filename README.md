# TDM-GCC TLS crash reproduction

## Introduction

Code in shared libraries (DLLs)
compiled with [TDM-GCC][tdm-gcc]
and which uses [thread-local storage][tls]
(hereafter referred to as “TLS”,
but not to be confused with Transport Layer Security,
the evolution of SSL)
crashes when run from an executable compiled with [MSVC][msvc].
This is reproducible with the latest 64-bit release of TDM-GCC, 9.2.0,
and the second-latest release, 5.1.0-2.
I haven't tried any 32-bit targets.

The issue manifests itself
when simply accessing (reading or writing)
a TLS variable:

```c
static _Thread_local int foo = 0;

void bar()
{
    ++foo; /* Crash! */
}
```

However, there are much more subtle ways
of provoking the issue,
because a number of libraries –
notably, pthreads –
rely on TLS.
For example, `pthread_self(3)` will hard crash.
Surprising behaviour from a function
whose documentation boldly states

> This function always succeeds.

[tdm-gcc]: https://jmeubank.github.io/tdm-gcc/
[tls]: https://en.wikipedia.org/wiki/Thread-local_storage
[msvc]: https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B

## Reproduction

`main.c` is about as small as it can be.
The core of `tls.c` is lines 22–29;
everything else is there to enable compilation under both MSVC and GCC.
The `Makefile` mostly helps keep compiler flags and output filenames straight.

To build and reproduce, you'll need to have installed:

* some version of TDM-GCC
* [the Microsoft C++ Build Tools][msvc-build-tools]
  or [Visual Studio Community][msvsc],
  and a Windows SDK
  (I don't think it matters which one,
  but I've been using “Windows 10 SDK (10.0.18362.0)”)

Launch the “x64 Native Tools Command Prompt for VS 2019”
to get a command-line environment where MSVC will work.
Then:

```
C:\path\to\tlscrash>mingw32-make VARIANT=msvc
cl /LD /Fetls-msvc.dll tls.c
...
cl  /Femain-msvc.exe main.c
...
C:\path\to\tlscrash>mingw32-make CC=C:\Tools\TDM-GCC-64-9.2.0\bin\x86_64-w64-mingw32-gcc.exe VARIANT=tdm-gcc-64-9.2.0
C:\Tools\TDM-GCC-64-9.2.0\bin\x86_64-w64-mingw32-gcc.exe -fPIC -shared -otls-tdm-gcc-64-9.2.0.dll tls.c
C:\Tools\TDM-GCC-64-9.2.0\bin\x86_64-w64-mingw32-gcc.exe  -omain-tdm-gcc-64-9.2.0.exe main.c
```

If you have any other toolchains installed,
you can compile with those for comparison, too:
```
C:\path\to\tlscrash>mingw32-make CC=C:\Tools\mingw-w64\x86_64-8.1.0-release-posix-seh-rt_v6-rev0\bin\x86_64-w64-mingw32-gcc.exe VARIANT=mingw-w64-8.1.0
C:\Tools\mingw-w64\x86_64-8.1.0-release-posix-seh-rt_v6-rev0\bin\x86_64-w64-mingw32-gcc.exe -fPIC -shared -otls-mingw-w64-8.1.0.dll tls.c
C:\Tools\mingw-w64\x86_64-8.1.0-release-posix-seh-rt_v6-rev0\bin\x86_64-w64-mingw32-gcc.exe  -omain-mingw-w64-8.1.0.exe main.c
```

The `main` executables expect one argument:
the name of the library to load.

Expected success:

```
C:\path\to\tlscrash>main-msvc.exe tls-msvc.dll
Now running library code.
The TLS value is 0.
Returning from the library code.
```

Expected failure:

```
C:\Users\scoleman\Projects\tlscrash>main-msvc.exe tls-tdm-gcc-64-9.2.0.dll
Now running library code.
```

Or, in a MSYS2 environment
(e.g., Git Bash):

```
$ ./main-msvc.exe tls-tdm-gcc-64-9.2.0.dll
Segmentation fault
```

[msvc-build-tools]: https://visualstudio.microsoft.com/visual-cpp-build-tools/
[msvsc]: https://visualstudio.microsoft.com/vs/community/

## Toolchains tested

To isolate this to TDM-GCC,
I've tried a variety of different toolchains.
This failure seems to be particular to TDM-GCC,
and not to the latest version.

| DLL ↓/EXE →                            | MSVC | TDM-GCC-64 5.1.0-2 | TDM-GCC-64 9.2.0 | MinGW-w64 ??? (GCC 8.1.0) | MinGW-w64 5.0.3 (GCC 7.3.0), Ubuntu host |
| ---------------------------------------- | ---- | ------------------ | ---------------- | ------------------------- | ---------------------------------------- |
| MSVC                                     | ✔ | ✔ | ✔ | ✔ | ✔ |
| TDM-GCC-64 5.1.0-2                       | ❌ | ✔ | ✔ | ✔ | ✔ |
| TDM-GCC-64 9.2.0                         | ❌ | ✔ | ✔ | ✔ | ✔ |
| MinGW-w64 ??? (GCC 8.1.0)                | ✔ | ✔ | ✔ | ✔ | ✔ |
| MinGW-w64 5.0.3 (GCC 7.3.0), Ubuntu host | ✔ | ✔ | ✔ | ✔ | ✔ |
