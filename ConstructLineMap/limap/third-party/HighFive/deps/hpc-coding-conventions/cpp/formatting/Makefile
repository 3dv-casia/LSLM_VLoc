CLANG_FORMAT ?= clang-format
PANDOC ?= pandoc

SNIPPETS = $(wildcard snippets/*.cpp)

all: README.html

clean:
	$(RM) README.md

distclean: clean
	$(RM) README.html

# README.html
.md.html: $<
	$(PANDOC) -s -S -c github-pandoc.css $< -o $@
README.html: github-pandoc.css

# README.md
README.md: README.md.jinja formatting.py $(SNIPPETS)
	@# format snippets that triggered rebuild of target
	$(foreach snippet,$(filter snippets/%,$?),$(CLANG_FORMAT) -i $(snippet);)

	./formatting.py $< $@

# update all snippets when .clang-format is changed
$(SNIPPETS): ../.clang-format
	$(CLANG_FORMAT) -i $@
	touch $@

.SUFFIXES: .md .html
