#include "camera/camera.h"
#include "camera/film.h"
#include "shape/sphere.h"
#include "shape/plane.h"
#include "shape/scene.h"
#include "shape/model.h"
#include "material/diffuse_material.h"
#include "material/specular_material.h"
#include "material/dielectric_material.h"
#include "material/conductor_material.h"
#include "material/ground_material.h"
#include "renderer/normal_renderer.h"
#include "renderer/debug_renderer.h"
#include "renderer/path_tracing_renderer.h"

int main(){

    Film film {192 * 4,108 * 4};
    //film.resize(1920, 1080);
    Camera camera { film, { -10, 1.5, 0 }, { 0, 0, 0 }, 45 };
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
    RNG rng { 1234 };
    for (int i = -3; i <= 3; i ++) {
        scene.addShape(
                sphere,
                new DielectricMaterial { 1.f + 0.2f * (i + 3), { 1, 1, 1 } },
                { 0, 0.5, i * 2 },
                { 0.8, 0.8, 0.8 }
        );
    }
    for (int i = -3; i <= 3; i ++) {
        glm::vec3 c = RGB::GenerateHeatmapRGB((i + 3.f) / 6.f);
        scene.addShape(
                sphere,
                new ConductorMaterial {
                        glm::vec3(2.f - c * 2.f),
                        glm::vec3(2.f + c * 3.f),
                },
                { 0, 2.5, i * 2 },
                { 0.8, 0.8, 0.8 }
        );
    }
    scene.addShape(
            model,
            new DielectricMaterial { 1.8, RGB(128, 191, 131) },
            { -5, 0.4, 1.5 },
            { 2, 2, 2 }
    );
    scene.addShape(
            model,
            new ConductorMaterial { { 0.1, 1.2, 1.8 }, { 5, 2.5, 2 } },
            { -5, 0.4, -1.5 },
            { 2, 2, 2 }
    );
    scene.addShape(plane, new GroundMaterial { RGB(120, 204, 157) }, { 0, -0.5, 0 });
    auto *light_material = new DiffuseMaterial { { 1, 1, 1 } };
    light_material->setEmissive({ 0.95, 0.95, 1 });
    scene.addShape(plane, light_material, { 0, 10, 0 });

    scene.build();

    // NormalRenderer normal_renderer {camera, scene};
    // normal_renderer.render(1, "../../normal.ppm");

    // BoundsTestCountRenderer btc_renderer { camera, scene };
    // btc_renderer.render(1, "../../BTC.ppm");
    // TriangleTestCountRenderer ttc_renderer { camera, scene };
    // ttc_renderer.render(1, "../../TTC.ppm");

    PathTracingRenderer path_tracing_renderer {camera, scene};
    path_tracing_renderer.render(128, "../../path_tracing.ppm");

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


//871K Dragon BVH bucket
//Total Node Count: 1742425
//Leaf Node Count: 871213
//Triangle Count: 871306
//Mean Leaf Node Triangle Count: 1.00011
//Max Leaf Node Triangle Count: 3
//render 128spp 5990 ms
//Load Model 1670 ms

//871K Dragon BVH Change Thread Pool
//Load Model 1657 ms

//BVH Node Allocator
//Load Model 1870 ms

//reserve vector
//Load Model 1666 ms

//render 100 shape instances scene
//Profile "Render 128spp ../../simple.ppm": 11866 ms

//render 100 shape instances scene with scene BVH
//Profile "Render 128spp ../../simple.ppm": 3993 ms

//None thread local RNG MacOS
//Profile "Render 128spp ../../path_tracing.ppm": 9401 ms

//thread local RNG MacOS
//Profile "Render 128spp ../../path_tracing.ppm": 6498 ms
