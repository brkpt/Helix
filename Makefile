SOURCE=Grid.cpp stdafx.cpp triangle.cpp Win32DX9.cpp
OBJS=$(SOURCE:.cpp=.o)
EXE=Win32DX9.exe

%.o : %.cpp
	gcc -o $@ -I C:/PROGRA~1/MICROS~1.0SD/Include -g $<
	
all: $(EXE)

$(EXE): $(OBJS)
	gcc -mwindows -o $@ $< C:/PROGRA~1/MICROS~1.0SD/Lib/x86/d3d9.lib

