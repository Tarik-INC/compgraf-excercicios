// main.cpp - Wavefront OBJ viewer application

// V-ART example: Load and Display a OBJ file.

// This application shows how to create an object from a Wavefront OBJ file.

// Made from picker example, keeps the piking code.

// Changelog
// Oct 19, 2012 - Bruno de Oliveira Schneider
// - Adapted to new keyboard handler methods.
// Sep 26, 2012 - Bruno de Oliveira Schneider
// - Application created.

#include <vart/scene.h>
#include <vart/box.h>
#include <vart/cylinder.h>
#include <vart/light.h>
#include <vart/meshobject.h>
#include <vart/sphere.h>
#include <vart/contrib/viewerglutogl.h>
#include <vart/contrib/mousecontrol.h>
#include <vart/transform.h>
#include <vart/baseaction.h>
#include <vart/translationaction.h>
#include <vart/rotationaction.h>
#include <vart/scaleaction.h>
#include <vart/shearaction.h>
#include <vart/callback.h>

#include <iostream>
#include <cmath>
#include <math.h>

using namespace std;
using namespace VART;
// Define the keyboard handler
class KeyboardHandler : public VART::ViewerGlutOGL::KbHandler
{
    public:
        KeyboardHandler() {
        }
        virtual void OnKeyDown(int key) {
            switch (key)
            {
                case 'j':
                    break;
                case 'l':
                    break;
                case 'i':
                    break;
                case 'k':
                    break;
            }
        }
    private:
};

class ClickHandlerClass : public VART::MouseControl::ClickHandler
{
    public:
        ClickHandlerClass() {
        }
        virtual ~ClickHandlerClass() {};
        virtual void OnClick() {
            if (mouseCtrlPtr->LastClickIsDown()) {
                VART::MouseControl::ButtonID lastButton = mouseCtrlPtr->GetLastClickButton();

                if (lastButton == VART::MouseControl::LEFT) {
                    scenePtr->Pick(mouseCtrlPtr->GetClickXPosition(),
                                   mouseCtrlPtr->GetClickYPosition(), &pickList);
                    if (!pickList.empty()) {
                        cout << pickList.size() << " selected objects: ";
                        while (!pickList.empty()) {
                            cout << pickList.front()->GetDescription() << ":\n";
#ifdef DEBUG
                            VART::MeshObject* ptObj = dynamic_cast<VART::MeshObject*>(pickList.front());
                            if (ptObj)
                                cout << *ptObj << "\n";
#endif
                            pickList.pop_front();
                        }
                        cout << endl;
                    }
                }
            }
        }
        list<VART::GraphicObj*> pickList;
        VART::Scene* scenePtr;
};

class MyIHClass : public ViewerGlutOGL::IdleHandler {
    public:
        // transformacoes que ocorrerao durante a execucao do programa
        Transform* rotationfan;
        Transform* rotationEixo;
        Transform* rotationSuporte;
        virtual ~MyIHClass() {}
        virtual void OnIdle() {
            // rotacao da helice no eixo X
            rotationfan->MakeRotation(Point4D(0, 1.237193, 0), Point4D(1,0,0,0), rotationFanAngle);
            rotationEixo->MakeRotation(Point4D(0, 1.237193, 0), Point4D(1,0,0,0), rotationFanAngle);
            rotationSuporte->MakeYRotation(rotationBaseAngle);
            // varia o angulo de rotacao da helice
            rotationFanAngle += 0.03;
            if(rotationBaseAngle > M_PI/4) {
             rotateBaseEixoLeft = false;
            }
            else if (rotationBaseAngle <  - M_PI / 4)
            {
                rotateBaseEixoLeft = true;
            }
            if(rotateBaseEixoLeft) {
                rotationBaseAngle += 0.01;
            } else {
                rotationBaseAngle -= 0.01;
            }
            //rotationEixoAngle += 0.03;
            viewerPtr->PostRedisplay();
        }
    private:
        bool rotateBaseEixoLeft = true;
        // angulo de rotacao da helice
        double rotationFanAngle = 0;
        //double rotationEixoAngle = 0;
        double rotationBaseAngle = 0;
};

// The application itself:
int main(int argc, char* argv[])
{
    VART::ViewerGlutOGL::Init(&argc, argv); // Initialize GLUT

    if (argc < 2) {
        cerr << "Fatal error: No filename given.\nUsage: " << argv[0] << " filename\n"
             << "Where 'filename' is a Wavefront OBJ file.\n";
        return 1;
    }

    static VART::Scene scene; // create a scene
    static VART::ViewerGlutOGL viewer; // create a viewer (application window)

    KeyboardHandler kbh; // create a keyboard handler
    // create a camera (scene observer)
    VART::Camera camera(VART::Point4D(0,0,6),VART::Point4D(0,0,0),VART::Point4D(0,1,0,0));
    // create some objects
    list<VART::MeshObject*> objects;
    ClickHandlerClass clickHandler;

    clickHandler.scenePtr = &scene;

    // Initialize scene objects
    VART::MeshObject::ReadFromOBJ("fan.obj", &objects);
    
    Transform* tFan = new Transform();
    Transform* tEixo = new Transform();
    Transform* tGrade = new Transform();
    Transform* tSupp = new Transform();
    Transform* tBase = new Transform();
    // Build up the scene
    list<VART::MeshObject*>::iterator iter = objects.begin();
    for (; iter != objects.end(); ++iter) {
        if((*iter)->GetDescription() == "helice")
            tFan->AddChild(**iter);
        if((*iter)->GetDescription() == "grade")
            tGrade->AddChild(**iter);
        if((*iter)->GetDescription() == "eixo")
            tEixo->AddChild(**iter);
        if((*iter)->GetDescription() == "suporte-eixo")
            tSupp->AddChild(**iter);
        if((*iter)->GetDescription() == "base")
            tBase->AddChild(**iter);
    }
    tFan->MakeIdentity();
    tEixo->MakeIdentity();
    tGrade->MakeIdentity();
    tSupp->MakeIdentity();
    tBase->MakeIdentity();
    
    
    
    // adiciona os elementos na cena
    MyIHClass idle;
    idle.rotationfan = tFan;
    idle.rotationEixo = tEixo;
    idle.rotationSuporte = tSupp;
    scene.AddObject(tFan);
    scene.AddObject(tEixo);
    scene.AddObject(tGrade);
    scene.AddObject(tSupp);
    scene.AddObject(tBase);
    
    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    scene.AddCamera(&camera);
    scene.MakeCameraViewAll();
    
    
    
    // Set up the viewer
    viewer.SetTitle("V-ART OBJ Viewer");
    viewer.SetScene(scene); // attach the scene
    viewer.SetKbHandler(&kbh); // attach the keyboard handler
    viewer.SetClickHandler(&clickHandler);
    viewer.SetIdleHandler(&idle);
    // Run application
    scene.DrawLightsOGL(); // Set OpenGL's lights' state
    VART::ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)
    return 0;
}
