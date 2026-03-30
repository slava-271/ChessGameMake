QT_LIBS = $(QT_PATH)/lib

QT_INCLUDE = $(QT_PATH)/include
QT_HEADERS = -I$(QT_INCLUDE) -I$(QT_INCLUDE)/QtCore -I$(QT_INCLUDE)/QtWidgets -I$(QT_INCLUDE)/QtGui -I$(QT_INCLUDE)/QtNetwork 

BUILD_DIR = build
AUTOGEN_DIR = build/autogen
TARGET = $(BUILD_DIR)/ChessGameMake

CXX ?= g++

SOURCES = main.cpp mainwindow.cpp bitboard.cpp chessboard.cpp
HEADERS = mainwindow.h bitboard.h chessboard.h
MOC_HEADERS = mainwindow.h chessboard.h
RCC_HEADERS = pieces.qrc

MOC_PREFIX = $(addprefix moc_, $(MOC_HEADERS))
RCC_PREFIX = $(addprefix qrc_, $(RCC_HEADERS))
CODEGEN_PREFIX = $(MOC_PREFIX) $(RCC_PREFIX)


MOC_SOURCES = $(addprefix $(AUTOGEN_DIR)/, $(MOC_PREFIX:.h=.cpp))
RCC_SOURCES = $(addprefix $(AUTOGEN_DIR)/, $(RCC_PREFIX:.qrc=.cpp))
CODEGEN_SOURCES = $(MOC_SOURCES) $(RCC_SOURCES)
ALL_SOURCES = $(SOURCES) $(CODEGEN_SOURCES)


SOURCE_OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
MOC_OBJECTS = $(addprefix $(BUILD_DIR)/, $(MOC_PREFIX:.h=.o))
RCC_OBJECTS = $(addprefix $(BUILD_DIR)/, $(RCC_PREFIX:.qrc=.o))
ALL_OBJECTS = $(SOURCE_OBJECTS) $(MOC_OBJECTS) $(RCC_OBJECTS)

LFLAGS = -Wl,-rpath,$(QT_LIBS) -Wl,-rpath-link,$(QT_LIBS)
USED_LIBS = -L$(QT_LIBS) -lQt6Core -lQt6Widgets -lQt6Gui -lQt6Network -pthread
DEFINES = -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
CXXFLAGS = -std=c++20 -pipe -fPIC -MMD -MP $(DEFINES) 

CODEGEN_PATH = $(QT_PATH)/libexec
MOC = $(CODEGEN_PATH)/moc
RCC = $(CODEGEN_PATH)/rcc


all: $(TARGET)

build: all

directories: 
	mkdir -p $(AUTOGEN_DIR)


$(TARGET): $(ALL_OBJECTS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(LFLAGS) $^ -o $@ $(USED_LIBS)

codegen: $(CODEGEN_SOURCES)

build-debug: $(CXXFLAGS) += -g3 -Wall -Wextra -O0 -DQT_DEBUG 
build-debug: $(TARGET)
build-release: $(CXXFLAGS) += -O2 -DNDEBUG  -DQT_NO_DEBUG
build-release: $(TARGET)

$(AUTOGEN_DIR)/moc_%.cpp: %.h | directories
	$(MOC) $< -o $@

$(AUTOGEN_DIR)/qrc_%.cpp: %.qrc | directories
	$(RCC) $< -o $@

$(BUILD_DIR)/%.o: %.cpp | directories
	$(CXX) $(CXXFLAGS) $(QT_HEADERS)  -c $< -o $@ 

$(BUILD_DIR)/%.o: $(AUTOGEN_DIR)/%.cpp | directories
	$(CXX) $(CXXFLAGS) $(QT_HEADERS)  -c $< -o $@ 	


clean: 
	rm -rf $(BUILD_DIR)

DEPENDS = $($(ALL_OBJECTS:.o=.d))
-include $(DEPENDS)

.PHONY: all clean build build-release build-debug codegen




