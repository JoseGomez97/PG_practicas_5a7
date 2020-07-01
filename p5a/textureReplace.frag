#version 420 core

uniform sampler2D texUnit;

in vec2 texCoordFrag;
out vec4 fragColor;

void main() { 
  vec4 color = texture(texUnit, texCoordFrag); 
  float grey = color.r * 0.3 + color.g * 0.59 + color.b * 0.11;
  
  if (int(gl_FragCoord.y) % 4 == 0 || int(gl_FragCoord.y) % 4 == 1)
  {
    grey *= 0.2;
  }
  else if (int(gl_FragCoord.y) % 4 == 2)
  {
    grey *= 0.8;
  }

  fragColor = vec4(grey, grey, grey, color.a);
}
