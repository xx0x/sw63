#include "src/Application.h"

SW63::Application app;

void button_pressed()
{
    app.ButtonCallback();
}

void charge_started()
{
    app.ChargeCallback();
}

void setup()
{
    app.Init(button_pressed, charge_started);
}

void loop()
{
    app.Process();
}
