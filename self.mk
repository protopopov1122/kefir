LIBC_HEADERS=
LIBC_LIBS=

$(BOOTSTRAP_DIR)/stage1/kefir: $(BIN_DIR)/kefir
	@echo "Bootstrapping $@"
	@LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(BIN_DIR)/libs $(MAKE) -f $(ROOT)/bootstrap.mk bootstrap \
		SOURCE=$(SOURCE_DIR) \
		HEADERS=$(HEADERS_DIR) \
		BOOTSTRAP=$(BOOTSTRAP_DIR)/stage1 \
		KEFIRCC=$(BIN_DIR)/kefir \
		LIBC_HEADERS="$(LIBC_HEADERS)" \
		LIBC_LIBS="$(LIBC_LIBS)"

$(BOOTSTRAP_DIR)/stage2/kefir: $(BOOTSTRAP_DIR)/stage1/kefir
	@echo "Bootstrapping $@"
	@$(MAKE) -f $(ROOT)/bootstrap.mk bootstrap \
		SOURCE=$(SOURCE_DIR) \
		HEADERS=$(HEADERS_DIR) \
		BOOTSTRAP=$(BOOTSTRAP_DIR)/stage2 \
		KEFIRCC=$(BOOTSTRAP_DIR)/stage1/kefir \
		LIBC_HEADERS="$(LIBC_HEADERS)" \
		LIBC_LIBS="$(LIBC_LIBS)"

$(BOOTSTRAP_DIR)/stage2/comparison.done: $(BOOTSTRAP_DIR)/stage1/kefir $(BOOTSTRAP_DIR)/stage2/kefir
	@echo "Comparing stage1 and stage2 results"
	@$(SCRIPTS_DIR)/bootstrap_compare.sh "$(BOOTSTRAP_DIR)/stage1" "$(BOOTSTRAP_DIR)/stage2"
	@touch "$@"

bootstrap: $(BOOTSTRAP_DIR)/stage2/comparison.done
	@echo "Bootstrap successfully finished"

clean_bootstrap:
	@echo "Removing $(BOOTSTRAP_DIR)"
	@rm -rf $(BOOTSTRAP_DIR)

.PHONY: bootstrap
