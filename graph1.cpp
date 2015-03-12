//--------------------------------------------------------
// Default libraries
//--------------------------------------------------------
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#if defined(__APPLE__)                                                                                                                                                                                                            
#include <OpenGL/gl.h>                                                                                                                                                                                                            
#include <OpenGL/glu.h>                                                                                                                                                                                                           
#include <GLUT/glut.h>                                                                                                                                                                                                            
#else                                                                                                                                                                                                                             

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)                                                                                                                                                                       
#include <windows.h>                                                                                                                                                                                                              
#endif                                                                                                                                                                                                                            
#include <GL/gl.h>                                                                                                                                                                                                                
#include <GL/glu.h>                                                                                                                                                                                                               
#include <GL/glut.h>                                                                                                                                                                                                              
#endif          


//--------------------------------------------------------
// Global contants
//--------------------------------------------------------
#define WORLD_WIDTH 58			// Relative width of world
#define WORLD_HEIGHT 68			// Relative height of world
#define SCREEN_WIDTH 600		// Window screen width
#define SCREEN_HEIGHT 600		// Window screen height

#define VERTICES_COUNT 10		// max verticles count

#define RESOLUTION 700			// Catmull-Rom spline resolution
#define TENSION 0.0f			// Catmull-Rom tension parameter

#define CC_STACK_SIZE 200		// Catmull-Clark vertex array size
#define CC_RESAMPLE 4			// Catmull-Clark resolution

#define BEZIER_STEP 0.01f		// Bezier curve resolution

#define PI 3.1415926535f		// define PI constant value
#define POINT_RADIUS 2			// Vertex point radius
#define ANIMATE_RADIUS 2.5f		// Animation circle radius

#define TICK 100.f				// Animation resolution
#define ROTATE_TIME 5.f			// Rotate resolution


//--------------------------------------------------------
// Main vector type (only 2 dimension)
//--------------------------------------------------------
struct Vector {
	float x, y;
	Vector() {
		x = y = 0;
	}
	Vector(float x0, float y0) {
		x = x0; y = y0;
	}
	Vector operator*(float a) const {
		return Vector(x * a, y * a);
	}
	Vector operator/(float a) const {
		return Vector(x / a, y / a);
	}
	Vector operator+(const Vector& v) const {
		return Vector(x + v.x, y + v.y);
	}
	Vector operator-(const Vector& v) const {
		return Vector(x - v.x, y - v.y);
	}
	Vector operator-() const {
		return Vector(-x, -y);
	}
	bool operator==(const Vector& v) const {
		return x == v.x && y == v.y;
	}
	bool operator!=(const Vector& v) const {
		return x != v.x || y != v.y;
	}
	float operator*(const Vector& v) const {
		return (x * v.x + y * v.y);
	}
	float Length() { return sqrt(x * x + y * y); }
};


//--------------------------------------------------------
// Spektrum illetve szin
//--------------------------------------------------------
struct Color {
	float r, g, b;

	Color() {
		r = g = b = 0;
	}
	Color(float r0, float g0, float b0) {
		r = r0; g = g0; b = b0;
	}
	Color operator*(float a) {
		return Color(r * a, g * a, b * a);
	}
	Color operator*(const Color& c) {
		return Color(r * c.r, g * c.g, b * c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
};


//--------------------------------------------------------
// Few global variables for animating
//--------------------------------------------------------
long actualTime = 0;
Vector chosen;
bool moveCamera = false;
bool animStarted = false;


//--------------------------------------------------------
// Catmull-Rom Spline
//--------------------------------------------------------
class CatmullRomSpline {
private:
	Color color;
	int count;
	Vector vertices[VERTICES_COUNT + 2];
	float times[VERTICES_COUNT + 2];

	Vector getVelocityAt(int index) {
		const Vector& vx_this = vertices[index];
		const float& t_this = times[index];

		const Vector prevVelocity = (index > 0) ? (vx_this - vertices[index - 1]) / (t_this - times[index - 1]) : Vector();
		const Vector nextVelocity = (index < count - 1) ? (vertices[index + 1] - vx_this) / (times[index + 1] - t_this) : Vector();

		return (prevVelocity + nextVelocity) * ((1.0f - TENSION) / 2.0f);
	}

	Vector getSpline(float t) {
		int index = indexOf(t);

		const float& t_this = times[index];
		const float& t_next = times[index + 1];

		const Vector& vx_this = vertices[index];
		const Vector& vx_next = vertices[index + 1];
		const Vector  vl_this = getVelocityAt(index);
		const Vector  vl_next = getVelocityAt(index + 1);

		const float tau = t_next - t_this;
		const float tau2 = pow(tau, 2);
		const float tau3 = pow(tau, 3);

		const Vector dist = vx_next - vx_this;

		const Vector& a = (dist * -2.0f) / tau3 + (vl_next + vl_this) / tau2;
		const Vector& b = (dist * 3.0f) / tau2 - (vl_next + vl_this * 2.0f) / tau;
		const Vector& c = vl_this;
		const Vector& d = vx_this;

		const float dt = t - t_this;
		const float dt2 = pow(dt, 2);
		const float dt3 = pow(dt, 3);

		return a * dt3 + b * dt2 + c * dt + d;
	}

	int indexOf(float t) const {
		for (int i = 0; i < count; i++) {
			if (times[i] > t) return i - 1;
		}
		return 0;
	}

public:
	CatmullRomSpline() {
		color = Color(0.097f, 0.58f, 0.187f);
	}

	void addVertex(Vector v, float t) {
		if (count < VERTICES_COUNT) {
			if (!count) {
				vertices[0] = v;
				times[0] = t - 1;
			}

			vertices[count] = vertices[count + 1] = v;
			times[count] = t;
			times[count + 1] = times[count] + 1;
			count++;
		}
	}

	void addVertices(Vector* vArr, float* tArr, int newCount) {
		for (int i = 0; i < newCount; i++) {
			if (!i) {
				vertices[0] = vArr[i];
				times[0] = tArr[i] - 1;
			}

			vertices[i] = vertices[i + 1] = vArr[i];
			times[i] = tArr[i];
			times[i + 1] = times[i] + 1;
		}
		count = newCount;
	}

	void drawSpline() {
		const float& t_start = times[0];
		const float& t_end = times[count];
		const float elapsedT = t_end - t_start;

		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < RESOLUTION; i++){
			float t = elapsedT * ((float)i / RESOLUTION) + t_start;
			Vector r = getSpline(t);
			glVertex2f(r.x, r.y);
		}
		glEnd();
	}

} cmrSpline;


//--------------------------------------------------------
// Catmull-Clark Spline
//--------------------------------------------------------
class CatmullClarkSpline {
private:
	Color color;
	int count;
	int drawableCount;
	Vector vertices[VERTICES_COUNT];
	Vector temp[CC_STACK_SIZE];
	Vector drawable[CC_STACK_SIZE];

	Vector getAverge(Vector& a, Vector& b) {
		return Vector((a.x + b.x) / 2, (a.y + b.y) / 2);
	}

	void generateSplinePoints() {
		for (int i = 0; i < count; i++) {
			temp[i] = vertices[i];
		}
		drawableCount = count;

		for (int k = 0; k < CC_RESAMPLE; k++) {
			drawableCount = 2 * drawableCount - 1;
			drawable[0] = temp[0];
			drawable[drawableCount - 1] = temp[drawableCount / 2];

			for (int i = 1; i < drawableCount; i += 2) {
				drawable[i] = getAverge(temp[i / 2], temp[i / 2 + 1]);
			}

			for (int i = 2; i < drawableCount - 1; i += 2) {
				drawable[i] = temp[i / 2] * 0.5f + (drawable[i - 1] + drawable[i + 1]) * 0.25f;
			}

			if (k < CC_RESAMPLE - 1) {
				for (int i = 0; i < drawableCount; i++) {
					temp[i] = drawable[i];
				}
			}
		}
	}

public:
	CatmullClarkSpline() {
		color = Color(0.1f, 0.1f, 1.f);
	}

	void addVertex(Vector v) {
		if (count < VERTICES_COUNT) {
			vertices[count++] = v;
		}
	}

	void addVertices(Vector* arr, int newCount) {
		for (int i = 0; i < newCount; i++) {
			vertices[i] = arr[i];
		}
		count = newCount;
	}

	void drawSpline() {
		generateSplinePoints();
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < drawableCount; i++){
			glVertex2f(drawable[i].x, drawable[i].y);
		}
		glEnd();
	}

} ccSpline;


//--------------------------------------------------------
// Bezier Curve
//--------------------------------------------------------
class BezierCurve {
private:
	Color color;
	int count;
	Vector vertices[VERTICES_COUNT];

	float binomialTheorem(int index, float t) {
		float selection = 1;
		const int n = count - 1;

		for (int i = 1; i <= index; i++) {
			selection *= (float)(n - i + 1) / i;
		}
		return selection * pow(t, index) * pow(1 - t, n - index);
	}

	Vector r(float t) {
		Vector temp(0, 0);
		for (int i = 0; i < count; i++) {
			const float& b = binomialTheorem(i, t);
			temp = temp + vertices[i] * b;
		}
		return temp;
	}

public:
	BezierCurve() {
		color = Color(1.f, 0.f, 0.f);
	}

	void addVertex(Vector v) {
		if (count < VERTICES_COUNT) {
			vertices[count++] = v;
		}
	}

	void addVertices(Vector* arr, int newCount) {
		for (int i = 0; i < newCount; i++) {
			vertices[i] = arr[i];
		}
		count = newCount;
	}

	void drawCurve() {
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_STRIP);
		for (float t = 0.0; t <= 1.0; t += BEZIER_STEP) {
			glVertex2f(r(t).x, r(t).y);
		}
		glEnd();
	}

} bezierCurve;


//--------------------------------------------------------
// Convex hull
//--------------------------------------------------------
class ConvexHull {
private:
	Color color;
	int count;
	int cornerCount;
	Vector vertices[VERTICES_COUNT];
	Vector corners[VERTICES_COUNT];
	Vector firstCorner;
	enum Orientation { LEFT, INLINE, RIGHT };

	int getLowestIndex() {
		int lowest = 0;

		for (int i = 1; i < count; i++) {
			if (vertices[i].y < vertices[lowest].y) {
				lowest = i;
			}
			else {
				if (vertices[i].y == vertices[lowest].y && vertices[i].x < vertices[lowest].x) {
					lowest = i;
				}
			}
		}
		return lowest;
	}

	void swapArrayElements(Vector* arr, int from, int to) {
		Vector temp = arr[from];
		arr[from] = arr[to];
		arr[to] = temp;
	}

	Orientation getOrientation(Vector a, Vector b, Vector c) {
		float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
		if (area > 0.f)
			return LEFT;
		else if (area < 0.f)
			return RIGHT;
		return INLINE;
	}

	int sqrDist(Vector a, Vector b)  {
		int dx = a.x - b.x, dy = a.y - b.y;
		return dx * dx + dy * dy;
	}

	bool needToSwapByOrientation(Vector a, Vector b)  {
		int order = getOrientation(firstCorner, a, b);
		if (order == INLINE)
			return sqrDist(firstCorner, a) > sqrDist(firstCorner, b);
		return order != LEFT;
	}

	void sortVertices() {
		for (int i = 1; i < count - 1; i++) {
			for (int j = 1; j < count - i; j++) {
				if (needToSwapByOrientation(vertices[j], vertices[j + 1])) {
					swapArrayElements(vertices, j, j + 1);
				}
			}
		}

		for (int i = 0; i < 3; i++) {
			corners[i] = vertices[i];
		}
		cornerCount = 3;
	}

	void searchHullCorners() {
		int lowestIndex = getLowestIndex();
		swapArrayElements(vertices, 0, lowestIndex);
		firstCorner = vertices[0];

		sortVertices();

		for (int i = 3; i < count; i++) {
			Vector last = corners[cornerCount - 1];
			cornerCount--;

			while (getOrientation(corners[cornerCount - 1], last, vertices[i]) != LEFT)   {
				last = corners[cornerCount - 1];
				cornerCount--;
			}

			corners[cornerCount++] = last;
			corners[cornerCount++] = vertices[i];
		}
	}

public:
	ConvexHull() {
		color = Color(0.251f, 0.882f, 0.815f);
	}

	void addVertex(Vector v) {
		if (count < VERTICES_COUNT) {
			vertices[count++] = v;
		}
	}

	void addVertices(Vector* arr, int newCount) {
		for (int i = 0; i < newCount; i++) {
			vertices[i] = arr[i];
		}
		count = newCount;
	}

	void drawHull() {
		if (count > 2) {
			searchHullCorners();

			glColor3f(color.r, color.g, color.b);
			glBegin(GL_POLYGON);
			for (int i = 0; i < cornerCount; i++) {
				glVertex2f(corners[i].x, corners[i].y);
			}
			glEnd();
		}
	}

} convexHull;


//--------------------------------------------------------
// Store vertices for easy access
//--------------------------------------------------------
class Vertices {
private:
	Color color;
	int count;
	int selectedIndex;
	Vector position[VERTICES_COUNT];
	Vector origins[VERTICES_COUNT];
	float values[VERTICES_COUNT];
	float evenDegree;
	float oddDegree;

	float radian(float degree) {
		return (degree / 180.0f) * PI;
	}

	Vector getCirclePoint(int index, int degree) {
		Vector temp;
		const float rad = radian(degree);

		temp.x = position[index].x + cos(rad) * POINT_RADIUS;
		temp.y = position[index].y + sin(rad) * POINT_RADIUS;

		return temp;
	}

	Vector getAnimatedCircleOrigin(int index) {
		Vector temp;
		if (index % 2 == 0) {
			float rad = radian(evenDegree);
			temp.x = cos(rad) * ANIMATE_RADIUS;
			temp.y = sin(rad) * ANIMATE_RADIUS;
		}
		else {
			float rad = radian(oddDegree);
			temp.x = cos(rad) * ANIMATE_RADIUS;
			temp.y = sin(rad) * ANIMATE_RADIUS;
		}
		return temp;
	}

	void repositionAll() {
		bezierCurve.addVertices(position, count);
		cmrSpline.addVertices(position, values, count);
		ccSpline.addVertices(position, count);
		convexHull.addVertices(position, count);

		if (moveCamera) chosen = position[selectedIndex];
	}

public:
	Vertices() {
		color = Color(0.f, 0.f, 0.f);
		evenDegree = oddDegree = 90.f;
	}

	void addVertex(Vector v, long t) {
		if (count < VERTICES_COUNT) {
			position[count] = v;
			values[count] = t;

			Vector animPos = getAnimatedCircleOrigin(count);
			origins[count++] = position[count] - animPos;
		}
	}

	int getIndexOfClickedPoint(Vector& mousePos) {

		for (int i = 0; i < count; i++) {
			Vector distVector = mousePos - position[i];
			float dist = sqrt(pow(distVector.x, 2) + pow(distVector.y, 2));

			if (dist < POINT_RADIUS && dist < POINT_RADIUS) {
				return i;
			}
		}
		return -1;
	}

	void animateVertices(float t) {
		for (int i = 0; i < count; i++) {
			if (i % 2 == 0) {
				const float rad = radian(evenDegree);

				position[i].x = origins[i].x + cos(rad) * ANIMATE_RADIUS;
				position[i].y = origins[i].y + sin(rad) * ANIMATE_RADIUS;
			} else {
				const float rad = radian(oddDegree);

				position[i].x = origins[i].x + cos(rad) * ANIMATE_RADIUS;
				position[i].y = origins[i].y + sin(rad) * ANIMATE_RADIUS;
			}
		}
		repositionAll();

		evenDegree -= (t / 1000.f) * (360.f / ROTATE_TIME);
		oddDegree += (t / 1000.f) * (360.f / ROTATE_TIME);
	}

	void moveCameraTo(int index) {
		selectedIndex = index;
		moveCamera = true;
	}

	void drawCircles() {
		glColor3f(color.r, color.g, color.b);
		for (int i = 0; i < count; i++) {
			glBegin(GL_POLYGON);
			for (int deg = 0; deg < 360; deg += 20) {
				const Vector& cp = getCirclePoint(i, deg);
				glVertex2f(cp.x, cp.y);
			}
			glEnd();
		}
	}
} vertices;

// Save vertex (relative click position) and the associated click time
void addVertex(const Vector& v, long time) {
	vertices.addVertex(v, time);
	cmrSpline.addVertex(v, time);
	ccSpline.addVertex(v);
	bezierCurve.addVertex(v);
	convexHull.addVertex(v);
}

// Calculate relative position from window
Vector getWorldPosFromWindow(int x, int y) {
	Vector temp;
	const float& width = (float)(600 / 2);
	const float& height = (float)(600 / 2);

	temp.x = ((x - width) / width) * ((WORLD_WIDTH / 2));
	temp.y = (-1 * (y - height) / height) * ((WORLD_HEIGHT / 2));

	return temp + chosen;
}

// Animation event
void onAnimation(long tStart, long tEnd) {
	for (long t = tStart; t < tEnd; t += TICK) {
		float remained = (float) tEnd - t;
		float step = (remained >= TICK) ? TICK : remained;
		vertices.animateVertices(step);
	}
}

// Initialization viewport, after create OpenGL context
void onInitialization() {
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

// Drawing, if the application window becomes invalid
void onDisplay() {
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);		// torlesi szin beallitasa
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles


	const double& qtWidth = WORLD_WIDTH / 2;
	const double& qtHeight = WORLD_HEIGHT / 2;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	Vector bottomLeft(chosen.x - qtWidth, chosen.y - qtHeight);
	Vector topRight(chosen.x + qtWidth, chosen.y + qtHeight);

	gluOrtho2D(bottomLeft.x, topRight.x, bottomLeft.y, topRight.y);

	convexHull.drawHull();
	cmrSpline.drawSpline();
	ccSpline.drawSpline();
	bezierCurve.drawCurve();
	vertices.drawCircles();

	glutSwapBuffers();     				// Buffercsere: rajzolas vege
}

// OnKeyDown event
void onKeyboard(unsigned char key, int x, int y) {
	if (key == 'd') glutPostRedisplay(); 		// d beture rajzold ujra a kepet

	if (key == ' ') {
		animStarted = true;
	}
}

// onKeyUp event
void onKeyboardUp(unsigned char key, int x, int y) {}

// Mouse click event only
void onClick(int button, int state, int x, int y, long time = 0.f) {
	Vector mousePos(getWorldPosFromWindow(x, y));
	if (button == GLUT_LEFT_BUTTON) {
		addVertex(mousePos, time);
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		int clickedIndex = vertices.getIndexOfClickedPoint(mousePos);
		if (clickedIndex >= 0) {
			vertices.moveCameraTo(clickedIndex);
		}
	}
	glutPostRedisplay();
}

// All mouse events
void onMouse(int button, int state, int x, int y) {
	long time = glutGet(GLUT_ELAPSED_TIME);

	if (state == GLUT_DOWN) {
		onClick(button, state, x, y, time);
	}
}

// Mouse motion event
void onMouseMotion(int x, int y) { }

// `Idle' esemenykezelo, jelzi, hogy az ido telik, az Idle esemenyek frekvenciajara csak a 0 a garantalt minimalis ertek
void onIdle() {
	long oldTime = actualTime;
	actualTime = glutGet(GLUT_ELAPSED_TIME);

	if (animStarted) {
		onAnimation(oldTime, actualTime);
		glutPostRedisplay();
	}
}

// Main program (please do not modify)
int main(int argc, char **argv) {
	glutInit(&argc, argv); 						// GLUT initialize
	glutInitWindowSize(600, 600);				// Application window size
	glutInitWindowPosition(100, 100);			// Window position
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);	// 8 bits for R,G,B,A + duble buffering + depth buffer

	glutCreateWindow("Grafika hazi feladat");	// Create application window on screen

	glMatrixMode(GL_MODELVIEW);					// Set matrix mode to modelview (unit matrix)
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);				// set matrix mode to projection (unit matrix)
	glLoadIdentity();

	onInitialization();							// Runs my initialization function

	glutDisplayFunc(onDisplay);					// Registering event handlers
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();								// Event handler loop

	return 0;
}

