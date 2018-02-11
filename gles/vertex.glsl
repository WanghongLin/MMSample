attribute vec4 aPosition;
attribute vec2 aTexCoord;

varying vec2 textureCoordinate;

void main() {
    gl_Position = aPosition;
    textureCoordinate = aTexCoord;
}
