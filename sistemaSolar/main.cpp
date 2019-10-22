
#include <vart/scene.h>
#include <vart/light.h>
#include <vart/sphere.h>
#include <vart/transform.h>
#include <vart/rotationaction.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/texture.h>

#include <iostream>
#include <math.h>

using namespace std;
using namespace VART;

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler
{
public:
    MyIHClass() : terraRadians(0), terraSolRadians(0), luaRadians(0) {}
    virtual ~MyIHClass() {}
    virtual void OnIdle()
    {
        terraRadians += 0.05;
        terraSolRadians += 0.001;
        luaRadians += 0.015;
        terraSolRotPtr->MakeZRotation(terraSolRadians);
        terraRotPtr->MakeZRotation(terraRadians);
        luaRotPtr->MakeZRotation(luaRadians);
        viewerPtr->PostRedisplay();
    }
    //protected:
    Transform *terraRotPtr;
    Transform *terraSolRotPtr;
    Transform *luaRotPtr;

private:
    float terraRadians;
    float terraSolRadians;
    float luaRadians;
};

int main(int argc, char *argv[])
{
    if(argc < 4) {
        cerr << "Informe o nome dos arquivos com as texturas do sol, da terra e da lua respectivamente";
        return 0;
    }
    ViewerGlutOGL::Init(&argc, argv); //Inicializa a GLUT
    static Scene scene;
    static ViewerGlutOGL viewer;
    Camera camera(Point4D(0, 0, 10), Point4D(0, 0, 0), Point4D(0, 1, 0, 0));

    Sphere terra(0.5);
    Texture terraTex;
    Material terraMat;
    
    Sphere sol(1.0);
    Texture solTex;
    Material solMat;

    Sphere lua(0.1);
    Texture luaTex;
    Material luaMat;

    if (terraTex.LoadFromFile(argv[1]) && solTex.LoadFromFile(argv[2]) && luaTex.LoadFromFile(argv[3]))
    {
        terraMat.SetTexture(terraTex);
        solMat.SetTexture(solTex);
        luaMat.SetTexture(luaTex);
    }
    else
    {
        cerr << "Nao foi possível realizar a leitura do arquivo";
        return 0;
    }

    sol.SetMaterial(solMat);
    terra.SetMaterial(terraMat);
    lua.SetMaterial(luaMat);

    Transform terraTranslat;
    Transform terraRot;
    Transform terraSolRot;
    terraTranslat.MakeTranslation(4, 0, 0);
    terraRot.AddChild(terra);
    terraTranslat.AddChild(terraRot);
    terraSolRot.AddChild(terraTranslat);
    
    Transform luaTranslat;
    Transform luaRot;
    luaTranslat.MakeTranslation(1, 0, 0);
    luaTranslat.AddChild(lua);
    luaRot.AddChild(luaTranslat);

    terraTranslat.AddChild(luaRot);
    scene.AddObject(&terraSolRot);
    scene.AddObject(&sol);
    scene.AddCamera(&camera);
    scene.AddLight(Light::BRIGHT_AMBIENT());
    scene.MakeCameraViewAll();

    MyIHClass idleHandler;
    idleHandler.terraRotPtr = &terraRot;
    idleHandler.terraSolRotPtr = &terraSolRot;
    idleHandler.luaRotPtr = &luaRot;

    viewer.SetTitle("Sistema Solar");
    viewer.SetScene(scene);
    viewer.SetIdleHandler(&idleHandler);

    scene.DrawLightsOGL();
    ViewerGlutOGL::MainLoop();
    return 1;
};