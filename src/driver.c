#include <math.h>  // pow, M_PI
#include <stdlib.h>  // rand, srand, RAND_MAX
#include <stdio.h>  // sprintf
#include <time.h>  // time
#include <emscripten.h>  // EM_ASM
#include <emscripten/html5.h>  // emscripten_set_main_loop, emscripten_console_log
#include "canvas.h"  // HTMLCanvasElement, CanvasRenderingContext2D,
                     // createCanvas, freeCanvas
#include "window.h"  // Window, freeWindow

#define PARTICLE_COUNT 115
#define PARTICLE_SIZE 3
#define THRESHOLD 250.0
#define SPEED_MULTIPLIER 2.5

struct Particle {
	double x;
	double y;
	double vx;
	double vy;
};


HTMLCanvasElement *canvas;
CanvasRenderingContext2D *context;
struct Particle particles[PARTICLE_COUNT];


double min(double a, double b) {
	return a < b ? a : b;
}


double rand_01() {
	return (double)rand() / (double)(RAND_MAX);
}


double random_x() {
	return canvas->getWidth(canvas) * rand_01();
}


double random_y() {
	return canvas->getHeight(canvas) * rand_01();
}


// Generate a value with a magnitude between [0.001953125, 0.0625],
// negative half the time, and with a bias toward 0.
// https://www.desmos.com/calculator/7uspuyiuu5
double random_speed() {
	return pow(0.5, (5 * rand_01()) + 4) * (rand() % 2 ? 1 : -1) * SPEED_MULTIPLIER;
}


double distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}


void line_between(double x1, double y1, double x2, double y2) {
	double dist = distance(x1, y1, x2, y2);
	if (dist < THRESHOLD) {
		// Change the thickness and opacity of the line connecting
		// two particles based on their distance from each other.
		// The closer they are, the thicker and more opaque the line.
		double opacity = (THRESHOLD / dist) - 1;
		char color[20 + 8 + 1 + 1];
		sprintf(color, "rgba(229, 227, 223, %f)", opacity);
		context->setLineWidth(context, min(opacity, PARTICLE_SIZE));
		context->setStrokeStyle(context, color);
		context->beginPath(context);
		context->moveTo(context, x1, y1);
		context->lineTo(context, x2, y2);
		context->stroke(context);
	}
}


void draw_particle(double x, double y) {
	context->beginPath(context);
	context->arc(context, x, y, PARTICLE_SIZE, 0, 2 * M_PI);
	context->fill(context);
}


void animate() {
	int canvas_width = Window()->getInnerWidth();
	int canvas_height = Window()->getInnerHeight();
	canvas->setWidth(canvas, canvas_width);
	canvas->setHeight(canvas, canvas_height);

	// I don't know why I have to re-set the fill style every frame, but it
	// goes to #000000 otherwise.
	context->setFillStyle(context, "#e5e3df");

	// Draw the particles and the lines between them.
	for (int i = 0; i < PARTICLE_COUNT; ++i) {
		draw_particle(particles[i].x, particles[i].y);

		for (int j = i + 1; j < PARTICLE_COUNT; ++j) {
			line_between(particles[i].x, particles[i].y, particles[j].x, particles[j].y);
		}

		particles[i].x = particles[i].x + particles[i].vx;
		particles[i].y = particles[i].y + particles[i].vy;

		// Bounce off the edges of the screen.
		// Snap to the edge of the screen, too, so it doesn't disappear in case
		// the browser is resized.
		if (particles[i].x < PARTICLE_SIZE) {  // Left edge
			particles[i].vx *= -1;
			particles[i].x = PARTICLE_SIZE;
		}
		else if (particles[i].x > canvas_width - PARTICLE_SIZE) {  // Right edge
			particles[i].vx *= -1;
			particles[i].x = canvas_width - PARTICLE_SIZE;
		}
		if (particles[i].y < PARTICLE_SIZE) {  // Top edge
			particles[i].vy *= -1;
			particles[i].y = PARTICLE_SIZE;
		}
		else if (particles[i].y > canvas_height - PARTICLE_SIZE) {  // Bottom edge
			particles[i].vy *= -1;
			particles[i].y = canvas_height - PARTICLE_SIZE;
		}
	}
}


int main() {
	srand(time(NULL));

	emscripten_console_log("Initializing canvas...");
	canvas = createCanvas("root");
	context = canvas->getContext(canvas, "2d");
	canvas->setWidth(canvas, Window()->getInnerWidth());
	canvas->setHeight(canvas, Window()->getInnerHeight());
	emscripten_console_log("Initialized canvas.");

	// Populate particle array with random values.
	emscripten_console_log("Generating particles...");
	for (int i = 0; i < PARTICLE_COUNT; i++) {
		particles[i].x = random_x();
		particles[i].y = random_y();
		particles[i].vx = random_speed();
		particles[i].vy = random_speed();
	}
	emscripten_console_log("Generated particles.");

	emscripten_console_log("Starting simulation.");
	emscripten_set_main_loop(&animate, 0, 1);
	return 0;
}
