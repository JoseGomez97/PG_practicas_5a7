#version 420

$GLMatrices

uniform float cylinderRadius; // radio del cilindro
uniform float cylinderHeight; // altura del cilindro
uniform vec3 cylinderAxis; // dirección del eje (vector unitario)
uniform vec3 cylinderPos; // centro de la base del cilindro

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in vec4 color[3];
out vec4 fragColor;

// linalg.norm
float linalgnorm(vec3 vec) {
	return sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
}

// Check if q in cylinder
bool point_in_cylinder(vec3 p1, vec3 p2, vec3 vec, float r, vec3 q) {
	// Check that q is between the two circular faces of the cylinder
	if (dot(q-p1, p2-p1) >= 0 && dot(q-p2, p2-p1) <= 0) {
		// Check that q is inside the curved surface of the cylinder
		if(linalgnorm(cross(q-p1, vec)) <= r * linalgnorm(vec)) {
			return true;
		}
	}
	return false;
}

void main() {

	vec3 cylinderPos2 = cylinderHeight * cylinderAxis + cylinderPos;

	int i, nVertIn = 0;
	for (i = 0; i < gl_in.length(); i++) {
		if (point_in_cylinder(cylinderPos, cylinderPos2, cylinderAxis, cylinderRadius, vec3(inverse(viewMatrix) * gl_in[i].gl_Position))) {
			nVertIn++;
		}
	}
	// Si todos los vertices están dentro, no dibujamos el triangulo
	if (nVertIn < 3) {
		for (i = 0; i < gl_in.length(); i++) {
			gl_Position = projMatrix * gl_in[i].gl_Position;
			if (nVertIn == 2) {
				// Si hay dos vértices dentro, se dibuja de rojo
				fragColor = vec4(0.0, 1.0, 0.0, 1.0);
			} else if (nVertIn == 1) {
				// Si hay un vértice dentro, se dibuja de verde
				fragColor = vec4(1.0, 0.0, 0.0, 1.0);
			} else if (nVertIn == 0) {
				// Si no hay vértices dentro, no se modifica
				fragColor = color[i];
			}
			EmitVertex();
		}
		EndPrimitive();
	}
}