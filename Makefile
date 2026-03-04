CXX = g++
CXXFLAGS = -Wall -Wextra
LDFLAGS = -ljack -lm -lpthread
JACK_LIB_PATH = /usr/lib/x86_64-linux-gnu/pipewire-0.3/jack
TARGET = denzel
SRC = main.cpp jackAPI.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -L$(JACK_LIB_PATH) -Wl,-rpath,$(JACK_LIB_PATH)

clean:
	rm -f $(TARGET)
