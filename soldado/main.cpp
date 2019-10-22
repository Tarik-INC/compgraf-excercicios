#include <vart/scene.h>
#include <vart/box.h>
#include <vart/cylinder.h>
#include <vart/light.h>
#include <vart/meshobject.h>
#include <vart/sphere.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>

#include <iostream>

using namespace std;
using namespace VART;

int main(int argc, char* argv[]) {
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT

    if(argc < 2) {
        cerr << "Error: no filename given.\n Correct usage: " << argv[0] << "filename\n"
        <<"where 'filename' is a obj file.\n";
        return 1;
    }

    static Scene scene;
    static ViewerGlutOGL viewer;

    Camera camera(Point4D(0,0,6), Point4D(0, 0, 0), Point4D(0,1,0,0));
    
    list<MeshObject*> objects;
    MeshObject::ReadFromOBJ(argv[1], &objects);
    

    list<MeshObject*>::iterator it = objects.begin();
    for(; it != objects.end(); ++it) {
        scene.AddObject(*it);
    }

    scene.AddLight(Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);
    scene.MakeCameraViewAll();

    viewer.SetTitle("Soldado");
    viewer.SetScene(scene);

    scene.DrawLightsOGL();
    ViewerGlutOGL::MainLoop();

    return 0;
}