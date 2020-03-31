#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../helpers/ProgramUtilities.h"
#include "../helpers/ResourcesManager.h"

#include "Score.h"

#define NUM_BAR_LINES 10

Score::Score(const MidiEventMicrosecondList& barLines){
	_barLineLength = std::max(barLines.size(), (size_t) NUM_BAR_LINES);
	_barLineIndex = 0;
	_barLines = (float*) calloc(_barLineLength, sizeof(float));
	for (int i = 0; i < barLines.size(); i++)
		_barLines[i] = barLines[i] / 1000000.0f;
	
	// Load font atlas.
	GLuint textureId = ResourcesManager::getTextureFor("font");
	ScreenQuad::init(textureId, "background_frag");
}

Score::~Score() {
	free(_barLines);
}

void Score::setScaleAndMinorWidth(const float scale, const float width){
	glUseProgram(_programId);
	GLuint speedID = glGetUniformLocation(_programId, "mainSpeed");
	glUniform1f(speedID, scale);
	GLuint widthId = glGetUniformLocation(_programId, "minorsWidth");
	glUniform1f(widthId, width);
	glUseProgram(0);
}

void Score::setDisplay(const bool digits, const bool horiz, const bool vert){
	glUseProgram(_programId);
	GLuint id1 = glGetUniformLocation(_programId, "useDigits"); glUniform1i(id1, digits);
	GLuint id2 = glGetUniformLocation(_programId, "useHLines"); glUniform1i(id2, horiz);
	GLuint id3 = glGetUniformLocation(_programId, "useVLines"); glUniform1i(id3, vert);
	glUseProgram(0);
}

void Score::setColors(const glm::vec3 & linesColor, const glm::vec3 & textColor, const glm::vec3 & keysColor){
	glUseProgram(_programId);
	GLuint id1 = glGetUniformLocation(_programId, "linesColor"); glUniform3fv(id1, 1, &linesColor[0]);
	GLuint id2 = glGetUniformLocation(_programId, "textColor"); glUniform3fv(id2, 1, &textColor[0]);
	GLuint id3 = glGetUniformLocation(_programId, "keysColor"); glUniform3fv(id3, 1, &keysColor[0]);
	glUseProgram(0);
}

void Score::_draw(float time, glm::vec2 invScreenSize, float keyboardHeight) {
	while (_barLines[_barLineIndex] < time)
		_barLineIndex++;
	glUseProgram(_programId);
	glUniform1f(glGetUniformLocation(_programId, "keyboardHeight"), keyboardHeight);
	GLuint barLineIndexID = glGetUniformLocation(_programId, "barLineIndex");
	glUniform1i(barLineIndexID, _barLineIndex);
	GLuint barLinesID = glGetUniformLocation(_programId, "barLines");
	glUniform1fv(barLinesID, NUM_BAR_LINES, _barLines + std::min(_barLineIndex, _barLineLength - NUM_BAR_LINES));
	ScreenQuad::draw(time, invScreenSize, keyboardHeight);
}

void Score::reset() {
	_barLineIndex = 0;
}