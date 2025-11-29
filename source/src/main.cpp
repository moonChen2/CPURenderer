#include "camera/camera.h"
#include "thread/thread_pool.h"
#include "camera/film.h"
#include "shape/sphere.h"
#include "shape/plane.h"
#include "shape/scene.h"
#include "shape/model.h"
#include "renderer/normal_renderer.h"
#include "renderer/simple_rt_renderer.h"
#include "renderer/debug_renderer.h"


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
    Model model {"../../models/dragon_871k.obj"};

    Scene scene {};
    scene.addShape(
            model,
            { RGB(202, 159, 117) },
            { 0, 0, 0 },
            { 3, 3, 3 }
    );
    // scene.addShape(
    //         sphere,
    //         { { 1, 1, 1 }, false, RGB(255, 128, 128) },
    //         { 0, 0, 2.5 }
    // );
    // scene.addShape(
    //         sphere,
    //         { { 1, 1, 1 }, false, RGB(128, 128, 255) },
    //         { 0, 0, -2.5 }
    // );
    // scene.addShape(
    //         sphere,
    //         { { 1, 1, 1 }, true },
    //         { 3, 0.5, -2 }
    // );
    // scene.addShape(plane, { RGB(120, 204, 157) }, { 0, -0.5, 0 });

    NormalRenderer normal_renderer {camera, scene};
    normal_renderer.render(1, "../../normal.ppm");

    BoundsTestCountRenderer btc_renderer { camera, scene };
    btc_renderer.render(1, "../../BTC.ppm");
    TriangleTestCountRenderer ttc_renderer { camera, scene };
    ttc_renderer.render(1, "../../TTC.ppm");
    BoundsDepthRenderer bd_renderer { camera, scene };
    bd_renderer.render(1, "../../BD.ppm");

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
// render 128spp 26320 ms

// Add rapid obj
// Load model 2 ms

// Add BVH
// render 128spp 4770 ms

// dragon 87k
// origin render 128spp 3644 ms
// flatten render 128spp 4221 ms
// triangle index render 128spp 4427 ms
// 32bit pack  render 128spp 4244 ms
// *bbx iteration direction, left->right or right->left ?
// inv direction render 128spp

//Without SAH
//Total Node Count: 142643
//Leaf Node Count: 71322
//Triangle Count: 87130
//Mean Leaf Node Triangle Count: 1.22164
//Max Leaf Node Triangle Count: 23

//With SAH
//Total Node Count: 164067
//Leaf Node Count: 82034
//Triangle Count: 87130
//Mean Leaf Node Triangle Count: 1.06212
//Max Leaf Node Triangle Count: 8

//3 axis SAH
//Total Node Count: 174195
//Leaf Node Count: 87098
//Triangle Count: 87130
//Mean Leaf Node Triangle Count: 1.00037
//Max Leaf Node Triangle Count: 3


//871K Dragon BVH
//Total Node Count: 1742419
//Leaf Node Count: 871210
//Triangle Count: 871306
//Mean Leaf Node Triangle Count: 1.00011
//Max Leaf Node Triangle Count: 3
//render 128spp 4770 ms
//Load Model 8367 ms
