#ifndef Score_h
#define Score_h
#include <GLFW/glfw3.h>
#include <gl3w/gl3w.h>
#include <glm/glm.hpp>

#include "ScreenQuad.h"
#include "../libmidi/Midi.h"


class Score : public ScreenQuad {
	size_t _barLineLength, _barLineIndex;
	float* _barLines;

public:

	/// Init function with measure time.
	Score(const MidiEventMicrosecondList& barLines);
	~Score();
	
	void setScaleAndMinorWidth(const float scale, const float width);
	
	void setDisplay(const bool digits, const bool horiz, const bool vert);
	
	void setColors(const glm::vec3 & linesColor, const glm::vec3 & textColor, const glm::vec3 & keysColor);

	void _draw(float time, glm::vec2 invScreenSize);

	void reset();
};

#endif
