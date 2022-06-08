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
uniform vec2 resolution;
uniform float frame;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

#define SEE_ATMOSPHERE
#define CRAPPY_AA
// #define EXPERIMENTAL_PLANET_SHADOW

struct ray{
	vec3 ro;
    vec3 rd;
};
struct sph{
    vec3 c;
	float r;
};

const vec3 center=vec3(0);

const int VIEW_SAMPLES=10; //25

const int DEPTH_SAMPLES=4; //8

const sph atm=sph(center,1.0);
const vec3 sunInt=vec3(1.);
const sph earth=sph(center,0.9);
const vec3 waves=700./vec3(700,510,440);
const vec3 RGBScatter=waves*waves*waves*waves;
const float scatterStrength=50.;
const float densFall=7.;

float dot2(vec3 k){return dot(k,k);}
const float pi=4.*atan(1.);

float phase(float cosTheta) {
	return (1. + cosTheta * cosTheta) / (16.0 * pi);
}

float sphere(ray r, sph s, float n){//sphere ray intersection
	vec3 rc = r.ro-s.c;
	float c = dot(rc, rc) - (s.r*s.r);
	float b = dot(r.rd, rc);
	float d = b*b - c;
	float t = -b - n*sqrt(abs(d));
	float st = step(0.0, min(t,d));
	return max(0.,mix(-1., t, st));
}

float suc(ray r,vec3 center,float radius){// sun glare
    vec3 o=r.ro-center;
    float a=dot(o,o);
    float b=dot(r.rd,o);
    float c=dot(r.rd,r.rd);
    float k=sqrt(a*c-b*b);
    float integ=radius*(pi/2.-atan(b,k))/k/3.;
    integ*=integ;
    integ*=integ;//not integral anymore but looks great
    return integ;
}

float density(vec3 p){
    float l=distance(p,center)-earth.r;
    l/=atm.r-earth.r;
    return exp(-l*densFall)*(1.-l);
}

float depth(vec3 s,vec3 e){
    float d=0.;
    for(int i=0;i<DEPTH_SAMPLES;i++){
        vec3 p=s+(e-s)*float(i)/float(DEPTH_SAMPLES);
        d+=density(p);
    }
    return d/float(DEPTH_SAMPLES)*distance(s,e);
}

vec3 light(ray r,vec3 sunPos,vec3 orig){
    if(distance(r.ro,center)<atm.r){
    }else{
        float d1=sphere(r,atm,1.);
        if(d1==0.)return orig;
        r.ro+=r.rd*d1;
    }
    float d2=sphere(r,earth,1.);
    if(d2==0.)d2=sphere(r,atm,-1.);
    float viewDepth=0.;
    vec3 l=vec3(0.);
    for(int i=0;i<VIEW_SAMPLES;i++){
        vec3 p=r.ro+r.rd*(float(i)+0.5)/float(VIEW_SAMPLES+1)*d2;
        ray k=ray(p,normalize(sunPos-p));
        #ifdef EXPERIMENTAL_PLANET_SHADOW
        if(sphere(k,earth,1.)==0.){
        #endif
            float sunDepth=depth(k.ro,k.ro+k.rd*sphere(k,atm,-1.));
            viewDepth=depth(r.ro,p);
            vec3 transmitance=exp(-(sunDepth+viewDepth)*RGBScatter);
        
        
            l+=transmitance*density(p)*phase(dot(r.rd,normalize(sunPos-p)));
        #ifdef EXPERIMENTAL_PLANET_SHADOW
        }
        #endif
    }
    vec3 origTransmitance=exp(-viewDepth*RGBScatter);
    return orig*origTransmitance+l/float(VIEW_SAMPLES)*d2*sunInt*RGBScatter*scatterStrength;
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

const int star_iterations = 10;
const vec4 AmbientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
const vec4 DiffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
const vec4 SpecularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.1, 0.1, 0.1, 1.0);
const float Shininess = 1.0;

void main( ){
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);
    
    finalColor=vec4(0);
    ray r=ray(viewPos,-viewD);
    sph sun=sph(vec3(-500,0,-100),3.);
    finalColor.rgb+=starField(r.rd);
    finalColor.rgb*=(0.25 * sin(frame * (finalColor.rgb * 5.0) + 720.0 * finalColor.rgb) + 0.75);
    finalColor.rgb+=clamp(0.,1000.,suc(r,sun.c,sun.r))*sunInt;
 
    float d=sphere(r,earth,1.);

    // NOTE: Implement here your fragment shader code
    vec3 l = vec3(0.0);

    l = normalize(lights[0].position - fragPosition);
    
    float Kd = max(dot(l, normal), 0.0);
    float Ks = pow(max(dot(normal, l - viewD), 0.0), Shininess);
    
    vec3 ambient = AmbientProduct.xyz;
    vec3 diffuse = Kd * DiffuseProduct.xyz;
    specular = Ks * SpecularProduct.xyz;

    if( dot(l, normal) < 0.0 ) {
		specular = vec3(0.0, 0.0, 0.0);
    }
    
    if(d>0.){
        vec3 planet;
        planet = texelColor.rgb;
        
        #ifdef CRAPPY_AA
        //cheap antialiasing
        float z=length(r.ro-earth.c);
        float w=z-earth.r;
        z=sqrt(z*z-earth.r*earth.r);//distance from cam to edge of planet(the point where it aliases)
        z=(d-z)/(w-z);//1 on the closest point of the sphere to the carmera 0 on the furthest
        z=smoothstep(10./resolution.y,90./resolution.y,z);
        finalColor.rgb=mix(finalColor.rgb,planet,z);
        finalColor = texelColor * vec4(ambient + diffuse + specular, 1.0);
        #else
        finalColor = texelColor * vec4(ambient + diffuse + specular, 1.0);
        #endif
    }
    #ifdef SEE_ATMOSPHERE
    finalColor=vec4(light(r,sun.c,finalColor.rgb),1.);
    #endif

    // finalColor = pow(finalColor, vec4(1.0/2.2));
}