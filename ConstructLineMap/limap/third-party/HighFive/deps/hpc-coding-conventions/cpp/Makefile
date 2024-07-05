subdirs = formatting

all clean distclean:
	$(foreach dir,$(subdirs),$(MAKE) -C $(dir) $(MAKEFLAGS) $@;)
