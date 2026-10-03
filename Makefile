CXX      ?= g++
CXXFLAGS ?= -O2 -march=native -std=c++17
BIN      := bin

all: $(BIN)/cert $(BIN)/exact $(BIN)/mc $(BIN)/hybrid_double

$(BIN)/%: src/%.cpp | $(BIN)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BIN):
	mkdir -p $(BIN)

quick: all            ## ~1 min: n=120 (python) + lower bounds n=41..44 + MC calibration
	bash scripts/reproduce_quick.sh

full: all             ## ~30 min, needs ~3.5 GB RAM: adds n=90 certificate, exact a(26..40), MC sweep 41..100
	bash scripts/reproduce_full.sh

clean:
	rm -rf $(BIN) U_*.bin

.PHONY: all quick full clean
