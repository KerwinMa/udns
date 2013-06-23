# $Id: Makefile,v 1.15 2004/06/30 18:36:14 mjt Exp $
# libudns Makefile
#
UDNS_SRCS = udns_dn.c udns_dntosp.c udns_parse.c udns_resolver.c udns_misc.c \
	udns_rr_a.c udns_rr_ptr.c udns_rr_mx.c udns_rr_txt.c udns_bl.c
UDNS_HDRS = udns.h
UDNS_GENS = udns_codes.c
UDNS_DIST = udns.h udns.3 $(UDNS_SRCS) Makefile TODO ex-dig.c
UDNS_OBJS = $(UDNS_SRCS:.c=.o) $(UDNS_GENS:.c=.o)
UDNS_NAME = udns
UDNS_LIBS = lib$(UDNS_NAME).a
UDNS_VERS = 0.0.2

CFLAGS = -Wall -W -Wmissing-prototypes -O2 -DHAVE_POLL
AWK = awk

lib$(UDNS_NAME).a: $(UDNS_OBJS)
	-rm -f $@
	$(AR) rv $@ $(UDNS_OBJS)
.c.o:
	$(CC) $(CFLAGS) -c $<

udns_codes.c:	udns.h Makefile
	@echo Generating $@
	@set -e; exec >$@.tmp; \
	set T type C class R rcode; \
	echo "/* Automatically generated. */"; \
	echo "#include \"udns.h\""; \
	echo "#include <stdio.h>"; \
	while [ "$$1" ]; do \
	 echo; \
	 echo "const struct dns_nameval dns_$${2}tab[] = {"; \
	 $(AWK) "/^  DNS_$${1}_[A-Z0-9_]+[ 	]*=/ \
	  { printf \" {%s,\\\"%s\\\"},\\n\", \$$1, substr(\$$1,7) }" \
	  $< ; \
	 echo " {0,0}};"; \
	 echo "const char *dns_$${2}name(enum dns_$${2} code) {"; \
	 echo " static char nm[20];"; \
	 echo " switch(code) {"; \
	 $(AWK) "BEGIN{i=0} \
	   /^  DNS_$${1}_[A-Z0-9_]+[ 	]*=/ \
	   {printf \" case %s: return dns_$${2}tab[%d].name;\\n\",\$$1,i++}\
	   " $< ; \
	 echo " }"; \
	 echo " sprintf(nm,\"%d\", code);"; \
	 echo " return nm;"; \
	 echo "}"; \
	 shift 2; \
	done
	@mv $@.tmp $@

$(UDNS_NAME).3.html: $(UDNS_NAME).3
	groff -man -Thtml $< > $@.tmp
	mv $@.tmp $@

$(UDNS_OBJS): $(UDNS_HDRS)

dist: $(UDNS_NAME)-$(UDNS_VERS).tar.gz
$(UDNS_NAME)-$(UDNS_VERS).tar.gz: $(UDNS_DIST)
	tar cvfz $@ $(UDNS_DIST)
clean:
	rm -f $(UDNS_OBJS) $(UDNS_GENS)
distclean: clean
	rm -f $(UDNS_LIBS) $(UDNS_NAME).3.html
