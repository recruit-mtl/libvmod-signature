AM_CPPFLAGS = @VMOD_INCLUDES@ -Wall -Werror

vmoddir = @VMOD_DIR@
vmod_LTLIBRARIES = libvmod_signature.la

libvmod_signature_la_LDFLAGS = -module -export-dynamic -avoid-version -shared

libvmod_signature_la_SOURCES = \
	vmod_signature.c

nodist_libvmod_signature_la_SOURCES = \
	vcc_if.c \
	vcc_if.h

vmod_signature.lo: vcc_if.c vcc_if.h

vcc_if.c: vcc_if.h

vcc_if.h: @VMODTOOL@ $(top_srcdir)/src/vmod_signature.vcc
	@VMODTOOL@ $(top_srcdir)/src/vmod_signature.vcc

VMOD_TESTS = $(top_srcdir)/src/tests/*.vtc
.PHONY: $(VMOD_TESTS)

$(top_srcdir)/src/tests/*.vtc: libvmod_signature.la
	@VARNISHTEST@ -Dvarnishd=@VARNISHD@ -Dvmod_topbuild=$(abs_top_builddir) $@

check: $(VMOD_TESTS)

EXTRA_DIST = \
	vmod_signature.vcc \
	$(VMOD_TESTS)

CLEANFILES = \
	$(builddir)/vcc_if.c \
	$(builddir)/vcc_if.h \
	$(builddir)/vmod_signature.rst \
	$(builddir)/vmod_signature.man.rst
