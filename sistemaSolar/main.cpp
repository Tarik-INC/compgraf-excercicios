
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
    MyIHClass() : terraRadians(0), terraSolRadians(0) {}
    virtual ~MyIHClass() {}
    virtual void OnIdle()
    {
        terraRadians += 0.05;
        terraSolRadians += 0.001;
        terraSolRotPtr->MakeZRotation(terraSolRadians);
        terraRotPtr->MakeZRotation(terraRadians);
        viewerPtr->PostRedisplay();
    }
    //protected:
    Transform *terraRotPtr;
    Transform *terraSolRotPtr;

private:
    float terraRadians;
    float terraSolRadians;
};

int main(int argc, char *argv[])
{
    if(argc < 3) {
        cerr << "Informe o nome dos arquivos com as texturas do sol e da terra respectivamente";
        return 0;
    }
    ViewerGlutOGL::Init(&argc, argv); //Inicializa a GLUT
    static Scene scene;
    static ViewerGlutOGL viewer;
    Camera camera(Point4D(0, 0, 20), Point4D(0, 0, 0), Point4D(0, 0, 1, 0));

    Sphere terra(0.5);
    Texture terraTex;
    Material terraMat;
    Sphere sol(1.0);
    Texture solTex;
    Material solMat;

    if (terraTex.LoadFromFile(argv[1]) && solTex.LoadFromFile(argv[2]))
    {
        terraMat.SetTexture(terraTex);
        solMat.SetTexture(solTex);
    }
    else
    {
        cerr << "Nao foi possível realizar a leitura do arquivo";
        return 0;
    }

    sol.SetMaterial(solMat);
    terra.SetMaterial(terraMat);


    Transform terraTranslat;
    Transform terraRot;
    Transform terraSolRot;
    terraTranslat.MakeTranslation(4.0, 0, 0);
    terraRot.AddChild(terra);
    terraTranslat.AddChild(terraRot);
    terraSolRot.AddChild(terraTranslat);
    

    scene.AddObject(&terraSolRot);
    // scene.AddObject(&terraTranslat);
    scene.AddObject(&sol);
    scene.AddCamera(&camera);
    scene.AddLight(Light::BRIGHT_AMBIENT());
    scene.MakeCameraViewAll();

    MyIHClass idleHandler;
    idleHandler.terraRotPtr = &terraRot;
    idleHandler.terraSolRotPtr = &terraSolRot;

    viewer.SetTitle("Sistema Solar");
    viewer.SetScene(scene);
    viewer.SetIdleHandler(&idleHandler);

    scene.DrawLightsOGL();
    ViewerGlutOGL::MainLoop();
    return 1;
};