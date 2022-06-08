#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

#define     MAX_LIGHTS              4
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct MaterialProperty {
    vec3 color;
    int useSampler;
    sampler2D sampler;
};

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
// uniform vec4 ambient;
uniform vec3 viewPos;

const vec4 AmbientProduct = vec4(0.2, 0.2, 0.2, 1.0)*vec4(0.2, 0.2, 1.0, 1.0);
const vec4 DiffuseProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.8, 0.8, 0.8, 1.0);
const vec4 SpecularProduct = vec4(1.0, 1.0, 1.0, 1.0)*vec4(0.1, 0.1, 0.1, 1.0);
const float Shininess = 1.0;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);

    // NOTE: Implement here your fragment shader code
    vec3 light = vec3(0.0);

    light = normalize(lights[0].position - fragPosition);
    
    float Kd = max(dot(light, normal), 0.0);
    float Ks = pow(max(dot(normal, light - viewD), 0.0), Shininess);
    
    vec3 ambient = AmbientProduct.xyz;
    vec3 diffuse = Kd * DiffuseProduct.xyz;
    specular = Ks * SpecularProduct.xyz;
    

    float NdotL = max(dot(normal, light), 0.0);
    lightDot += lights[0].color.rgb*NdotL;

    if( dot(light, normal) < 0.0 ) {
		specular = vec3(0.0, 0.0, 0.0);
    }
    
    finalColor = texelColor * vec4(ambient + diffuse + specular, 1.0);

    // Gamma correction
    finalColor = pow(finalColor, vec4(1.0/2.2));
}