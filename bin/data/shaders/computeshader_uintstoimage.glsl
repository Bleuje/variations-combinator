#version 440

uniform float width;
uniform float height;
uniform float countPerColorChange;
uniform int colormode;
uniform float randomizer;
uniform float time;
uniform int threeD;

layout(std430,binding=0) buffer mutex
{
	uint m[];
};

layout(rgba32f,binding=3) uniform writeonly image2D displayImage;

#define PI 3.1415926538
#define TWO_PI (2.*3.1415926538)

vec3 pal( in float t, in vec3 a, in vec3 b, in vec3 c, in vec3 d )
{
    return a + b*cos( 6.28318*(c*t+d) );
}

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

uint hash( uvec2 v ) {
    return hash( v.x ^ hash(v.y) );
}

float nrand(vec2 v ) {
	const uint mantissaMask = 0x007FFFFFu;
	const uint one          = 0x3F800000u;

	uint h = hash( floatBitsToUint( v ) );
	h &= mantissaMask;
	h |= one;

	float  r2 = uintBitsToFloat( h );
	return r2 - 1.0;
}

float prand(vec2 v)
{
	return nrand(v);
}

const float ALPHA = 0.14;
const float INV_ALPHA = 1.0 / ALPHA;
const float K = 2.0 / (PI * ALPHA);

float inv_error_function(float x)
{
	float y = log(1.0 - x*x);
	float z = K + 0.5 * y;
	return sqrt(sqrt(z*z - y * INV_ALPHA) - z) * sign(x);
}



float gaussian_rand( vec2 n)
{
	float t = randomizer;
	float x = nrand( n + 0.07*t );
    
	return inv_error_function(x*2.0-1.0)*0.15;
}

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main(){
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	uint cpt = m[ pos.x * int(height) + pos.y ];
	
	vec2 fpos = vec2(pos);

	float greyValue = float(cpt)/countPerColorChange;

	greyValue = max(0.,greyValue);
	vec4 color0 = vec4(greyValue,greyValue,greyValue,1.0);
	
	vec3 rgb;
	if(colormode==0) rgb = vec3(0.88)-0.76*tanh(color0.rgb); // black on light grey
	if(colormode==1) rgb = vec3(tanh(color0.rgb*0.7)); // white on black
	if(colormode>=2)
	{
		float greyValue2 = (threeD>0 ? 4.0*tanh(greyValue/2.0) : 1.1*tanh(greyValue/1.5));
		
		if(colormode==2) color0.rgb = aces(pal(1.0*greyValue2 + 0.25 -0.17*time, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.0,0.33,0.67)));
		if(colormode==6) color0.rgb = pal(1.25*greyValue2, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.0,0.10,0.20));
		if(colormode==7) color0.rgb = pal(1.25*greyValue2, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,1.0),vec3(0.3,0.20,0.20));
		if(colormode==5) color0.rgb = pal(1.25*greyValue2, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,1.0,0.5),vec3(0.8,0.90,0.30));
		if(colormode==3) color0.rgb = pal(1.25*greyValue2, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(1.0,0.7,0.4),vec3(0.0,0.15,0.20));
		if(colormode==4) color0.rgb = pal(0.8*greyValue2, vec3(0.5,0.5,0.5),vec3(0.5,0.5,0.5),vec3(2.0,1.0,0.0),vec3(0.5,0.20,0.25));
		
		if(colormode!=3) rgb = aces(vec3(tanh(pow(max(greyValue-0.01,0),0.05))*tanh(color0.rgb)));
		else rgb = vec3(0.88)-vec3(tanh(pow(max(greyValue-0.01,0),0.05))*tanh(color0.rgb));

		//if(threeD>0) rgb = pow(rgb,vec3(0.8));
	}
	
	imageStore(displayImage,pos,vec4(rgb,1.0));
}
