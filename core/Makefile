CC			=	cc
CFLAGS			=	-pipe -O -W -Wall -Wpointer-arith -Wno-unused-parameter -Werror -g -g -O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2
CFLAGS32		=
LINK			=	$(CC)

ifeq ($(shell getconf LONG_BIT),32)
CFLAGS32		=       -D_FILE_OFFSET_BITS=64
endif

USE_NXS_MODULES		=	-D USE_NXS_MYSQL -D USE_NXS_ILIST -D USE_NXS_MPOOL -D USE_NXS_NCURSES -D USE_NXS_XML -D USE_NXS_JSON -D USE_NXS_REST_API -D USE_NXS_CURL

NXS_CORE_INCLUDES	=	-I modules/ -I /usr/include/libxml2 -I /usr/include/libevent-nxs

# headers/nxs-ilist.h
NXS_CORE_HEADERS =	modules/nxs-core/nxs-core.h \
						modules/nxs-mem/nxs-mem.h \
						modules/nxs-list/nxs-list.h \
						modules/nxs-stack/nxs-stack.h \
						modules/nxs-tree/nxs-tree.h \
						modules/nxs-array/nxs-array.h \
						modules/nxs-mpool/nxs-mpool.h \
						modules/nxs-string/nxs-string.h \
						modules/nxs-log/nxs-log.h \
						modules/nxs-proc/nxs-proc.h \
						modules/nxs-args/nxs-args.h \
						modules/nxs-fs/nxs-fs.h \
						modules/nxs-cfg/nxs-cfg.h \
						modules/nxs-mysql/nxs-mysql.h \
						modules/nxs-crypt-gost/nxs-crypt-gost.h \
						modules/nxs-metadata/nxs-metadata.h \
						modules/nxs-net/nxs-net.h \
						modules/nxs-poll/nxs-poll.h \
						modules/nxs-buf/nxs-buf.h \
						modules/nxs-bchain/nxs-bchain.h \
						modules/nxs-pipesem/nxs-pipesem.h \
						modules/nxs-ncurses/nxs-ncurses.h \
						modules/nxs-wstring/nxs-wstring.h \
						modules/nxs-xml/nxs-xml.h \
						modules/nxs-json/nxs-json.h \
						modules/nxs-cfg-json/nxs-cfg-json.h \
						modules/nxs-rest-api-common/nxs-rest-api-common.h \
						modules/nxs-rest-api/nxs-rest-api.h \
						modules/nxs-curl/nxs-curl.h \
						modules/nxs-base64/nxs-base64.h

default:	nxs-core

clean:
	rm -f ./objs/*.o ./objs/*.a || true

# objs/nxs-ilist.o
nxs-core:	objs/libnxs-general.a \
				objs/libnxs-mysql.a \
				objs/libnxs-ncurses.a \
				objs/libnxs-xml.a \
				objs/libnxs-json.a \
				objs/libnxs-cfg-json.a \
				objs/libnxs-rest-api.a \
				objs/libnxs-curl.a

objs/libnxs-general.a:				objs/nxs-args.o \
										objs/nxs-array.o \
										objs/nxs-base64.o \
										objs/nxs-bchain.o \
										objs/nxs-buf.o \
										objs/nxs-cfg.o \
										objs/nxs-crypt-gost.o \
										objs/nxs-fs.o \
										objs/nxs-list.o \
										objs/nxs-log.o \
										objs/nxs-mem.o \
										objs/nxs-metadata.o \
										objs/nxs-mpool.o \
										objs/nxs-net.o \
										objs/nxs-pipesem.o \
										objs/nxs-poll.o \
										objs/nxs-proc.o \
										objs/nxs-rest-api-common.o \
										objs/nxs-stack.o \
										objs/nxs-string.o \
										objs/nxs-tree.o \
										objs/nxs-wstring.o
	
	ar csr objs/libnxs-general.a objs/nxs-args.o objs/nxs-array.o objs/nxs-base64.o objs/nxs-bchain.o objs/nxs-buf.o objs/nxs-cfg.o objs/nxs-crypt-gost.o objs/nxs-fs.o objs/nxs-list.o objs/nxs-log.o objs/nxs-mem.o objs/nxs-metadata.o objs/nxs-mpool.o objs/nxs-net.o objs/nxs-pipesem.o objs/nxs-poll.o objs/nxs-proc.o objs/nxs-rest-api-common.o objs/nxs-stack.o objs/nxs-string.o objs/nxs-tree.o objs/nxs-wstring.o

objs/libnxs-mysql.a:				objs/nxs-mysql.o

	ar csr objs/libnxs-mysql.a objs/nxs-mysql.o

objs/libnxs-ncurses.a:				objs/nxs-ncurses.o

	ar csr objs/libnxs-ncurses.a objs/nxs-ncurses.o

objs/libnxs-xml.a:					objs/nxs-xml.o

	ar csr objs/libnxs-xml.a objs/nxs-xml.o

objs/libnxs-json.a:					objs/nxs-json.o \
										objs/nxs-buf.o \
										objs/nxs-fs.o \
										objs/nxs-poll.o

	ar csr objs/libnxs-json.a objs/nxs-json.o objs/nxs-buf.o objs/nxs-fs.o objs/nxs-poll.o

objs/libnxs-cfg-json.a:				objs/nxs-cfg-json.o \
										objs/nxs-json.o \
										objs/nxs-buf.o \
										objs/nxs-fs.o \
										objs/nxs-poll.o

	ar csr objs/libnxs-cfg-json.a objs/nxs-cfg-json.o objs/nxs-json.o objs/nxs-buf.o objs/nxs-fs.o objs/nxs-poll.o

objs/libnxs-rest-api.a:				objs/nxs-rest-api.o \
										objs/nxs-rest-api-common.o

	ar csr objs/libnxs-rest-api.a objs/nxs-rest-api.o objs/nxs-rest-api-common.o
	
objs/libnxs-curl.a:					objs/nxs-curl.o \
										objs/nxs-rest-api-common.o

	ar csr objs/libnxs-curl.a objs/nxs-curl.o objs/nxs-rest-api-common.o
	


objs/nxs-mem.o:				modules/nxs-mem/nxs-mem.c \
					$(NXS_CORE_HEADERS)
		
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-mem.o modules/nxs-mem/nxs-mem.c

objs/nxs-mpool.o:			modules/nxs-mpool/nxs-mpool.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-mpool.o modules/nxs-mpool/nxs-mpool.c

objs/nxs-list.o:			modules/nxs-list/nxs-list.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-list.o modules/nxs-list/nxs-list.c

objs/nxs-stack.o:			modules/nxs-stack/nxs-stack.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-stack.o modules/nxs-stack/nxs-stack.c

objs/nxs-tree.o:			modules/nxs-tree/nxs-tree.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-tree.o modules/nxs-tree/nxs-tree.c
	
#objs/nxs-ilist.o:			modules/nxs-ilist/nxs-ilist.c \
#					$(NXS_CORE_HEADERS)
#			
#	$(CC) -c $(CFLAGS) $(CFLAGS32) $(NXS_CORE_INCLUDES) -o objs/nxs-ilist.o /nxs-ilist/nxs-ilist.c
	
objs/nxs-array.o:			modules/nxs-array/nxs-array.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-array.o modules/nxs-array/nxs-array.c

objs/nxs-string.o:			modules/nxs-string/nxs-string.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-string.o modules/nxs-string/nxs-string.c
	
objs/nxs-wstring.o:			modules/nxs-wstring/nxs-wstring.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-wstring.o modules/nxs-wstring/nxs-wstring.c

objs/nxs-log.o:				modules/nxs-log/nxs-log.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-log.o modules/nxs-log/nxs-log.c

objs/nxs-proc.o:			modules/nxs-proc/nxs-proc.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-proc.o modules/nxs-proc/nxs-proc.c

objs/nxs-args.o:			modules/nxs-args/nxs-args.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-args.o modules/nxs-args/nxs-args.c

objs/nxs-fs.o:				modules/nxs-fs/nxs-fs.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-fs.o modules/nxs-fs/nxs-fs.c

objs/nxs-cfg.o:				modules/nxs-cfg/nxs-cfg.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-cfg.o modules/nxs-cfg/nxs-cfg.c
	
objs/nxs-mysql.o:			modules/nxs-mysql/nxs-mysql.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-mysql.o modules/nxs-mysql/nxs-mysql.c
	
objs/nxs-crypt-gost.o:			modules/nxs-crypt-gost/nxs-crypt-gost.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-crypt-gost.o modules/nxs-crypt-gost/nxs-crypt-gost.c
	
objs/nxs-metadata.o:			modules/nxs-metadata/nxs-metadata.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-metadata.o modules/nxs-metadata/nxs-metadata.c

objs/nxs-net.o:				modules/nxs-net/nxs-net.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-net.o modules/nxs-net/nxs-net.c

objs/nxs-poll.o:			modules/nxs-poll/nxs-poll.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-poll.o modules/nxs-poll/nxs-poll.c

objs/nxs-buf.o:				modules/nxs-buf/nxs-buf.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-buf.o modules/nxs-buf/nxs-buf.c

objs/nxs-bchain.o:			modules/nxs-bchain/nxs-bchain.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-bchain.o modules/nxs-bchain/nxs-bchain.c

objs/nxs-pipesem.o:			modules/nxs-pipesem/nxs-pipesem.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-pipesem.o modules/nxs-pipesem/nxs-pipesem.c
	
objs/nxs-ncurses.o:			modules/nxs-ncurses/nxs-ncurses.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-ncurses.o modules/nxs-ncurses/nxs-ncurses.c

objs/nxs-xml.o:				modules/nxs-xml/nxs-xml.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-xml.o modules/nxs-xml/nxs-xml.c

objs/nxs-json.o:			modules/nxs-json/nxs-json.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-json.o modules/nxs-json/nxs-json.c

objs/nxs-cfg-json.o:			modules/nxs-cfg-json/nxs-cfg-json.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-cfg-json.o modules/nxs-cfg-json/nxs-cfg-json.c

objs/nxs-rest-api-common.o:		modules/nxs-rest-api-common/nxs-rest-api-common.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-rest-api-common.o modules/nxs-rest-api-common/nxs-rest-api-common.c

objs/nxs-rest-api.o:			modules/nxs-rest-api/nxs-rest-api.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-rest-api.o modules/nxs-rest-api/nxs-rest-api.c
	
objs/nxs-curl.o:			modules/nxs-curl/nxs-curl.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-curl.o modules/nxs-curl/nxs-curl.c

objs/nxs-base64.o:			modules/nxs-base64/nxs-base64.c \
					$(NXS_CORE_HEADERS)
			
	$(CC) -c $(CFLAGS) $(CFLAGS32) $(USE_NXS_MODULES) $(NXS_CORE_INCLUDES) -o objs/nxs-base64.o modules/nxs-base64/nxs-base64.c
