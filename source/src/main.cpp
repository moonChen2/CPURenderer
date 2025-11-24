#include "camera/camera.h"
#include "thread/thread_pool.h"
#include "camera/film.h"
#include "shape/sphere.h"
#include "shape/plane.h"
#include "shape/scene.h"
#include "shape/model.h"
#include "renderer/normal_renderer.h"
#include "renderer/simple_rt_renderer.h"


int main(){

    std::atomic<int> count = 0;

    Film film {192 * 4,108 * 4};
    Camera camera {film, {-3.6,0,0},{0,0,0},45};
    Sphere sphere{
        {0,0,0},
        1
    };
    Plane plane {
        {0,0,0},
        {0,1,0}
    };
    Model model {"../../models/simple_dragon.obj"};

    Scene scene {};
    scene.addShape(
            model,
            { RGB(202, 159, 117) },
            { 0, 0, 0 },
            { 1, 3, 2 }
    );
    scene.addShape(
            sphere,
            { { 1, 1, 1 }, false, RGB(255, 128, 128) },
            { 0, 0, 2.5 }
    );
    scene.addShape(
            sphere,
            { { 1, 1, 1 }, false, RGB(128, 128, 255) },
            { 0, 0, -2.5 }
    );
    scene.addShape(
            sphere,
            { { 1, 1, 1 }, true },
            { 3, 0.5, -2 }
    );
    scene.addShape(plane, { RGB(120, 204, 157) }, { 0, -0.5, 0 });

    NormalRenderer normal_renderer {camera, scene};
    normal_renderer.render(1, "../../normal.ppm");

    film.clear();

    SimpleRTRenderer simple_renderer {camera, scene};
    simple_renderer.render(32, "../../simple.ppm");

}

