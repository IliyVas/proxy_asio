CXX=c++
BINDIR=bin
OBJDIR=obj
SRCDIR=src
MKDIRS=$(BINDIR) $(OBJDIR)
IDIR =lib/asio/include
CXXFLAGS=-c -Wall -I$(IDIR) -DASIO_STANDALONE -O3
LDFLAGS=-pthread

SRC_FILES := $(wildcard $(SRCDIR)/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(SRC_FILES:.cpp=.o)))
TARGET=$(BINDIR)/proxy_asio

.PHONY: all
all: $(OBJ_FILES) $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CXX) $(LDFLAGS) $(OBJ_FILES) -o $@

.PHONY: clean
clean:
	rm -rf $(MKDIRS)

$(info $(shell mkdir -p $(MKDIRS)))
