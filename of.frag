#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect currentFrame, previousFrame;
uniform float lambda;
uniform vec2 scale;

vec2 offset = vec2(1.,1.);
vec2 texcoord0 = gl_TexCoord[0].st;
vec2 texcoord1 = gl_TexCoord[1].st;

void main()
{   
	vec4 a = texture2DRect(currentFrame, texcoord0);
	vec4 b = texture2DRect(previousFrame , texcoord1);
	vec2 x1 = vec2(offset.x,0.);
	vec2 y1 = vec2(0.,offset.y);

	//get the difference
	vec4 curdif = b-a;
	
	//calculate the gradient
	vec4 gradx = texture2DRect(previousFrame, texcoord1+x1)-texture2DRect(previousFrame, texcoord1-x1);
	gradx += texture2DRect(currentFrame, texcoord0+x1)-texture2DRect(currentFrame, texcoord0-x1);
	vec4 grady = texture2DRect(previousFrame, texcoord1+y1)-texture2DRect(previousFrame, texcoord1-y1);
	grady += texture2DRect(currentFrame, texcoord0+y1)-texture2DRect(currentFrame, texcoord0-y1);
	vec4 gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));
	
	if ( gradmag.r == 0. ) {
		gl_FragColor = vec4(0.5,0.5,0.5,1);
		return;
	}

	vec4 vx = curdif*(gradx/gradmag);
	float vxd = vx.r;//assumes greyscale
	//format output for flowrepos, out(-x,+x,-y,+y)
	//~ vec2 xout = vec2(max(vxd,0.),abs(min(vxd,0.)))*scale.x;
	float xout = ((vxd/2.)*scale.x+0.5);

	vec4 vy = curdif*(grady/gradmag);
	float vyd = vy.r;//assumes greyscale
	//format output for flowrepos, out(-x,+x,-y,+y)
	//~ vec2 yout = vec2(max(vyd,0.),abs(min(vyd,0.)))*scale.y;
	float yout = ((vyd/2.)*scale.y+0.5);
	
	//~ gl_FragColor = vec4(xout.xy,yout.xy);
	gl_FragColor = vec4(xout,yout,0.5,1.);
}
