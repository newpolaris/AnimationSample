// The fragment shader takes the object's diffused color
// from a atexture, then applies a single-directional light.
// The lighting model is just N dot L. Because there is no
// ambient term to the light, so parts of the model can appear
// as all black
#version 330 core
in vec3 norm;
in vec3 fragPos;
in vec2 uv;
uniform vec3 light;
uniform sampler2D tex0;
out vec4 FragColor;
void main() {
    vec4 diffuseColor = texture(tex0, uv);
    vec3 n = normalize(norm);
    vec3 l = normalize(light);
    float diffuseIntensity = clamp(dot(n, l), 0, 1);
    FragColor = diffuseColor * diffuseIntensity;
}
