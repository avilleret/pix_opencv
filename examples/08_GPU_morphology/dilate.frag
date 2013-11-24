// up to 7x7 dilate algorithm
// Antoine Villeret - 2012

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect frame;
uniform int size;
uniform int pattern; // 0 cross, 1 square

vec2 texcoord0 = gl_TexCoord[0].st;

vec2 texcoord00 = texcoord0 + vec2(-3, -3);
vec2 texcoord01 = texcoord0 + vec2(-2, -3);
vec2 texcoord02 = texcoord0 + vec2(-1, -3);
vec2 texcoord03 = texcoord0 + vec2(0, -3);
vec2 texcoord04 = texcoord0 + vec2(1, -3);
vec2 texcoord05 = texcoord0 + vec2(2, -3);
vec2 texcoord06 = texcoord0 + vec2(3, -3);

vec2 texcoord10 = texcoord0 + vec2(-3, -2);
vec2 texcoord11 = texcoord0 + vec2(-2, -2);
vec2 texcoord12 = texcoord0 + vec2(-1, -2);
vec2 texcoord13 = texcoord0 + vec2(0, -2);
vec2 texcoord14 = texcoord0 + vec2(1, -2);
vec2 texcoord15 = texcoord0 + vec2(2, -2);
vec2 texcoord16 = texcoord0 + vec2(3, -2);

vec2 texcoord20 = texcoord0 + vec2(-3, -1);
vec2 texcoord21 = texcoord0 + vec2(-2, -1);
vec2 texcoord22 = texcoord0 + vec2(-1, -1);
vec2 texcoord23 = texcoord0 + vec2(0, -1);
vec2 texcoord24 = texcoord0 + vec2(1, -1);
vec2 texcoord25 = texcoord0 + vec2(2, -1);
vec2 texcoord26 = texcoord0 + vec2(3, -1);

vec2 texcoord30 = texcoord0 + vec2(-3, 0);
vec2 texcoord31 = texcoord0 + vec2(-2, 0);
vec2 texcoord32 = texcoord0 + vec2(-1, 0);
vec2 texcoord33 = texcoord0 + vec2(0, 0); // center pixel == texcoord0
vec2 texcoord34 = texcoord0 + vec2(1, 0);
vec2 texcoord35 = texcoord0 + vec2(2, 0);
vec2 texcoord36 = texcoord0 + vec2(3, 0);

vec2 texcoord40 = texcoord0 + vec2(-3, 1);
vec2 texcoord41 = texcoord0 + vec2(-2, 1);
vec2 texcoord42 = texcoord0 + vec2(-1, 1);
vec2 texcoord43 = texcoord0 + vec2(0, 1);
vec2 texcoord44 = texcoord0 + vec2(1, 1);
vec2 texcoord45 = texcoord0 + vec2(2, 1);
vec2 texcoord46 = texcoord0 + vec2(3, 1);

vec2 texcoord50 = texcoord0 + vec2(-3, 2);
vec2 texcoord51 = texcoord0 + vec2(-2, 2);
vec2 texcoord52 = texcoord0 + vec2(-1, 2);
vec2 texcoord53 = texcoord0 + vec2(0, 2);
vec2 texcoord54 = texcoord0 + vec2(1, 2);
vec2 texcoord55 = texcoord0 + vec2(2, 2);
vec2 texcoord56 = texcoord0 + vec2(3, 2);

vec2 texcoord60 = texcoord0 + vec2(-3, 3);
vec2 texcoord61 = texcoord0 + vec2(-2, 3);
vec2 texcoord62 = texcoord0 + vec2(-1, 3);
vec2 texcoord63 = texcoord0 + vec2(0, 3);
vec2 texcoord64 = texcoord0 + vec2(1, 3);
vec2 texcoord65 = texcoord0 + vec2(2, 3);
vec2 texcoord66 = texcoord0 + vec2(3, 3);

void main (void)
{

	float closed = texture2DRect(frame, texcoord33).r; // assume binary image
	
	// 1 pixel cross
	closed = max(closed, texture2DRect(frame, texcoord34).r);
	closed = max(closed, texture2DRect(frame, texcoord32).r);
	closed = max(closed, texture2DRect(frame, texcoord23).r);
	closed = max(closed, texture2DRect(frame, texcoord43).r);
	
	if ( pattern == 1 ) {
		// 1 pixel square
		closed = max(closed, texture2DRect(frame, texcoord22).r);
		closed = max(closed, texture2DRect(frame, texcoord44).r);
		closed = max(closed, texture2DRect(frame, texcoord24).r);
		closed = max(closed, texture2DRect(frame, texcoord42).r);
	}
	
	if ( size >= 2 ) {
		// 2 pixels cross
		closed = max(closed, texture2DRect(frame, texcoord35).r);
		closed = max(closed, texture2DRect(frame, texcoord31).r);
		closed = max(closed, texture2DRect(frame, texcoord13).r);
		closed = max(closed, texture2DRect(frame, texcoord53).r);
		
		if ( pattern == 1 ) {	
			// 1 pixel square
			closed = max(closed, texture2DRect(frame, texcoord11).r);
			closed = max(closed, texture2DRect(frame, texcoord55).r);
			closed = max(closed, texture2DRect(frame, texcoord15).r);
			closed = max(closed, texture2DRect(frame, texcoord51).r);
		}
	}
	
	if ( size >= 3 ) {
		// 3 pixels cross
		closed = max(closed, texture2DRect(frame, texcoord36).r);
		closed = max(closed, texture2DRect(frame, texcoord30).r);
		closed = max(closed, texture2DRect(frame, texcoord03).r);
		closed = max(closed, texture2DRect(frame, texcoord63).r);
	
		if ( pattern == 1 ) {	

			// 3 pixel square
			closed = max(closed, texture2DRect(frame, texcoord00).r);
			closed = max(closed, texture2DRect(frame, texcoord66).r);
			closed = max(closed, texture2DRect(frame, texcoord06).r);
			closed = max(closed, texture2DRect(frame, texcoord60).r);
		}
	}
	
	gl_FragColor = vec4(vec3(closed),1);
}
