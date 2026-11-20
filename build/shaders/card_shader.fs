#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

const vec3 TARGETCOLOR = vec3(0.7216);

uniform sampler2D texture0;
uniform vec3 replacementColor;
uniform float tolerance;
uniform float feather;

void main() {
    vec4 textureColor = texture(texture0, fragTexCoord);

    float difference = distance(textureColor.rgb, TARGETCOLOR);

    float mask = 1.0 - smoothstep(tolerance, tolerance + feather, difference);

    vec3 finalRGB = mix(textureColor.rgb, replacementColor, mask);

    finalColor = vec4(finalRGB, textureColor.a * fragColor.a);
}