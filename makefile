OBJS = \
src/app/application.cpp \
src/app/settings.cpp \
src/app/client.cpp \
src/app/host.cpp \
src/app/lobby.cpp \
src/app/Main.cpp \
\
src/net/Net.cpp \
src/net/net_message.cpp \
\
src/tab/tab_cell.cpp \
src/tab/tab_font.cpp \
src/tab/tab_scrollbar.cpp \
src/tab/tab_table.cpp \
\
src/ui/ui_lobby.cpp \
src/ui/ui_main.cpp \
src/ui/ui_log.cpp \
src/ui/ui.cpp \
\
src/map/proptype.cpp \
src/map/prop.cpp \
src/map/map2d.cpp \
\
src/util/geom.cpp \
src/util/shader.cpp \
src/util/texture.cpp \
src/util/animation.cpp \
src/util/shader_font.cpp \
src/util/raw_data.cpp \
\
src/game/game.cpp \
src/game/character.cpp \
src/game/movement.cpp \

CC = g++

DIRECTORY_OPTIONS = -Isrc -Isrc/tab -Isrc/net -Isrc/ui -Isrc/app -Isrc/util -Isrc/map -Isrc/game

COMPILER_FLAGS = -std=gnu++0x -ggdb

LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_net -lSDL2_image -lGL -lGLEW

OBJ_NAME = af




all : $(OBJS) 

	$(CC) $(OBJS) $(COMPILER_FLAGS) $(DIRECTORY_OPTIONS) $(LINKER_FLAGS) -o $(OBJ_NAME)
