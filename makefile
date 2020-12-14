CXX=g++
CFLAGS=-O3 -Werror -Wpedantic -std=c++17 

ODIR=obj
IDIR=include
SDIR=src

IFLAGS=-Iinclude -Ilib
LFLAGS=-larmadillo

_OBJ=ray.o hitable.o BVH.o hitableList.o material.o path.o sphere.o main.o camera.o
_DEPS=ray.h BVH.h  hitable.h  hitableList.h  material.h  camera.h  path.h  ppmWriter.h sphere.h  texture.h
_SRC=movingCamera.cpp
_SRC=BVH.cpp  camera.cpp  hitable.cpp  hitableList.cpp  material.cpp  path.cpp  ray.cpp main.cpp

OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
SRC = $(patsubst %,$(SDIR)/%,$(_SRC))


all: main 

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CXX) $(CFLAGS) $(IFLAGS) -c -o $@.out $<

main: $(OBJ)
	$(CXX) $(CFLAGS) $(IFLAGS) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o 
