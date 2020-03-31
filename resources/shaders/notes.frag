#version 330

in INTERFACE {
	float trackId;
	vec2 uv;
	vec2 noteSize;
} In;

uniform vec3 primaryColor;
uniform vec3 secondaryColor;
uniform vec2 inverseScreenSize;
uniform float keyboardHeight;

#define cornerRadius 0.01

out vec4 fragColor;


void main(){
	
	// If lower area of the screen, discard fragment as it should be hidden behind the keyboard.
	if(gl_FragCoord.y < keyboardHeight/inverseScreenSize.y){
		discard;
	}
	
	// Rounded corner (super-ellipse equation).
	float radiusPosition = pow(abs(In.uv.x/(0.5*In.noteSize.x)), In.noteSize.x/cornerRadius) + pow(abs(In.uv.y/(0.5*In.noteSize.y)), In.noteSize.y/cornerRadius);
	
	if(	radiusPosition > 1.0){
		discard;
	}
	
	// Fragment color.
	fragColor.rgb = mix(primaryColor, secondaryColor, mod(In.trackId, 2));
	
	if(	radiusPosition > 0.8){
		fragColor.rgb *= 1.05;
	}
	fragColor.a = 1.0;
}
