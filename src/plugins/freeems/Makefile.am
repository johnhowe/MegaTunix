plugindir = $(datadir)/MegaTunix/plugins

INCLUDES = \
        -I$(top_srcdir)/include \
        @GTK_CFLAGS@ \
        @DEPRECATED_CFLAGS@ \
        @LIBXML2_CFLAGS@ \
        @LIBGLADE_CFLAGS@ \
        @MEGATUNIX_DEFINES@


plugin_LTLIBRARIES = libfreeems.la 

libfreeems_la_SOURCES = \
	Makefile \
	datamgmt.c \
	datamgmt.h \
	fileio.c \
	fileio.h \
	freeems_benchtest.c \
	freeems_benchtest.h \
	freeems_errors.c \
	freeems_errors.h \
	freeems_globaldefs.h \
	freeems_comms.c \
	freeems_comms.h \
	freeems_menu_handlers.c \
	freeems_menu_handlers.h \
	freeems_gui_handlers.c \
	freeems_gui_handlers.h \
	freeems_helpers.c \
	freeems_helpers.h \
	freeems_plugin.c \
	freeems_plugin.h \
	freeems_rtv_loader.c \
	freeems_rtv_loader.h \
	freeems_rtv_processor.c \
	freeems_rtv_processor.h \
	interrogate.c \
	interrogate.h \
	packet_handlers.c \
	packet_handlers.h \
	vetable_gui.c \
	vetable_gui.h 

libfreeems_la_LDFLAGS = -shared $(no_undefined) -module
libfreeems_la_LIBADD = ../../../mtxcommon/libmtxcommon.la ../../../widgets/libmtxcombo_mask.la ../../../widgets/libmtxcurve.la ../../../widgets/libmtxgauge.la @EXTRA_LIBS@ @GTK_LIBS@ @LIBXML2_LIBS@ @LIBGLADE_LIBS@


COMMON_FLAGS = @PROF_FLAGS@ @DEBUG_FLAGS@ 

if CROSS_COMPILING
no_undefined = -no-undefined
endif

AM_CFLAGS += $(COMMON_FLAGS)

CLEANFILES = *~ core gmon.out nohup.out 

EXTRA_DIST = README


