all: debug

CXXFLAGS := -c -m64 -std=c++11 -fpermissive -DLINUX -DPPOSIX -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
CFLAGS := -c -m64 -DLINUX -DPPOSIX
INCLUDE := -I../.. -I../../ungif -I../../jpeg -I../../zip

PP_SOURCES := ppincludebase.cpp \
           ppblock.cpp \
           ppconapp.cpp \
           pperror.cpp \
           ppinifile.cpp \
           pplogfile.cpp \
           ppmsg.cpp \
           ppprocess.cpp \
           ppprocesskill.cpp \
           ppsocket.cpp \
           ppsocketc.cpp \
           ppsockets.cpp \
           ppstring.cpp \
           ppunistring.cpp \
           ppthread.cpp \
           ppwinsvc.cpp \
           ppwinwnd.cpp \
           ppdir.cpp \
           ppsysinfo.cpp \
           ppimage.cpp \
           ppmstream.cpp \
           ppfstream.cpp \
           ppiofile.cpp \
           ppcontainer.cpp \
           ppstringutf8.cpp \
           ppstringx.cpp \
           ppi18n_ascii.cpp \
           ppi18n_codepage.cpp \
           ppi18n_1252.cpp \
           ppi18n_stream.cpp \
           ppclient.cpp \
           ppinifileex.cpp \
           ppprocesselevated.cpp \
           ppipc.cpp \
           ppsig.cpp \
           ppi18n_casefolding.cpp \
           ppsocketudp.cpp 

UNGIF_C_SOURCES := dgif_lib.c gif_err.c gif_hash.c gifalloc.c 
UNGIF_CPP_SOURCES := ppgif.cpp

JPEG_C_SOURCES := jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c \
                  jcphuff.c jcprepct.c jcsample.c jdapimin.c jdapistd.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c \
                  jdmaster.c jdmerge.c jdphuff.c jdpostct.c jdsample.c jfdctflt.c jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c \
                  jidctred.c jmemmgr.c jmemnobs.c jquant1.c jquant2.c jutils.c 
JPEG_CPP_SOURCES := ppjpeg.cpp ppjpegout.cpp ppjerror.cpp

ZIP_C_SOURCES := infblock.c infcodes.c inffast.c inflate.c inftrees.c infutil.c adler32.c crc32.c deflate.c trees.c zutil.c unzip.c
ZIP_CPP_SOURCES := ppzzip.cpp gzio2.cpp ppgzip.cpp ppdeflate.cpp ppinflate.cpp zip2.cpp

PP_OBJS        := $(subst .cpp,.o,$(PP_SOURCES))
UNGIF_C_OBJS   := $(subst .c,.o,$(UNGIF_C_SOURCES))
UNGIF_CPP_OBJS := $(subst .cpp,.o,$(UNGIF_CPP_SOURCES))
JPEG_C_OBJS    := $(subst .c,.o,$(JPEG_C_SOURCES))
JPEG_CPP_OBJS  := $(subst .cpp,.o,$(JPEG_CPP_SOURCES))
ZIP_C_OBJS     := $(subst .c,.o,$(ZIP_C_SOURCES))
ZIP_CPP_OBJS   := $(subst .cpp,.o,$(ZIP_CPP_SOURCES))

OBJS_LIST := $(PP_OBJS) $(UNGIF_C_OBJS) $(UNGIF_CPP_OBJS) $(JPEG_C_OBJS) $(JPEG_CPP_OBJS) $(ZIP_C_OBJS) $(ZIP_CPP_OBJS)

export

create_soft_links:
	@for f in $(UNGIF_C_SOURCES)  ; do if [ ! -e ../$$f ]; then ln -s ungif/$$f ../$$f; fi; done
	@for f in $(UNGIF_CPP_SOURCES); do if [ ! -e ../$$f ]; then ln -s ungif/$$f ../$$f; fi; done
	@for f in $(JPEG_C_SOURCES)   ; do if [ ! -e ../$$f ]; then ln -s jpeg/$$f  ../$$f; fi; done
	@for f in $(JPEG_CPP_SOURCES) ; do if [ ! -e ../$$f ]; then ln -s jpeg/$$f  ../$$f; fi; done
	@for f in $(ZIP_C_SOURCES)    ; do if [ ! -e ../$$f ]; then ln -s zip/$$f   ../$$f; fi; done
	@for f in $(ZIP_CPP_SOURCES)  ; do if [ ! -e ../$$f ]; then ln -s zip/$$f   ../$$f; fi; done

debug: create_soft_links
	@mkdir -p Debug64
	@echo -e 'all: bld'                                             > Debug64/Makefile
	@echo                                                          >> Debug64/Makefile
	@echo -e '%.o: ../../%.c'                                      >> Debug64/Makefile
	@echo -e '\tgcc -g $$(CFLAGS) $$(INCLUDE) $$< -o $$@'          >> Debug64/Makefile
	@echo                                                          >> Debug64/Makefile
	@echo -e '%.o: ../../%.cpp'                                    >> Debug64/Makefile
	@echo -e '\tg++ -g $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@'        >> Debug64/Makefile
	@echo                                                          >> Debug64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                                  >> Debug64/Makefile
	@echo -e '\tar rvs libplib.a $$(OBJS_LIST)'                    >> Debug64/Makefile
	make -C Debug64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

debug_shared: create_soft_links
	@mkdir -p DebugShared64
	@echo -e 'all: bld'                                             > DebugShared64/Makefile
	@echo                                                          >> DebugShared64/Makefile
	@echo -e '%.o: ../../%.c'                                      >> DebugShared64/Makefile
	@echo -e '\tgcc -g -fPIC $$(CFLAGS) $$(INCLUDE) $$< -o $$@'    >> DebugShared64/Makefile
	@echo                                                          >> DebugShared64/Makefile
	@echo -e '%.o: ../../%.cpp'                                    >> DebugShared64/Makefile
	@echo -e '\tg++ -g -fPIC $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@'  >> DebugShared64/Makefile
	@echo                                                          >> DebugShared64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                                  >> DebugShared64/Makefile
	@echo -e '\tar rvs libplib.a $$(OBJS_LIST)'                    >> DebugShared64/Makefile
	make -C DebugShared64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

release: create_soft_links
	@mkdir -p Release64
	@echo -e 'all: bld'                                             > Release64/Makefile
	@echo                                                          >> Release64/Makefile
	@echo -e '%.o: ../../%.c'                                      >> Release64/Makefile
	@echo -e '\tgcc -O3 $$(CFLAGS) $$(INCLUDE) $$< -o $$@'         >> Release64/Makefile
	@echo                                                          >> Release64/Makefile
	@echo -e '%.o: ../../%.cpp'                                    >> Release64/Makefile
	@echo -e '\tg++ -O3 $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@'       >> Release64/Makefile
	@echo                                                          >> Release64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                                  >> Release64/Makefile
	@echo -e '\tar rvs libplib.a $$(OBJS_LIST)'                    >> Release64/Makefile
	make -C Release64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

release_shared: create_soft_links
	@mkdir -p ReleaseShared64
	@echo -e 'all: bld'                                             > ReleaseShared64/Makefile
	@echo                                                          >> ReleaseShared64/Makefile
	@echo -e '%.o: ../../%.c'                                      >> ReleaseShared64/Makefile
	@echo -e '\tgcc -O3 -fPIC $$(CFLAGS) $$(INCLUDE) $$< -o $$@'   >> ReleaseShared64/Makefile
	@echo                                                          >> ReleaseShared64/Makefile
	@echo -e '%.o: ../../%.cpp'                                    >> ReleaseShared64/Makefile
	@echo -e '\tg++ -O3 -fPIC $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@' >> ReleaseShared64/Makefile
	@echo                                                          >> ReleaseShared64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                                  >> ReleaseShared64/Makefile
	@echo -e '\tar rvs libplib.a $$(OBJS_LIST)'                    >> ReleaseShared64/Makefile
	make -C ReleaseShared64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

clean:
	rm -rf Debug64 DebugShared64
	rm -rf Release64 ReleaseShared64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

