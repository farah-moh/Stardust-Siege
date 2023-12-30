#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} fsin;

#define TYPE_DIRECTIONAL    0
#define TYPE_POINT          1
#define TYPE_SPOT           2

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 emissive;
    float shininess;
};

struct Light {
    int type;
    //diffuse, specular and ambient are properties that are used by all light types
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    //position and attenutation is useful for point and spot lights while direction is useful only for the latter
    vec3 position, direction;
    //(c2, c1, c0) which are factors for d^2, d, 1 where d is the distance from the camera to the point
    vec3 attenuation;
    //(outer_angle, inner_angle)
    //Light is at full intensity within inner_angle, is attenuated when between the two angles
    // and is 0 outside outer_angle
    vec2 cone;
};

struct LitMaterial {
    sampler2D albedo;
    sampler2D specular;
    sampler2D roughness;
    sampler2D emissive;
    sampler2D ambient_occlusion;
};

uniform LitMaterial tex_material;
#define MAX_LIGHT_COUNT 30
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;
out vec4 frag_color;

void main() {
    Material sampled;

    sampled.diffuse = texture(tex_material.albedo, fsin.tex_coord).rgb;      // Albedo is used to sample the diffuse
    sampled.specular = texture(tex_material.specular, fsin.tex_coord).rgb;
    sampled.emissive = texture(tex_material.emissive, fsin.tex_coord).rgb;
    //For corners to be darker
    sampled.ambient = sampled.diffuse * texture(tex_material.ambient_occlusion, fsin.tex_coord).r;
    // Ambient is computed by multiplying the diffuse by the ambient occlusion factor. This allows occluded crevices to look darker.

    // Roughness is used to compute the shininess (specular power).
    float roughness =  texture(tex_material.roughness, fsin.tex_coord).r;

    // It is noteworthy that we clamp the roughness to prevent its value from ever becoming 0 or 1 to prevent lighting artifacts (unexpected behavior).
    sampled.shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;

    // Then we normalize the normal and the view. These are done once and reused for every light type.
    vec3 normal = normalize(fsin.normal); // Although the normal was already normalized, it may become shorter during interpolation.
    vec3 view = normalize(fsin.view);
    int count = min(light_count, MAX_LIGHT_COUNT);

    vec3 accumulated_light = vec3(0.0f);

        //for each light in the scene, we compute it's effect on the pixel
    for(int index = 0; index < count; index++){
        Light light = lights[index];
        vec3 light_direction;
        float attenuation = 1;
        if(light.type == TYPE_DIRECTIONAL)
            //here the direction of the light is simply the dirction of the directional light
            light_direction = light.direction;
        else {
            //but for spot and point lights, the light is emitting radially outwards from the light position
            light_direction = fsin.world - light.position;
            float distance = length(light_direction);
            //normalizing the light vector so that the angle can be computed later without having to worry about vector length
            light_direction /= distance;
            //attenuate based on how far the point is from the light and the attenuation factors of the light
            attenuation *= 1.0f / dot(light.attenuation, vec3(distance * distance, distance, 1));
            if(light.type == TYPE_SPOT){
                //compute the angle between the direction of the cone center "light.direction" and the 
                // direction from the light to the point of interest "light_direction"
                float angle = acos(dot(light.direction, light_direction));
                //attentuate further based on how far from the cone center the point is
                // light.cone.x: outer_angle, light.cone.y: inner_angle
                attenuation *= smoothstep(light.cone.x, light.cone.y, angle);
            }
        }
        //compute the reflected light vector
        vec3 reflected = reflect(light_direction, normal);
        //negative is taken as normal is pointing away from the object but light is pointing towards
        float lambert = max(0.0f, dot(normal, -light_direction));
        float phong = pow(max(0.0f, dot(view, reflected)), sampled.shininess);

        vec3 diffuse = sampled.diffuse * light.diffuse * lambert;
        vec3 specular = sampled.specular * light.specular * phong;
        vec3 ambient = sampled.ambient * light.ambient;
        //add the effect of this light to the effects of all prior lights and account for attenuation
        accumulated_light += (diffuse + specular + ambient) * attenuation;
    }

    //use the albedo's alpha for transperancy
    frag_color = fsin.color * vec4(accumulated_light + sampled.emissive, texture(tex_material.albedo, fsin.tex_coord).a);

}
