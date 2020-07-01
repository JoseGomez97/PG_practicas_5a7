
#include <glm/gtc/matrix_transform.hpp>

#include "PGUPV.h"

using namespace PGUPV;

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;
using std::vector;

/*

Muestra una esfera texturada con una foto de la Tierra.

*/

class MyRender : public Renderer {
public:
  MyRender(){};
  void setup(void);
  void render(void);
  void reshape(uint w, uint h);
  void update(uint ms);

private:
  std::shared_ptr<GLMatrices> mats;
  std::shared_ptr<Program> program;
  //std::shared_ptr<Texture2D> earthTexture;
  std::shared_ptr<Texture2D> starsTexture;
  Sphere sph;
  float angle;
};

glm::mat4 rotationFromStoT(glm::vec3 s, glm::vec3 t) {
    glm::vec3 v = glm::cross(s, t);
    float e = glm::dot(s, t);
    float h = 1 / (1 + e);

    glm::mat4 res = {
      glm::vec4(e + h * std::pow(v[0],2), h * v.x * v.y + v.z,     h * v.x * v.z - v.y,      0),
      glm::vec4(h * v.x * v.y - v.z,      e + h * std::pow(v.y,2), h * v.y * v.z + v.x,      0),
      glm::vec4(h * v.x * v.z + v.y,      h * v.y * v.z - v.x,     e + h * std::pow(v.z, 2), 0),
      glm::vec4(0,                        0,                       0,                        1)
    };

    return res;
}

glm::mat4 computeReflectionMatrix(glm::vec3 p, glm::vec3 n) {
    glm::mat4 res = glm::mat4(1.0f);

    // Devolver el plano reflector a su sitio
    res = glm::translate(res, p);
    res *= rotationFromStoT(glm::vec3(0, 1, 0), n);

    // Reflejar con respecto al plano XZ
    res = glm::scale(res, glm::vec3(1, -1, 1));

    // Convertir el plano al XZ
    res *= rotationFromStoT(n, glm::vec3(0, 1, 0));
    res = glm::translate(res, -p);

    return res;
}

void MyRender::setup() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  angle = 0.0f;

  /* Este shader necesita las coordenadas de los vértices y sus 
  coordenadas de textura */
  program = std::make_shared<Program>();
  program->addAttributeLocation(Mesh::VERTICES, "position");
  program->addAttributeLocation(Mesh::TEX_COORD0, "texCoord");

  mats = GLMatrices::build();
  program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);

  program->loadFiles("../p5b/textureReplace");
  program->compile();

  // Localización de los uniform (unidad de textura)
  GLint texUnitLoc = program->getUniformLocation("texUnit");
  // Comunicamos la unidad de textura al shader
  program->use();
  glUniform1i(texUnitLoc, 0);

  // Cargamos la nueva textura desde un fichero
  /*earthTexture = std::make_shared<Texture2D>();
  earthTexture->loadImage("../recursos/imagenes/tierra.jpg");
  earthTexture->bind(GL_TEXTURE0);*/
  starsTexture = std::make_shared<Texture2D>();

  setCameraHandler(std::make_shared<OrbitCameraHandler>());
}

void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

  starsTexture->loadImage("../recursos/imagenes/estrellas.jpg");
  starsTexture->bind(GL_TEXTURE0);
  sph.render();
  
  mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix() * computeReflectionMatrix(glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)));
  sph.render();

  starsTexture->loadImage("../recursos/imagenes/estrellas2.jpg");
  starsTexture->bind(GL_TEXTURE0);
  mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix() * computeReflectionMatrix(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
  sph.render();
  
  mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix() * computeReflectionMatrix(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)));
  sph.render();

  CHECK_GL();
}

void MyRender::reshape(uint w, uint h) {
  glViewport(0, 0, w, h);
  mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

void MyRender::update(uint ms) {
  angle += glm::radians(45.0f) * ms / 1000.0f;
  if (angle > TWOPIf)
    angle -= TWOPIf;
  mats->setMatrix(GLMatrices::MODEL_MATRIX,
                  glm::rotate(glm::mat4(1.0f), angle, vec3(0.1, 1.0, -0.1)));
}

int main(int argc, char *argv[]) {
  App &myApp = App::getInstance();
  myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER |
                                PGUPV::MULTISAMPLE);
  myApp.getWindow().setRenderer(std::make_shared<MyRender>());
  return myApp.run();
}
