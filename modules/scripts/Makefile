ENA_FILES := $(wildcard *.ena)

all:
	@mkdir -p $(output)/bin
ifneq ($(ENA_FILES),)
	@for f in $(ENA_FILES); do \
		cp "$$f" "$(output)/bin/$${f%.ena}"; \
	done
endif