//
// Created by persikey on 4/25/24.
//

#include "RandomFiller.hpp"
#include <random>

namespace GameOfLife
{
RandomFiller::RandomFiller(double alpha, int seed)
: m_alpha(alpha)
{
}

void RandomFiller::Fill(std::shared_ptr<ICheckBoard> checkboard)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    if(m_seed != 0)
    {
        gen.seed(m_seed);
    }

    for (index_t j = 0; j < checkboard->Height(); ++j)
    {
        for (index_t i = 0; i < checkboard->Length() ; ++i)
        {
            double random_number = std::uniform_real_distribution<double>(0.0, 1.0)(gen);

            checkboard->Set(i, j, (random_number < m_alpha) ? cell_t::ALIVE : cell_t::DEAD);
        }
    }
}


} // GameOfLife