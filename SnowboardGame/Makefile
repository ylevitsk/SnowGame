MAIN = Main.cpp World.cpp BoundingSphere.cpp Shader.cpp Camera.cpp Texture.cpp Text.cpp -o game.out
MODELS = Player.cpp Terrain.cpp Tree.cpp
UTIL = third_party/*.cpp
LIBS = glfw3 freetype2 assimp

FLAGS = `pkg-config --cflags --libs --static $(LIBS)`
LINUX_EXTRA = -DGL_GLEXT_PROTOTYPES

mac:
	g++ $(MAIN) $(MODELS) $(UTIL) $(FLAGS) -Wno-shift-op-parentheses     
linux:
	g++ $(MAIN) $(MODELS) $(UTIL) $(FLAGS) $(LINUX_EXTRA)
windows:
	g++ $(MAIN) $(MODELS) $(UTIL) $(FLAGS)

run:
	./game.out