SRCS=$(wildcard *.cpp)
DEPS=$(SRCS:.cpp=.d)
OBJS=$(SRCS:.cpp=.o)
PROG=main

INPUTS0=$(sort $(wildcard tests/debug0/in*))
INPUTS1=$(sort $(wildcard tests/debug1/in*))
INPUTS2=$(sort $(wildcard tests/debug2/in*))
SOLNS0=$(INPUTS0:tests/debug0/in%=tests/debug0/sol%)
SOLNS1=$(INPUTS2:tests/debug1/in%=tests/debug1/sol%)
SOLNS2=$(INPUTS3:tests/debug2/in%=tests/debug2/sol%)
OUTS0=$(INPUTS0:tests/debug0/in%=tests/debug0/out%)
OUTS1=$(INPUTS1:tests/debug1/in%=tests/debug1/out%)
OUTS2=$(INPUTS2:tests/debug2/in%=tests/debug2/out%)
DIFFS0=$(INPUTS0:tests/debug0/in%=tests/debug0/diff%)
DIFFS1=$(INPUTS1:tests/debug1/in%=tests/debug1/diff%)
DIFFS2=$(INPUTS2:tests/debug2/in%=tests/debug2/diff%)

.SECONDARY: $(OUTS0) $(OUTS1) $(OUTS2)

CXX=g++
CXXFLAGS=-Wall -pedantic -O2 -std=c++14 -I ~/Packages/boost_1_76_0
RM=rm -fv
DIFF=diff

COMPILE.c = $(CXX) $(CXXFLAGS)
OUTPUT_OPTION=-MMD -MP -c
LINK.o = $(CXX) $(LDFLAGS)

all: $(PROG)

%.o: %.cpp
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(PROG) : $(OBJS)
	$(LINK.o) $^ -o $@

.PHONY: clean run test

run: $(PROG)
	./$(PROG)

clean:
	$(RM) $(PROG) $(OBJS) $(DEPS) $(OUTS0) $(OUTS1) $(OUTS2) $(DIFFS0) $(DIFFS1) $(DIFFS2)

test0: $(DIFFS0)

test1: $(DIFFS1)

test2: $(DIFFS2)

tests/debug0/out%: tests/debug0/in% $(PROG)
	./$(PROG) < $< > $@

tests/debug1/out%: tests/debug1/in% $(PROG)
	./$(PROG) < $< > $@

tests/debug2/out%: tests/debug2/in% $(PROG)
	./$(PROG) < $< > $@

tests/debug0/diff%: tests/debug0/out% tests/debug0/sol%
	-$(DIFF) $< $(<:tests/debug0/out%=tests/debug0/sol%) > $@
	-@[ -s $@ ] && echo "Differences found in $@" || true

tests/debug1/diff%: tests/debug1/out% tests/debug1/sol%
	-$(DIFF) $< $(<:tests/debug1/out%=tests/debug1/sol%) > $@
	-@[ -s $@ ] && echo "Differences found in $@" || true

tests/debug2/diff%: tests/debug2/out% tests/debug2/sol%
	-$(DIFF) $< $(<:tests/debug2/out%=tests/debug2/sol%) > $@
	-@[ -s $@ ] && echo "Differences found in $@" || true

-include $(DEPS)