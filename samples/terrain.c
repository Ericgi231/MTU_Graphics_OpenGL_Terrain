/** @file Draw an infinite city
 *
 * @author Eric Grant
 */

// Includes
//
#include "libkuhl.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <unistd.h>

// Global Vars
//
static GLuint program = 0; /**< id value for the GLSL program */
static GLuint cloudProgram = 1; /**< id value for the GLSL program */

static int quadWidth = 2000; //CHANGE THIS TO SET QUADS PER ROW
static float cloudHeight = 1;
static kuhl_geometry megaQuad;
static kuhl_geometry clouds;
static int showNorms = 0;
static int freeCam = 0;

//Math Helpers
//

/**
 * Copies vector 1 into vector 2
 * 
 * @param vec3 output vector
 * @param vec1 left operand
 * @param vec2 right operand
 * @return result vector
 */
float * vecCpy(float vec2[3], float vec1[3]){
    vec2[0] = vec1[0];
    vec2[1] = vec1[1];
    vec2[2] = vec1[2];
    return vec2;
}

/**
 * Computes vector from subbing vec2 from vec1
 * 
 * @param vec3 output vector
 * @param vec1 left operand
 * @param vec2 right operand
 * @return result vector
 */
float * vecSub(float vec3[3], float vec1[3], float vec2[3]){
    vec3[0] = vec1[0] - vec2[0];
    vec3[1] = vec1[1] - vec2[1];
    vec3[2] = vec1[2] - vec2[2];
    return vec3;
}

/**
 * Computes vector from adding vec2 to vec1
 * 
 * @param vec3 output vector
 * @param vec1 left operand
 * @param vec2 right operand
 * @return result vector
 */
float * vecAdd(float vec3[3], float vec1[3], float vec2[3]){
    vec3[0] = vec1[0] + vec2[0];
    vec3[1] = vec1[1] + vec2[1];
    vec3[2] = vec1[2] + vec2[2];
    return vec3;
}

/**
 * Normalizes vector with 3 dimensions
 * 
 * @param vec pixel location vector
 * @return normalized vector
 */
float * vecNormalize(float vec[3]){
    float magnitude = sqrt(pow(vec[0],2.0) + pow(vec[1], 2.0) + pow(vec[2],2.0));
    vec[0] = vec[0] / magnitude;
    vec[1] = vec[1] / magnitude;
    vec[2] = vec[2] / magnitude;
    return vec;
}

/**
 * Computes cross product of vec1 and vec2
 * 
 * @param vec3 output vector
 * @param vec1 left operand
 * @param vec2 right operand
 * @return result vector
 */
float * vecCrossProd(float vec3[3], float vec1[3], float vec2[3]){
	//         0  1  2
	// vec1 = [a, b ,c]
	// vec2 = [d, e, f]
	vec3[0] = (vec1[1]*vec2[2])-(vec1[2]*vec2[1]); //bf - ce (12 - 21)
	vec3[1] = (vec1[2]*vec2[0])-(vec1[0]*vec2[2]); //cd - af (20 - 02)
	vec3[2] = (vec1[0]*vec2[1])-(vec1[1]*vec2[0]); //ae - bd (01 - 10)
	return vec3;
}

/**
 * Computes dot product of two vectors
 * 
 * @param vec1 vector 1
 * @param vec2 vector 2
 * @return dot product of input vectors
 */
float vecDotProd(float vec1[3], float vec2[3]){
    return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
}

/**
 * Computes normal given two vertices on a triangle
 * 
 * @param vec4 output vector
 * @param vec1 vector 1
 * @param vec2 vector 2
 * @param vec3 vector 3
 * @return result vector
 */
float * triNormal(float vec4[3], float vec1[3], float vec2[3], float vec3[3]){
	float temp1[3], temp2[3];
	vecSub(temp1, vec1, vec2);
	vecSub(temp2, vec1, vec3);
	vecCrossProd(vec4, temp1, temp2);
	vecNormalize(vec4);
	return vec4;
}

/**
 * Generates a random number within a given range
 * 
 * @param start start number
 * @param end end number
 * @return random number
*/
float randomRange(float start, float end){
	float n = drand48(); //0 <> 1
	n = n * (end-start); // 0 <> end-start
	n = n + start; // start <> end
	return n;
}

// Object Init
//

/**
 * Create a 1x1 quad comprised of many tris
 * 
 */
void init_MegaQuad_Test()
{
	int triWidth = 1000;
	long triCount = triWidth * triWidth;
	long vertexCount = 2*(triCount+2);
	kuhl_geometry_new(&megaQuad, program, vertexCount, GL_TRIANGLE_STRIP);
	GLfloat *vertexPositions = malloc(3*vertexCount * sizeof(GLfloat));
	//direction to build
	//0 = left to right
	//1 = right to left
	int dir = 0;
	int verts = 0;
	//direction to move after placing vertex
	//0 = move up offset
	//1 = move down right offset
	//2 = move down left offset
	int mov = 0;
	float xp = 0, zp = 0, off = 1.0/triWidth;
	//printf("ready to start filling\n");
	for (long i = 0; i <= 3*vertexCount; i+=3) {
		if(i == 3*vertexCount){
			printf("%f, %f\n", xp, zp);
		}
		if (i == 0) {
			printf("%f, %f\n", xp, zp);
		}
		vertexPositions[i+0] = xp; //x
		vertexPositions[i+1] = 0;  //y
		vertexPositions[i+2] = -zp; //z
		verts++;
		if (mov == 0) {
			zp += off;
			//if end of row and did not move up last time, move up again and swap direction
			if (verts == triWidth + 1) { 
				dir = 1 - dir;
				mov = 0;
				verts=0;
			} else {
				mov = dir ? 2 : 1; //determine next direction normally
			}
		} 
		else if (mov == 1) {
			zp -= off;
			xp += off;
			mov = 0;
		} 
		else {
			zp -= off;
			xp -= off;
			mov = 0;
		}
	}
	kuhl_geometry_attrib(&megaQuad, vertexPositions, 3, "in_Position", KG_WARN);
	free(vertexPositions);
}

/**
 * Create a 1x1 quad comprised of many tris
 * 
 */
void init_MegaQuad()
{
	int vertexCount = (quadWidth+1)*(quadWidth+1);
	kuhl_geometry_new(&megaQuad, program, vertexCount, GL_TRIANGLE_STRIP);
	GLfloat *vertexPositions = malloc(3*vertexCount * sizeof(GLfloat));
	GLuint *indexData = malloc(quadWidth*quadWidth*6 * sizeof(GLfloat));
	//GLfloat vertexPositions[3*vertexCount];
	//GLuint indexData[quadWidth*quadWidth*6];
	int n1 = 0;
	int n2 = 0;
	float off = 1.0/quadWidth;
	for (int i = 0; i < quadWidth+1; i++) {
		for (int j = 0; j < quadWidth+1; j++) {
			vertexPositions[n1] = (float)j*off;
			vertexPositions[n1+1] = 0;
			vertexPositions[n1+2] = (float)-i*off;
			n1+=3;
		}
	}

	for (int i = 0; i < quadWidth; i++) {
		for (int j = 0; j < quadWidth; j++) {
			indexData[n2] = j+(quadWidth+1)*i;
			indexData[n2+1] = j+1+(quadWidth+1)*i;
			indexData[n2+2] = j+(quadWidth+1)*(i+1);

			indexData[n2+3] = j+1+(quadWidth+1)*i;
			indexData[n2+4] = j+(quadWidth+1)*(i+1);
			indexData[n2+5] = j+(quadWidth+1)*(i+1)+1;
			n2+=6;
		}
	}

	kuhl_geometry_attrib(&megaQuad, vertexPositions, 3, "in_Position", KG_WARN);
	kuhl_geometry_indices(&megaQuad, indexData, quadWidth*quadWidth*6);
	free(vertexPositions);
	free(indexData);

	GLuint texId = 0;
	kuhl_read_texture_file_wrap("../images/terrain/elev.png", &texId, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	kuhl_geometry_texture(&megaQuad, texId, "texE", KG_WARN);
	texId = 1;
	kuhl_read_texture_file_wrap("../images/terrain/topo.jpg", &texId, GL_REPEAT, GL_REPEAT);
	kuhl_geometry_texture(&megaQuad, texId, "texT", KG_WARN);
}

/**
 * Create a 1x1 quad comprised of 2 tris
 * 
 */
void init_Clouds(){
	kuhl_geometry_new(&clouds, cloudProgram, 4, GL_TRIANGLES);
	GLfloat vertexPositions[] = {
		0, cloudHeight, 0,
		1, cloudHeight, 0,
		1, cloudHeight, -1,
		0, cloudHeight, -1
	};
	kuhl_geometry_attrib(&clouds, vertexPositions, 3, "in_Position", KG_WARN);
	GLuint indexData[] = { 
		0, 1, 2,  
		0, 2, 3
	};
	kuhl_geometry_indices(&clouds, indexData, 6);
	GLuint texId = 2;
	kuhl_read_texture_file_wrap("../images/terrain/cloud.jpg", &texId, GL_REPEAT, GL_REPEAT);
	kuhl_geometry_texture(&clouds, texId, "texC", KG_WARN);
}

// Input
//

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (kuhl_keyboard_handler(window, key, scancode, action, mods))
		return;
	if(action == GLFW_PRESS || action == GLFW_REPEAT) {
		if(key == GLFW_KEY_N)
		{
			if (!showNorms) {
				showNorms = 1;
				glUseProgram(program);
				glUniform1i(kuhl_get_uniform("showNorm"), 1);
				glUseProgram(0);
				printf("Show normals enabled.\n");
			} else {
				showNorms = 0;
				glUseProgram(program);
				glUniform1i(kuhl_get_uniform("showNorm"), 0);
				glUseProgram(0);
				printf("Show normals disabled.\n");
			}
		}
		if (key == GLFW_KEY_SPACE) {
			freeCam = 1 - freeCam;
			if (freeCam) {
				printf("Free cam enabled.\n");
			} else {
				printf("Free cam disabled.\n");
			}
		}
	}
}

// Drawing
//

void display()
{
	/* Render the scene once for each viewport. Frequently one
	 * viewport will fill the entire screen. However, this loop will
	 * run twice for HMDs (once for the left eye and once for the
	 * right). */
	viewmat_begin_frame();
	for(int viewportID=0; viewportID<viewmat_num_viewports(); viewportID++)
	{
		//view port
		viewmat_begin_eye(viewportID);
		int viewport[4];
		viewmat_get_viewport(viewport, viewportID);
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
		glEnable(GL_SCISSOR_TEST);
		glClearColor(.008,.02,.078,0); // set clear color to grey
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST); // turn on depth testing
		glEnable(GL_BLEND); // enable use of the alpha value
		// tell OpenGL how to use the alpha value:
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		kuhl_errorcheck();

		// view & projection matrix
		float viewMat[16], perspective[16];
		viewmat_get(viewMat, perspective, viewportID);
		if (!freeCam) {
			float speed = 6;
			float time = glfwGetTime();
			float sinLoop = sin(time/speed);
			float cosLoop = cos(time/speed);
			float camX = 10 + sinLoop*4;
			float camY = 1.2 + cosLoop*0.5;
			float camZ = 0 - fabs(sinLoop*4);
			float lookX = 10;
			float lookY = 0;
			float lookZ = -5;
			mat4f_lookat_new(viewMat, camX, camY, camZ, lookX, lookY, lookZ, 0, 1, 0);
		}

		// model view
		float modelview[16];
		mat4f_mult_mat4f_many(modelview, viewMat, NULL);

		/* Tell OpenGL which GLSL program the subsequent
		 * glUniformMatrix4fv() calls are for. */
		kuhl_errorcheck();
		glUseProgram(program);
		kuhl_errorcheck();
		
		/* Send the perspective projection matrix to the vertex program. */
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value

		float scaleMat[16];
		mat4f_scale_new(scaleMat, 20, 1, 10);
		mat4f_mult_mat4f_many(modelview, viewMat, scaleMat, NULL);
		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
							1, // number of 4x4 float matrices
							0, // transpose
							modelview); // value

		//draw geometry
		//
		kuhl_geometry_draw(&megaQuad);

		kuhl_errorcheck();
		glUseProgram(cloudProgram);
		kuhl_errorcheck();
		
		/* Send the perspective projection matrix to the vertex program. */
		glUniformMatrix4fv(kuhl_get_uniform("Projection"),
		                   1, // number of 4x4 float matrices
		                   0, // transpose
		                   perspective); // value

		glUniformMatrix4fv(kuhl_get_uniform("ModelView"),
							1, // number of 4x4 float matrices
							0, // transpose
							modelview); // value

		//draw geometry
		//
		kuhl_geometry_draw(&clouds);

		glUseProgram(0); // stop using a GLSL program.
		viewmat_end_eye(viewportID);
	} // finish viewport loop
	viewmat_end_frame();

	/* Check for errors. If there are errors, consider adding more
	 * calls to kuhl_errorcheck() in your code. */
	kuhl_errorcheck();

}

// Main
//

int main(int argc, char** argv)
{
	//init
	kuhl_ogl_init(&argc, argv, 512, 512, 32, 4);
	glfwSetKeyCallback(kuhl_get_window(), keyboard);
	program = kuhl_create_program("terrain.vert", "terrain.frag");
	cloudProgram = kuhl_create_program("terrain-clouds.vert", "terrain-clouds.frag");
	glUseProgram(program);
	glUniform1i(kuhl_get_uniform("showNorm"), 0);
	glUseProgram(0);

	dgr_init();     /* Initialize DGR based on config file. */

	//values here are ignored after the viewmat is replaced in display
	static float initCamPos[3]  = {10,2,0.5}; // location of camera
	static float initCamLook[3] = {10,0,-3}; // a point the camera is facing at
	static float initCamUp[3]   = {0,1,0}; // a vector indicating which direction is up
	viewmat_init(initCamPos, initCamLook, initCamUp);

	//create objects
	init_MegaQuad();
	init_Clouds();

	//main loop
	while(!glfwWindowShouldClose(kuhl_get_window()))
	{
		display();
		kuhl_errorcheck();

		/* process events (keyboard, mouse, etc) */
		glfwPollEvents();
	}
	exit(EXIT_SUCCESS);
}
