POSTFIX ?= usr/local
DESTDIR ?= /

RM ?= rm -rf

.PHONY: all
all: unifetch

unifetch: unifetch.c unifetch_config.h

.PHONY: clean
clean:
	$(RM) unifetch
	$(RM) README

.PHONY: install
install: unifetch unifetch.1
	install -d $(DESTDIR)$(POSTFIX)/bin/
	install -m 775 unifetch $(DESTDIR)$(POSTFIX)/bin/
	install -d $(DESTDIR)$(POSTFIX)/share/man/man1/
	install -m 644 unifetch.1 $(DESTDIR)$(POSTFIX)/share/man/man1/

.PHONY: uninstall
uninstall:
	$(RM) $(DESTDIR)$(POSTFIX)/bin/unifetch

README: unifetch.1
	mandoc -mdoc -T ascii unifetch.1 | col -b > README
