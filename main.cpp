//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <GL/glut.h>

#include <math.h>
#include <vector>
#include "camera.hh"
#include "windows.h"
#include "Wave.h"

using namespace std;

bool LightEnabled = true;
bool time = true;
bool frame = false;
bool derive_normals = false;

static GLfloat MatSpec[] = { .90, .90, 1.0, 1.0 };
static GLfloat MatShininess[] = { 50.0 };
static GLfloat LightPos[] = { 50.0, -50.0, 50.0, 0.0 };
static GLfloat blue[] = { .53f, .84f, 1.f, .9f };

float t = 0, dt = 0.01;
static int scale = 5000;
const int size_x = 50;
const int size_y = 50;
float FPS;

GLfloat vertices[(size_x)* (size_y) * 3];
GLfloat normals[(size_x)* (size_y) * 3];
int indices[(size_x - 1)* (size_y - 1) * 6];
vector<Wave> waves = vector<Wave>();
Camera camera;

float  multiply_x(float v1[], float v2[]) {
	GLfloat x = v1[1] * v2[2] - v1[2] * v2[1];
	return x;
}

float  multiply_y(float v1[], float v2[]) {
	GLfloat y = v1[2] * v2[0] - v1[0] * v2[2];
	return y;
}

float  multiply_z(float v1[], float v2[]) {
	GLfloat z = v1[0] * v2[1] - v1[1] * v2[0];
	return z;
}

pair<vector<GLfloat>, vector<GLfloat>> generate_vertices(int size_x, int size_y, float time, vector<Wave> waves) {
	vector<GLfloat>  vertices = vector<GLfloat>();
	vector<GLfloat>  normals = vector<GLfloat>();

	for (int i = 0; i < size_x; i++)
		for (int j = 0; j < size_y; j++) {

			GLfloat h = 0;
			GLfloat n_x = 0;
			GLfloat n_y = 0;

			for (int k = 0; k < waves.size(); k++) {

				if (derive_normals) {
					n_x += waves[k].d_dx(j*scale, i*scale, time);
					n_y += waves[k].d_dy(j*scale, i*scale, time);
				}
				h += waves[k].f(j*scale, i*scale, time);
			}

			vertices.emplace_back((GLfloat)j);
			vertices.emplace_back(h);
			vertices.emplace_back((GLfloat)i);

			if (LightEnabled) {

				if (derive_normals) {
					float length = sqrt(n_x*n_x + n_y*n_y + 1.);
					normals.emplace_back(-(GLfloat)(n_x / length));
					normals.emplace_back(-(GLfloat)(n_y / length));
					normals.emplace_back((GLfloat)(1. / length));
				}
				else {
					int size = 3 * (j + 1 + size_y*i);

					if (i > 1 && j > 1) {
						float a[3] = { 0, 0, 0 };
						float b[3] = { 0, 0, 0 };
						float c[3] = { 0, 0, 0 };
						float d[3] = { 0, 0, 0 };
						float e[3] = { 0, 0, 0 };
						float f[3] = { 0, 0, 0 };
						float temp1, temp2, center_x, center_y, center_z;
						temp1 = size - 3 * (size_y + 2);
						temp2 = size - 3 * (size_y + 1);

						center_x = vertices[temp2 - 3];
						center_y = vertices[temp2 - 2];
						center_z = vertices[temp2 - 1];

						a[0] = vertices[temp1 - 3] - center_x;
						a[1] = vertices[temp1 - 2] - center_y;
						a[2] = vertices[temp1 - 1] - center_z;

						temp1 = size - 3 * (size_y * 2 + 2);
						b[0] = vertices[temp1 - 3] - center_x;
						b[1] = vertices[temp1 - 2] - center_y;
						b[2] = vertices[temp1 - 1] - center_z;

						temp1 = size - 3 * (size_y * 2 + 1);
						c[0] = vertices[temp1 - 3] - center_x;
						c[1] = vertices[temp1 - 2] - center_y;
						c[2] = vertices[temp1 - 1] - center_z;

						temp1 = size - 3 * (size_y);
						d[0] = vertices[temp1 - 3] - center_x;
						d[1] = vertices[temp1 - 2] - center_y;
						d[2] = vertices[temp1 - 1] - center_z;

						temp1 = size;
						e[0] = vertices[temp1 - 3] - center_x;
						e[1] = vertices[temp1 - 2] - center_y;
						e[2] = vertices[temp1 - 1] - center_z;

						temp1 = size - 3;
						f[0] = vertices[temp1 - 3] - center_x;
						f[1] = vertices[temp1 - 2] - center_y;
						f[2] = vertices[temp1 - 1] - center_z;

						float a_b[] = { multiply_x(a, b), multiply_y(a, b), multiply_z(a, b) };
						float b_c[] = { multiply_x(b, c), multiply_y(b, c), multiply_z(b, c) };
						float  c_d[] = { multiply_x(c, d), multiply_y(c, d), multiply_z(c, d) };
						float  d_e[] = { multiply_x(d, e), multiply_y(d, e), multiply_z(d, e) };
						float  e_f[] = { multiply_x(e, f), multiply_y(e, f), multiply_z(e, f) };
						float  f_a[] = { multiply_x(f, a), multiply_y(f, a), multiply_z(f, a) };

						GLfloat v[] = { 0, 0, 0 };

						v[0] = a_b[0] + b_c[0] + c_d[0] + d_e[0] + e_f[0] + f_a[0];
						v[1] = a_b[1] + b_c[1] + c_d[1] + d_e[1] + e_f[1] + f_a[1];
						v[2] = a_b[2] + b_c[2] + c_d[2] + d_e[2] + e_f[2] + f_a[2];

						GLfloat length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

						normals.emplace_back(v[0] / length);
						normals.emplace_back(v[1] / length);
						normals.emplace_back(v[2] / length);
					}
					else if (i > 0 && j > 0) {
						float temp1, temp2, center_x, center_y, center_z;

						float a[3] = { 0, 0, 0 };
						float b[3] = { 0, 0, 0 };

						temp1 = size - 3;
						temp2 = size;

						center_x = vertices[temp2 - 3];
						center_y = vertices[temp2 - 2];
						center_z = vertices[temp2 - 1];

						a[0] = vertices[temp1 - 3] - center_x;
						a[1] = vertices[temp1 - 2] - center_y;
						a[2] = vertices[temp1 - 1] - center_z;

						temp1 = size - 3 * (size_y + 1);
						b[0] = vertices[temp1 - 3] - center_x;
						b[1] = vertices[temp1 - 2] - center_y;
						b[2] = vertices[temp1 - 1] - center_z;

						float a_b[] = { multiply_x(a, b), multiply_y(a, b), multiply_z(a, b) };

						GLfloat length = sqrt(a_b[0] * a_b[0] + a_b[1] * a_b[1] + a_b[2] * a_b[2]);

						normals.emplace_back(a_b[0] / length);
						normals.emplace_back(a_b[1] / length);
						normals.emplace_back(a_b[2] / length);
					}
					else {
						normals.emplace_back(0);
						normals.emplace_back(0);
						normals.emplace_back(0);
					}
				}
			}
		}
	return pair<vector<GLfloat>, vector<GLfloat>>(vertices, normals);
}

vector<GLfloat> generate_indices(int size_x, int size_y) {
	vector<GLfloat> indices = vector<GLfloat>();

	for (int j = 0; j < size_y*size_x - size_x; j += size_x)
		for (int i = 0; i < size_x - 1; i++) {
			indices.emplace_back(i + j + size_y);
			indices.emplace_back(i + j + 1);
			indices.emplace_back(i + j);
			indices.emplace_back(i + j + size_y);
			indices.emplace_back(i + j + 1 + size_y);
			indices.emplace_back(i + j + 1);
		}
	return indices;
}

void CalculateFrameRate() {
	static float framesPerSecond = 0.0f;    //фпс
	static float lastTime = 0.0f;           //Тут хранится время, прошедшее с последнего кадра

											//Тут мы получаем текущий tick count и умножаем его на 0.001 для конвертации из миллисекунд в секунды.
	float currentTime = timeGetTime() * 0.001f;

	//Увеличиваем счетчик кадров
	++framesPerSecond;

	//cекунда прошла
	if (currentTime - lastTime > 1.0f) {
		lastTime = currentTime;
		FPS = framesPerSecond;
		framesPerSecond = 0;
	}
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, (double)w / (double)h, 1.0, 500.);
	glDepthRange(-50, 100);
	gluLookAt(20, 40, -50, 50, 00, 50, 0, 50, 20);
//	camera.look_at();
	//camera.set_perspective(w, h);
	glMatrixMode(GL_MODELVIEW);
/*	camera.look_at();
	camera.set_perspective(w, h);*/
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();


	pair<vector<GLfloat>, vector<GLfloat>> vert_normals = generate_vertices(size_x, size_y, t, waves);
	vector<GLfloat> vertices_v = vert_normals.first;
	vector<GLfloat> normals_v = vert_normals.second;
	vector<GLfloat> indices_v = generate_indices(size_x, size_y);

	std::copy(vertices_v.begin(), vertices_v.end(), vertices);
	std::copy(normals_v.begin(), normals_v.end(), normals);
	std::copy(indices_v.begin(), indices_v.end(), indices);

	// activate and specify pointer to vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glNormalPointer(GL_FLOAT, 0, normals);

	if (frame)
		glDrawElements(GL_POINTS, indices_v.size(), GL_UNSIGNED_INT, indices);
	else
		glDrawElements(GL_TRIANGLES, indices_v.size(), GL_UNSIGNED_INT, indices);

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	//Finish rendering
	glFlush();
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {

	switch (key)
	{
	case 27:	//ESC
		exit(0);
		break;
	case 97:		//a
		camera.translate(0,5.0,0);
		break;
	case 100:		//d
		camera.move_to(0, 0.0, 0);
	//	Camera.RotateY(-5.0);
		break;
	case 119:		//w
		camera.move_backward(10);
		break;
	case 115:		//s
		camera.move_forward(10);
		break;
	case 120:		//x
	//	Camera.RotateX(10.0);
		break;
	case 121:		//y
	//	Camera.RotateX(-10.0);
		break;
	case 99:		//c
	//	Camera.StrafeRight(-5);
		break;
	case 118:		//v
	//	Camera.StrafeRight(5);
		break;
	case 'k':
		frame = !frame;
		break;
	case 'q':
		derive_normals = !derive_normals;
		break;
	case 'l':
		LightEnabled = !LightEnabled;
		if (LightEnabled) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
		break;
	case 't':
		time = !time;
		break;
	case '+':
		dt *= 10;
		break;
	case '-':
		dt /= 10;
		break;
	}
}

void idle(void) {
	if (time)
		t += dt;
	display();
	glDisable(GL_DEPTH_TEST);

	CalculateFrameRate();
	cout << FPS << endl;

	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char * argv[]) {
	waves.emplace_back(Wave(0.4, 6, 4));
	waves.emplace_back(Wave(0.9, 10, 5));
	waves.emplace_back(Wave(3, 3, 7));
//	waves.emplace_back(Wave(0.7, 5, 5));

	waves[0].set_direction_vector(01., 1., 0, 0);
	waves[1].set_direction_vector(0.1, 0.3, 0, 0);

	waves[2].set_direction_vector(0.2, 0.1, 0, 0);
//	waves[3].set_direction_vector(0.3, 0.1, 0, 0);

	glutInit(&argc, argv);
	camera = Camera();

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Waves");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	

	
	//Lighting stuff:
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	CalculateFrameRate();
	cout << FPS << endl;

	glEnable(GL_DEPTH_TEST);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();

	return 0;
}
