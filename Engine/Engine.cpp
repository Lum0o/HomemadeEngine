// Engine.cpp : Définit le point d'entrée de l'application.
//

#include "App.h"

int main()
{
    App app = App();
    app.Init();
    return app.update();
}
