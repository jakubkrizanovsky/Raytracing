include .env

TARGET = a.out
SRC := ./src

CC = g++
CFLAGS = -std=c++17 -I$(SRC) -I$(VULKAN_SDK_PATH)/include -I$(LIBS_PATH)/include
LDFLAGS = -L$(LIBS_PATH)/lib -L$(VULKAN_SDK_PATH)/lib -Wl,-rpath,$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

compSources = $(shell find ./shaders -type f -name "*.comp")
compObjFiles = $(patsubst %.comp, %.comp.spv, $(compSources))

$(TARGET): $(compObjFiles) $(SRC)/*.cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)/*.cpp $(LDFLAGS)

%.spv: %
	${GLSLC} $< -o $@

.PHONY: run clean

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f shaders/*.spv
