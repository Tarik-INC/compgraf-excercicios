
#include <vart/scene.h>
#include <vart/light.h>
#include <vart/sphere.h>
#include <vart/transform.h>
#include <vart/rotationaction.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/texture.h>

#include <iostream> 

using namespace std;
using namespace VART;

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler
{
public:
    MyIHClass() : redRadians(0), greenRadians(0) {}
    virtual ~MyIHClass() {}
    virtual void OnIdle()
    {
        redRadians += 0.01;
        redRotPtr->MakeZRotation(redRadians);
        greenRadians += 0.01;
        greenRotPtr->MakeYRotation(greenRadians);
        viewerPtr->PostRedisplay();
    }
    //protected:
    VART::Transform *redRotPtr;
    VART::Transform *greenRotPtr;

private:
    float redRadians;
    float greenRadians;
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
    Camera camera(Point4D(0, 0, 6), Point4D(0, 0, 0), Point4D(0, 0, 1, 0));

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
    Transform solRot;
    cout << "Foi ate aqui";

    terraTranslat.MakeTranslation(4, 0, 0);
    terraTranslat.AddChild(terra);
    terraRot.AddChild(terraTranslat);

    scene.AddObject(&terraTranslat);
    scene.AddObject(&sol);
    scene.AddCamera(&camera);
    scene.AddLight(Light::BRIGHT_AMBIENT());
    MyIHClass idleHandler;

    viewer.SetTitle("Sistema Solar");
    viewer.SetScene(scene);
    // viewer.SetIdleHandler(&idleHandler);

    scene.DrawLightsOGL();
    ViewerGlutOGL::MainLoop();
    return 1;
};