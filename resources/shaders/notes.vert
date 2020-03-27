#version 330

layout(location = 0) in vec2 v;
layout(location = 1) in vec4 id; //note id, start, duration, track

uniform float time;
uniform float mainSpeed;
uniform float minorsWidth = 1.0;

#define notesCount 52.0

out INTERFACE {
	float isMinor;
	vec2 uv;
	vec2 noteSize;
} Out;

int noteShift[88] = int[88](
	0, 0, 1, 2, 2, 3, 3, 4, 5, 5, 6, 6, 7, 7, 8, 9, 9, 10, 10, 11,
	12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 21, 22, 23,
	23, 24, 24, 25, 26, 26, 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34,
	35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46,
	47, 47, 48, 48, 49, 49, 50, 51
);

bool noteIsMinor[88] = bool[88](
	false, true, false, false, true, false, true, false, false, true, false,
	true, false, true, false, false, true, false, true, false, false, true,
	false, true, false, true, false, false, true, false, true, false, false,
	true, false, true, false, true, false, false, true, false, true, false,
	false, true, false, true, false, true, false, false, true, false, true,
	false, false, true, false, true, false, true, false, false, true, false,
	true, false, false, true, false, true, false, true, false, false, true,
	false, true, false, false, true, false, true, false, true, false, false
);

void main(){
	float _noteShift = float(noteShift[int(id.x) - 21]);
	float _isMinor = noteIsMinor[int(id.x) - 21] ? 1.0 : 0.0;
	
	float scalingFactor = _isMinor != 0.0 ? minorsWidth : 1.0;
	// Size of the note : width, height based on duration and current speed.
	Out.noteSize = vec2(0.9*2.0/notesCount * scalingFactor, id.z*mainSpeed);
	
	// Compute note shift.
	// Horizontal shift based on note id, width of keyboard, and if the note is minor or not.
	// Vertical shift based on note start time, current time, speed, and height of the note quad.
	const float a = (1.0/(notesCount-1.0)) * (2.0 - 2.0/notesCount);
	const float b = -1.0 + 1.0/notesCount;
	vec2 noteShift = vec2(_noteShift * a + b + _isMinor/notesCount, Out.noteSize.y * 0.5 - 0.5 + mainSpeed * (id.y - time));
	
	// Scale uv.
	Out.uv = Out.noteSize * v;
	Out.isMinor = _isMinor;
	// Output position.
	gl_Position = vec4(Out.noteSize * v + noteShift, 0.0 , 1.0) ;
	
}
