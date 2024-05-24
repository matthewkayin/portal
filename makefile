ifeq ($(OS),WINDOWS_NT)
	PLATFORM := WIN32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),DARWIN)
		PLATFORM := OSX
	endif
endif

ASSEMBLY := portal
EXTENSION :=
DIR := $(subst /,\,${CURDIR})
LIB_DIR := lib
BUILD_DIR := bin
OBJ_DIR := obj
COMPILER_FLAGS := -g -std=c++11 -Wall -O0
INCLUDE_FLAGS := -Isrc -Ivendor
LINKER_FLAGS := -g -L$(LIB_DIR) -lSDL2 -lSDL2_ttf
DEFINES := -D_CRT_SECURE_NO_WARNINGS

ifeq (PLATFORM,WIN32)
	EXTENSION := .exe
	LINKER_FLAGS += -luser32

# Make does not offer a recursive wildcard function, so here's one:
	rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
	SRC_FILES := $(call rwildcard,src/,*.cpp) # Get all .c files
	SRC_FILES += $(call rwildcard,vendor/,*.cpp) # Get all .c files
	DIRECTORIES := .\src $(subst $(DIR),,$(shell dir .\src /S /AD /B | findstr /i src)) # Get all directories under src.
	DIRECTORIES += .\vendor $(subst $(DIR),,$(shell dir .\vendor /S /AD /B | findstr /i vendor)) # Get all directories under src.
else
	SRC_FILES := $(shell find src -name *.cpp)
	SRC_FILES += $(shell find vendor -name *.cpp)
	DIRECTORIES := $(shell find src -type d)
	DIRECTORIES += $(shell find vendor -type d)
endif

OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o) # Get all compiled .c.o objects for engine

all: scaffold compile link

.PHONY: scaffold
scaffold:
	@echo Scaffolding...
ifeq (PLATFORM,WIN32)
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(BUILD_DIR) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && copy $(LIB_DIR) $(BUILD_DIR)
else
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
endif
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # link
	@echo Linking $(ASSEMBLY)...
ifeq (PLATFORM,WIN32)
	@clang++ $(OBJ_FILES) -o $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
else
	@clang++ $(OBJ_FILES) -o $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)
endif

.PHONY: compile
compile: #compile .c files
	@echo Compiling...

.PHONY: clean
clean: # clean build directory
ifeq (PLATFORM,WIN32)
	if exist $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) del $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)
	rmdir /s /q $(OBJ_DIR)
else
	@rm -rf $(BUILD_DIR)/$(ASSEMBLY)$(EXTENSION)
	@rm -rf $(OBJ_DIR)/$(ASSEMBLY)
endif

$(OBJ_DIR)/%.cpp.o: %.cpp # compile .c to .c.o object
	@echo   $<...
	@clang++ $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)