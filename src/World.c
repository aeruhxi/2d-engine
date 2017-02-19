#include "World.h"
#include "Model.h"
#include "Shader.h"
#include "GMath.h"
#include "Entity.h"
#include <stdio.h>

static Model model;
static Tilemap tilemap;
static GLuint bg;
static float* compMat4;
static float scale = TILE_SIZE/2;
static float WIDTH, HEIGHT;
static float RATIO;
Entity *Player;

//load the game world
void loadWorld(float w, float h, Level lvl, Entity *player) {

	tilemap = lvl.map;
	bg = lvl.bg;
	WIDTH = w;
	HEIGHT = h;
	Player = player;

	shaders("shaders/vertex.glsl", "shaders/fragment.glsl");
	model = loadModel();

	RATIO = w/h;
	float* proj = loadOrtho(0.0, w*RATIO, 0.0, h*RATIO, -1.0f, 1.0f);

	bindShader();
	loadProjectionMatrix(proj);
	unBindShader();

	free(proj);
}

void bindModel() {
	glBindVertexArray(model.vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.indiID);

}

void unBindModel() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

//prepare OpenGL before starting loop
void prepOGL() {

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	bindModel();
	bindShader();
}

//prepare to render something on da screen
void prepRender() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void renBackground() {
	//backGround
	compMat4 = createMat4(NULL);
	loadIdentity(compMat4);
	glBindTexture(GL_TEXTURE_2D, bg);
	//translateMat(compMat4, (Vec2D){(0), (0)});
	scale2DMat(compMat4,(Vec2D){(WIDTH/2*RATIO), (HEIGHT/2*RATIO)} );
	loadCompositeMatrix(compMat4);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void drawMap() {
	//Render tilemap
	for(int i = 0; i < tilemap.nTiles; ++i) {

		loadIdentity(compMat4);

		glBindTexture(GL_TEXTURE_2D, tilemap.tiles[i].texture);

		translateMat(compMat4, (Vec2D){tilemap.tiles[i].pos.x * TILE_SIZE,
									   tilemap.tiles[i].pos.y * TILE_SIZE});
		scaleMat(compMat4, scale);
		loadCompositeMatrix(compMat4);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

//Render the world
void renderWorld() {

	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr,"OpenGL error: %d\n", err);
    }

	renBackground();
	drawMap();


	//Entities
	loadIdentity(compMat4);
	translateMat(compMat4, (Vec2D){Player->pos.x,Player->pos.y});
	scaleMat(compMat4, scale);
	loadCompositeMatrix(compMat4);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

//cleanUp
void worldCleanUp() {
	unBindShader();
	unBindModel();
	free(compMat4);
	freeMap(&tilemap);
	shaderCleanUp();
}