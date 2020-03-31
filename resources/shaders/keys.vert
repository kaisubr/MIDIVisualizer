#version 330

layout(location = 0) in vec2 v;

out INTERFACE {
	vec2 uv;
} Out ;

#define bottomLimit 0.10

void main(){
	// Input are in -0.5,0.5
	// We directly output the position.
	float yShift = v.y == -0.5 ? -1.0 : mix(-1.0, 1.0, bottomLimit);
	gl_Position = vec4(v.x*2.0, yShift, 0.0, 1.0);
	// Output the UV coordinates computed from the positions.
	Out.uv = v.xy + 0.5;
	
}
