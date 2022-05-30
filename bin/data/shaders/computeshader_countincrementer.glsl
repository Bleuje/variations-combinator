#version 440

uniform float width;
uniform float height;
uniform int nx;
uniform int ny;
uniform int sequenceLength;
uniform int doSinusoid;
uniform float sinusoidStretch;
uniform int operationsMode;
uniform int distortionMode;
uniform float randomizer;
uniform int aIndex;
uniform float aRadius;
uniform float aAngle;
uniform int aState;
uniform int threeD;
uniform float mouseX;
uniform float mouseY;
uniform float rot1;
uniform float rot2;
uniform float rot3;

uniform int projectionIndex;
uniform float projDist;

#define PI 3.1415926538
#define TWO_PI (2.*3.1415926538)

struct RandomParameters{
	int foldType;
	float weight;
	float tx;
	float ty;
	float controlledRotationAngle;

	bool lines_squared;
	float lines_scale;
	float twintrian_weight;
	float rdu01_0;
	int julian_power;
	float julian_dist;
	int juliascope_power;
	float juliascope_dist;
	float rectangles_x;
	float rectangles_y;
	float scry_weight;
	float fan2_x;
	float fan2_y;
	float pdj_a;
	float pdj_b;
	float pdj_c;
	float pdj_d;
	int wedgejulia_count;
	float wedgejulia_angle;
	int wedgejulia_power;
	float wedgejulia_dist;
	float rotation_angle;
	float curl_c1;
	float curl_c2;
	float butterfly_xscl;
	float rings2_val;
	float popcorn_c;
	float popcorn_f;
	float waves_b;
	float waves_e;
	float waves_c;
	float waves_f;
	float bipolar_shift;
	float cell_size;
	float supershape_rnd;
	float supershape_m;
	float supershape_n1;
	float supershape_n2;
	float supershape_n3;
	float supershape_holes;
	float loonie_weight;
	float blade_weight;
	float rays_weight;
	int ngon_sides;
	float ngon_power;
	float ngon_circle;
	float ngon_corners;
	float oscilloscope_separation;
	float oscilloscope_frequency;
	float oscilloscope_amplitude;
	float oscilloscope_damping;
	float oscilloscope2_separation;
	float oscilloscope2_frequencyx;
	float oscilloscope2_frequencyy;
	float oscilloscope2_amplitude;
	float oscilloscope2_damping;
	float oscilloscope2_perturbation;
	float blocky_x;
	float blocky_y;
	float blocky_mp;
	float blocky_weight;
	float waves2_freqx;
	float waves2_freqy;
	float waves2_scalex;
	float waves2_scaley;
	float blob_low;
	float blob_high;
	float blob_waves;
	float bsplit_shiftx;
	float bsplit_shifty;
	float bswirl_in;
	float bswirl_out;
	float bwraps7_cellsize, bwraps7_space, bwraps7_gain, bwraps7_inner_twist, bwraps7_outer_twist;
	float blurzoom_x, blurzoom_y, blurzoom_length;
	float bmod_radius, bmod_distance;
	float bcollide_num, bcollide_a;
	float boarders2_c, boarders2_left, boarders2_right;
	float lazysuzan_twist, lazysuzan_space, lazysuzan_spin, lazysuzan_x, lazysuzan_y, lazysuzan_weight;
	float glynnsim1_radius, glynnsim1_radius1, glynnsim1_phi1, glynnsim1_thickness, glynnsim1_pow, glynnsim1_contrast;
	float rings_c2;
	float escher_beta;
	float radialblur_angle;
	float split_xsize, split_ysize;
	


	float julian_cpower;
	float julian_abspower;
	float juliascope_cpower;
	float juliascope_abspower;
	float fan2_dx;
	float fan2_dx2;
	float wedgejulia_cf;
	float wedgejulia_cn;
	float wedgejulia_rN;
	float rings2_val2;
	float bipolar_ps;
	float supershape_pm_4;
	float supershape_pneg1_n1;
	float ngon_pow;
	float ngon_b;
	float blob_bdiff;
	float bwraps7_g2, bwraps7_r2, bwraps7_rfactor;
	float bcollide_bcn_pi, bcollide_pi_bcn, bcollide_bca_bcn;
	float boarders2_cl, boarders2_cr;
	float glynnsim1_x1, glynnsim1_y1, glynnsim1_abspow;
	float escher_vc, escher_vd;
	float radialblur_spinvar, radialblur_zoomvar;
};

layout(std430,binding=4) buffer parameters
{
	RandomParameters params[];
};

layout(std430,binding=0) buffer mutex
{
	uint m[];
};

layout(rgba32f,binding=1) uniform writeonly image2D debugImage;

vec2 LoopedPosition(vec2 pos)
{
	return vec2(mod(pos.x + width,width),mod(pos.y+height,height));
}

float random(vec2 st) {
    return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123);
}

const float ALPHA = 0.14;
const float INV_ALPHA = 1.0 / ALPHA;
const float K = 2.0 / (PI * ALPHA);
/*
float prand(vec2 p)
{
	//return mod(7.1234234*p.x+193.9873657*p.y,1.0);
	return fract(sin(dot(p.xy, vec2(17.98988, 153.23223)))* 14358.5453325);
}

float nrand( vec2 n )
{
	return fract(sin(dot(n.xy, vec2(17.98988, 153.23223)))* 14358.5453325);
	//return prand(n);
}
*/
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


float sq(float v) {
	return pow(v,2.0);
}



float uniformRandom(float a, float b, vec2 seed) {
	return a + (b-a)*prand(seed);
}

float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x,y), atan(y,x), s);
}

float atan2(vec2 p)
{
	return atan2(p.y,p.x);
}


float length2(vec2 p)
{
	return 0.+length(p);
}

float eps = 0.00001;

vec2 lines(vec2 p, float weight, int index) {
	float r;
	if(params[index].lines_squared) r=0.5f*sq(gaussian_rand(13.*p+vec2(1.4,3.211)+vec2(555.555*p.y)));
	else r=0.25f*gaussian_rand(13.*p+vec2(3.4,3.911)+vec2(7777.7777+654*p.x));
	float y = params[index].lines_scale * (floor(p.y/params[index].lines_scale) - 0.5f + r);

	return vec2(weight * p.x, weight * y);      
}

vec2 twintrian(vec2 p, float weight, int index) {
    float a = uniformRandom(0,1,10.*p) * params[index].twintrian_weight * length2(p);
    float sa = sin(a);
    float cla = cos(a) + log(sq(sa));
    if(cla<-30) cla = -30;
    
    return vec2(weight * 0.8f * p.x * cla, weight * 0.8f * p.x * (cla - sa * PI));
}



vec2 julian(vec2 p, float weight, int index) {
	//float a = (atan2(vec2(p.y,p.x)) + 2.*PI * floor(params[index].julian_abspower * nrand(10.*p+vec2(321.52,654.4))))/params[index].julian_power;
	float a = (atan2(vec2(p.y,p.x)) + 2.*PI * floor(params[index].julian_abspower * prand(p)))/params[index].julian_power;
	float r = weight * 2.0f * pow(sq(p.x)+sq(p.y)+eps,params[index].julian_cpower);
	return vec2(r*cos(a),r*sin(a));
}

vec2 juliascope(vec2 p, float weight, int index) {
	//int rnd = int(params[index].juliascope_abspower * nrand(11.*p+vec2(9321.52,64.44)));
	int rnd = int(params[index].juliascope_abspower * prand(p));
	float a;
	if((rnd + 1234) % 2 == 0)
		a = (2 * PI * rnd +atan2(vec2(p.y,p.x)))/params[index].juliascope_power;
	else
		a = (2 * PI * rnd -atan2(vec2(p.y,p.x)))/params[index].juliascope_power;
	float r = weight * 2.0f * pow(sq(p.x)+sq(p.y)+eps,params[index].juliascope_cpower);
	return vec2(r*cos(a),r*sin(a));
}

vec2 rectangles(vec2 p, float weight, int index) {
    float x = weight * ((2 * floor(p.x/params[index].rectangles_x) + 1)* params[index].rectangles_x - p.x);
    float y = weight * ((2 * floor(p.y/params[index].rectangles_y) + 1)* params[index].rectangles_y - p.y);
    
    return vec2(x,y);
}

vec2 sinusoidal(vec2 p, float weight, int index) {
	return vec2(weight * 3.0f * sin(p.x/1.5),weight * 3.0f * sin(p.y/1.5));
}

vec2 tanhBounder(vec2 p, float weight, int index) {
	return vec2(weight * 3.0 * tanh(p.x/3.0),weight * 3.0 * tanh(p.y/3.0));
}

vec2 scry(vec2 p, float weight, int index) {
	float r2 = sq(p.x)*sq(p.y);
	float r = 4.0f / (length2(p) * (r2 + 1.0f/params[index].scry_weight));
	float x = weight * r * p.x;
	float y = weight * r * p.y;
	return vec2(x,y);
}

vec2 fan2(vec2 p, float weight, int index) {
    float r = weight * 0.9f * length2(p);
    float theta = atan2(vec2(p.x,p.y));
    float t = theta + params[index].fan2_y - floor((theta + params[index].fan2_y) / params[index].fan2_dx) * params[index].fan2_dx;
    float ang;
    if(t > params[index].fan2_dx2)
      ang = theta - params[index].fan2_dx2;
     else
      ang = theta + params[index].fan2_dx2;
    
    return vec2(r * sin(ang), r * cos(ang));
}

vec2 pdj(vec2 p, float weight, int index) {
	return vec2( weight * 1.5f * (sin(params[index].pdj_a * p.y) - cos(params[index].pdj_b * p.x)),
                weight * 1.5f * (sin(params[index].pdj_c * p.x) - cos(params[index].pdj_d * p.y)));
}


vec2 butterfly(vec2 p, float weight, int index) {
      float y2 = 2.0f * p.y;
      float r = weight * 1.3029400317411197908970256609023f * sqrt(abs(p.y * p.x) / (eps + sq(p.x) + sq(y2)));
      return vec2(r * p.x * params[index].butterfly_xscl,r * y2); 
  }

vec2 wedgejulia(vec2 p, float weight, int index) {
    float r = 1.35*weight * pow(sq(p.x)+sq(p.y),params[index].wedgejulia_cn);
    //int t_rnd = int((params[index].wedgejulia_rN) * nrand(20.1*p+vec2(9321.52,1654.4)));
    int t_rnd = int((params[index].wedgejulia_rN) * prand(p));
    float a = (atan2(vec2(p.y,p.x)) + 2.*PI * t_rnd) / params[index].wedgejulia_power;
    float c = floor((params[index].wedgejulia_count * a + PI) * (1/PI) * 0.5f);
    a = a * params[index].wedgejulia_cf + c * params[index].wedgejulia_angle;
    
    return vec2(r * cos(a), r * sin(a));
}

vec2 rings2(vec2 p, float weight, int index) {
    float r = length2(p);
    float theta = atan2(vec2(p.y,p.x));
    float d = weight*(r - 2.0f * params[index].rings2_val2 * floor( (r+params[index].rings2_val2)/(2.0f * params[index].rings2_val2)) + r * (1.0f - params[index].rings2_val2) );
    //float d = weight*(2.0 - rings2_val2 * ((int) ((r / rings2_val2 + 1) / 2) * 2 / r + 1));
    return vec2(d*sin(theta),d*cos(theta));
  } 

vec2 rotate(vec2 p, float weight, int index) {
	float angle = params[index].rotation_angle;
	float ca = cos(angle);
	float sa = sin(angle);
	return vec2(ca * p.x - sa * p.y, sa * p.x + ca * p.y);
}
  
vec2 cross(vec2 p, float weight, int index) {
    float r = sqrt(1.0f / (sq(sq(p.x)-sq(p.y)))+1e-6f);
    return vec2(weight * 0.8f * p.x * r,weight * 0.8f * p.y * r);
}

vec2 curl(vec2 p, float weight, int index) {
    float re = 1 + params[index].curl_c1 * p.x + params[index].curl_c2 * (sq(p.x)-sq(p.y));
    float im = params[index].curl_c1 * p.y + params[index].curl_c2 * 2 * p.x * p.y;
    float r = weight / (re * re + im * im +eps);
    
    return vec2(r * (p.x * re + p.y * im),r * (p.y * re - p.x * im));
}

vec2 heart(vec2 p, float weight, int index) {
	float r = length2(p);
	float theta = atan2(vec2(p.y,p.x));
	float sinr = sin(r * theta);
	float cosr = cos(r * theta);

	return vec2(weight * r * sinr, -r * weight * cosr); 
}
  
vec2 ex(vec2 p, float weight, int index) {
	float r = length2(p);
	float theta = atan2(vec2(p.x,p.y));
	float xsin = sin(theta + r);
	float ycos = cos(theta - r);
	float x = weight * 0.7f * r * xsin * xsin * xsin;
	float y = weight * 0.7f * r * ycos * ycos * ycos;
	return vec2(x+y,x-y);
}

vec2 popcorn(vec2 p, float weight, int index) {
	float x = p.x + params[index].popcorn_c * sin(tan(3.0f * p.y));
	float y = p.y + params[index].popcorn_f * sin(tan(3.0f * p.x));

	return vec2(weight * 0.85f * x, weight * 0.85f * y);  
}

vec2 waves(vec2 p, float weight, int index) {
    float x = p.x + params[index].waves_b * sin(p.y * (1.0f / (params[index].waves_c * params[index].waves_c + 0.07) ));
    float y = p.y + params[index].waves_e * sin(p.x * (1.0f / (params[index].waves_f * params[index].waves_f + 0.07) ));
        
    return vec2(weight * x, weight * y);  
}

vec2 horseshoe(vec2 p, float weight, int index) {
    float r = weight / (1.25f * (length2(p) + 1e-6f));
    float x = r * ((p.x - p.y) * (p.x + p.y));
    float y = r * 2.0f * p.x * p.y;
    return vec2(x,y);
  }
  
vec2 polar(vec2 p, float weight, int index) {
    float r = length2(p);
    float theta = atan2(vec2(p.x,p.y));
    float x = theta / PI;
    float y = r - 2.0f;
    return vec2(weight * 1.5f * x, weight * 1.5f * y);
}
/*
vec2 bipolar(vec2 p, float weight, int index) {
	float HALF_PI = PI/2.0;

    float r2 = sq(p.x) + sq(p.y);
    float t = r2 + 1.0;
    float x2 = p.x + p.x;
    float y = 0.5 * atan2(p.y+p.y,r2-1.0) + params[index].bipolar_ps;
    
    if(y > HALF_PI) 
      y = -HALF_PI + mod(y + HALF_PI, PI);
    else 
       if(y < -HALF_PI)
        y = HALF_PI - mod(HALF_PI - y, PI); 
    
    float f = t + x2;
    float g = t - x2;
    
    if( (g==0) || (f/g <= 0)) return vec2(p.x,p.y);
    
    return vec2( weight * 0.25 * HALF_PI * log(f/g),
                        weight * HALF_PI * y);
  }  
*/
vec2 ennepers(vec2 p, float weight, int index) {
    float xx = (p.x - ((sq(p.x) * p.x) / 3.0)) + p.x * sq(p.y);
    float yy = (p.y - ((sq(p.y) * p.y) / 3.0)) + p.y * sq(p.x);
    
    return vec2(weight * xx, weight * yy);
  }

vec2 cell(vec2 p, float weight, int index) {
	float cell_size = params[index].cell_size;
	
	float x = floor(p.x/cell_size);
	float y = floor(p.y/cell_size);

	float dx = p.x - x * cell_size;
	float dy = p.y - y * cell_size;


	if (y >= 0) {
	if (x >= 0) {
	y *= 2;
	x *= 2;
	}
	else {
	y *= 2;
	x = -(2 * x + 1);
	}
	}
	else {
	if (x >= 0) {
	y = -(2 * y + 1);
	x *= 2;
	}
	else {
	y = -(2 * y + 1);
	x = -(2 * x + 1);
	}
	}

	return vec2(weight * (dx + x * cell_size-cell_size), -weight * (dy + y * cell_size-cell_size));  
}

vec2 boarders(vec2 p, float weight, int index) {
    float roundx = round(p.x);
    float roundy = round(p.y);
    float offsetx = p.x - roundx;
    float offsety = p.y - roundy;

    float x = offsetx * 0.5 + roundx;
    float y = offsety * 0.5 + roundy;
    
    if(prand(p)<0.75) {
      if(abs(offsetx) >= abs(offsety)) {
        
       if(offsetx >= 0) {
         x += 0.25;
         y += 0.25 * offsety / offsetx; 
       } else {
         x -= 0.25;
         y -= 0.25 * offsety / offsetx; 
       }
        
      } else {
        
        if(offsety >= 0) {
         x += offsetx / offsety * 0.25;
         y += 0.25;
       } else {
         x -= offsetx / offsety * 0.25;
         y -= 0.25; 
       }
        
      }
    }
    return vec2(weight * x, weight * y);
  } 

vec2 supershape(vec2 p, float weight,int index) {
float QUARTER_PI = PI/4.0;
    float theta = params[index].supershape_pm_4 * atan2(p.y,p.x) + QUARTER_PI;
    float t1 = pow(abs(cos(theta)),params[index].supershape_n2);
    float t2 = pow(abs(sin(theta)),params[index].supershape_n3);
    float r = weight * ((params[index].supershape_rnd * prand(p) + (1.0 - params[index].supershape_rnd) * length2(p)) - params[index].supershape_holes) * pow(t1+t2,params[index].supershape_pneg1_n1) / length2(p);
    return vec2(r * p.x, r * p.y);
  }
  
  vec2 loonie(vec2 p, float weight,int index) {
    float r2 = sq(p.x)+sq(p.y);
    float w2 = params[index].loonie_weight * params[index].loonie_weight;
    
    if( (r2 < w2) && r2 != 0) {
      float r = weight * sqrt(w2/r2 -1.0);
      return vec2(r*p.x,r*p.y);
    } else 
      return vec2(p.x,p.y);
  }  

  vec2 foci(vec2 p, float weight,int index) {
    float expx = exp(p.x) * 0.5;
    float expnx = 0.25 / expx;
    float tmp = expx + expnx - cos(p.y) + 1.0e-8;
    float r = weight / tmp;
    return vec2(r * (expx - expnx), r * sin(p.y));
  }  

  vec2 blade(vec2 p, float weight,int index) {
    float a = prand(p) * params[index].blade_weight * length2(p);
    float sa = sin(a);
    float ca = cos(a);
    
    return vec2(weight * 0.8 * p.x * (ca + sa), weight * 0.8 * p.x * (ca - sa));
  }
  
  vec2 rays(vec2 p, float weight,int index) {
    float a = PI * prand(p) * params[index].rays_weight;
    float r = tan(a) * params[index].rays_weight / (length2(p)*length2(p) + 1.0e-8);
    
    return vec2(weight * r * cos(p.x), weight * r * sin(p.y));
  }
  
  vec2 ngon(vec2 p, float weight, int index) {
    float rfactor = pow(sq(p.x)+sq(p.y),params[index].ngon_pow);
    float theta = atan2(p.y,p.x);
    float phi = theta - (params[index].ngon_b * floor(theta/params[index].ngon_b));
    if(phi > params[index].ngon_b/2.0) phi -= params[index].ngon_b;
    
    float amp = 2.0 * weight * (params[index].ngon_corners * (1.0 / (cos(phi) + 1.0e-10) -1.0) + params[index].ngon_circle);
    amp /= (rfactor + 1e-8);
    
    return vec2(p.x * amp,p.y * amp);
  }
  
  vec2 diamond(vec2 p, float weight, int index) {
    float r = length(p);
    float theta = atan2(p.x,p.y);
    float x = weight * 3.0 * sin(theta) * cos(r);
    float y = weight * 3.0 * cos(theta) * sin(r);
    return vec2(x,y);
  }
  
  vec2 oscilloscope(vec2 p, float weight, int index) {
    float t;
    if(params[index].oscilloscope_damping < 0.1)
      t = params[index].oscilloscope_amplitude * cos(params[index].oscilloscope_frequency * p.x) + params[index].oscilloscope_separation;
     else
      t = params[index].oscilloscope_amplitude * exp(-abs(p.x) * params[index].oscilloscope_damping) * cos(params[index].oscilloscope_frequency * p.x) + params[index].oscilloscope_separation;
     
     if(abs(p.y) <= t)
       return vec2(weight*p.x, weight*-p.y);
     else
       return vec2(weight*p.x, weight*p.y); 
  }
  

vec2 oscilloscope2(vec2 p, float weight, int index) {
  float t, pt = params[index].oscilloscope2_perturbation * sin(params[index].oscilloscope2_frequencyy * p.y);
  if(abs(params[index].oscilloscope2_damping)<1.0e-6) {
    t = params[index].oscilloscope2_amplitude * (cos(params[index].oscilloscope2_frequencyx * p.x + pt)) + params[index].oscilloscope2_separation;
  } else {
    t = params[index].oscilloscope2_amplitude * exp(-abs(p.x)*params[index].oscilloscope2_damping) * (cos(params[index].oscilloscope2_frequencyx * p.x + pt)) + params[index].oscilloscope2_separation;
  }
  
  if(abs(p.y)<=t) {
    return vec2(weight * -p.x, weight * -p.y);
  } else {
    return vec2(weight * p.x, weight * p.y);
  }
}

vec2 blocky(vec2 p, float weight, int index) {
	float HALF_PI = PI/2.0;
    float r = weight / ((cos(p.x) + cos(p.y)) / params[index].blocky_mp + 1.0);
    float tmp = sq(p.y) + sq(p.x) + 1.0;
    float x2 = 2.0 * p.x;
    float y2 = 2.0 * p.y;
    float xmax = 0.5 * (sqrt(tmp + x2) + sqrt(tmp - x2));
    float ymax = 0.5 * (sqrt(tmp + y2) + sqrt(tmp - y2));
    float a = p.x / xmax;
    float b = sqrt(max(0.,1.0 - sq(a)));
    float x = params[index].blocky_weight * r*atan2(a,b)*params[index].blocky_x / HALF_PI;
    a = p.y / ymax;
    b = sqrt(max(0.,1.0 - sq(a)));
    float y = params[index].blocky_weight * r * atan2(a,b)*params[index].blocky_y / HALF_PI;
    
    return vec2(3.0*x,3.0*y);
  }
  
vec2 waves2(vec2 p, float weight, int index) {
    float x = weight * (p.x + params[index].waves2_scalex * sin(p.y * params[index].waves2_freqx));
    float y = weight * (p.y + params[index].waves2_scaley * sin(p.x * params[index].waves2_freqy));
    return vec2(x,y);
  }
  
  vec2 blob(vec2 p, float weight, int index) {
    float theta = atan2(p.x,p.y);
    float r = 0.86 * length(p);
    r = weight * r * (params[index].blob_low + params[index].blob_bdiff * 0.5 + 0.5 * sin(params[index].blob_waves * theta));
    return vec2(r * sin(theta), r * cos(theta));
  }

vec2 bsplit(vec2 p, float weight, int index) {
   if(p.x+params[index].bsplit_shiftx == 0 || p.x + params[index].bsplit_shiftx == PI) {
     return vec2(weight * p.x,weight * p.y);
   } else {
     float xx = weight / tan(p.x+params[index].bsplit_shiftx) * cos(p.y + params[index].bsplit_shifty);
     float yy = weight / sin(p.x+params[index].bsplit_shiftx) * (-1 * p.y + params[index].bsplit_shifty);
     return vec2(xx,yy);
   }
 }
 
vec2 bswirl(vec2 p, float weight, int index) {
    float tau = 0.5 * (log(sq(p.x+1.0) + sq(p.y)) - log(sq(p.x - 1.0) + sq(p.y)));
    float sigma = PI - atan2(p.y, p.x+1.0) - atan2(p.y, 1.0 - p.x);
    sigma = sigma + tau * params[index].bswirl_out + params[index].bswirl_in / tau;
    float temp = cosh(tau) - cos(sigma);  
    return vec2(weight * sinh(tau) / temp, weight * sin(sigma) / temp);
  }

vec2 bwraps7(vec2 p, float weight, int index) {
    if(abs(params[index].bwraps7_cellsize) < 1e-10) 
      return vec2(weight * p.x, weight * p.y);
     
    float cx = (floor(p.x / params[index].bwraps7_cellsize) + 0.5) * params[index].bwraps7_cellsize;
    float cy = (floor(p.y / params[index].bwraps7_cellsize) + 0.5) * params[index].bwraps7_cellsize;
    float lx = p.x - cx;
    float ly = p.y - cy;
    if( (sq(lx) + sq(ly)) > params[index].bwraps7_r2)
        return vec2(weight * p.x, weight * p.y);
    
    lx *= params[index].bwraps7_g2;
    ly *= params[index].bwraps7_g2;
    float r = params[index].bwraps7_rfactor / ((sq(lx) + sq(ly))/ 4.0 + 1.0);
    lx *= r;
    ly *= r;
    
    r = (sq(lx) + sq(ly)) / params[index].bwraps7_r2;
    float theta = params[index].bwraps7_inner_twist * (1.0 - r) + params[index].bwraps7_outer_twist * r;
    float s = sin(theta);
    float c = cos(theta);
    
    float x = cx + c * lx + s * ly;
    float y = cy - s * lx + c * ly;
    
    return vec2(weight * x, weight * y);
  }

/*
vec2 blur(vec2 p, float weight, int index) {
    float a = TWO_PI * prand(p);
    float r = weight * 3.0 * prand(0.1911157*p);
    return vec2(r*cos(a),r*sin(a));
  }
  */
  
vec2 exponential(vec2 p, float weight, int index) {
    float r = PI * p.y;
    float sinr = sin(r);
    float cosr = cos(r);
    float d = weight * exp(p.x - 1);      
    return vec2(d * cosr, d * sinr);  
  }
  
vec2 blurzoom(vec2 p, float weight, int index) {
    float z = 1.0 + params[index].blurzoom_length * prand(p);
    float x = weight * ((p.x - params[index].blurzoom_x) * z + params[index].blurzoom_x);   
    float y = weight * ((p.y - params[index].blurzoom_y) * z + params[index].blurzoom_y);
    return vec2(x,y);
  }

vec2 bmod(vec2 p, float weight, int index) {
    float tau = 0.5 * (log(sq(p.x+1.0) + sq(p.y)) - log(sq(p.x - 1.0) + sq(p.y)));
    float sigma = PI - atan2(p.y, p.x+1.0) - atan2(p.y, 1.0 - p.x);
    if(tau < params[index].bmod_radius && -tau < params[index].bmod_radius)
    {
      float m = (2.0 * params[index].bmod_radius);
      float input = (tau + params[index].bmod_radius + params[index].bmod_distance * params[index].bmod_radius);
      float res0 = mod(input,m) - params[index].bmod_radius;
      if(input<0)
      {
      	tau = res0-m;
      }
      else
      {
      	tau = res0;
      }
    }
    float temp = cosh(tau) - cos(sigma);  
    return vec2(weight * sinh(tau) / temp, weight * sin(sigma) / temp);
  }

vec2 bcollide(vec2 p, float weight, int index) {
    float tau = 0.5 * (log(sq(p.x+1.0) + sq(p.y)) - log(sq(p.x - 1.0) + sq(p.y)));
    float sigma = PI - atan2(p.y, p.x+1.0) - atan2(p.y, 1.0 - p.x);
    int alt = int(floor(sigma * params[index].bcollide_bcn_pi)); // round instead?
    if(alt % 2 == 0)
    {
      float m = params[index].bcollide_pi_bcn;
      float input = alt * params[index].bcollide_pi_bcn + (sigma + params[index].bcollide_bca_bcn);
      float res0 = mod(input,m);
      if(input<0)
      {
      	sigma = res0-m;
      }
      else
      {
      	sigma = res0;
      }
    }
    else
    {
	float m = params[index].bcollide_pi_bcn;
	float input = alt * params[index].bcollide_pi_bcn + (sigma - params[index].bcollide_bca_bcn);
	float res0 = mod(input,m);
	if(input<0)
	{
		sigma = res0-m;
	}
	else
	{
		sigma = res0;
	}
    }
    float temp = cosh(tau) - cos(sigma);  
    return vec2(weight * sinh(tau) / temp, weight * sin(sigma) / temp);
  }

vec2 boarders2(vec2 p, float weight, int index) {
    float roundx = round(p.x);
    float roundy = round(p.y);
    float offsetx = p.x - roundx;
    float offsety = p.y - roundy;
    float x,y;
    if(prand(p) >= params[index].boarders2_cr) {
      x = offsetx * params[index].boarders2_c + roundx;
      y = offsety * params[index].boarders2_c + roundy;
    } else {
      if(abs(offsetx) >= abs(offsety)) {
        if(offsetx >= 0.0) {
          x = offsetx * params[index].boarders2_c + roundx + params[index].boarders2_cl;
          y = offsety * params[index].boarders2_c + roundy + params[index].boarders2_cl * offsety/offsetx;
        } else {
          x = offsetx * params[index].boarders2_c + roundx - params[index].boarders2_cl;
          y = offsety * params[index].boarders2_c + roundy - params[index].boarders2_cl * offsety/offsetx;
        }
      } else {
        if(offsety >= 0.0) {
          y = offsety * params[index].boarders2_c + roundy + params[index].boarders2_cl;
          x = offsetx * params[index].boarders2_c + roundx + offsetx/offsety * params[index].boarders2_cl;
        } else {
          y = offsety * params[index].boarders2_c + roundy - params[index].boarders2_cl;
          x = offsetx * params[index].boarders2_c + roundx - offsetx/offsety * params[index].boarders2_cl;
        }
      }
    }
    
    return vec2(weight * x, weight * y);
   }

vec2 eyefish(vec2 p, float weight, int index) {
    float r = weight * 4.0  / (length(p) + 1.0) ;
    return vec2(r * p.x, r * p.y);  
  }

vec2 disc(vec2 p, float weight, int index) {
    float r = PI*length(p);
    float theta = 3.0 * weight * atan2(p.x,p.y) / PI;
    float x = theta * sin(r);
    float y = theta * cos(r);
    return vec2(x,y);
  }

vec2 cot(vec2 p, float weight, int index) {
    float d = weight / (1.0e-10 + cosh(2.0*p.y) - cos(2.0 * p.x));
    return vec2(d * sin(2.0 * p.x), -d * sinh(2.0 * p.y));
  }
  
vec2 julia(vec2 p, float weight, int index) {
    float r = weight * 1.75 * sqrt(length(p));
    float theta = 0.5 * atan2(p.x,p.y) + int(2.0 * prand(0.2*p)) * PI;
    float x = r * cos(theta);
    float y = r * sin(theta);
    return vec2(x,y);
  } 

vec2 spiral(vec2 p, float weight, int index) {
    float r = length(p) + 1.0e-10;
    float theta = atan2(p.x,p.y);
    float x = weight * 2.0 * (cos(theta) + sin(r)) / r;
    float y = weight * 2.0 * (sin(theta) - cos(r)) / r;
    return vec2(x,y);
  }

vec2 tangent(vec2 p, float weight, int index) {
    return vec2(weight * sin(p.x) / (cos(p.y)+1.0e-10), weight * tan(p.y));
  }  
/*
vec2 lazysuzan(vec2 p, float weight, int index) {
    float xx = p.x - params[index].lazysuzan_x;
    float yy = p.y - params[index].lazysuzan_y;
    float rr = sqrt(sq(xx)+sq(yy));
    
    if(rr < params[index].lazysuzan_weight) {
      float a = atan2(yy,xx) + params[index].lazysuzan_spin + params[index].lazysuzan_twist * (params[index].lazysuzan_weight - rr);
      rr = weight * rr;
      return vec2(rr * cos(a) + params[index].lazysuzan_x, rr * sin(a) - params[index].lazysuzan_y);
    } else {
      rr = weight * (1.0 + params[index].lazysuzan_space / rr);
      return vec2(rr * xx + params[index].lazysuzan_x, rr * yy - params[index].lazysuzan_y);
    }
  }
  
vec2 glynnsim1_circle(vec2 p, int index) {
    float r = params[index].glynnsim1_radius1 * (params[index].glynnsim1_thickness + (1.0 - params[index].glynnsim1_thickness) * prand(0.1*p));
    float phi = TWO_PI * prand(0.1348621*p);
    return vec2(r * cos(phi) + params[index].glynnsim1_x1, r * sin(phi) + params[index].glynnsim1_y1);
  }

  vec2 glynnsim1(vec2 p, float weight, int index) {
    float r = length(p);
    if(r < params[index].glynnsim1_radius) {
      vec2 toolpoint = glynnsim1_circle(p,index);
      toolpoint = weight * toolpoint;
      return toolpoint;
    } else {
      float alpha = params[index].glynnsim1_radius / r;
      float x = p.x;
      float y = p.y;
      if(prand(0.084348621*p) <= params[index].glynnsim1_contrast * pow(alpha, params[index].glynnsim1_abspow)) {
        float tmp = sq(alpha);
        x = tmp * p.x;
        y = tmp * p.y;
      }
      float z = sq(x - params[index].glynnsim1_x1) + sq(y - params[index].glynnsim1_y1);
      if(z < sq(params[index].glynnsim1_radius1)) {
        vec2 toolpoint = glynnsim1_circle(p,index);
        toolpoint = weight * toolpoint;
        return toolpoint;
      } else {
        return vec2(weight * x, weight * y);
      }
    }
  }
  
  */
  
vec2 rings(vec2 p, float weight, int index) {
    float r = length(p);
    float theta = atan2(p.x,p.y);
    float r2 = weight * ( mod(r + params[index].rings_c2,2.0*params[index].rings_c2) + r * (1.0 - params[index].rings_c2) );
    float x = r2 * cos(theta);
    float y = r2 * sin(theta); 
    return vec2(x,y); 
  }
  /*
vec2 escher(vec2 p, float weight, int index) {
    float a = atan2(p.y, p.x);
    float lnr = 0.5 * log(length(p));
    float m = weight * exp(params[index].escher_vc * lnr - params[index].escher_vd * a);
    float n = params[index].escher_vc * a + params[index].escher_vd * lnr;
    return vec2(m * cos(n), m * sin(n));
  }
  */
vec2 radialblur(vec2 p, float weight, int index) {
    float rndG = prand(0.19321654987*p) + prand(0.178321654987*p) + prand(0.1538321654987*p) + prand(0.132383214987*p) - 2.0;
    float a = atan2(p.y,p.x) + params[index].radialblur_spinvar * rndG;
    float r = length(p);
    float rz = params[index].radialblur_zoomvar * rndG - 1.0;
    float x = r * cos(a) + rz * p.x;
    float y = r * sin(a) + rz * p.y;
    return vec2(weight * x, weight * y);
  }
  
vec2 split(vec2 p, float weight, int index) {
    float x,y;
    if(cos(p.x*params[index].split_xsize * PI) >= 0)
      y = weight * p.y;
     else
      y = -weight * p.y;
    if(cos(p.y*params[index].split_ysize * PI) >= 0)
      x = weight * p.x;
     else
      x = -weight * p.x;
    return vec2(x,y);  
  } 
//////////////////////////////////


vec2 rotate(vec2 v, float a) {
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return m * v;
}

vec2 applyFold(vec2 p,int type,int index)
{
	vec2 result;
	switch (type) {
		case 0:
			result = lines(p, params[index].weight, index);
		break;
		case 1:
			result = twintrian(p, params[index].weight, index);
		break;
		case 2:
			result = julian(p, params[index].weight, index);
		break;
		case 3:
			result = juliascope(p, params[index].weight, index);
		break;
		case 4:
			result = rectangles(p, params[index].weight, index);
		break;
		case 5:
			result = sinusoidal(p, params[index].weight, index);
		break;
		case 6:
			result = scry(p, params[index].weight, index);
		break;
		case 7:
			result = fan2(p, params[index].weight, index);
		break;
		case 8:
			result = pdj(p, params[index].weight, index);
		break;
		case 9:
			result = butterfly(p, params[index].weight, index);
		break;
		case 10:
			result = wedgejulia(p, params[index].weight, index);
		break;
		case 11:
			result = rotate(p, params[index].weight, index);
		break;
		case 12:
			result = cross(p, params[index].weight, index);
		break;
		case 13:
			result = curl(p, params[index].weight, index);
		break;
		case 14:
			result = rings2(p, params[index].weight, index);
		break;
		case 15:
			result = heart(p, params[index].weight, index);
		break;
		case 16:
			result = ex(p, params[index].weight, index);
		break;
		case 17:
			result = popcorn(p, params[index].weight, index);
		break;
		case 18:
			result = waves(p, params[index].weight, index);
		break;
		case 19:
			result = horseshoe(p, params[index].weight, index);
		break;
		case 20:
			result = polar(p, params[index].weight, index);
		break;
		case 21:
			result = ennepers(p, params[index].weight, index);
		break;
		case 22:
			result = cell(p, params[index].weight, index);
		break;
		case 23:
			result = boarders(p, params[index].weight, index);
		break;
		case 24:
			result = supershape(p, params[index].weight, index);
		break;
		case 25:
			result = loonie(p, params[index].weight, index);
		break;
		case 26:
			result = foci(p, params[index].weight, index);
		break;
		case 27:
			result = blade(p, params[index].weight, index);
		break;
		case 28:
			result = ngon(p, params[index].weight, index);
		break;
		case 29:
			result = diamond(p, params[index].weight, index);
		break;
		case 30:
			result = oscilloscope(p, params[index].weight, index);
		break;
		case 31:
			result = oscilloscope2(p, params[index].weight, index);
		break;
		case 32:
			result = blocky(p, params[index].weight, index);
		break;
		case 33:
			result = waves2(p, params[index].weight, index);
		break;
		case 34:
			result = blob(p, params[index].weight, index);
		break;
		case 35:
			result = bsplit(p, params[index].weight, index);
		break;
		case 36:
			result = bswirl(p, params[index].weight, index);
		break;
		case 37:
			result = bwraps7(p, params[index].weight, index);
		break;
		case 38:
			result = exponential(p, params[index].weight, index);
		break;
		case 39:
			result = blurzoom(p, params[index].weight, index);
		break;
		case 40:
			result = bmod(p, params[index].weight, index);
		break;
		case 41:
			result = bcollide(p, params[index].weight, index);
		break;
		case 42:
			result = boarders2(p, params[index].weight, index);
		break;
		case 43:
			result = eyefish(p, params[index].weight, index);
		break;
		case 44:
			result = disc(p, params[index].weight, index);
		break;
		case 45:
			result = cot(p, params[index].weight, index);
		break;
		case 46:
			result = julia(p, params[index].weight, index);
		break;
		case 47:
			result = spiral(p, params[index].weight, index);
		break;
		case 48:
			result = tangent(p, params[index].weight, index);
		break;
		case 49:
			result = rings(p, params[index].weight, index);
		break;
		case 50:
			result = split(p, params[index].weight, index);
		break;
	}
	if(index==aIndex && aState==1) result += aRadius*vec2(cos(aAngle),sin(aAngle));
	//if(index==aIndex && aState==1) result = rotate(result, aAngle);
	if(params[index].controlledRotationAngle!=0) result = rotate(result,params[index].controlledRotationAngle);
	return result + vec2(params[index].tx,params[index].ty);
}

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main(){
	float px = float(gl_GlobalInvocationID.x)/float(nx);
	float py = float(gl_GlobalInvocationID.y)/float(ny);
	
	float w0 = 3.; // size of initial square
	
	vec2 inputPos = vec2((2.*px-1.)*w0,(2.*py-1.)*w0);
	
	float ga0 = 0.09;
	float gnx0 = ga0*gaussian_rand(vec2(gl_GlobalInvocationID)*0.9945+523.921);
	float gny0 = ga0*gaussian_rand(vec2(gl_GlobalInvocationID)*0.6045+9654.287);
	vec2 gn0 = vec2(gnx0,gny0);
	
	vec2 curPos = inputPos + gn0;
	
	vec2 target = vec2(0,0);
	
	float factor = 1;
	
	if(threeD==0)
	{
	
		if(operationsMode==0)
		{
			for(int i=0;i<sequenceLength;i++)
			{
				curPos = applyFold(curPos, params[i].foldType, i);
			}
		}
		else if(operationsMode==1)
		{	
			
			for(int i=0;i<sequenceLength-2;i++)
			{
				curPos = applyFold(curPos, params[i].foldType, i);
			}
			if(sequenceLength>1)
			{
				curPos = applyFold(curPos, params[sequenceLength-2].foldType, sequenceLength-2)+applyFold(curPos, params[sequenceLength-1].foldType, sequenceLength-1);
				curPos /= 1.8;
			}
			else
			{
				curPos = applyFold(curPos, params[sequenceLength-1].foldType, sequenceLength-1);
			}
			
			/*
			for(int i=0;i<floor(sequenceLength/2);i++)
			{
				curPos = applyFold(curPos, params[2*i].foldType, 2*i)+applyFold(curPos, params[2*i+1].foldType, 2*i+1);
				curPos /= 1.8;
			}
			if(sequenceLength%2==1)
			{
				curPos = applyFold(curPos, params[sequenceLength-1].foldType, sequenceLength-1);
			}*/
		}
		else if(operationsMode==2)
		{

			for(int i=0;i<sequenceLength-3;i++)
			{
				curPos = applyFold(curPos, params[i].foldType, i);
			}
			if(sequenceLength>2)
			{
				curPos = applyFold(curPos, params[sequenceLength-3].foldType, sequenceLength-2)+applyFold(curPos, params[sequenceLength-2].foldType, sequenceLength-2);
				curPos /= 1.8;
				curPos = applyFold(curPos, params[sequenceLength-1].foldType, sequenceLength-1);
			}
			else if(sequenceLength>1)
			{
				curPos = applyFold(curPos, params[sequenceLength-2].foldType, sequenceLength-1);
				curPos = applyFold(curPos, params[sequenceLength-1].foldType, sequenceLength-1);
			}
			else
			{
				curPos = applyFold(curPos, params[sequenceLength-1].foldType, sequenceLength-1);
			}
		}
		else if(operationsMode==3)
		{
			for(int i=0;i<sequenceLength;i++)
			{
				curPos = applyFold(curPos, params[i].foldType, i);
				float a = floor(5.*prand(inputPos))/4.;
				float sf = 0.8+0.4*a;
				if(i<sequenceLength-1) curPos *= sf;
			}/*
			for(int i=0;i<sequenceLength;i++)
			{
				curPos = applyFold(curPos, params[i].foldType, i);
			}*/
		}
		
		if(doSinusoid==1)
		{
			curPos = sinusoidal(curPos*sinusoidStretch, 1.0, 0);
		}
		if(doSinusoid==2)
		{
			curPos = tanhBounder(curPos*sinusoidStretch*2.0, 1.0, 0);
		}
		
		vec2 outputPos = curPos;
		
		float lp = min(1.0,max(abs(inputPos.x),abs(inputPos.y))/3.0);
		
		if(distortionMode==1) outputPos = mix(outputPos,inputPos,0.5*lp+0.5);
		else if(distortionMode==2) outputPos = mix(outputPos,inputPos,0.92); 
		
		target = width/2. + outputPos*width/w0*0.43; // line to change for margin
	
	}
	else if(threeD==1)
	{
		vec2 curPos1 = curPos;
		vec2 curPos2 = curPos;
		for(int i=0;i<sequenceLength/2;i++)
		{
			curPos1 = applyFold(curPos1, params[i].foldType, i);
		}
		for(int i=sequenceLength/2;i<sequenceLength;i++)
		{
			curPos2 = applyFold(curPos2, params[i].foldType, i);
		}

		float xx,yy,zz,tt;

		if(projectionIndex==0)
		{
			tt = curPos1.x;
			xx = curPos1.y;
			yy = curPos2.x;
			zz = curPos2.y;
		}
		else if(projectionIndex==1)
		{
			xx = curPos1.x;
			tt = curPos1.y;
			yy = curPos2.x;
			zz = curPos2.y;
		}
		else if(projectionIndex==2)
		{
			xx = curPos1.x;
			yy = curPos1.y;
			tt = curPos2.x;
			zz = curPos2.y;
		}
		else if(projectionIndex==3)
		{
			xx = curPos1.x;
			yy = curPos1.y;
			zz = curPos2.x;
			tt = curPos2.y;
		}
		
		vec3 curPos3d;
		bool giveUp = false;
		
		if(projDist-tt>0)
		{
			curPos3d = 1.7*vec3(xx,yy,zz)/(projDist-tt);
		}
		else
		{
			curPos3d = vec3(xx,yy+zz,tt);
			giveUp = true;
		}
		
		curPos3d.xy = rotate(curPos3d.xy, rot1);
		curPos3d.xz = rotate(curPos3d.xz, rot2);
		curPos3d.yz = rotate(curPos3d.yz, rot3);
		
		if(false && doSinusoid==1)
		{
		
			float len = length(curPos3d);
			float len2 = mix(len,1.6,0.8);
			curPos3d *= len2/len;
		}
		else
		{
			curPos3d *= 1.0;
		}
		
		float dz = 2.6;
		
		float dof = width/2.7;
		
		if(dz-curPos3d.z>0 && !giveUp)
		{
			float projx = width/2.0 + dof*curPos3d.x/(dz-curPos3d.z);
			float projy = height/2.0 + dof*curPos3d.y/(dz-curPos3d.z);
			target = vec2(projx,projy);
		}
		else
		{
			target = vec2(2*width,2*height);
		}
		
		factor = max(0.9,2.0*dz/(dz-curPos3d.z));
	}

// gaussian noise attempt :

	float ga = 3.0;
	float gnx = ga*gaussian_rand(vec2(gl_GlobalInvocationID)*0.9945+123.321);
	float gny = ga*gaussian_rand(vec2(gl_GlobalInvocationID)*0.6045+654.987);
	vec2 gn = vec2(gnx,gny);
	
	ivec2 iTarget = ivec2(target+gn);
	
	for(int i=-1;i<=1;i++)
	for(int j=-1;j<=1;j++)
	{
		if(i!=0||j!=0) continue;
		if(iTarget.x+i>=width||iTarget.x+i<0||iTarget.y+j>=height||iTarget.y+j<0) continue;
		ivec2 pos = ivec2(LoopedPosition(vec2(iTarget.x+i,iTarget.y+j)));
		uint val;
		val = uint(3-abs(i)-abs(j));
		if(threeD==1) val = uint((3-abs(i)-abs(j))*factor);
		atomicAdd(m[ pos.x * int(height) + pos.y ],val);
	}
}
