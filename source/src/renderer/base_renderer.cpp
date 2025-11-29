#include "renderer/base_renderer.h"
#include <iostream>
#include "thread/thread_pool.h"
#include "util/progress.h"
#include "util/profile.h"


void BaseRenderer::render(size_t spp, const std::filesystem::path &filename) {
    PROFILE("Render " + std::to_string(spp) + "spp " + filename.string())
    size_t current_spp = 0, increase = 1;
    auto &film = camera.getFilm();
    //渲染前先把上次的结果清空
    film.clear();
    Progress progress(film.width * film.height * spp);
    //current spp 0 1 2 4 8 16
    //increase    1 1 2 4 8 16
    while (current_spp < spp) {
        // render increase
        thread_pool.parallelFor(film.width, film.height, [&](size_t x, size_t y) {
            for (int i = 0; i < increase; i++) {
                film.addSample(x, y, renderPixel({x,y}));
            }
            progress.update(increase);
        });

        thread_pool.wait();
        current_spp += increase;
        increase = std::min<size_t>(current_spp, 32);

        film.save(filename);
        std::cout << current_spp << "spp has been saved to " << filename << std::endl;
    }
}
