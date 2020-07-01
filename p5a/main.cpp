
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
  std::shared_ptr<Texture2D> earthTexture;
  Sphere sph;
  float angle;
};

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

  program->loadFiles("../p5a/textureReplace");
  program->compile();

  // Localización de los uniform (unidad de textura)
  GLint texUnitLoc = program->getUniformLocation("texUnit");
  // Comunicamos la unidad de textura al shader
  program->use();
  glUniform1i(texUnitLoc, 0);

  // Cargamos la nueva textura desde un fichero
  earthTexture = std::make_shared<Texture2D>();
  earthTexture->loadImage("../recursos/imagenes/tierra.jpg");
  earthTexture->bind(GL_TEXTURE0);

  setCameraHandler(std::make_shared<OrbitCameraHandler>());
}

void MyRender::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

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
