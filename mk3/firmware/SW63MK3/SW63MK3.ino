#include "src/Application.h"

SW63::Application app;

void button_pressed()
{
    app.ButtonCallback();
}

void setup()
{
    app.Init(button_pressed);
}

void loop()
{
    app.Process();
}
