DESTDIR=/opt/kefir

install: $(LIBKEFIR_SO) $(BIN_DIR)/kefir
	@echo "Creating directories..."
	@install -d "$(DESTDIR)"/include/kefir
	@install -d "$(DESTDIR)"/lib
	@install -d "$(DESTDIR)"/bin
	@echo "Installing libraries.."
	@install -D "$(LIBKEFIR_SO).$(LIBKEFIR_SO_VERSION)" -t "$(DESTDIR)"/lib
	@ln -sf "$(DESTDIR)"/lib/libkefir.so.$(LIBKEFIR_SO_VERSION) "$(DESTDIR)"/lib/libkefir.so
	@install -D "$(LIBKEFIRRT_A)" -t "$(DESTDIR)"/lib
	@echo "Installing headers..."
	@cp -r --no-dereference -p "$(HEADERS_DIR)"/kefir "$(DESTDIR)"/include/kefir
	@echo "Installing binaries..."
	@install "$(BIN_DIR)"/kefir "$(DESTDIR)"/bin/kefircc
	@install "$(SCRIPTS_DIR)"/kefir.sh "$(DESTDIR)"/bin/kefir

uninstall:
	@echo "Removing binaries..."
	@rm -rf "$(DESTDIR)"/bin/kefir "$(DESTDIR)"/bin/kefircc
	@echo "Removing headers..."
	@rm -rf "$(DESTDIR)"/include/kefir
	@echo "Removing libraries..."
	@rm -rf "$(DESTDIR)"/lib/libkefir.so
	@rm -rf "$(DESTDIR)"/lib/libkefir.so.$(LIBKEFIR_SO_VERSION)
	@rm -rf "$(DESTDIR)"/lib/libkefirrt.a

.PHONY: install
