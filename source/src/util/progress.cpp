#include "util/progress.h"

#include <iostream>

Progress::Progress(size_t total, size_t step)
    : total(total), current(0), percent(0), last_percent(0), step(20)
{
    std::cout << "0%" << std::endl;
}


void Progress::update(size_t count) {
    Guard guard(spin_lock);
    current += count;
    percent = 100 * static_cast<float>(current) / static_cast<float>(total) ;

    if ((percent - last_percent >= step) || (percent == 100)) {
        last_percent = percent;
        std::cout<< percent << "%" <<std::endl;
    }
}
