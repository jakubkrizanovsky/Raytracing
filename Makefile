include .env

ARGS = --gpu scenes/default.json

TARGET = raytracer.out
SRC_DIR = ./src
OBJ_DIR = ./obj

CXX = g++
CXXFLAGS = -o0 -std=c++17 -I$(SRC_DIR) -I$(VULKAN_SDK_PATH)/include -I$(LIBS_PATH)/include -MMD -MP -Wall
LDFLAGS = -L$(LIBS_PATH)/lib -L$(VULKAN_SDK_PATH)/lib -Wl,-rpath,$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan -ltbb

SRC_FILES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
DEP_FILES := $(OBJ_FILES:.o=.d)

COMP_SRCS = $(shell find ./shaders -type f -name "*.comp")
COMP_OBJS = $(patsubst %.comp, %.comp.spv, $(COMP_SRCS))

$(TARGET): $(COMP_OBJS) $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_FILES) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.spv: %
	${GLSLC} $< -o $@

.PHONY: run clean

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -f $(TARGET)
	rm -rf $(OBJ_DIR)
	rm -f shaders/*.spv

-include $(DEP_FILES)
