
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec3 replacementColor;
uniform float tolerance;
uniform float feather;

const vec3 TARGETCOLOR = vec3(0.7216);

void main() {
    vec4 textureColor = texture2D(texture0, fragTexCoord);

    float difference = distance(textureColor.rgb, TARGETCOLOR);

    float mask = 1.0 - smoothstep(
        tolerance,
        tolerance + feather,
        difference
    );

    vec3 finalRGB = mix(
        textureColor.rgb,
        replacementColor,
        mask
    );

    gl_FragColor = vec4(
        finalRGB,
        textureColor.a * fragColor.a
    );
}
