#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#define WIDTH 320
#define HEIGHT 240

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues) {
	std::vector<float> result;
	float num = from;
	float diff = (to-from) / (numberOfValues-1);
	for(int i=0; i<numberOfValues; i++) {
		result.push_back(num);
		num += diff;
	}
	return result;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
	std::vector<glm::vec3> result;

	float num1 = from.x;
	float diff1 = (to.x-from.x) / (numberOfValues-1);
	float num2 = from.y;
	float diff2 = (to.y-from.y) / (numberOfValues-1);
	float num3 = from.z;
	float diff3 = (to.z-from.z) / (numberOfValues-1);

	for(int i=0; i<numberOfValues; i++) {
		glm::vec3 answer(num1, num2, num3);
		result.push_back(answer);
		num1 += diff1;
		num2 += diff2;
		num3 += diff3;
	}
	return result;
}

void draw(DrawingWindow &window) {
	window.clearPixels();
	/*
	RedNoise Implementation
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
	*/

	/*
	Greyscale interpolation
	std::vector<float> result;
	result = interpolateSingleFloats(256, 0, window.width);
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = result[x];
			float green = result[x];
			float blue = result[x];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
	*/

	// Two Dimensional Colour Interpolation
	glm::vec3 topLeft(255, 0, 0);        // red 
	glm::vec3 topRight(0, 0, 255);       // blue 
	glm::vec3 bottomRight(0, 255, 0);    // green 
	glm::vec3 bottomLeft(255, 255, 0);   // yellow
	std::vector<glm::vec3> leftmostColumn = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	std::vector<glm::vec3> rightmostColumn = interpolateThreeElementValues(topRight, bottomRight, window.height);
	for (size_t y = 0; y < window.height; y++) {
		glm::vec3 rowLeftPixel = leftmostColumn[y];
		glm::vec3 rowRightPixel = rightmostColumn[y];
		std::vector<glm::vec3> row = interpolateThreeElementValues(rowLeftPixel, rowRightPixel, window.width);
		for (size_t x = 0; x < window.width; x++) {
			float red = row[x].x;
			float green = row[x].y;
			float blue = row[x].z;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	std::vector<float> result;
	result = interpolateSingleFloats(2.2, 8.5, 7);
	for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	std::cout << std::endl;

	std::vector<glm::vec3> resultThreeElements;
	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	resultThreeElements = interpolateThreeElementValues(from, to, 4);
	for(size_t i=0; i<resultThreeElements.size(); i++) {
		glm::vec3 resultThree = resultThreeElements[i];
		std::cout << "(" << resultThree.x << ", " << resultThree.y << ", " << resultThree.z << ")\n";
	}
	std::cout << std::endl;

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
