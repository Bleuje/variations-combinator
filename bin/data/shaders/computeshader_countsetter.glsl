#version 440

uniform float width;
uniform float height;
uniform uint value;

layout(std430,binding=0) buffer mutex
{
	uint m[];
};

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main(){
	m[ gl_GlobalInvocationID.x * int(height) + gl_GlobalInvocationID.y ] = value;
}
