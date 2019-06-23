/*!
 * @file
 * @brief This file contains implementation of cpu side for phong shading.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 *
 */

#include <assert.h>
#include <math.h>

#include <student/buffer.h>
#include <student/bunny.h>
#include <student/camera.h>
#include <student/gpu.h>
#include <student/linearAlgebra.h>
#include <student/mouseCamera.h>
#include <student/student_cpu.h>
#include <student/student_pipeline.h>
#include <student/student_shader.h>
#include <student/swapBuffers.h>
#include <student/uniforms.h>
#include <student/vertexPuller.h>

#include <student/globals.h>

/**
 * @brief This structure contains all global variables for this method.
 */
struct PhongVariables {
  /// This variable contains GPU handle.
  GPU gpu;
  /// This variable contains light poistion in world-space.
  Vec3 lightPosition;
  // ulozeni hlavniho objektu programu
  ProgramID program;
  // pro vbo a ebo ze cvika, dva buffery
  BufferID buffers[2];
  // pullery pro buffery
  VertexPullerID pullers[2];

} phong;  ///<instance of all global variables for triangle example.

/// \addtogroup cpu_side Úkoly v cpu části
/// @{

void phong_onInit(int32_t width, int32_t height) {
  // create gpu
  phong.gpu = cpu_createGPU();
  // set viewport size
  cpu_setViewportSize(phong.gpu, (size_t)width, (size_t)height);
  // init matrices
  cpu_initMatrices(width, height);
  // init lightPosition
  init_Vec3(&phong.lightPosition, 1000.f, 1000.f, 1000.f);

/// \todo Doprogramujte inicializační funkci.
/// Zde byste měli vytvořit buffery na GPU, nahrát data do bufferů, vytvořit
/// vertex puller a správně jej nakonfigurovat, vytvořit program, připojit k
/// němu shadery a nastavit interpolace.
/// Také byste zde měli zarezervovat unifromní proměnné pro matice, pozici
/// kamery, světla a další vaše proměnné.
/// Do bufferů nahrajte vrcholy králička (pozice, normály) a indexy na vrcholy
/// ze souboru bunny.h.
/// Shader program by měl odkazovat na funkce/shadery v souboru
/// student_shader.h.
/// V konfiguraci vertex pulleru nastavte dvě čtecí hlavy.
/// Jednu pro pozice vrcholů a druhou pro normály vrcholů.
/// Nultý vertex/fragment atribut by měl obsahovat pozici vertexu.
/// První vertex/fragment atribut by měl obsahovat normálu vertexu.
/// Budete využívat minimálně 4 uniformní proměnné
/// Uniformní proměnná pro view matici by měla být pojmenována "viewMatrix".
/// Uniformní proměnná pro projekční matici by měla být pojmenována
/// "projectionMatrix".
/// Uniformní proměnná pro pozici kamery by se měla jmenovat "cameraPosition".
/// Uniformní proměnná pro pozici světla by se měla jmenovat "lightPosition".
/// Je důležité udržet pozice atributů a názvy uniformních proměnných z důvodu
/// akceptačních testů.
/// Interpolace vertex atributů do fragment atributů je také potřeba nastavit.
/// Oba vertex atributy nastavte na \link SMOOTH\endlink interpolaci -
/// perspektivně korektní interpolace.<br>
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - cpu_reserveUniform()
///  - cpu_createProgram()
///  - cpu_attachVertexShader()
///  - cpu_attachFragmentShader()
///  - cpu_setAttributeInterpolation()
///  - cpu_createBuffers()
///  - cpu_bufferData()
///  - cpu_createVertexPullers()
///  - cpu_setVertexPullerHead()
///  - cpu_enableVertexPullerHead()
///  - cpu_setIndexing()

  // Pozice uniformnich promennych, podobne jako na cviku
  cpu_reserveUniform(phong.gpu, "projectionMatrix", UNIFORM_MAT4);
  cpu_reserveUniform(phong.gpu, "viewMatrix"      , UNIFORM_MAT4);
  // to same, ale pro kameru a svetlo
  cpu_reserveUniform(phong.gpu, "cameraPosition"  , UNIFORM_VEC3);
  cpu_reserveUniform(phong.gpu, "lightPosition"   , UNIFORM_VEC3);
  

  // vytvoreni programu
  phong.program = cpu_createProgram(phong.gpu);

  // SHADERY
  // prilepime vertex a fragment shader, jmena jsou v student_shader.h
  cpu_attachVertexShader(phong.gpu, phong.program, phong_vertexShader);
  cpu_attachFragmentShader(phong.gpu, phong.program, phong_fragmentShader);
  // nastaveni interpolace
  cpu_setAttributeInterpolation(phong.gpu, phong.program, 0, ATTRIB_VEC3, SMOOTH);
  cpu_setAttributeInterpolation(phong.gpu, phong.program, 1, ATTRIB_VEC3, SMOOTH);
  
  // DATA
  // vytvoreni bufferu
  cpu_createBuffers(phong.gpu, 2, phong.buffers);
  // nahrajeme si do bufferu pozici vrcholu kralicka
  cpu_bufferData(phong.gpu, phong.buffers[0], sizeof(bunnyVertices), bunnyVertices);
  cpu_bufferData(phong.gpu, phong.buffers[1], sizeof(bunnyIndices), bunnyIndices);

  // PULLERY
  // vytvorime si misto pro dva pullery
  cpu_createVertexPullers(phong.gpu, 2, phong.pullers);

  // hlava 1, ebo v projektu
  cpu_setVertexPullerHead(
    phong.gpu,
    *phong.pullers,         // pullery
    0,                      // id hlavy
    phong.buffers[0],       // zdroj cteni, cteme Verticies
    sizeof(float) * 0,      // cteme prvni trojci
    sizeof(float) * 6       // skok stejny
  );

  // hlava 2, vbo v projektu
  cpu_setVertexPullerHead(
    phong.gpu,          
    *phong.pullers,         // id pullery
    1,                      // id hlavy
    phong.buffers[0],       // zdroj cteni, cteme Verticies
    sizeof(float) * 3,      // cteme ob tri polozky, jelikoz je to dane v bunny.h
    sizeof(float) * 6       // stejne jako na cviku
  );

  // povolime pullery, jako glEnableVertexAttribArray
  cpu_enableVertexPullerHead(phong.gpu, *phong.pullers, 0);
  cpu_enableVertexPullerHead(phong.gpu, *phong.pullers, 1);

  // musime nastavit indexovani, na cviku to byla ta normala
  cpu_setIndexing(
    phong.gpu,            
    *phong.pullers,         // id pullery
    phong.buffers[1],       // podle Indicies
    4                       // velikost indexu, int
  );

}

/// @}

void phong_onExit() { cpu_destroyGPU(phong.gpu); }

/// \addtogroup cpu_side
/// @{

void phong_onDraw(SDL_Surface* surface) {
  assert(surface != NULL);

  // clear depth buffer
  cpu_clearDepth(phong.gpu, +INFINITY);
  Vec4 color;
  init_Vec4(&color, .1f, .1f, .1f, 1.f);
  // clear color buffer
  cpu_clearColor(phong.gpu, &color);

/// \todo Doprogramujte kreslící funkci.
/// Zde byste měli aktivovat shader program, aktivovat vertex puller, nahrát
/// data do uniformních proměnných a
/// vykreslit trojúhelníky pomocí funkce cpu_drawTriangles.
/// Data pro uniformní proměnné naleznete v externích globálních proměnnénych
/// viewMatrix, projectionMatrix, cameraPosition a globální proměnné
/// phong.lightPosition.<br>
/// <b>Seznam funkcí, které jistě využijete:</b>
///  - cpu_useProgram()
///  - cpu_bindVertexPuller()
///  - cpu_uniformMatrix4fv()
///  - cpu_uniform3f()
///  - cpu_drawTriangles()
///  - getUniformLocation()

  // PROGRAM
  cpu_useProgram(phong.gpu, phong.program);

  // UNIFORMS
  // data z matic do uniformnich promennych, stejne jako cviko
  cpu_uniformMatrix4fv(
    phong.gpu, 
    getUniformLocation(phong.gpu, "viewMatrix"),
    (const float*) &viewMatrix
  );

  cpu_uniformMatrix4fv(
    phong.gpu, 
    getUniformLocation(phong.gpu, "projectionMatrix"),
    (const float*) &projectionMatrix
  );

  // to same pro kameru a svetlo
  cpu_uniform3f(
    phong.gpu,
    getUniformLocation(phong.gpu, "cameraPosition"),
    cameraPosition.data[0], // bereme po trojicich
    cameraPosition.data[1],
    cameraPosition.data[2]
  );

  cpu_uniform3f(
    phong.gpu,
    getUniformLocation(phong.gpu, "lightPosition"),
    phong.lightPosition.data[0],
    phong.lightPosition.data[1],
    phong.lightPosition.data[2]
  );

  // BINDING
  // nastavime pullery a muzeme kreslit
  cpu_bindVertexPuller(phong.gpu, *phong.pullers);
  // samotne vykresleni
  cpu_drawTriangles(
    phong.gpu,
    sizeof(bunnyIndices) / sizeof(VertexIndex) // musime podelit pocet indexu, jinak vykreslujeme nesnysk
  );

  // copy image from gpu to SDL surface
  cpu_swapBuffers(surface, phong.gpu);
}

/// @}
