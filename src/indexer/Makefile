CXX := g++

# LLVM C++ options and LLVM link options
LLVM_CXXFLAGS := $(shell llvm-config --cxxflags)
#LLVMLDFLAGS := $(shell llvm-config --ldflags) -L/home/codesearch/.jumbo/lib 
LLVMLDFLAGS := $(shell llvm-config --ldflags)
CXXFLAGS := $(LLVM_CXXFLAGS)

SOURCES = index.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXES = $(OBJECTS:.o=)
CLANGLIBS = \
				-lclangTooling\
				-lclangFrontendTool\
				-lclangFrontend\
				-lclangDriver\
				-lclangSerialization\
				-lclangCodeGen\
				-lclangParse\
				-lclangSema\
				-lclangStaticAnalyzerFrontend\
				-lclangStaticAnalyzerCheckers\
				-lclangStaticAnalyzerCore\
				-lclangAnalysis\
				-lclangARCMigrate\
				-lclangRewriteFrontend\
				-lclangRewrite\
				-lclangEdit\
				-lclangAST\
				-lclangLex\
				-lclangBasic\
				$(shell llvm-config --libs)\
				$(shell llvm-config --system-libs)\
                -lcurses

all:	$(OBJECTS) $(EXES)

%: %.o
	$(CXX) -o $@  $< $(CLANGLIBS) $(LLVMLDFLAGS) 

.PHONY: all clean

clean:
	rm -f $(OBJECTS) $(EXES) *.o
