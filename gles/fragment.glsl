precision mediump float;

uniform sampler2D sTexture;
varying vec2 textureCoordinate;

void main() {
    vec4 tc = texture2D(sTexture, textureCoordinate);
    float luminace = 0.3 * tc.r + 0.59 * tc.g + 0.11 * tc.b;
    gl_FragColor = vec4(luminace, luminace, luminace, 1.0);
}
