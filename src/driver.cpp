#include <math.h>  // pow
#include <stdlib.h>  // rand, srand
#include <time.h>  // time
#include <emscripten.h>  // EM_ASM, EMSCRIPTEN_KEEPALIVE
#include <emscripten/bind.h>  // emscripten::{EMSCRIPTEN_BINDINGS, value_object, function}
#include "canvas.h"  // HTMLCanvasElement, createCanvas, freeCanvas
#include "window.h"  // Window, freeWindow

using namespace emscripten;

typedef struct Particle {
	double x;
	double y;
	double vx;
	double vy;
} Particle;


// Generate a value with a magnitude between [0.016, 0.5], zero half the time,
// and with a bias toward 0.
// Formula is f(x) = 0.5^x, 1 < x < 6.
double random_speed() {
	return pow(0.5, (((double)rand() / (double)(RAND_MAX) * 5) + 4)) * ((rand() % 1) - 1);
}


// void line_between(CanvasRenderingContext2D *context, struct Particle *p1, struct Particle *p2) {
// 	context->beginPath(context);
// 	context->moveTo(context, p1->x, p1->y);
// 	context->lineTo(context, p2->x, p2->y);
// 	context->stroke(context);
// }


struct Particle create_particle(double x, double y, double vx, double vy) {
	struct Particle result = {x, y, vx, vy};
	return result;
}


int main() {
	srand(time(nullptr));

	HTMLCanvasElement *canvas = createCanvas("root");
	// canvas->setHeight(canvas, Window()->getInnerHeight());
	// canvas->setWidth(canvas, Window()->getInnerWidth());

	// CanvasRenderingContext2D *ctx = canvas->getContext(canvas, "2d");

	EM_ASM({
		// C functions
		const randomSpeed = Module["random_speed"];
		// const lineBetween = Module["line_between"];
		const createParticle = Module["create_particle"];

		function lineBetween(context, particle, particle2) {
			context.beginPath();
			context.moveTo(particle["x"], particle["y"]);
			context.lineTo(particle2["x"], particle2["y"]);
			context.stroke();
    }

		var count = 0;

		function animate(canvas, context, particles, particleSize, threshold) {
			// Does these things:
			// - Update the canvas size in case the window size changes
			// - Bounce the particles off the edges
			// - Draw the particles
			// - Draw lines between particles
			// - Move the particles

			canvas.width = window.innerWidth;
			canvas.height = window.innerHeight;
			context.fillStyle = "#e5e3df";

			const checked = [];

			for (let i = 0; i < particles.length; ++i) {
				checked.push(i);
				const particle = particles[i];

				// Draw this particle.
				context.beginPath();
				context.arc(particle["x"], particle["y"], particleSize, 0, 2 * Math.PI);
				context.fill();

				for (let j = 0; j < particles.length; ++j) {
					// Avoid checking the same pair multiple times.
					if (!checked.includes(j)) {
						const particle2 = particles[j];
						const distance = ((particle["x"] - particle2["x"]) ** 2 + (particle["y"] - particle2["y"]) ** 2) ** (1/2);
						if (distance < threshold) {
							// Change the thickness and opacity of the line connecting
							// two particles based on their distance from each other.
							// The closer they are, the thicker and more opaque the line.
							context.lineWidth = Math.min((threshold / distance) - 1, particleSize);
							const opacity = (threshold / distance) - 1;
							context.strokeStyle = `rgba(229, 227, 223, ${opacity})`;
							lineBetween(context, particle, particle2);
						}
					}
				}

				// Apply the particle's velocity to its position.
				if (count < 10 && i == 0) {
					console.log(JSON.parse(JSON.stringify(particle)));
					console.log("particle.vx:   ", particle.vx);
					console.log('particle["vx"]:', particle["vx"]);
					console.log("particle.vx:   ", particle.vx);
					console.log('particle["vx"]:', particle["vx"]);
				}
				particle["x"] = particle["x"] + particle.vx;
				particle["y"] = particle["y"] + particle.vy;

				// Bounce off the edges of the screen.
				// Snapping the position to the edge of the screen isn't
				//   entirely necessary, but it will keep a particle from
				//   disappearing if it ends up outside of the window when
				//   the browser is resized.
				if (particle["x"] < particleSize) {
					// Left edge
					particle["vx"] *= -1;
					particle["x"] = particleSize;
				}
				else if (particle["x"] > canvas.width - particleSize) {
					// Right edge
					particle["vx"] *= -1;
					particle["x"] = canvas.width - particleSize;
				}
				if (particle["y"] < particleSize) {
					// Top edge
					particle["vy"] *= -1;
					particle["y"] = particleSize;
				}
				else if (particle["y"] > canvas.height - particleSize) {
					// Bottom edge
					particle["vy"] *= -1;
					particle["y"] = canvas.height - particleSize;
				}
			}

			count++;

			window.requestAnimationFrame(() => {
				animate(canvas, context, particles, particleSize, threshold);
			});
		}


		// Add or delete particles to match a new number.
		function setParticleCount(canvas, particles, count) {
			const quota = count - particles.length;

			// New count is smaller than current count
			if (quota < 0) {
				// Delete enough particles to have only as many as the new count
				particles.splice(0, -quota);
			} else {
				// Add enough particles to have as many as the new count
				for (let i = 0; i < count; ++i) {
					particles.push(createParticle(
						Math.random() * canvas.width,   // from 0 to canvas width
						Math.random() * canvas.height,  // from 0 to canvas height
						randomSpeed(),
						randomSpeed(),
					));
				}
			}
		}


		function init() {
			const PARTICLE_COUNT = 115;
			const PARTICLE_SIZE = 3;
			const THRESHOLD = 250;

			const canvas = document.querySelector("canvas");
			const context = canvas.getContext("2d");
			const particleCount = PARTICLE_COUNT;
			const particleSize = PARTICLE_SIZE;
			const threshold = THRESHOLD;

			const particles = [];  // type: Particle[]

			canvas.width = window.innerWidth;
			canvas.height = window.innerHeight;

			setParticleCount(canvas, particles, particleCount);
			animate(canvas, context, particles, particleSize, threshold);
		}

		init();
	});

	freeCanvas(canvas);
	freeWindow(Window());
	return 0;
}


EMSCRIPTEN_BINDINGS(module_constellations) {
	function("random_speed", &random_speed);
	// function("line_between", &line_between);
	value_object<Particle>("Particle")
		.field("x", &Particle::x)
		.field("y", &Particle::y)
		.field("vx", &Particle::vx)
		.field("vy", &Particle::vy);
	function("create_particle", &create_particle);
}
