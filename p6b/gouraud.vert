#version 420

$GLMatrices
$Lights
$Material

uniform bool phong;

in vec4 position;
in vec3 normal;

out vec4 color;
out vec3 fragN;
out vec3 fragL[4];
out vec3 fragV;

// Suponiendo fuentes puntuales
vec4 iluminacion(vec3 pos, vec3 N, vec3 V) {

  // Componente emisiva del material
  vec4 color = emissive;

  for (int i = 0; i < lights.length(); i++) {

    // Si la fuente est� apagada, no tenerla en cuenta
    if (lights[i].enabled == 0)
      continue;

    // Vector iluminaci�n (desde v�rtice a la fuente)
    vec3 L = normalize(vec3(lights[i].positionEye) - pos);

    // Multiplicador de la componente difusa
    float diffuseMult = max(dot(N, L), 0.0);

    float specularMult = 0.0;
    if (diffuseMult > 0.0) {
      // Multiplicador de la componente especular
      vec3 R = reflect(-L, N);
      specularMult = max(0.0, dot(R, V));
      specularMult = pow(specularMult, shininess);
    }
    
    // Distancia (desde v�rtice a la fuente)
    float d = distance(vec3(lights[i].positionEye), pos);

    // Factor atenuaci�n
    float factorAtenuacion = 1 / max(1, lights[i].attenuation[0] + lights[i].attenuation[1] * d + lights[i].attenuation[2] * d * d);

    // Efecto foco
    float efectoFoco;
    if (lights[i].spotCutoff == 180) // La fuente no es focal
    {
        efectoFoco = 1;
    }
    else if (max(dot(-L,lights[i].spotDirectionEye), 0) < lights[i].spotCosCutoff) // V�rtice fuera del cono iluminado
    { 
        efectoFoco = 0;
    }
    else
    {
        efectoFoco = pow(max(dot(-L,lights[i].spotDirectionEye), 0), lights[i].spotExponent);
    }

    color += factorAtenuacion * efectoFoco *
             ( lights[i].ambient * ambient +
               lights[i].diffuse * diffuse * diffuseMult +
               lights[i].specular * specular * specularMult );

  }

  return color;
}

void main() {

  // Normal en el espacio de la c�mara
  vec3 eN = normalize(normalMatrix * normal);
  fragN = eN;

  // V�rtice en el espacio de la c�mara
  vec3 eposition = vec3(modelviewMatrix * position);

  // Vector vista (desde v�rtice a la c�mara)
  vec3 V = normalize(-eposition.xyz);
  fragV = V;

  // C�lculo de la iluminaci�n
  if(phong) {  // Por fragmento
    for (int i = 0; i < lights.length(); i++) {
      vec3 L = normalize(vec3(lights[i].positionEye) - eposition);
      fragL[i] = L;
    }
  } else { // Por v�rtice
    color = iluminacion(eposition, eN, V);
  }

  gl_Position = modelviewprojMatrix * position;
}
