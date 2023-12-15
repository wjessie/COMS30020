#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>
#include <TexturePoint.h>

#define WIDTH 320
#define HEIGHT 240

void draw(DrawingWindow &window) {
	window.clearPixels();
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = rand() % 256;
			float green = 0.0;
			float blue = 0.0;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

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

void drawGreyscale(DrawingWindow &window) {
	std::vector<float> result;
	result = interpolateSingleFloats(255, 0, window.width);
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
			float red = result[x];
			float green = result[x];
			float blue = result[x];
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues) {
	std::vector<glm::vec3> result;

	float xNum = from.x;
	float xDiff = (to.x-from.x) / (numberOfValues-1);
	float yNum = from.y;
	float yDiff = (to.y-from.y) / (numberOfValues-1);
	float zNum = from.z;
	float zDiff = (to.z-from.z) / (numberOfValues-1);

	for(int i=0; i<numberOfValues; i++) {
		glm::vec3 answer(xNum, yNum, zNum);
		result.push_back(answer);
		xNum += xDiff;
		yNum += yDiff;
		zNum += zDiff;
	}
	return result;
}

void drawTwoDimensionalColour(DrawingWindow &window) {
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

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colour) {
	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float numberOfSteps = fmax(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	for (float i = 0.0; i <= numberOfSteps; i++) {
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);
		uint32_t lineColour = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
		window.setPixelColour(round(x), round(y), lineColour);
	}
}

void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangleVertices, Colour colour) {
	drawLine(window, triangleVertices[0], triangleVertices[1], colour);
	drawLine(window, triangleVertices[0], triangleVertices[2], colour);
	drawLine(window, triangleVertices[1], triangleVertices[2], colour);
}

std::vector<CanvasPoint> interpolateVertices(CanvasPoint from, CanvasPoint to) {
	std::vector<CanvasPoint> interpolatedPoints;
	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float numberOfSteps = fmax(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;
	for (float i = 0.0; i <= numberOfSteps; i++) {
		float x = round(from.x + (xStepSize * i));
		float y = round(from.y + (yStepSize * i));
		interpolatedPoints.push_back(CanvasPoint(x, y));
	}
	return interpolatedPoints;
}

void drawFilledFlatBottomTriangle(DrawingWindow &window, CanvasTriangle triangleVertices, Colour colour) {
	std::vector<CanvasPoint> topToLeft = interpolateVertices(triangleVertices[0], triangleVertices[1]);
	std::vector<CanvasPoint> topToRight = interpolateVertices(triangleVertices[0], triangleVertices[2]);
	for (int y = triangleVertices[0].y; y <= triangleVertices[2].y; y++) {
		int xLeft = INT16_MAX;
		int xRight = INT16_MIN;
		for (int i = 0; i < topToLeft.size(); i++) {
			if (topToLeft[i].y == y && topToLeft[i].x < xLeft) {
				xLeft = topToLeft[i].x;
			}
		}
		for (int i = 0; i < topToRight.size(); i++) {
			if (topToRight[i].y == y && topToRight[i].x > xRight) {
				xRight = topToRight[i].x;
			}
		}
		drawLine(window, CanvasPoint(xLeft, y), CanvasPoint(xRight, y), colour);
	}
}

void drawFilledFlatTopTriangle(DrawingWindow &window, CanvasTriangle triangleVertices, Colour colour) {
	std::vector<CanvasPoint> leftToBottom = interpolateVertices(triangleVertices[0], triangleVertices[2]);
	std::vector<CanvasPoint> rightToBottom = interpolateVertices(triangleVertices[1], triangleVertices[2]);
	for (int y = triangleVertices[0].y; y <= triangleVertices[2].y; y++) {
		int xLeft = INT16_MAX;
		int xRight = INT16_MIN;
		for (int i = 0; i < leftToBottom.size(); i++) {
			if (leftToBottom[i].y == y && leftToBottom[i].x < xLeft) {
				xLeft = leftToBottom[i].x;
			}
		}
		for (int i = 0; i < rightToBottom.size(); i++) {
			if (rightToBottom[i].y == y && rightToBottom[i].x > xRight) {
				xRight = rightToBottom[i].x;
			}
		}
		drawLine(window, CanvasPoint(xLeft, y), CanvasPoint(xRight, y), colour);
	}
}

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangleVertices, Colour colour) {
	// Sort the vertices by vertical position, top to bottom (y-coordinates)
	if (triangleVertices[0].y > triangleVertices[1].y) std::swap(triangleVertices[0], triangleVertices[1]);
	if (triangleVertices[0].y > triangleVertices[2].y) std::swap(triangleVertices[0], triangleVertices[2]);
	if (triangleVertices[1].y > triangleVertices[2].y) std::swap(triangleVertices[1], triangleVertices[2]);

	if (triangleVertices[1].y == triangleVertices[2].y) {
		// For filling flat-bottom triangles, top to bottom and left to right, no need to divide into 2 triangles
		// There is only a top vertex, left bottom vertex, and right bottom vertex
		drawFilledFlatBottomTriangle(window, triangleVertices, colour);
	}
	else if (triangleVertices[0].y == triangleVertices[1].y) {
		// For filling flat-top triangles, top to bottom and left to right, no need to divide into 2 triangles
		// There is only a left top vertex, right top vertex, and bottom vertex 
		drawFilledFlatTopTriangle(window, triangleVertices, colour);
	}
	else {
		// Calculate the x-coordinate of the extra point for dividing the triangle
		// The y-coordinate is the same as the y-coordinate of the middle vertex
		float xDiffBottomTop = triangleVertices[2].x - triangleVertices[0].x;
		float yDiffBottomTop = triangleVertices[2].y - triangleVertices[0].y;
		float yDiffMiddleTop = triangleVertices[1].y - triangleVertices[0].y;
		float xDiffMiddleTop = round((xDiffBottomTop * yDiffMiddleTop) / yDiffBottomTop);
		float extraX = triangleVertices[0].x + xDiffMiddleTop;
		CanvasPoint leftMiddlePoint(extraX, triangleVertices[1].y);

		// Determine which middle point is left and which is right
		CanvasPoint rightMiddlePoint = triangleVertices[1];
		if (extraX > triangleVertices[1].x) std::swap(leftMiddlePoint, rightMiddlePoint);
		drawLine(window, leftMiddlePoint, rightMiddlePoint, Colour(0, 255, 0));

		// Fill top triangle, from top to bottom and left to right
		std::vector<CanvasPoint> topToLeftSide = interpolateVertices(triangleVertices[0], leftMiddlePoint);
		std::vector<CanvasPoint> topToRightSide = interpolateVertices(triangleVertices[0], rightMiddlePoint);
		for (int y = triangleVertices[0].y; y <= triangleVertices[1].y; y++) {
			int xLeft = INT16_MAX;
			int xRight = INT16_MIN;
			for (int i = 0; i < topToLeftSide.size(); i++) {
				if (topToLeftSide[i].y == y && topToLeftSide[i].x < xLeft) {
					xLeft = topToLeftSide[i].x;
				}
			}
			for (int i = 0; i < topToRightSide.size(); i++) {
				if (topToRightSide[i].y == y && topToRightSide[i].x > xRight) {
					xRight = topToRightSide[i].x;
				}
			}
			drawLine(window, CanvasPoint(xLeft, y), CanvasPoint(xRight, y), colour);
		}

		// Fill bottom triangle, from top to bottom and left to right
		std::vector<CanvasPoint> leftToBottomSide = interpolateVertices(leftMiddlePoint, triangleVertices[2]);
		std::vector<CanvasPoint> rightToBottomSide = interpolateVertices(rightMiddlePoint, triangleVertices[2]);
		for (int y = leftMiddlePoint.y; y <= triangleVertices[2].y; y++) {
			int xLeft = INT16_MAX;
			int xRight = INT16_MIN;
			for (int i = 0; i < leftToBottomSide.size(); i++) {
				if (leftToBottomSide[i].y == y && leftToBottomSide[i].x < xLeft) {
					xLeft = leftToBottomSide[i].x;
				}
			}
			for (int i = 0; i < rightToBottomSide.size(); i++) {
				if (rightToBottomSide[i].y == y && rightToBottomSide[i].x > xRight) {
					xRight = rightToBottomSide[i].x;
				}
			}
			drawLine(window, CanvasPoint(xLeft, y), CanvasPoint(xRight, y), colour);
		}
	}

	// Draw a white stroked triangle over the top of the filled triangle
	drawStrokedTriangle(window, triangleVertices, Colour(255, 255, 255));
}

std::vector<std::vector<uint32_t>> repositionPixels(TextureMap textureMap) {
	// 2D vector is indexed by [row][column], which is opposite of coordinates (x, y)
	// Thus, row corresponds with y and column corresponds with x
	std::vector<std::vector<uint32_t>> reconstructedPixels(textureMap.height, std::vector<uint32_t>(textureMap.width));
	std::vector<uint32_t> linearPixelData = textureMap.pixels;
	for(size_t i = 0; i < textureMap.height; i++) {
		for (size_t j = 0; j < textureMap.width; j++) {
			reconstructedPixels[i][j] = linearPixelData[(i * textureMap.width) + j];
		}
	}
	return reconstructedPixels;
}

void loadTextureMap(DrawingWindow &window, CanvasTriangle canvasPoints, TextureMap textureMap) {
	std::vector<std::vector<uint32_t>> imagePixels = repositionPixels(textureMap);

	// // testing
	// for (int y = 0; y < window.height; y++) {
	// 	for (int x = 0; x < window.width; x++) {
	// 		uint32_t colour = imagePixels[y][x];
	// 		window.setPixelColour(x, y, colour);
	// 	}
	// }

	

}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
		else if (event.key.keysym.sym == SDLK_u) {
			float red = rand() % 256;
			float green = rand() % 256;
			float blue = rand() % 256;
			Colour randColour(red, green, blue);
			CanvasPoint v0(rand() % window.width, rand() % window.height);
			CanvasPoint v1(rand() % window.width, rand() % window.height);
			CanvasPoint v2(rand() % window.width, rand() % window.height);
			CanvasTriangle randTriangle(v0, v1, v2);
			drawStrokedTriangle(window, randTriangle, randColour);
		}
		else if (event.key.keysym.sym == SDLK_f) {
			float red = rand() % 256;
			float green = rand() % 256;
			float blue = rand() % 256;
			Colour randColour(red, green, blue);
			CanvasPoint v0(rand() % window.width, rand() % window.height);
			CanvasPoint v1(rand() % window.width, rand() % window.height);
			CanvasPoint v2(rand() % window.width, rand() % window.height);
			CanvasTriangle randTriangle(v0, v1, v2);
			drawFilledTriangle(window, randTriangle, randColour);
		}
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	// Test interpolateSingleFloats function
	std::vector<float> result;
	result = interpolateSingleFloats(2.2, 8.5, 7);
	for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	std::cout << std::endl << std::endl;

	// Test interpolateThreeElementValues function
	std::vector<glm::vec3> resultThreeElements;
	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	resultThreeElements = interpolateThreeElementValues(from, to, 4);
	for(size_t i=0; i<resultThreeElements.size(); i++) {
		glm::vec3 resultThree = resultThreeElements[i];
		std::cout << "(" << resultThree.x << ", " << resultThree.y << ", " << resultThree.z << ")\n";
	}
	std::cout << std::endl;

	// drawGreyscale(window);
	// drawTwoDimensionalColour(window);

	// Line drawing points and colours
	CanvasPoint topLeftCorner(0, 0);
	CanvasPoint topRightCorner(window.width-1, 0);
	CanvasPoint centre((window.width-1)/2, (window.height-1)/2);
	CanvasPoint topCentre((window.width-1)/2, 0);
	CanvasPoint bottomCentre((window.width-1)/2, window.height-1);
	CanvasPoint oneThirdLeft((window.width-1)/3, (window.height-1)/2);
	CanvasPoint oneThirdRight(((window.width-1)/3)*2, (window.height-1)/2);
	Colour WHITE(255, 255, 255);

	// // Draw a line from the top-left corner of the window to the centre of the window
	// drawLine(window, topLeftCorner, centre, WHITE);
	// // Draw a line from the top-right corner of the window to the centre of the window
	// drawLine(window, topRightCorner, centre, WHITE);
	// // Draw a vertical line all the way down the middle of the screen
	// drawLine(window, topCentre, bottomCentre, WHITE); 
	// // Draw a horizontal line a third the width of the screen, centred both horizontally and vertically
	// drawLine(window, oneThirdLeft, oneThirdRight, WHITE);

	// Texture mapping
	TextureMap texture("texture.ppm");
	std::cout << "texture map height: " << texture.height << "\n";
	std::cout << "texture map width: " << texture.width << "\n";

	CanvasPoint canvasPoint1(160, 10);
	TexturePoint texturePoint1(195, 5);
	canvasPoint1.texturePoint = texturePoint1;

	CanvasPoint canvasPoint2(300, 230);
	TexturePoint texturePoint2(395, 380);
	canvasPoint2.texturePoint = texturePoint2;

	CanvasPoint canvasPoint3(10, 150);
	TexturePoint texturePoint3(65, 330);
	canvasPoint3.texturePoint = texturePoint3;

	CanvasTriangle textureTriangle(canvasPoint1, canvasPoint2, canvasPoint3);

	loadTextureMap(window, textureTriangle, texture);

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);

		// draw(window);

		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
