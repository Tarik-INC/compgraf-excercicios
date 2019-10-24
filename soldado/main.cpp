#include <vart/scene.h>
#include <vart/box.h>
#include <vart/cylinder.h>
#include <vart/light.h>
#include <vart/meshobject.h>
#include <vart/sphere.h>
#include <vart/transform.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>
#include <vart/point4d.h>

#include <iostream>
#include <math.h>

using namespace std;
using namespace VART;

const unsigned int MAX_KEYS = 10; // max number of keys to keep control
enum Key
{
    UP,
    DOWN,
    RIGHT,
    LEFT
};
bool keyPressed[MAX_KEYS];

// Define the keyboard handler
class KeyboardHandler : public ViewerGlutOGL::KbHandler
{
public:
    KeyboardHandler()
    {
        for (unsigned int i = 0; i < MAX_KEYS; ++i)
            keyPressed[i] = false;
    }
    virtual void OnKeyDown(int key)
    {
        switch (key)
        {
        case KEY_LEFT:
            keyPressed[LEFT] = true;
            break;
        case KEY_RIGHT:
            keyPressed[RIGHT] = true;
            break;
        case KEY_UP:
            keyPressed[UP] = true;
            break;
        case KEY_DOWN:
            keyPressed[DOWN] = true;
            break;
        }
    }
    virtual void OnKeyUp(int key)
    {
        switch (key)
        {
        case KEY_LEFT:
            keyPressed[LEFT] = false;
            break;
        case KEY_RIGHT:
            keyPressed[RIGHT] = false;
            break;
        case KEY_UP:
            keyPressed[UP] = false;
            break;
        case KEY_DOWN:
            keyPressed[DOWN] = false;
            break;
        }
    }

private:
    
};

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler
{
public:
    MyIHClass() : soldadoTranslatPtr(NULL), soldadoRotPtr(NULL), radians(0) {}
    virtual ~MyIHClass() {}
    virtual void OnIdle()
    {
        bool keyIsPressed = false;

        if(keyPressed[UP]) {
            soldadoTranslatPtr->GetTranslation(&soldadoPos);
            soldadoTranslatPtr->MakeTranslation(soldadoPos + Point4D(0, 0, 0.1));
            keyIsPressed = true;
        } 
        if(keyPressed[DOWN]) {
            soldadoTranslatPtr->GetTranslation(&soldadoPos);
            soldadoTranslatPtr->MakeTranslation(soldadoPos + Point4D(0, 0, -0.1));
            keyIsPressed = true;
        }
        if(keyPressed[LEFT]) {
            radians += 0.01;
            soldadoTranslatPtr->MakeYRotation(radians);
            keyIsPressed = true;
        } 
        if(keyPressed[RIGHT]) {
            radians -= 0.01;
            soldadoTranslatPtr->MakeYRotation(radians);
            keyIsPressed = true;
        }

        if(keyIsPressed) {
        viewerPtr->PostRedisplay();
        }
    };

    Transform* soldadoTranslatPtr;
    Transform* soldadoRotPtr;
protected:
private:
    Point4D soldadoPos;
    float radians;
};

int main(int argc, char* argv[]) {
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT

    if(argc < 2) {
        cerr << "Error: no filename given.\n Correct usage: " << argv[0] << "filename\n"
        <<"where 'filename' is a obj file.\n";
        return 1;
    }

    static Scene scene;
    static ViewerGlutOGL viewer;
    viewer.SetSize(854, 480);


    Camera camera(Point4D(0,0,6), Point4D(0, 0, 0), Point4D(0,1,0,0));
    
    
    list<MeshObject*> objects;
    MeshObject::ReadFromOBJ(argv[1], &objects);
    
    Transform soldadoTranslat;
    soldadoTranslat.MakeIdentity();
    // Transform soldadoScale;
    Transform soldadoRot;
    soldadoRot.MakeIdentity();

    list<MeshObject*>::iterator it = objects.begin();
    for(; it != objects.end(); ++it) {
        // soldadoScale.AddChild(**it);
        soldadoTranslat.AddChild(**it);
        soldadoRot.AddChild(soldadoTranslat);
    }
    
    scene.AddObject(&soldadoRot);
    // soldadoScale.MakeScale()
    MeshObject box;
    box.MakeBox(-3, 3, -3, 8, 0, 3);
    box.SetMaterial(Material::PLASTIC_BLUE());
    // Transform boxTranslat;
    Transform boxRot;
    boxRot.AddChild(box);
    boxRot.MakeXRotation(M_PI/2);
    // boxRot.MakeIdentity();
    scene.AddObject(&boxRot);

    scene.AddLight(Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);
    scene.MakeCameraViewAll();
    // scene.MakeCameraViewAll();

    MyIHClass idleHandler;
    idleHandler.soldadoRotPtr = &soldadoRot;
    idleHandler.soldadoTranslatPtr = &soldadoTranslat;
    KeyboardHandler kbh;

    viewer.SetTitle("Soldado");
    viewer.SetScene(scene);
    viewer.SetKbHandler(&kbh);
    viewer.SetIdleHandler(&idleHandler);

    scene.DrawLightsOGL();
    ViewerGlutOGL::MainLoop();

    return 0;
}