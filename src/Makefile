.PHONY: all clean install uninstall dvi dist test gcov_report style check

BUILD_DIR = ../build
PREFIX ?= ../build
INSTALL_DIR = $(PREFIX)/games

all: release

release:
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake .. -DRelease=ON && make

test:
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake .. -DBUILD_TESTING=ON && make && ./$(BUILD_DIR)/snake_tests

gcov_report: clean
	mkdir -p $(BUILD_DIR) && mkdir -p $(BUILD_DIR)/report && cd $(BUILD_DIR) && cmake .. -DBUILD_TESTING=ON -DCMAKE_CXX_FLAGS="--coverage" && make && ./$(BUILD_DIR)/snake_tests
	gcovr -r . --html --html-details -o $(BUILD_DIR)/report/coverage.html --decisions --exclude-throw-branches --exclude-unreachable-branches --exclude ".*test.*" ../build/

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.tar.gz
	rm -f coverage*
	rm -rf gcov_report
	rm -f *.dat

install: all
	@if [ ! -d $(INSTALL_DIR) ]; then mkdir -p $(INSTALL_DIR); fi
	install -d $(INSTALL_DIR)
	install -m 755 $(BUILD_DIR)/tetris_cli $(INSTALL_DIR)/
	install -m 755 $(BUILD_DIR)/snake_cli $(INSTALL_DIR)/
	install -m 755 $(BUILD_DIR)/tetris_desktop $(INSTALL_DIR)/
	install -m 755 $(BUILD_DIR)/snake_desktop $(INSTALL_DIR)/
	@echo "Installed to $(INSTALL_DIR)"

uninstall:
	rm -f $(INSTALL_DIR)/tetris_cli
	rm -f $(INSTALL_DIR)/snake_cli
	rm -f $(INSTALL_DIR)/tetris_desktop
	rm -f $(INSTALL_DIR)/snake_desktop
	@echo "Uninstalled from $(INSTALL_DIR)"

dvi:
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && cmake .. -DBUILD_DVI=ON && make doxygen
	@echo "Документация готова в $(BUILD_DIR)/docs/html/index.html"

dist:
	mkdir -p ../dist
	cp -r ../src ../CMakeLists.txt ../dist/
	cd ../dist && tar -czf ../dist/brickgame.tar.gz src CMakeLists.txt
	rm -rf ../dist/src ../dist/CMakeLists.txt

style:
	find ../src -name "*.cpp" -o -name "*.h" -o -name "*.c" | xargs clang-format -n --style=Google

cppcheck:
	cppcheck --library=qt --enable=all  --suppress=missingIncludeSystem --suppress=unusedFunction ../src/brick_game/snake

valgrind: test
	valgrind --leak-check=full ./../build/snake_tests

check: clean style cppcheck valgrind
	@echo "All checks passed!"