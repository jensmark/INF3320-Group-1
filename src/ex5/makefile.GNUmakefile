MACHINE= $(shell uname -s)

ifeq ($(MACHINE),Darwin)
    OPENGL_INC= -FOpenGL
    OPENGL_LIB= -framework OpenGL
    SDL_INC= `sdl-config --cflags`
    SDL_LIB= `sdl-config --libs`
else
    OPENGL_INC= -I/usr/X11R6/include
    OPENGL_LIB= -I/usr/lib64 -lGL -lGLU
    SDL_INC= `sdl2-config --cflags`
    SDL_LIB= `sdl2-config --libs`
endif

OBJS        = ex5.o
CC          = g++
CFLAGS      = -g
INCLUDE     = $(SDL_INC) $(OPENGL_INC)
LIBS        = $(SDL_LIB) $(OPENGL_LIB)

EXEC        = ex5

$(EXEC): $(OBJS)
    $(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

%.o:    %.cpp
    $(CC) -c $(CFLAGS) -o $@ $< $(INCLUDE)

clean:
    rm -f $(EXEC) $(OBJS)