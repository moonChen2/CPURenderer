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
    simple_renderer.render(128, "../../simple.ppm");

}

// Debug Model
// Load model 43ms
// parallelFor 200ms
// Film::save 101ms
// render 128spp 992444ms


// Release Model
// Load model 40ms
// parallelFor 100ms
// Film::save 76ms
// render 128spp 94363ms

// Change parallelFor
// parallelFor 0ms

// Change Film::save
// Film::save 5ms

// Add Bounds
// Load model 42 ms
// render 128spp 50519 ms

// Add rapid obj
// Load model 2 ms