#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuse_texture;
uniform sampler2D depthMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float shininess;

void main()
{           
    float gamma = 2.2;
    vec3 color = texture(diffuse_texture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.2 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);  // point light
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    // Blinn-Phong lighting model
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = lightColor * spec; // assuming bright white light color

    // attenuation
    /*
    float max_distance = 1.5;
    float distance = length(lightPos - fs_in.FragPos);
    float attenuation = 1.0 / (distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    */
    
    vec3 lighting = (ambient + diffuse + specular) * color;
    FragColor = vec4(lighting, 1.0);

    // gamma encode
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}