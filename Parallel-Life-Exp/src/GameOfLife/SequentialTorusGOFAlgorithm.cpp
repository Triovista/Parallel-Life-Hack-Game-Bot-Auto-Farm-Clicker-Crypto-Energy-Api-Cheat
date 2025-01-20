#include "SequentialTorusGOFAlgorithm.hpp"

namespace GameOfLife
{
    
void SequentialTorusGOFAlgorithm::Compute(std::shared_ptr<IBorderCheckBoard> m_current_state, std::shared_ptr<IBorderCheckBoard> m_next_state)
{
    FillBorder(m_current_state);

    for (index_t j = 0; j < m_current_state->Height(); ++j)
    {
        for (index_t i = 0; i < m_current_state->Length(); ++i)
        {
            uint8_t neighbors_counter = 1;

            CellChecker(neighbors_counter, m_current_state->Get(i-1, j-1));
            CellChecker(neighbors_counter, m_current_state->Get(i, j-1));
            CellChecker(neighbors_counter, m_current_state->Get(i+1, j-1));

            CellChecker(neighbors_counter, m_current_state->Get(i-1, j+1));
            CellChecker(neighbors_counter, m_current_state->Get(i, j+1));
            CellChecker(neighbors_counter, m_current_state->Get(i+1, j+1));

            CellChecker(neighbors_counter, m_current_state->Get(i-1, j));
            CellChecker(neighbors_counter, m_current_state->Get(i+1, j));

            m_next_state->Set(i,j,
                              (neighbors_counter & static_cast<uint8_t>(m_current_state->Get(i,j))) ? cell_t::ALIVE : cell_t::DEAD);
        }
    }

}

}
