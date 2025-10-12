include .env

CC = g++
CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include -I$(LIBS_PATH)/include
LDFLAGS = -L$(LIBS_PATH)/lib -L$(VULKAN_SDK_PATH)/lib -Wl,-rpath,$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

TARGET = a.out

compSources = $(shell find ./shaders -type f -name "*.comp")
compObjFiles = $(patsubst %.comp, %.comp.spv, $(compSources))

$(TARGET): $(compObjFiles) *.cpp *.hpp
	$(CC) $(CFLAGS) -o $(TARGET) *.cpp $(LDFLAGS)

%.spv: %
	${GLSLC} $< -o $@

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f shaders/*.spv
