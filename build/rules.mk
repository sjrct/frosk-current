#
# build/rules.mk
#
# Contains special rules to build. Should be included after WHAT_TO_BUILD and
#  WHAT_TO_CLEAN are set.
#

ifndef RULES_MK_INCLUDED
RULES_MK_INCLUDED := yes

#
# default target
#
.DEFAULT_GOAL := build
.PHONY: build
build: $(WHAT_TO_BUILD)

#
# clean special target
#
.PHONY:
clean: $(WHAT_TO_CLEAN)

#
# Help special target
#
.PHONY: help
help:
	@ echo
	@ echo "Special make targets:"
	@ echo "    build - builds stuff in local directory and below (default)"
	@ echo "    clean - remove files built in local directory and below"
	@ echo "    help  - show this help"
	@ echo
	@ echo "Verbosity options (V=?):"
	@ echo "    Lvl | Effect"
	@ echo "    ====|=========="
	@ echo "    0   | No output"
	@ echo "    1   | Abbreviated commands"
	@ echo "    2   | Long significant commands"
	@ echo "    3   | Long form of all commands"
	@ echo

endif
