CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = -ljack -lm -lpthread -lrt

FTXUI_DIR = ../FTXUI/build
FTXUI_INC = -I$(FTXUI_DIR)/../include
FTXUI_LIB = $(FTXUI_DIR)/libftxui-component.a $(FTXUI_DIR)/libftxui-dom.a $(FTXUI_DIR)/libftxui-screen.a

TARGET = denzel
SRC = main.cpp jackAPI.cpp biquad.cpp filterChain.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC:.cpp=.o)
	$(CXX) $(CXXFLAGS) $(FTXUI_INC) -o $@ $^ $(FTXUI_LIB) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(FTXUI_INC) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
