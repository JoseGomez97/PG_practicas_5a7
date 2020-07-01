#version 420 core

uniform sampler2D texUnit;

in vec2 texCoordFrag;
out vec4 fragColor;

void main() { 
  vec4 color = texture(texUnit, texCoordFrag); 
  float grey = color.r * 0.3 + color.g * 0.59 + color.b * 0.11;

  if (grey < 0.2)
    discard;
  else
    fragColor = color;
}
