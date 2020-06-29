ifndef VARIANT
define VARIANT_MESSAGE
Use the VARIANT environment variable to inform the name of the output library
and executable. For example:

    $(MAKE) VARIANT=mingw-w64

will cause the output filenames to have the `-mingw-w64` suffix
endef
$(error $(VARIANT_MESSAGE))
endif

ifeq ($(findstring msvc,$(VARIANT)),msvc)
CC := cl
OUTFLAG := /Fe
tls-%: CFLAGS := /LD
else
OUTFLAG := -o
tls-%: CFLAGS := -fPIC -shared
endif

all: tls-$(VARIANT).dll main-$(VARIANT).exe

tls-%.dll: tls.c
	$(CC) $(CFLAGS) $(OUTFLAG)$@ $^

main-%.exe: main.c
	$(CC) $(CFLAGS) $(OUTFLAG)$@ $^

.PHONY: clean
clean:
	del *.dll *.exe *.exp *.lib *.obj
