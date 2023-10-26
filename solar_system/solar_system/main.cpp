// Include C++ headers
#include <iostream>
#include <string>
#include <vector>
#include <random>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h> 

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024*1.4
#define W_HEIGHT 768*1.4
#define TITLE "Solar System"

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

// Global variables
GLFWwindow* window;
Camera* camera;
Light* light;
GLuint shaderProgram, shaderProgram2, orbitShaderProgram, depthProgram, explosionShaderProgram;
GLuint depthFrameBuffer, depthCubeMap;
GLuint viewMatrixLocation;
GLuint projectionMatrixLocation;
GLuint modelMatrixLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation, lightPosLocation;
GLuint lightPowerLocation;
GLuint depthMapSampler;
GLuint lightVPLocation1, lightVPLocation2, lightVPLocation3, lightVPLocation4, lightVPLocation5, lightVPLocation6;
GLuint shadowModelLocation;
GLuint MVPLocation, MVPLocation2, MVPLocation3, ColorLocation;
GLuint textureSampler, textureSampler2, textureSampler3;
GLuint timeLocation;
GLuint randLocation;

GLuint asteroid_texture, room_texture, skybox_texture, sun_texture, mercury_texture, venus_texture, earth_texture, mars_texture, jupiter_texture, saturn_texture,
saturn_rings_texture, uranus_texture, uranus_rings_texture, neptune_texture, pluto_texture, moon_texture, io_texture, titan_texture;
GLuint timeUniform;

GLuint asteroidVAO, orbitVAO, roomVAO, skyboxVAO, sunVAO, mercuryVAO, venusVAO, earthVAO, marsVAO, jupiterVAO, saturnVAO, saturn_ringsVAO, uranusVAO,
uranus_ringsVAO, neptuneVAO, plutoVAO, moonVAO, ioVAO, titanVAO;

GLuint asteroidVerticiesVBO, asteroidUVVBO, orbitVerticiesVBO, roomVerticiesVBO, roomUVVBO, skyboxVerticiesVBO, skyboxUVVBO, sunVerticiesVBO, sunUVVBO, mercuryVerticiesVBO, mercuryUVVBO, venusVerticiesVBO, venusUVVBO,
earthVerticiesVBO, earthUVVBO, marsVerticiesVBO, marsUVVBO, jupiterVerticiesVBO, jupiterUVVBO, saturnVerticiesVBO, saturnUVVBO,
saturn_ringsVerticiesVBO, saturn_ringsUVVBO, uranusVerticiesVBO, uranusUVVBO, uranus_ringsVerticiesVBO, uranus_ringsUVVBO, 
neptuneVerticiesVBO, neptuneUVVBO, plutoVerticiesVBO, plutoUVVBO, moonVerticiesVBO, moonUVVBO, ioVerticiesVBO, ioUVVBO, 
titanVerticiesVBO, titanUVVBO;

GLuint asteroidNormalsVBO, roomNormalsVBO, skyboxNormalsVBO, sunNormalsVBO, mercuryNormalsVBO, venusNormalsVBO, earthNormalsVBO, marsNormalsVBO, jupiterNormalsVBO,
saturnNormalsVBO, saturn_ringsNormalsVBO, uranusNormalsVBO, uranus_ringsNormalsVBO, neptuneNormalsVBO, plutoNormalsVBO, moonNormalsVBO,
ioNormalsVBO, titanNormalsVBO;

std::vector<vec3> asteroidNormals, asteroidVertices, orbitNormals, orbitVertices, roomNormals, roomVertices, skyboxNormals, sunVertices, sunNormals, mercuryVertices, mercuryNormals, venusVertices, venusNormals, earthVertices,
earthNormals, marsVertices, marsNormals, jupiterVertices, jupiterNormals, saturnVertices, saturnNormals, saturn_ringsVertices,
saturn_ringsNormals, uranusVertices, uranusNormals, uranus_ringsVertices, uranus_ringsNormals, neptuneVertices, neptuneNormals, plutoVertices,
plutoNormals, moonVertices, moonNormals, ioVertices, ioNormals, titanVertices, titanNormals;

std::vector<vec2> asteroidUVs, orbitUVs, roomUVs, skyboxUVs, sunUVs, mercuryUVs, venusUVs, earthUVs, marsUVs, jupiterUVs, saturnUVs, saturn_ringsUVs, uranusUVs,
uranus_ringsUVs, neptuneUVs, plutoUVs, moonUVs, ioUVs, titanUVs;


#define num_of_asteroids 400
mat4 modelMatrices[num_of_asteroids];

GLuint RENDER_SUN;
GLuint RENDER_PLANETS;
GLuint RENDER_SKYBOX;
GLuint RENDER_ORBITS;
GLuint RENDER_ROOM;
GLuint RENDER_ASTEROIDS;
GLuint RENDER_EXPLOSION;
GLuint explosion;

GLfloat current_time;
GLfloat checkpoint;
vec4 current_mercury_pos, current_venus_pos, current_earth_pos, current_mars_pos, current_jupiter_pos, current_saturn_pos, current_saturn_rings_pos,
current_uranus_pos, current_uranus_rings_pos, current_neptune_pos, current_pluto_pos, current_moon_pos, current_io_pos, current_titan_pos ;



void uploadLight(const Light& light) {
    glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
    glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
    glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
    glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
        light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
    glUniform1f(lightPowerLocation, light.power);
}


void createContext() {
    // Create and compile our GLSL program from the shaders
    shaderProgram = loadShaders("texture.vertexshader", "texture.fragmentshader");
    shaderProgram2 = loadShaders("simpleTexture.vertexshader", "simpleTexture.fragmentshader");
    orbitShaderProgram = loadShaders("orbits.vertexshader", "orbits.fragmentshader");
    depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader", "Depth.geometryshader");
    explosionShaderProgram = loadShaders("Explosion.vertexshader", "Explosion.fragmentshader", "Explosion.geometryshader");

    // Draw wire frame triangles or fill: GL_LINE, or GL_FILL
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Get a pointer location to model matrix in the vertex shader
    MVPLocation = glGetUniformLocation(shaderProgram2, "MVP");
    MVPLocation2 = glGetUniformLocation(orbitShaderProgram, "MVP");
    MVPLocation3 = glGetUniformLocation(explosionShaderProgram, "MVP");
    ColorLocation = glGetUniformLocation(orbitShaderProgram, "color");
    projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
    viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
    modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
    LaLocation = glGetUniformLocation(shaderProgram, "light.La");
    LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
    LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
    lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
    lightPosLocation = glGetUniformLocation(depthProgram, "lightPos");
    lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
    depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");
    lightVPLocation1 = glGetUniformLocation(depthProgram, "lightVP1");
    lightVPLocation2 = glGetUniformLocation(depthProgram, "lightVP2");
    lightVPLocation3 = glGetUniformLocation(depthProgram, "lightVP3");
    lightVPLocation4= glGetUniformLocation(depthProgram, "lightVP4");
    lightVPLocation5 = glGetUniformLocation(depthProgram, "lightVP5");
    lightVPLocation6 = glGetUniformLocation(depthProgram, "lightVP6");
    shadowModelLocation = glGetUniformLocation(depthProgram, "M");
    timeLocation = glGetUniformLocation(explosionShaderProgram, "time");
    randLocation = glGetUniformLocation(explosionShaderProgram, "jitter");

    // load objects
    loadOBJ("sphere.obj", sunVertices, sunUVs, sunNormals);
    loadOBJ("sphere.obj", mercuryVertices, mercuryUVs, mercuryNormals);
    loadOBJ("sphere.obj", venusVertices, venusUVs, venusNormals);
    loadOBJ("sphere.obj", earthVertices, earthUVs, earthNormals);
    loadOBJ("sphere.obj", marsVertices, marsUVs, marsNormals);
    loadOBJ("sphere.obj", jupiterVertices, jupiterUVs, jupiterNormals);
    loadOBJ("sphere.obj", saturnVertices, saturnUVs, saturnNormals);
    loadOBJ("saturn_rings.obj", saturn_ringsVertices, saturn_ringsUVs, saturn_ringsNormals);
    loadOBJ("sphere.obj", uranusVertices, uranusUVs, uranusNormals);
    loadOBJ("uranus_rings.obj", uranus_ringsVertices, uranus_ringsUVs, uranus_ringsNormals);
    loadOBJ("sphere.obj", neptuneVertices, neptuneUVs, neptuneNormals);
    loadOBJ("sphere.obj", plutoVertices, plutoUVs, plutoNormals);
    loadOBJ("sphere.obj", moonVertices, moonUVs, moonNormals);
    loadOBJ("sphere.obj", ioVertices, ioUVs, ioNormals);
    loadOBJ("sphere.obj", titanVertices, titanUVs, titanNormals);
    loadOBJ("orbit.obj", orbitVertices, orbitUVs, orbitNormals);
    loadOBJ("asteroid.obj", asteroidVertices, asteroidUVs, asteroidNormals);



    // asteroids
    // asteroidVAO
    glGenVertexArrays(1, &asteroidVAO);
    glBindVertexArray(asteroidVAO);

    // asteroidVBO
    glGenBuffers(1, &asteroidVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, asteroidVertices.size() * sizeof(glm::vec3),
        &asteroidVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // asteroid normals VBO
    glGenBuffers(1, &asteroidNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, asteroidNormals.size() * sizeof(glm::vec3),
        &asteroidNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // asteroid uvs VBO
    glGenBuffers(1, &asteroidUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, asteroidUVVBO);
    glBufferData(GL_ARRAY_BUFFER, asteroidUVs.size() * sizeof(glm::vec2),
        &asteroidUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // Generate a large list of semi-random model transformation matrices
    srand(glfwGetTime()); // initialize random seed	
    float radius = 10.0;
    float offset = 1.8f;
    for (int i = 0; i < num_of_asteroids; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // Displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)num_of_asteroids * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.25f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // Scale between 0.002 and 0.023f
        float scale = (rand() % 20) / 1000.0f + 0.03;
        model = glm::scale(model, glm::vec3(scale));

        // Random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }


    //orbit line
    glGenVertexArrays(1, &orbitVAO);
    glBindVertexArray(orbitVAO);

    glGenBuffers(1, &orbitVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, orbitVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, orbitVertices.size() * sizeof(glm::vec3),
        &orbitVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);


    //skybox VAO
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    const GLfloat skyboxVertices[] = {
      -60.0f,  60.0f, -60.0f,
      -60.0f, -60.0f, -60.0f,
       60.0f, -60.0f, -60.0f,
       60.0f, -60.0f, -60.0f,
       60.0f,  60.0f, -60.0f,
      -60.0f,  60.0f, -60.0f,

      -60.0f, -60.0f,  60.0f,
      -60.0f, -60.0f, -60.0f,
      -60.0f,  60.0f, -60.0f,
      -60.0f,  60.0f, -60.0f,
      -60.0f,  60.0f,  60.0f,
      -60.0f, -60.0f,  60.0f,

       60.0f, -60.0f, -60.0f,
       60.0f, -60.0f,  60.0f,
       60.0f,  60.0f,  60.0f,
       60.0f,  60.0f,  60.0f,
       60.0f,  60.0f, -60.0f,
       60.0f, -60.0f, -60.0f,

      -60.0f, -60.0f,  60.0f,
      -60.0f,  60.0f,  60.0f,
       60.0f,  60.0f,  60.0f,
       60.0f,  60.0f,  60.0f,
       60.0f, -60.0f,  60.0f,
      -60.0f, -60.0f,  60.0f,

      -60.0f,  60.0f, -60.0f,
       60.0f,  60.0f, -60.0f,
       60.0f,  60.0f,  60.0f,
       60.0f,  60.0f,  60.0f,
      -60.0f,  60.0f,  60.0f,
      -60.0f,  60.0f, -60.0f,

      -60.0f, -60.0f, -60.0f,
      -60.0f, -60.0f,  60.0f,
       60.0f, -60.0f, -60.0f,
       60.0f, -60.0f, -60.0f,
      -60.0f, -60.0f,  60.0f,
       60.0f, -60.0f,  60.0f
    };
    const GLfloat skyboxNormals[] = {
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,

     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,

     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,

     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,

     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,

     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f

    };
    const GLfloat skyboxUVs[] = {
     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f
    };

    //skybox VBO
    glGenBuffers(1, &skyboxVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),
        &skyboxVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // skybox normals VBO
    glGenBuffers(1, &skyboxNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxNormals),
        &skyboxNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // skybox uvs VBO
    glGenBuffers(1, &skyboxUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxUVs),
        &skyboxUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    //room VAO
    glGenVertexArrays(1, &roomVAO);
    glBindVertexArray(roomVAO);
    const GLfloat roomVertices[] = {
      -40.0f,  40.0f, -40.0f,
      -40.0f, -40.0f, -40.0f,
       40.0f, -40.0f, -40.0f,
       40.0f, -40.0f, -40.0f,
       40.0f,  40.0f, -40.0f,
      -40.0f,  40.0f, -40.0f,

      -40.0f, -40.0f,  40.0f,
      -40.0f, -40.0f, -40.0f,
      -40.0f,  40.0f, -40.0f,
      -40.0f,  40.0f, -40.0f,
      -40.0f,  40.0f,  40.0f,
      -40.0f, -40.0f,  40.0f,

       40.0f, -40.0f, -40.0f,
       40.0f, -40.0f,  40.0f,
       40.0f,  40.0f,  40.0f,
       40.0f,  40.0f,  40.0f,
       40.0f,  40.0f, -40.0f,
       40.0f, -40.0f, -40.0f,

      -40.0f, -40.0f,  40.0f,
      -40.0f,  40.0f,  40.0f,
       40.0f,  40.0f,  40.0f,
       40.0f,  40.0f,  40.0f,
       40.0f, -40.0f,  40.0f,
      -40.0f, -40.0f,  40.0f,

      -40.0f,  40.0f, -40.0f,
       40.0f,  40.0f, -40.0f,
       40.0f,  40.0f,  40.0f,
       40.0f,  40.0f,  40.0f,
      -40.0f,  40.0f,  40.0f,
      -40.0f,  40.0f, -40.0f,

      -40.0f, -40.0f, -40.0f,
      -40.0f, -40.0f,  40.0f,
       40.0f, -40.0f, -40.0f,
       40.0f, -40.0f, -40.0f,
      -40.0f, -40.0f,  40.0f,
       40.0f, -40.0f,  40.0f
    };
    const GLfloat roomNormals[] = {
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, 1.0f,

     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,
     1.0f, 0.0f, 0.0f,

     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,
     -1.0f, 0.0f, 0.0f,

     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,
     0.0f, 0.0f, -1.0f,

     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,
     0.0f, -1.0f, 0.0f,

     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f,
     0.0f, 1.0f, 0.0f

    };
    const GLfloat roomUVs[] = {
     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f,

     0.0f, 0.0f,
     1.0f, 0.0f,
     1.0f, 1.0f,
     1.0f, 1.0f,
     0.0f, 1.0f,
     0.0f, 0.0f
    };

    // room VBO
    glGenBuffers(1, &roomVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roomVertices),
        &roomVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // room normals VBO
    glGenBuffers(1, &roomNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roomNormals),
        &roomNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // room uvs VBO
    glGenBuffers(1, &roomUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, roomUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roomUVs),
        &roomUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // sunVAO
    glGenVertexArrays(1, &sunVAO);
    glBindVertexArray(sunVAO);

    // sunVBO
    glGenBuffers(1, &sunVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sunVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, sunVertices.size() * sizeof(glm::vec3),
        &sunVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    /* for (int i = 0; i < sunNormals.size(); i++) {
         for (int j = 0; j < 3; j++) {
             sunNormals[i][j] = -sunNormals[i][j];
         }
     }*/

     // sun normals VBO
    glGenBuffers(1, &sunNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sunNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, sunNormals.size() * sizeof(glm::vec3),
        &sunNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // sun uvs VBO
    glGenBuffers(1, &sunUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sunUVVBO);
    glBufferData(GL_ARRAY_BUFFER, sunUVs.size() * sizeof(glm::vec2),
        &sunUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // mercuryVAO
    glGenVertexArrays(1, &mercuryVAO);
    glBindVertexArray(mercuryVAO);

    // mercuryVBO
    glGenBuffers(1, &mercuryVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mercuryVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, mercuryVertices.size() * sizeof(glm::vec3),
        &mercuryVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // mercury normals VBO
    glGenBuffers(1, &mercuryNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mercuryNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, mercuryNormals.size() * sizeof(glm::vec3),
        &mercuryNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // mercury uvs VBO
    glGenBuffers(1, &mercuryUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mercuryUVVBO);
    glBufferData(GL_ARRAY_BUFFER, mercuryUVs.size() * sizeof(glm::vec2),
        &mercuryUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // venusVAO
    glGenVertexArrays(1, &venusVAO);
    glBindVertexArray(venusVAO);

    // venusVBO
    glGenBuffers(1, &venusVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, venusVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, venusVertices.size() * sizeof(glm::vec3),
        &venusVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // venus normals VBO
    glGenBuffers(1, &venusNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, venusNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, venusNormals.size() * sizeof(glm::vec3),
        &venusNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // venus uvs VBO
    glGenBuffers(1, &venusUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, venusUVVBO);
    glBufferData(GL_ARRAY_BUFFER, venusUVs.size() * sizeof(glm::vec2),
        &venusUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // earthVAO
    glGenVertexArrays(1, &earthVAO);
    glBindVertexArray(earthVAO);

    // earthVBO
    glGenBuffers(1, &earthVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, earthVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, earthVertices.size() * sizeof(glm::vec3),
        &earthVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // earth normals VBO
    glGenBuffers(1, &earthNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, earthNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, earthNormals.size() * sizeof(glm::vec3),
        &earthNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // earth uvs VBO
    glGenBuffers(1, &earthUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, earthUVVBO);
    glBufferData(GL_ARRAY_BUFFER, earthUVs.size() * sizeof(glm::vec2),
        &earthUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // marsVAO
    glGenVertexArrays(1, &marsVAO);
    glBindVertexArray(marsVAO);

    // marsVBO
    glGenBuffers(1, &marsVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, marsVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, marsVertices.size() * sizeof(glm::vec3),
        &marsVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // mars normals VBO
    glGenBuffers(1, &marsNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, marsNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, marsNormals.size() * sizeof(glm::vec3),
        &marsNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // mars uvs VBO
    glGenBuffers(1, &marsUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, marsUVVBO);
    glBufferData(GL_ARRAY_BUFFER, marsUVs.size() * sizeof(glm::vec2),
        &marsUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // jupiterVAO
    glGenVertexArrays(1, &jupiterVAO);
    glBindVertexArray(jupiterVAO);

    // jupiterVBO
    glGenBuffers(1, &jupiterVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, jupiterVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, jupiterVertices.size() * sizeof(glm::vec3),
        &jupiterVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // jupiter normals VBO
    glGenBuffers(1, &jupiterNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, jupiterNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, jupiterNormals.size() * sizeof(glm::vec3),
        &jupiterNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // jupiter uvs VBO
    glGenBuffers(1, &jupiterUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, jupiterUVVBO);
    glBufferData(GL_ARRAY_BUFFER, jupiterUVs.size() * sizeof(glm::vec2),
        &jupiterUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // saturnVAO
    glGenVertexArrays(1, &saturnVAO);
    glBindVertexArray(saturnVAO);

    // saturnVBO
    glGenBuffers(1, &saturnVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, saturnVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, saturnVertices.size() * sizeof(glm::vec3),
        &saturnVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // saturn normals VBO
    glGenBuffers(1, &saturnNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, saturnNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, saturnNormals.size() * sizeof(glm::vec3),
        &saturnNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // saturn uvs VBO
    glGenBuffers(1, &saturnUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, saturnUVVBO);
    glBufferData(GL_ARRAY_BUFFER, saturnUVs.size() * sizeof(glm::vec2),
        &saturnUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // saturn_ringsVAO
    glGenVertexArrays(1, &saturn_ringsVAO);
    glBindVertexArray(saturn_ringsVAO);

    // saturn_ringsVBO
    glGenBuffers(1, &saturn_ringsVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, saturn_ringsVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, saturn_ringsVertices.size() * sizeof(glm::vec3),
        &saturn_ringsVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // saturn_rings normals VBO
    glGenBuffers(1, &saturn_ringsNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, saturn_ringsNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, saturn_ringsNormals.size() * sizeof(glm::vec3),
        &saturn_ringsNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // saturn_rings uvs VBO
    glGenBuffers(1, &saturn_ringsUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, saturn_ringsUVVBO);
    glBufferData(GL_ARRAY_BUFFER, saturn_ringsUVs.size() * sizeof(glm::vec2),
        &saturn_ringsUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // uranusVAO
    glGenVertexArrays(1, &uranusVAO);
    glBindVertexArray(uranusVAO);

    // uranusVBO
    glGenBuffers(1, &uranusVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uranusVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, uranusVertices.size() * sizeof(glm::vec3),
        &uranusVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // uranus normals VBO
    glGenBuffers(1, &uranusNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uranusNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, uranusNormals.size() * sizeof(glm::vec3),
        &uranusNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // uranus uvs VBO
    glGenBuffers(1, &uranusUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uranusUVVBO);
    glBufferData(GL_ARRAY_BUFFER, uranusUVs.size() * sizeof(glm::vec2),
        &uranusUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // uranus_ringsVAO
    glGenVertexArrays(1, &uranus_ringsVAO);
    glBindVertexArray(uranus_ringsVAO);

    // uranus_ringsVBO
    glGenBuffers(1, &uranus_ringsVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uranus_ringsVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, uranus_ringsVertices.size() * sizeof(glm::vec3),
        &uranus_ringsVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // uranus_rings normals VBO
    glGenBuffers(1, &uranus_ringsNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uranus_ringsNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, uranus_ringsNormals.size() * sizeof(glm::vec3),
        &uranus_ringsNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // uranus_rings uvs VBO
    glGenBuffers(1, &uranus_ringsUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, uranus_ringsUVVBO);
    glBufferData(GL_ARRAY_BUFFER, uranus_ringsUVs.size() * sizeof(glm::vec2),
        &uranus_ringsUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // neptuneVAO
    glGenVertexArrays(1, &neptuneVAO);
    glBindVertexArray(neptuneVAO);

    // neptuneVBO
    glGenBuffers(1, &neptuneVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, neptuneVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, neptuneVertices.size() * sizeof(glm::vec3),
        &neptuneVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // neptune normals VBO
    glGenBuffers(1, &neptuneNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, neptuneNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, neptuneNormals.size() * sizeof(glm::vec3),
        &neptuneNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // neptune uvs VBO
    glGenBuffers(1, &neptuneUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, neptuneUVVBO);
    glBufferData(GL_ARRAY_BUFFER, neptuneUVs.size() * sizeof(glm::vec2),
        &neptuneUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // plutoVAO
    glGenVertexArrays(1, &plutoVAO);
    glBindVertexArray(plutoVAO);

    // plutoVBO
    glGenBuffers(1, &plutoVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, plutoVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, plutoVertices.size() * sizeof(glm::vec3),
        &plutoVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // pluto normals VBO
    glGenBuffers(1, &plutoNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, plutoNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, plutoNormals.size() * sizeof(glm::vec3),
        &plutoNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // pluto uvs VBO
    glGenBuffers(1, &plutoUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, plutoUVVBO);
    glBufferData(GL_ARRAY_BUFFER, plutoUVs.size() * sizeof(glm::vec2),
        &plutoUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // moonVAO
    glGenVertexArrays(1, &moonVAO);
    glBindVertexArray(moonVAO);

    // moonVBO
    glGenBuffers(1, &moonVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, moonVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, moonVertices.size() * sizeof(glm::vec3),
        &moonVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // moon normals VBO
    glGenBuffers(1, &moonNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, moonNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, moonNormals.size() * sizeof(glm::vec3),
        &moonNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // moon uvs VBO
    glGenBuffers(1, &moonUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, moonUVVBO);
    glBufferData(GL_ARRAY_BUFFER, moonUVs.size() * sizeof(glm::vec2),
        &moonUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // ioVAO
    glGenVertexArrays(1, &ioVAO);
    glBindVertexArray(ioVAO);

    // ioVBO
    glGenBuffers(1, &ioVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ioVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, ioVertices.size() * sizeof(glm::vec3),
        &ioVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // io normals VBO
    glGenBuffers(1, &ioNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ioNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, ioNormals.size() * sizeof(glm::vec3),
        &ioNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // io uvs VBO
    glGenBuffers(1, &ioUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ioUVVBO);
    glBufferData(GL_ARRAY_BUFFER, ioUVs.size() * sizeof(glm::vec2),
        &ioUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // titanVAO
    glGenVertexArrays(1, &titanVAO);
    glBindVertexArray(titanVAO);

    // titanVBO
    glGenBuffers(1, &titanVerticiesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, titanVerticiesVBO);
    glBufferData(GL_ARRAY_BUFFER, titanVertices.size() * sizeof(glm::vec3),
        &titanVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // titan normals VBO
    glGenBuffers(1, &titanNormalsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, titanNormalsVBO);
    glBufferData(GL_ARRAY_BUFFER, titanNormals.size() * sizeof(glm::vec3),
        &titanNormals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // titan uvs VBO
    glGenBuffers(1, &titanUVVBO);
    glBindBuffer(GL_ARRAY_BUFFER, titanUVVBO);
    glBufferData(GL_ARRAY_BUFFER, titanUVs.size() * sizeof(glm::vec2),
        &titanUVs[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // textures
    sun_texture = loadBMP("sun.bmp");
    mercury_texture = loadBMP("mercury.bmp");
    venus_texture = loadBMP("venus.bmp");
    earth_texture = loadBMP("earth.bmp");
    mars_texture = loadBMP("mars.bmp");
    jupiter_texture = loadBMP("jupiter.bmp");
    saturn_texture = loadBMP("saturn.bmp");
    saturn_rings_texture = loadBMP("saturn_rings.bmp");
    uranus_texture = loadBMP("uranus.bmp");
    uranus_rings_texture = loadBMP("uranus_rings.bmp");
    neptune_texture = loadBMP("neptune.bmp");
    pluto_texture = loadBMP("pluto.bmp");
    moon_texture = loadBMP("moon.bmp");
    io_texture = loadBMP("io.bmp");
    titan_texture = loadBMP("titan.bmp");
    skybox_texture = loadBMP("stars.bmp");
    room_texture = loadBMP("wall.bmp");
    asteroid_texture = loadBMP("asteroid.bmp");

    // Get a handle for our "textureSampler" uniform
    textureSampler = glGetUniformLocation(shaderProgram, "textureSampler");
    textureSampler2 = glGetUniformLocation(shaderProgram2, "textureSampler");
    textureSampler3 = glGetUniformLocation(explosionShaderProgram, "textureSampler");


    // Create a depth framebuffer and a texture to store the depthmap
    glGenFramebuffers(1, &depthFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
    glGenTextures(1, &depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

    for (int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glfwTerminate();
        throw runtime_error("Frame buffer not initialized correctly");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


mat4 sunModelMatrix() {
    mat4 sunScaling = glm::scale(mat4(), vec3(2.5f, 2.5f, 2.5f));
    mat4 sunRotation = glm::rotate(mat4(), 2 * 3.14f / 28 * current_time, vec3(0.0, 1.0, 0.0));
    mat4 sunTranslation = glm::translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
    mat4 sunModel = sunTranslation * sunRotation * sunScaling;
    return sunModel;
}
mat4 mercuryModelMatrix() {
    mat4 mercuryScaling = glm::scale(mat4(), vec3(0.2f, 0.2f, 0.2f));
    mat4 mercuryTranslation;
    mat4 mercuryModel;
    if (explosion == 1) {
        vec4 x = normalize(current_mercury_pos) * (current_time - checkpoint) / 2.0f + current_mercury_pos;
        mercuryTranslation = glm::translate(mat4(), vec3(x));
        mercuryModel = mercuryTranslation * mercuryScaling;
    }
    else {
        mercuryTranslation = glm::translate(mat4(), vec3(15.0f, 0.0f, 0.0f));
        mat4 mercuryRotation = glm::rotate(mat4(), 2 * 3.14f / 9 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 mercurySelfRotation = glm::rotate(mat4(), 2 * 3.14f / 58 * current_time, vec3(0.0, 1.0, 0.0));
        mercuryModel = mercuryScaling * mercuryRotation * mercuryTranslation * mercurySelfRotation;
        current_mercury_pos = mercuryModel * vec4(0, 0, 0, 1);   
    }
    return mercuryModel;
}
mat4 venusModelmatrix() {
    mat4 venusScaling = glm::scale(mat4(), vec3(0.4f, 0.4f, 0.4f));
    mat4 venusTranslation;
    mat4 venusModel;
    if (explosion == 1) {
        vec4 x = normalize(current_venus_pos) * (current_time - checkpoint) / 2.0f + current_venus_pos;
        venusTranslation = glm::translate(mat4(), vec3(x));
        venusModel = venusTranslation * venusScaling;
    }
    else {
        venusTranslation = glm::translate(mat4(), vec3(-10.0f, 0.0f, 0.0f));
        mat4 venusRotation = glm::rotate(mat4(), 2 * 3.14f / 23 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 venusSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 243 * current_time, vec3(0.0, 1.0, 0.0));
        venusModel = venusScaling * venusRotation * venusTranslation * venusSelfRotation;
        current_venus_pos = venusModel * vec4(0, 0, 0, 1);
    }
    return venusModel;
}
mat4 earthModelmatrix() {
    mat4 earthScaling = glm::scale(mat4(), vec3(0.5f, 0.5f, 0.5f));
    mat4 earthTranslation;
    mat4 earthModel;
    if (explosion == 1) {
        vec4 x = normalize(current_earth_pos) * (current_time - checkpoint)/2.0f + current_earth_pos;
        earthTranslation = glm::translate(mat4(), vec3(x));
        earthModel = earthTranslation * earthScaling;
    }
    else {
        earthTranslation = glm::translate(mat4(), vec3(12.0f, 0.0f, 0.0f));
        mat4 earthRotation = glm::rotate(mat4(), 2 * 3.14f / 37 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 earthSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        earthModel = earthScaling * earthRotation * earthTranslation * earthSelfRotation;
        current_earth_pos = earthModel * vec4(0, 0, 0, 1);   
    }
    return earthModel;
}
mat4 marsModelMatrix() {
    mat4 marsScaling = glm::scale(mat4(), vec3(0.2f, 0.2f, 0.2f));
    mat4 marsTranslation;
    mat4 marsModel;
    if (explosion == 1) {
        vec4 x = normalize(current_mars_pos) * (current_time - checkpoint) / 2.0f + current_mars_pos;
        marsTranslation = glm::translate(mat4(), vec3(x));
        marsModel = marsTranslation * marsScaling;
    }
    else {
        marsTranslation = glm::translate(mat4(), vec3(-40.0f, 0.0f, 0.0f));
        mat4 marsRotation = glm::rotate(mat4(), 2 * 3.14f / 69 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 marsSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        marsModel = marsScaling * marsRotation * marsTranslation * marsSelfRotation;
        current_mars_pos = marsModel * vec4(0, 0, 0, 1);
    }
    return marsModel;
}
mat4 jupiterModelMatrix() {
    mat4 jupiterScaling = glm::scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
    mat4 jupiterTranslation;
    mat4 jupiterModel;
    if (explosion == 1) {
        vec4 x = normalize(current_jupiter_pos) * (current_time - checkpoint) / 2.0f + current_jupiter_pos;
        jupiterTranslation = glm::translate(mat4(), vec3(x));
        jupiterModel = jupiterTranslation * jupiterScaling;
    }
    else {
        jupiterTranslation = glm::translate(mat4(), vec3(10.0f, 0.0f, 0.0f));
        mat4 jupiterRotation = glm::rotate(mat4(), 2 * 3.14f / 100 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 jupiterSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        jupiterModel = jupiterScaling * jupiterRotation * jupiterTranslation * jupiterSelfRotation;
        current_jupiter_pos = jupiterModel * vec4(0, 0, 0, 1);
    }
    return jupiterModel;    
}
mat4 saturnModelmatrix() {
    mat4 saturnScaling = glm::scale(mat4(), vec3(0.8f, 0.8f, 0.8f));
    mat4 saturnTranslation;
    mat4 saturnModel;
    if (explosion == 1) {
        vec4 x = normalize(current_saturn_pos) * (current_time - checkpoint) / 2.0f + current_saturn_pos;
        saturnTranslation = glm::translate(mat4(), vec3(x));
        saturnModel = saturnTranslation * saturnScaling;
    }
    else {
        saturnTranslation = glm::translate(mat4(), vec3(-17.0f, 0.0f, 0.0f));
        mat4 saturnRotation = glm::rotate(mat4(), 2 * 3.14f / 200 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 saturnSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        saturnModel = saturnScaling * saturnRotation * saturnTranslation * saturnSelfRotation;
        current_saturn_pos = saturnModel * vec4(0, 0, 0, 1);
    }
    return saturnModel;    
}
mat4 saturn_ringsModelMatrix() {
    mat4 saturn_ringsScaling = glm::scale(mat4(), vec3(0.4f, 0.4f, 0.4f));
    mat4 saturn_ringsTranslation;
    mat4 saturn_ringsModel;
    if (explosion == 1) {
        vec4 x = normalize(current_saturn_rings_pos) * (current_time - checkpoint) / 2.0f + current_saturn_rings_pos;
        saturn_ringsTranslation = glm::translate(mat4(), vec3(x));
        saturn_ringsModel = saturn_ringsTranslation * saturn_ringsScaling;
    }
    else {
        saturn_ringsTranslation = glm::translate(mat4(), vec3(-34.0f, 0.0f, 0.0f));
        mat4 saturn_ringsRotation = glm::rotate(mat4(), 2 * 3.14f / 200 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 saturn_ringsAngle = glm::rotate(mat4(), 3.14f / 60, vec3(1.0, 0.0, 0.0));
        mat4 saturn_ringsSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        saturn_ringsModel = saturn_ringsScaling * saturn_ringsRotation * saturn_ringsTranslation * saturn_ringsAngle * saturn_ringsSelfRotation;
        current_saturn_rings_pos = saturn_ringsModel * vec4(0, 0, 0, 1);    
    }
    return saturn_ringsModel;    
}
mat4 uranusModelMatrix() {
    mat4 uranusScaling = glm::scale(mat4(), vec3(0.6f, 0.6f, 0.6f));
    mat4 uranusTranslation;
    mat4 uranusModel;
    if (explosion == 1) {
        vec4 x = normalize(current_uranus_pos) * (current_time - checkpoint) / 2.0f + current_uranus_pos;
        uranusTranslation = glm::translate(mat4(), vec3(x));
        uranusModel = uranusTranslation * uranusScaling;
    }
    else {
        uranusTranslation = glm::translate(mat4(), vec3(27.0f, 0.0f, 0.0f));
        mat4 uranusRotation = glm::rotate(mat4(), 2 * 3.14f / 300 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 uranusSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        uranusModel = uranusScaling * uranusRotation * uranusTranslation * uranusSelfRotation;
        current_uranus_pos = uranusModel * vec4(0, 0, 0, 1);
    }
    return uranusModel;    
}
mat4 uranus_ringsModelMatrix() {
    mat4 uranus_ringsScaling = glm::scale(mat4(), vec3(0.4f, 0.4f, 0.4f));
    mat4 uranus_ringsTranslation;
    mat4 uranus_ringsModel;
    if (explosion == 1) {
        vec4 x = normalize(current_uranus_rings_pos) * (current_time - checkpoint) / 2.0f + current_uranus_rings_pos;
        uranus_ringsTranslation = glm::translate(mat4(), vec3(x));
        uranus_ringsModel = uranus_ringsTranslation * uranus_ringsScaling;
    }
    else {
        uranus_ringsTranslation = glm::translate(mat4(), vec3(40.5f, 0.0f, 0.0f));
        mat4 uranus_ringsRotation = glm::rotate(mat4(), 2 * 3.14f / 300 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 uranus_ringsAngle = glm::rotate(mat4(), 3.14f / 60, vec3(1.0, 0.0, 0.0));
        mat4 uranus_ringsSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        uranus_ringsModel = uranus_ringsScaling * uranus_ringsRotation * uranus_ringsTranslation * uranus_ringsAngle * uranus_ringsSelfRotation;
        current_uranus_rings_pos = uranus_ringsModel * vec4(0, 0, 0, 1);
    }
    return uranus_ringsModel;   
}
mat4 neptuneModelMatrix() {
    mat4 neptuneScaling = glm::scale(mat4(), vec3(0.4f, 0.4f, 0.4f));
    mat4 neptuneTranslation;
    mat4 neptuneModel;
    if (explosion == 1) {
        vec4 x = normalize(current_neptune_pos) * (current_time - checkpoint) / 2.0f + current_neptune_pos;
        neptuneTranslation = glm::translate(mat4(), vec3(x));
        neptuneModel = neptuneTranslation * neptuneScaling;
    }
    else {
        neptuneTranslation = glm::translate(mat4(), vec3(-45.0f, 0.0f, 0.0f));
        mat4 neptuneRotation = glm::rotate(mat4(), 2 * 3.14f / 400 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 neptuneSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 5 * current_time, vec3(0.0, 1.0, 0.0));
        neptuneModel = neptuneScaling * neptuneRotation * neptuneTranslation * neptuneSelfRotation;
        current_neptune_pos = neptuneModel * vec4(0, 0, 0, 1);
    }
    return neptuneModel;
}
mat4 plutoModelMatrix() {
    mat4 plutoScaling = glm::scale(mat4(), vec3(0.2f, 0.2f, 0.2f));
    mat4 plutoTranslation;
    mat4 plutoModel;
    if (explosion == 1) {
        vec4 x = normalize(current_pluto_pos) * (current_time - checkpoint) / 2.0f + current_pluto_pos;
        plutoTranslation = glm::translate(mat4(), vec3(x));
        plutoModel = plutoTranslation * plutoScaling;
    }
    else {
        plutoTranslation = glm::translate(mat4(), vec3(97.0f, 0.0f, 0.0f));
        mat4 plutoRotation = glm::rotate(mat4(), 2 * 3.14f / 500 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 plutoSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 6.4f * current_time, vec3(0.0, 1.0, 0.0));
        plutoModel = plutoScaling * plutoRotation * plutoTranslation * plutoSelfRotation;
        current_pluto_pos = plutoModel * vec4(0, 0, 0, 1);
    }
    return plutoModel;
}
mat4 moonModelMatrix() {
    mat4 moonScaling = glm::scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
    mat4 moonTranslation;
    mat4 moonModel;
    if (explosion == 1) {
        vec4 x = normalize(current_moon_pos) * (current_time - checkpoint) / 2.0f + current_moon_pos;
        moonTranslation = glm::translate(mat4(), vec3(x));
        moonModel = moonTranslation * moonScaling;
    }
    else {
        moonTranslation = glm::translate(mat4(), vec3(40.0f, 0.0f, 0.0f));
        mat4 moonRotation = glm::rotate(mat4(), 2 * 3.14f / 37 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 moonRotationAroundEarth = glm::rotate(mat4(), 2 * 3.14f / 27 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 moonAngle = glm::rotate(mat4(), 3.14f / 18, vec3(0.0, 0.0, 1.0));
        mat4 moonSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 27 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 moonTranslation2 = glm::translate(mat4(), vec3(6.0f, 0.0f, 0.0f));
        moonModel = moonScaling * moonRotation * moonTranslation * moonAngle * moonRotationAroundEarth * moonTranslation2 * moonSelfRotation;
        current_moon_pos = moonModel * vec4(0, 0, 0, 1);
    }
    return moonModel;    
}
mat4 ioModelMatrix() {
    mat4 ioScaling = glm::scale(mat4(), vec3(0.15f, 0.15f, 0.15f));
    mat4 ioTranslation;
    mat4 ioModel;
    if (explosion == 1) {
        vec4 x = normalize(current_io_pos) * (current_time - checkpoint) / 2.0f + current_io_pos;
        ioTranslation = glm::translate(mat4(), vec3(x));
        ioModel = ioTranslation * ioScaling;
    }
    else {
        ioTranslation = glm::translate(mat4(), vec3(67.0f, 0.0f, 0.0f));
        mat4 ioRotation = glm::rotate(mat4(), 2 * 3.14f / 100 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 ioRotationAroundJupiter = glm::rotate(mat4(), 2 * 3.14f / 8 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 ioAngle = glm::rotate(mat4(), 3.14f / 18, vec3(0.0, 0.0, 1.0));
        mat4 ioSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 8 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 ioTranslation2 = glm::translate(mat4(), vec3(10.0f, 0.0f, 0.0f));
        ioModel = ioScaling * ioRotation * ioTranslation * ioAngle * ioRotationAroundJupiter * ioTranslation2 * ioSelfRotation;
        current_io_pos = ioModel * vec4(0, 0, 0, 1);
    }
    return ioModel;    
}
mat4 titanModelMatrix() {
    mat4 titanScaling = glm::scale(mat4(), vec3(0.25f, 0.25f, 0.25f));
    mat4 titanTranslation;
    mat4 titanModel;
    if (explosion == 1) {
        vec4 x = normalize(current_titan_pos) * (current_time - checkpoint) / 2.0f + current_titan_pos;
        titanTranslation = glm::translate(mat4(), vec3(x));
        titanModel = titanTranslation * titanScaling;
    }
    else {
        titanTranslation = glm::translate(mat4(), vec3(-54.0f, 0.0f, 0.0f));
        mat4 titanRotation = glm::rotate(mat4(), 2 * 3.14f / 200 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 titanRotationAroundSaturn = glm::rotate(mat4(), 2 * 3.14f / 16 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 titanAngle = glm::rotate(mat4(), 3.14f / 18, vec3(0.0, 0.0, 1.0));
        mat4 titanSelfRotation = glm::rotate(mat4(), 2 * 3.14f / 16 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 titanTranslation2 = glm::translate(mat4(), vec3(7.0f, 0.0f, 0.0f));
        titanModel = titanScaling * titanRotation * titanTranslation * titanAngle * titanRotationAroundSaturn * titanTranslation2 * titanSelfRotation;
        current_titan_pos = titanModel * vec4(0, 0, 0, 1);
    }
    return titanModel;   
}


void depth_pass() {

    mat4 asteroidModel, mercuryModel, venusModel, earthModel, marsModel, jupiterModel, saturnModel, saturn_ringsModel, 
        uranusModel, uranus_ringsModel, neptuneModel, plutoModel, moonModel, ioModel, titanModel;

    // Setting viewport to shadow map size
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    // Binding the depth framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

    // Cleaning the framebuffer depth information (stored from the last render)
    glClear(GL_DEPTH_BUFFER_BIT);

    // Selecting the new shader program that will output the depth component
    glUseProgram(depthProgram);

    // Sending lightPos to depth fragment shader
    glUniform3f(lightPosLocation, light->lightPosition_worldspace.x,
        light->lightPosition_worldspace.y, light->lightPosition_worldspace.z);

    // Sending the light View-Projection matrices to the shader program
    mat4 light_VP1 = light->lightVP1();
    mat4 light_VP2 = light->lightVP2();
    mat4 light_VP3 = light->lightVP3();
    mat4 light_VP4 = light->lightVP4();
    mat4 light_VP5 = light->lightVP5();
    mat4 light_VP6 = light->lightVP6();

    glUniformMatrix4fv(lightVPLocation1, 1, GL_FALSE, &light_VP1[0][0]);
    glUniformMatrix4fv(lightVPLocation2, 1, GL_FALSE, &light_VP2[0][0]);
    glUniformMatrix4fv(lightVPLocation3, 1, GL_FALSE, &light_VP3[0][0]);
    glUniformMatrix4fv(lightVPLocation4, 1, GL_FALSE, &light_VP4[0][0]);
    glUniformMatrix4fv(lightVPLocation5, 1, GL_FALSE, &light_VP5[0][0]);
    glUniformMatrix4fv(lightVPLocation6, 1, GL_FALSE, &light_VP6[0][0]);


    //planets
    if (RENDER_PLANETS == 1) {

        // mercury
        mercuryModel = mercuryModelMatrix();
        glBindVertexArray(mercuryVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &mercuryModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, mercuryVertices.size());

        // venus
        venusModel = venusModelmatrix();
        glBindVertexArray(venusVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &venusModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, venusVertices.size());

        // earth
        earthModel = earthModelmatrix();
        glBindVertexArray(earthVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &earthModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, earthVertices.size());

        // mars
        marsModel = marsModelMatrix();
        glBindVertexArray(marsVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &marsModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, marsVertices.size());

        // jupiter
        jupiterModel = jupiterModelMatrix();
        glBindVertexArray(jupiterVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &jupiterModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, jupiterVertices.size());

        // saturn
        saturnModel = saturnModelmatrix();
        glBindVertexArray(saturnVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &saturnModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, saturnVertices.size());

        // saturn_rings
        saturn_ringsModel = saturn_ringsModelMatrix();
        glBindVertexArray(saturn_ringsVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &saturn_ringsModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, saturn_ringsVertices.size());

        // uranus
        uranusModel = uranusModelMatrix();
        glBindVertexArray(uranusVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &uranusModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, uranusVertices.size());

        // uranus_rings
        uranus_ringsModel = uranus_ringsModelMatrix();
        glBindVertexArray(uranus_ringsVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &uranus_ringsModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, uranus_ringsVertices.size());

        // neptune
        neptuneModel = neptuneModelMatrix();
        glBindVertexArray(neptuneVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &neptuneModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, neptuneVertices.size());

        // pluto
        plutoModel = plutoModelMatrix();
        glBindVertexArray(plutoVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &plutoModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, plutoVertices.size());

        // moon
        moonModel = moonModelMatrix();
        glBindVertexArray(moonVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &moonModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, moonVertices.size());

        // io
        ioModel = ioModelMatrix();
        glBindVertexArray(ioVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &ioModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, ioVertices.size());

        // titan
        titanModel = titanModelMatrix();
        glBindVertexArray(titanVAO);
        glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &titanModel[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, titanVertices.size());
    }


    // asteroids
    if (RENDER_ASTEROIDS == 1) {
        for (int i = 0; i < num_of_asteroids; i++) {
            glBindVertexArray(asteroidVAO);
            mat4 rotation = glm::rotate(mat4(), 2 * 3.14f / 180 * current_time, vec3(0.0, 1.0, 0.0));
            mat4 selfRotation = glm::rotate(mat4(), 2 * 3.14f / 10 * current_time, vec3(1.0, 1.0, 1.0));
            asteroidModel = rotation * modelMatrices[i] * selfRotation;
            glUniformMatrix4fv(shadowModelLocation, 1, GL_FALSE, &asteroidModel[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, asteroidVertices.size());
        }
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix) {
    mat4 asteroidModel, roomModel, skyboxModel, skyboxMVP, sunModel, sunMVP, mercuryModel, venusModel, earthModel, marsModel, jupiterModel, saturnModel, saturn_ringsModel,
        uranusModel, uranus_ringsModel, neptuneModel, plutoModel, moonModel, ioModel, titanModel, orbitMVP;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, W_WIDTH, W_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    // uploading the light parameters to the shader program
    uploadLight(*light);
    
    // Sending the shadow texture to the shaderProgram
    glActiveTexture(GL_TEXTURE1);								// Activate texture position
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);			// Assign texture to position 
    glUniform1i(depthMapSampler, 1);
    

    //sun
    if (RENDER_SUN == 1) {
        glUseProgram(shaderProgram2);

        sunModel = sunModelMatrix();
        sunMVP = projectionMatrix * viewMatrix * sunModel;
        glBindVertexArray(sunVAO);
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &sunMVP[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, sunVertices.size());
    }

    //planets
    if (RENDER_PLANETS == 1) {
        glUseProgram(shaderProgram);

        // mercury
        mercuryModel = mercuryModelMatrix();
        glBindVertexArray(mercuryVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &mercuryModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mercury_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, mercuryVertices.size());

        // venus
        venusModel = venusModelmatrix();
        glBindVertexArray(venusVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &venusModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, venus_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, venusVertices.size());

        // earth
        earthModel = earthModelmatrix();
        glBindVertexArray(earthVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &earthModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earth_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, earthVertices.size());

        // mars
        marsModel = marsModelMatrix();
        glBindVertexArray(marsVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &marsModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mars_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, marsVertices.size());

        // jupiter
        jupiterModel = jupiterModelMatrix();
        glBindVertexArray(jupiterVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &jupiterModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, jupiter_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, jupiterVertices.size());

        // saturn
        saturnModel = saturnModelmatrix();
        glBindVertexArray(saturnVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &saturnModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, saturn_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, saturnVertices.size());

        // saturn_rings
        saturn_ringsModel = saturn_ringsModelMatrix();
        glBindVertexArray(saturn_ringsVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &saturn_ringsModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, saturn_rings_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, saturn_ringsVertices.size());

        // uranus
        uranusModel = uranusModelMatrix();
        glBindVertexArray(uranusVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &uranusModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uranus_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, uranusVertices.size());

        // uranus_rings
        uranus_ringsModel = uranus_ringsModelMatrix();
        glBindVertexArray(uranus_ringsVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &uranus_ringsModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uranus_rings_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, uranus_ringsVertices.size());

        // neptune
        neptuneModel = neptuneModelMatrix();
        glBindVertexArray(neptuneVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &neptuneModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, neptune_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, neptuneVertices.size());

        // pluto
        plutoModel = plutoModelMatrix();
        glBindVertexArray(plutoVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &plutoModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pluto_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, plutoVertices.size());

        // moon
        moonModel = moonModelMatrix();
        glBindVertexArray(moonVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &moonModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, moon_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, moonVertices.size());

        // io
        ioModel = ioModelMatrix();
        glBindVertexArray(ioVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &ioModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, io_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, ioVertices.size());

        // titan
        titanModel = titanModelMatrix();
        glBindVertexArray(titanVAO);
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &titanModel[0][0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, titan_texture);
        glUniform1i(textureSampler, 0);

        glDrawArrays(GL_TRIANGLES, 0, titanVertices.size());
    }

    // skybox
    if (RENDER_SKYBOX == 1) {
        glUseProgram(shaderProgram2);
        glBindVertexArray(skyboxVAO);
        skyboxModel = glm::translate(mat4(), camera->position);
        skyboxMVP = projectionMatrix * viewMatrix * skyboxModel;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &skyboxMVP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skybox_texture);
        glUniform1i(textureSampler2, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // room
    if (RENDER_ROOM == 1) {
        glUseProgram(shaderProgram);
        glBindVertexArray(roomVAO);
        roomModel = mat4();
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &roomModel[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, room_texture);
        glUniform1i(textureSampler, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    //orbits
    if (RENDER_ORBITS == 1) {
        // orbits
        glUseProgram(orbitShaderProgram);
        vec4 color;
        if (explosion == 1)
            color = vec4(1.0f, 1.0f, 1.0f, 0.2f * exp(- (current_time-checkpoint)));
        else
            color = vec4(1.0f, 1.0f, 1.0f, 0.2f);

        // mercury orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(1.5f, 1.5f, 1.5f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // venus orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // earth orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(3.0f, 3.0f, 3.0f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // mars orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(4.0f, 4.0f, 4.0f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // jupiter orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(5.0f, 5.0f, 5.0f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // saturn orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(6.8f, 6.8f, 6.8f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // uranus orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(8.1f, 8.1f, 8.1f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // neptune orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(9.0f, 9.0f, 9.0f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // pluto orbit
        glBindVertexArray(orbitVAO);
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(9.7f, 9.7f, 9.7f));
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        if (explosion == 1)
            color = vec4(1.0f, 1.0f, 1.0f, 0.7f * exp(- (current_time-checkpoint)));
        else
            color = vec4(1.0f, 1.0f, 1.0f, 0.7f);
        // moon orbit
        glBindVertexArray(orbitVAO);
        mat4 moonAngle = glm::rotate(mat4(), 3.14f / 18, vec3(0.0, 0.0, 1.0));
        mat4 moonRotation = glm::rotate(mat4(), 2 * 3.14f / 37 * current_time, vec3(0.0, 1.0, 0.0));
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(0.45f, 0.45f, 0.45f)) * moonRotation *
            glm::translate(mat4(), vec3(13.3f, 0.0f, 0.0f)) * moonAngle;
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // io orbit
        glBindVertexArray(orbitVAO);
        mat4 ioRotation = glm::rotate(mat4(), 2 * 3.14f / 100 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 ioAngle = glm::rotate(mat4(), 3.14f / 18, vec3(0.0, 0.0, 1.0));
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(0.74f, 0.74f, 0.74f)) * ioRotation *
            glm::translate(mat4(), vec3(13.5f, 0.0f, 0.0f)) * ioAngle;
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());

        // titan orbit
        glBindVertexArray(orbitVAO);
        mat4 titanRotation = glm::rotate(mat4(), 2 * 3.14f / 200 * current_time, vec3(0.0, 1.0, 0.0));
        mat4 titanAngle = glm::rotate(mat4(), 3.14f / 18, vec3(0.0, 0.0, 1.0));
        orbitMVP = projectionMatrix * viewMatrix * glm::scale(mat4(), vec3(0.85f, 0.85f, 0.85f)) * titanRotation *
            glm::translate(mat4(), vec3(-15.9f, 0.0f, 0.0f)) * titanAngle;
        glUniformMatrix4fv(MVPLocation2, 1, GL_FALSE, &orbitMVP[0][0]);
        glUniform4fv(ColorLocation, 1, &color[0]);
        glDrawArrays(GL_TRIANGLES, 0, orbitVertices.size());
    }

    // asteroids
    if (RENDER_ASTEROIDS == 1) {
        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, asteroid_texture);
        glUniform1i(textureSampler, 0);
        for (int i = 0; i < num_of_asteroids; i++) {
            glBindVertexArray(asteroidVAO);
            mat4 rotation = glm::rotate(mat4(), 2 * 3.14f / 180 * current_time, vec3(0.0, 1.0, 0.0));
            mat4 selfRotation = glm::rotate(mat4(), 2 * 3.14f / 10 * current_time, vec3(1.0, 1.0, 1.0));
            asteroidModel = rotation * modelMatrices[i] * selfRotation;
            glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &asteroidModel[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, asteroidVertices.size());
        }
    }

    //Explosion
    if (RENDER_EXPLOSION == 1) {
        glUseProgram(explosionShaderProgram);
        //sun surface
        glBindVertexArray(sunVAO);
        sunModel = sunModelMatrix();
        sunMVP = projectionMatrix * viewMatrix * sunModel;
        glUniformMatrix4fv(MVPLocation3, 1, GL_FALSE, &sunMVP[0][0]);

        float time2 = glfwGetTime() - checkpoint;
        glUniform1f(timeLocation, time2);
        float x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        glm::vec3 random = glm::vec3(x, y, z);
        glUniform3fv(randLocation, 1, &random[0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun_texture);
        glUniform1i(textureSampler3, 0);

        glDrawArrays(GL_TRIANGLES, 0, sunVertices.size());

        // inner sun surface
        glBindVertexArray(sunVAO);
        sunModel = sunModelMatrix() * glm::rotate(mat4(), 2 * 3.14f / 120, vec3(0.0, 1.0, 0.0));
        sunMVP = projectionMatrix * viewMatrix * sunModel;
        glUniformMatrix4fv(MVPLocation3, 1, GL_FALSE, &sunMVP[0][0]);

        time2 = glfwGetTime() - checkpoint;
        glUniform1f(timeLocation, time2);
        x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        random = glm::vec3(x, y, z);
        glUniform3fv(randLocation, 1, &random[0]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun_texture);
        glUniform1i(textureSampler3, 0);

        glDrawArrays(GL_TRIANGLES, 0, sunVertices.size());

        light = new Light(window,
            vec4{ 1, 1, 1, 1 },
            vec4{ 1, 1, 1, 1 },
            vec4{ 1, 1, 1, 1 },
            vec3{ 0, 0, 0 },
            exp(7-time2/5)
        );
      
    }

}


void free() {
    glDeleteBuffers(1, &skyboxVerticiesVBO);
    glDeleteBuffers(1, &skyboxNormalsVBO);
    glDeleteTextures(1, &skybox_texture);
    glDeleteVertexArrays(1, &skyboxVAO);

    glDeleteBuffers(1, &roomVerticiesVBO);
    glDeleteBuffers(1, &roomNormalsVBO);
    glDeleteTextures(1, &room_texture);
    glDeleteVertexArrays(1, &roomVAO);

    glDeleteBuffers(1, &asteroidVerticiesVBO);
    glDeleteBuffers(1, &asteroidNormalsVBO);
    glDeleteTextures(1, &asteroid_texture);
    glDeleteVertexArrays(1, &asteroidVAO);

    glDeleteBuffers(1, &orbitVerticiesVBO);
    glDeleteVertexArrays(1, &orbitVAO);

    glDeleteBuffers(1, &sunVerticiesVBO);
    glDeleteBuffers(1, &sunNormalsVBO);
    glDeleteBuffers(1, &sunUVVBO);
    glDeleteTextures(1, &sun_texture);
    glDeleteVertexArrays(1, &sunVAO);

    glDeleteBuffers(1, &mercuryVerticiesVBO);
    glDeleteBuffers(1, &mercuryNormalsVBO);
    glDeleteBuffers(1, &mercuryUVVBO);
    glDeleteTextures(1, &mercury_texture);
    glDeleteVertexArrays(1, &mercuryVAO);

    glDeleteBuffers(1, &venusVerticiesVBO);
    glDeleteBuffers(1, &venusNormalsVBO);
    glDeleteBuffers(1, &venusUVVBO);
    glDeleteTextures(1, &venus_texture);
    glDeleteVertexArrays(1, &venusVAO);

    glDeleteBuffers(1, &earthVerticiesVBO);
    glDeleteBuffers(1, &earthNormalsVBO);
    glDeleteBuffers(1, &earthUVVBO);
    glDeleteTextures(1, &earth_texture);
    glDeleteVertexArrays(1, &earthVAO);

    glDeleteBuffers(1, &marsVerticiesVBO);
    glDeleteBuffers(1, &marsNormalsVBO);
    glDeleteBuffers(1, &marsUVVBO);
    glDeleteTextures(1, &mars_texture);
    glDeleteVertexArrays(1, &marsVAO);

    glDeleteBuffers(1, &jupiterVerticiesVBO);
    glDeleteBuffers(1, &jupiterNormalsVBO);
    glDeleteBuffers(1, &jupiterUVVBO);
    glDeleteTextures(1, &jupiter_texture);
    glDeleteVertexArrays(1, &jupiterVAO);

    glDeleteBuffers(1, &saturnVerticiesVBO);
    glDeleteBuffers(1, &saturnNormalsVBO);
    glDeleteBuffers(1, &saturnUVVBO);
    glDeleteTextures(1, &saturn_texture);
    glDeleteVertexArrays(1, &saturnVAO);

    glDeleteBuffers(1, &saturn_ringsVerticiesVBO);
    glDeleteBuffers(1, &saturn_ringsNormalsVBO);
    glDeleteBuffers(1, &saturn_ringsUVVBO);
    glDeleteTextures(1, &saturn_rings_texture);
    glDeleteVertexArrays(1, &saturn_ringsVAO);

    glDeleteBuffers(1, &uranusVerticiesVBO);
    glDeleteBuffers(1, &uranusNormalsVBO);
    glDeleteBuffers(1, &uranusUVVBO);
    glDeleteTextures(1, &uranus_texture);
    glDeleteVertexArrays(1, &uranusVAO);

    glDeleteBuffers(1, &uranus_ringsVerticiesVBO);
    glDeleteBuffers(1, &uranus_ringsNormalsVBO);
    glDeleteBuffers(1, &uranus_ringsUVVBO);
    glDeleteTextures(1, &uranus_rings_texture);
    glDeleteVertexArrays(1, &uranus_ringsVAO);

    glDeleteBuffers(1, &neptuneVerticiesVBO);
    glDeleteBuffers(1, &neptuneNormalsVBO);
    glDeleteBuffers(1, &neptuneUVVBO);
    glDeleteTextures(1, &neptune_texture);
    glDeleteVertexArrays(1, &neptuneVAO);

    glDeleteBuffers(1, &plutoVerticiesVBO);
    glDeleteBuffers(1, &plutoNormalsVBO);
    glDeleteBuffers(1, &plutoUVVBO);
    glDeleteTextures(1, &pluto_texture);
    glDeleteVertexArrays(1, &plutoVAO);

    glDeleteBuffers(1, &moonVerticiesVBO);
    glDeleteBuffers(1, &moonNormalsVBO);
    glDeleteBuffers(1, &moonUVVBO);
    glDeleteTextures(1, &moon_texture);
    glDeleteVertexArrays(1, &moonVAO);

    glDeleteBuffers(1, &ioVerticiesVBO);
    glDeleteBuffers(1, &ioNormalsVBO);
    glDeleteBuffers(1, &ioUVVBO);
    glDeleteTextures(1, &io_texture);
    glDeleteVertexArrays(1, &ioVAO);

    glDeleteBuffers(1, &titanVerticiesVBO);
    glDeleteBuffers(1, &titanNormalsVBO);
    glDeleteBuffers(1, &titanUVVBO);
    glDeleteTextures(1, &titan_texture);
    glDeleteVertexArrays(1, &titanVAO);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgram2);
    glDeleteProgram(orbitShaderProgram);
    glDeleteProgram(depthProgram);

    glfwTerminate();
}


void mainLoop() {

    do {
        current_time = glfwGetTime();

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            RENDER_SUN = 1;
            RENDER_PLANETS = 1;
            RENDER_SKYBOX = 1;
            RENDER_ORBITS = 1;
            RENDER_ROOM = 0;
            RENDER_ASTEROIDS = 0;
            RENDER_EXPLOSION = 0;
            explosion = 0;

        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            RENDER_SUN = 1;
            RENDER_PLANETS = 1;
            RENDER_SKYBOX = 0;
            RENDER_ORBITS = 1;
            RENDER_ROOM = 1;
            RENDER_ASTEROIDS = 0;
            RENDER_EXPLOSION = 0;
            explosion = 0;

        }
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            RENDER_SUN = 1;
            RENDER_PLANETS = 0;
            RENDER_SKYBOX = 0;
            RENDER_ORBITS = 0;
            RENDER_ROOM = 1;
            RENDER_ASTEROIDS = 1;
            RENDER_EXPLOSION = 0;
            explosion = 0;

        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            RENDER_SUN = 0;
            RENDER_EXPLOSION = 1;
            explosion = 1;
            checkpoint = glfwGetTime();
            
        }
        if (explosion == 0) {
            light = new Light(window,
                vec4{ 1, 1, 1, 1 },
                vec4{ 1, 1, 1, 1 },
                vec4{ 1, 1, 1, 1 },
                vec3{ 0, 0, 0 },
                150.0f
            );
        }

        depth_pass();

        // camera
        camera->update();
        mat4 projectionMatrix = camera->projectionMatrix;
        mat4 viewMatrix = camera->viewMatrix;

        lighting_pass(viewMatrix, projectionMatrix);

        glfwSwapBuffers(window);

        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);
}


void initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
            " If you have an Intel GPU, they are not 3.3 compatible." +
            "Try the 2.1 version.\n"));
    }
    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Black background color
    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Enable blending for transparency
    // change alpha in fragment shader
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Log
    logGLParameters();

    // Create camera
    camera = new Camera(window);

    
}


int main(void) {

    try {
        initialize();
        createContext();
        mainLoop();
        free();
    }
    catch (exception& ex) {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }
    return 0;
}