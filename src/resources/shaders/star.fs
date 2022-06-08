//TUNING PARAMETERS HAS MADE ME INSANE. PLS APRECIATE THE WORK XD


#version 410

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};
// Input lighting values
uniform Light lights[4];
// uniform vec4 ambient;
uniform vec3 viewPos;
// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

#define M_PI 3.1415926535897932384626433832795

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 hash33(vec3 p3){
	p3 = fract(p3 * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yxz+33.33);
    return fract((p3.xxy + p3.yxx)*p3.zyx);
}

float starField(vec3 rd){
    rd*=200.;
    float col=0.;
    for(int i=0;i<4;i++){
        vec3 CellUVs=floor(rd+float(i*1199));
        vec3 hash=(hash33(CellUVs)*2.-1.)*.8;
        float hash_magnitude =(1.0-length(hash));
        vec3 UVgrid=fract(rd)-.5;
        float radius=clamp(hash_magnitude-.5,0.,1.);
        float radialGradient=length(UVgrid-hash)/radius;
        radialGradient=clamp(1.-radialGradient,0.,1.);
        radialGradient*=radialGradient;
        col += radialGradient;
    }
    return col;
}

struct ray{
	vec3 ro;
    vec3 rd;
};

float suc(ray r,vec3 center,float radius){// sun glare
    vec3 o=r.ro-center;
    float a=dot(o,o);
    float b=dot(r.rd,o);
    float c=dot(r.rd,r.rd);
    float k=sqrt(a*c-b*b);
    float integ=radius*(M_PI/2.-atan(b,k))/k/3.;
    integ*=integ;
    integ*=integ;//not integral anymore but looks great
    return integ;
}

uniform float frame;
uniform vec2 resolution;

void main()
{
	float size = 50.0;
	float prob = 0.95;
	
    vec3 pos = fragPosition;
    vec3 viewD = normalize(viewPos - fragPosition);

	float color = 0.0;

    finalColor = vec4(0,0,0,1);
    
    color = starField(-viewD);
    color = color * (0.25 * sin(frame * (color * 5.0) + 720.0 * color) + 0.75);

    float sunInt = 1;    
    
    finalColor.rgb+=clamp(0.,1000.,suc(ray(pos, -viewD),vec3(-500,0,-100),3))*sunInt;
	
	finalColor.rgb += color;
}