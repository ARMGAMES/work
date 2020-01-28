all: debug

CXXFLAGS := -c -std=c++11 -m64 -fpermissive -DLINUX -DPPOSIX -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS 

SOURCES := commappfactory.cpp \
           commcliconn.cpp \
           commclisubscr.cpp \
           commconcli.cpp \
           commconn.cpp \
           commgrdnullcli.cpp \
           commgrdnullsrv.cpp \
           commmsgbody.cpp \
           commqueue.cpp \
           commrtable.cpp \
           commsrvconn.cpp \
           commsrvobj.cpp \
           commsrvsubscr.cpp \
           commsubscr.cpp \
           commtcp0.cpp \
           commtmlocal.cpp \
           commtmrw.cpp \
           commtmsimple.cpp \
           commtmwin.cpp \
           commgrdppwdcli.cpp \
           commgrdppwdsrv.cpp \
           commcligconn.cpp \
           commcligconnex.cpp \
           commgrdppwdroot.cpp \
           commgrdppwdsidcli.cpp \
           commgrdppwdsidsrv.cpp \
           commgrdppwdsidroot.cpp \
           commlocalapp.cpp \
           commdispatcher.cpp \
           commdispatchercommon.cpp \
           commtthread.cpp \
           commtmgr.cpp \
           commssl.cpp \
           commsslbio.cpp \
           commsslc.cpp \
           commssls.cpp \
           commssl0.cpp \
           commsslbase64.cpp \
           commsslchksum.cpp \
           commsslsign.cpp \
           commsslverify.cpp \
           commsslprivkey.cpp \
           commsslcipher.cpp \
           commsslcipher3.cpp \
           commsslrand.cpp \
           commsslentropy.cpp \
           commsslentropys.cpp \
           commsslentropywinc.cpp \
           commsslsysinfo.cpp \
           commtmrwplain.cpp \
           commtmrwssl.cpp \
           commtmsimpleplain.cpp \
           commtmsimplessl.cpp \
           commurl.cpp \
           commmime.cpp \
           commhttp.cpp \
           commhttpserver.cpp \
           commmail.cpp \
           commsmtp.cpp \
           commpop3.cpp \
           commcgi.cpp \
           commhtml.cpp \
           commhtmlparser.cpp \
           commlzhl.cpp \
           commtmlm.cpp \
           commlocalshared.cpp \
           commsharedmem.cpp \
           commlocalssl.cpp \
           commtmrwp.cpp \
           commsslp.cpp \
           commphyslocal.cpp \
           commredundant.cpp \
           commsrvcomposite.cpp \
           commtmcomposite.cpp \
           commmonitorlogger.cpp \
           i18ncommhtml.cpp \
           commhtmlex.cpp \
           commtok822.cpp \
           Lz.cpp \
           Huff.cpp \
           lz1.cpp \
           huff1.cpp \
           tok822_parse.cpp \
           commmailload.cpp \
           commtmrwpc.cpp \
           commsgmlparser.cpp \
           commsslpasswordhash.cpp \
           commsslpasswordhashgenerate.cpp \
           commsslrsa.cpp \
           commsslthrow.cpp \
           communix.cpp \
           commgrdrsasrv.cpp \
           commgrdrsacli.cpp \
           commsslaes.cpp \
           commssldh.cpp \
           commgrdrsaroot.cpp \
           commgrdaesencryptedsrv.cpp \
           commgrdaesencryptedcli.cpp \
           commgrdaesencryptedroot.cpp \
           commgrdtokensrv.cpp \
           commgrdtokencli.cpp \
           commgrdtokenroot.cpp \
           commsharedmemloop.cpp \
           commstreamencryption.cpp \
           commtmrwmp.cpp \
           commtmclient.cpp \
           commtmclientplain.cpp \
           commtmclientssl.cpp \
           commtmrwudp.cpp \
           commtmrwudpplain.cpp \
           commtmclientudp.cpp \
           commtmclientudpplain.cpp \
           commudp0.cpp \
           commclisubscrlight.cpp \
           commsrvsubscrlight.cpp \
           commgrdwebtokensrv.cpp \
           commgrdwebtokenroot.cpp \
           commgrdwebtokencli.cpp \
           commgrdrsasimplesrv.cpp \
           commgrdrsasimpleroot.cpp \
           commgrdrsasimplecli.cpp \
           commgrdwebtokenintsrv.cpp \
           commgrdwebtokenintroot.cpp \
           commgrdwebtokenintcli.cpp \
           commgrdpftppwdsrv.cpp \
           commgrdpftppwdroot.cpp \
           commgrdpftppwdcli.cpp \
           commgrdencryptedpwdroot.cpp \
           commgrdencryptedpwdsrv.cpp \
           commgrdencryptedpwdcli.cpp \
           commgrdencryption.cpp
OBJS_LIST := $(subst .cpp,.o,$(SOURCES))
INCLUDE := -I../../../plib -I/usr/include -I../.. -I../../lzhl -I../../lzhl1 -I/software/openssl/include
export

create_symbolic_links:
	@if [ ! -e ../Lz.cpp    ]; then ln -s lzhl/Lz.cpp ../Lz.cpp;        fi
	@if [ ! -e ../Huff.cpp  ]; then ln -s lzhl/Huff.cpp ../Huff.cpp;    fi
	@if [ ! -e ../lz1.cpp   ]; then ln -s lzhl1/lz1.cpp ../lz1.cpp;     fi
	@if [ ! -e ../huff1.cpp ]; then ln -s lzhl1/huff1.cpp ../huff1.cpp; fi

debug: create_symbolic_links
	@mkdir -p Debug64
	@echo 'all: bld'                                            > Debug64/Makefile
	@echo                                                      >> Debug64/Makefile
	@echo '%.o: ../../%.cpp'                                   >> Debug64/Makefile
	@echo -e '\tg++ -g $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@'    >> Debug64/Makefile
	@echo                                                      >> Debug64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                              >> Debug64/Makefile
	@echo -e '\tar rvs libcommlib2a.a $$(OBJS_LIST)'           >> Debug64/Makefile
	make -C Debug64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

debug_shared: create_symbolic_links
	@mkdir -p DebugShared64
	@echo 'all: bld'                                               > DebugShared64/Makefile
	@echo                                                         >> DebugShared64/Makefile
	@echo '%.o: ../../%.cpp'                                      >> DebugShared64/Makefile
	@echo -e '\tg++ -g -fPIC $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@' >> DebugShared64/Makefile
	@echo                                                         >> DebugShared64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                                 >> DebugShared64/Makefile
	@echo -e '\tar rvs libcommlib2a.a $$(OBJS_LIST)'              >> DebugShared64/Makefile
	make -C DebugShared64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

release: create_symbolic_links
	@mkdir -p Release64
	@echo -e 'all: bld'                                         > Release64/Makefile
	@echo                                                      >> Release64/Makefile
	@echo -e '%.o: ../../%.cpp'                                >> Release64/Makefile
	@echo -e '\tg++ -O3 $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@'   >> Release64/Makefile
	@echo                                                      >> Release64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                              >> Release64/Makefile
	@echo -e '\tar rvs libcommlib2a.a $$(OBJS_LIST)'           >> Release64/Makefile
	make -C Release64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

release_shared: create_symbolic_links
	@mkdir -p ReleaseShared64
	@echo -e 'all: bld'                                             > ReleaseShared64/Makefile
	@echo                                                          >> ReleaseShared64/Makefile
	@echo -e '%.o: ../../%.cpp'                                    >> ReleaseShared64/Makefile
	@echo -e '\tg++ -O3 -fPIC $$(CXXFLAGS) $$(INCLUDE) $$< -o $$@' >> ReleaseShared64/Makefile
	@echo                                                          >> ReleaseShared64/Makefile
	@echo -e 'bld: $$(OBJS_LIST)'                                  >> ReleaseShared64/Makefile
	@echo -e '\tar rvs libcommlib2a.a $$(OBJS_LIST)'               >> ReleaseShared64/Makefile
	make -C ReleaseShared64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

clean:
	rm -rf  Debug64 DebugShared64
	rm -rf  Release64/*.o ReleaseShared64
	@for f in `ls ..`; do if [ -h ../$$f ]; then rm ../$$f; fi; done;

