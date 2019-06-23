/*!
 * @file
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <assert.h>
#include <math.h>

#include <student/gpu.h>
#include <student/student_shader.h>
#include <student/uniforms.h>

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(GPUVertexShaderOutput *const      output,
                        GPUVertexShaderInput const *const input,
                        GPU const                         gpu) {
  /// \todo Naimplementujte vertex shader, který transformuje vstupní vrcholy do
  /// clip-space.<br>
  /// <b>Vstupy:</b><br>
  /// Vstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu ve
  /// world-space (vec3) a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).<br>
  /// <b>Výstupy:</b><br>
  /// Výstupní vrchol by měl v nultém atributu obsahovat pozici vrcholu (vec3)
  /// ve world-space a v prvním
  /// atributu obsahovat normálu vrcholu ve world-space (vec3).
  /// Výstupní vrchol obsahuje pozici a normálu vrcholu proto, že chceme počítat
  /// osvětlení ve world-space ve fragment shaderu.<br>
  /// <b>Uniformy:</b><br>
  /// Vertex shader by měl pro transformaci využít uniformní proměnné obsahující
  /// view a projekční matici.
  /// View matici čtěte z uniformní proměnné "viewMatrix" a projekční matici
  /// čtěte z uniformní proměnné "projectionMatrix".
  /// Zachovejte jména uniformních proměnných a pozice vstupních a výstupních
  /// atributů.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Využijte vektorové a maticové funkce.
  /// Nepředávajte si data do shaderu pomocí globálních proměnných.
  /// Pro získání dat atributů použijte příslušné funkce vs_interpret*
  /// definované v souboru program.h.
  /// Pro získání dat uniformních proměnných použijte příslušné funkce
  /// shader_interpretUniform* definované v souboru program.h.
  /// Vrchol v clip-space by měl být zapsán do proměnné gl_Position ve výstupní
  /// struktuře.<br>
  /// <b>Seznam funkcí, které jistě použijete</b>:
  ///  - gpu_getUniformsHandle()
  ///  - getUniformLocation()
  ///  - shader_interpretUniformAsMat4()
  ///  - vs_interpretInputVertexAttributeAsVec3()
  ///  - vs_interpretOutputVertexAttributeAsVec3()

  // odkaz na uniformni prommene
  Uniforms const handle_uniforms = gpu_getUniformsHandle(gpu);
  // nacteme si pozice matic
  UniformLocation const projectionMatrix  = getUniformLocation(gpu, "projectionMatrix");
  UniformLocation const viewMatrix = getUniformLocation(gpu, "viewMatrix");
  // nacteme si matice 
  Mat4 const *projection_matrix = shader_interpretUniformAsMat4(handle_uniforms, projectionMatrix);
  Mat4 const *view_matrix = shader_interpretUniformAsMat4(handle_uniforms, viewMatrix);

  // nacteme si pozici a normalu, funkce z program.h
  Vec3 const *position = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
  Vec3 const *normal   = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);

  // chceme implementovat nasledujici vzorec
  // gl_Position = projectionMatrix*viewMatrix*vec4(position,1.f);\n
  // docasne promenne
  Vec4 temp1;
  Mat4 temp2;
  // vec4(position,1.f)
  copy_Vec3Float_To_Vec4(&temp1, position, 1.f);
  // projectionMatrix*viewMatrix
  multiply_Mat4_Mat4(&temp2, projection_matrix, view_matrix);
  // pronasobime zbytek -> projectionMatrix*viewMatrix*vec4(position,1.f);\n
  // ulozime do prednastavene pozice
  multiply_Mat4_Vec4(&output->gl_Position, &temp2, &temp1);

  // nacteme si pozice pro ws (pozice & jeji normala)
  Vec3 *const position_ws  = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
  Vec3 *const normal_ws = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);

  // vektor pro pozici
  init_Vec3( 
    position_ws, 
    position->data[0], 
    position->data[1], 
    position->data[2]
  );

  // vektor pro normalu
  init_Vec3(
    normal_ws, 
    normal->data[0],
    normal->data[1], 
    normal->data[2]  
  );

}

void phong_fragmentShader(GPUFragmentShaderOutput *const      output,
                          GPUFragmentShaderInput const *const input,
                          GPU const                           gpu) {
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací
  /// model s phongovým stínováním.<br>
  
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat
  /// interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  
  /// <b>Výstup:</b><br>
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici
  /// světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v
  /// rasterizaci může dojít ke zkrácení.
  
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  
  /// Shininess faktor nastavte na 40.f
  
  /// Difuzní barvu materiálu nastavte podle normály povrchu.
  /// V případě, že normála směřuje kolmo vzhůru je difuzní barva čistě bílá.
  /// V případě, že normála směřuje vodorovně nebo dolů je difuzní barva čiště zelená.
  /// Difuzní barvu spočtěte lineární interpolací zelené a bíle barvy pomocí interpolačního parameteru t.
  /// Interpolační parameter t spočtěte z y komponenty normály pomocí t = y*y (samozřejmě s ohledem na negativní čísla).
  
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  
  /// Barvu světla nastavte na bílou.

  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  
  // struktura jak ve cviceni
  // positionWs a normalWs
  Vec3 const *position_ws = fs_interpretInputAttributeAsVec3(gpu, input, 0);
  Vec3 const *normal_ws = fs_interpretInputAttributeAsVec3(gpu, input, 1);

  // odkaz na uniformni prommene
  // cameraPos a lightPos
  Uniforms const handle_uniforms = gpu_getUniformsHandle(gpu);
  // nacteme si kameru a pozici svetla
  UniformLocation const cameraPosition = getUniformLocation(gpu, "cameraPosition");
  UniformLocation const lightPosition  = getUniformLocation(gpu, "lightPosition");
  // ulozime si ji
  Vec3 const *camera_position = shader_interpretUniformAsVec3(handle_uniforms, cameraPosition);
  Vec3 const *light_position = shader_interpretUniformAsVec3(handle_uniforms, lightPosition);

  // nadefinujeme si potrebne vektory a promenne
  Vec3 fColor;
  // pro material
  Vec3 dM;
  Vec3 sM;
  // difuzni a spekularni faktor
  float dF;
  float sF;
  // odlesky
  float shininess;
  // parametr T
  float t;
  //
  Vec3 N; // normalovy vektor
  Vec3 L; // vektor svetla
  Vec3 V; // vektor divaka
  Vec3 R; // vektor odrazeni

  // prace s hodnotami
  // nastavime spekularni barvu na bilou
  init_Vec3(&sM, 1.0f, 1.0f, 1.0f);
  init_Vec3(&dM, 0.0f, 1.0f, 0.0f);
  // nastavime shininess faktor
  shininess = 40.f;
  // nastaveni vektoru N
  normalize_Vec3(&N, normal_ws);
  // nastaveni vektoru L
  sub_Vec3(&L, light_position, position_ws); 
  normalize_Vec3(&L, &L);
  // nastaveni vektoru V
  sub_Vec3(&V, position_ws, camera_position); 
  normalize_Vec3(&V, &V);
  // nastaveni vektoru R
  // rozdil oproti cviku je pocitani pozice vektoru svetla
  reflect(&R, &L, &N);

  // spocitame parametr t s orezem
  t = pow(N.data[1], 2);
  t = (t < 0) ? 0 : (t > 1) ? 1 : t;  

  // k povrchu, nastavime nulu
  if (t == 0.f)
    // nastavime difuzni barvu 
    init_Vec3(&dM, 1.0f, 1.0f, 1.0f);
  // vodorovne nebo dolu
  else if (t == 1.f)
    // nastavime difuzni barvu 
    init_Vec3(&dM , 0.0f, 1.0f, 0.0f );
  // michame
  else
    // smichame barvy, vyuzijeme toho, ze uz bila i zelena existuje
    mix_Vec3(&dM, &dM, &sM, t);

  // difuzni faktor s orezen
  dF = dot_Vec3(&N,&L);
  dF = (dF < 0) ? 0 : (dF > 1) ? 1 : dF; 
  // spekularni faktor s orezem a odlesky
  sF = dot_Vec3(&V, &R);
  sF = (sF < 0) ? 0 : (sF > 1) ? 1 : sF;  
  sF = powf(sF, shininess);

  // spocitame vzorec
  // dF*dM*dL + sF*sM*sL
  multiply_Vec3_Float(&dM , &dM , dF);
  multiply_Vec3_Float(&sM, &sM, sF);
  // pridame vektor
  add_Vec3(&fColor, &dM, &sM);
  // ulozime do barvy
  copy_Vec3Float_To_Vec4(&output->color, &fColor, 1.f);
}

/// @}
