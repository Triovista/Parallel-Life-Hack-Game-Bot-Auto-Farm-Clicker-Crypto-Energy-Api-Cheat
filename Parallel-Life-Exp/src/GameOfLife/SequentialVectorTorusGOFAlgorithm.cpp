#include "SequentialVectorTorusGOFAlgorithm.hpp"
#include <iostream>
#include <cstring>

namespace GameOfLife
{

    std::atomic_int64_t SequentialVectorTorusGOFAlgorithm::counter = 0;
void SequentialVectorTorusGOFAlgorithm::Compute(std::shared_ptr<VectorBorderCheckBoard>& m_current_state, 
                                                std::shared_ptr<VectorBorderCheckBoard>& m_next_state)
{
    //counter = 0;
    FillRowsChunk(0, m_current_state->Length(), m_current_state);
    FillColumnsChunk(0, m_current_state->Height(), m_current_state);
    FillCorners(m_current_state);
        
    ComputeVectorChunk(0, m_current_state->Length() * m_current_state->Height(), m_current_state, m_next_state);
    //std::cout << "Counter: " << counter << std::endl;
}

void SequentialVectorTorusGOFAlgorithm::ComputeVectorChunk(index_t begin, index_t end,
    std::shared_ptr<VectorBorderCheckBoard>& m_current_state, std::shared_ptr<VectorBorderCheckBoard>& m_next_state)
{
    --end;
    const auto i_begin = (begin / m_current_state->Length()) * m_current_state->BigLength() + begin % m_current_state->Length() + m_current_state->SMALL_TO_BIG_INDEXING_OFFSET;
    const auto i_end = (end / m_current_state->Length()) * m_current_state->BigLength() + end % m_current_state->Length() + m_current_state->SMALL_TO_BIG_INDEXING_OFFSET;

    const auto data =  m_current_state->m_data;

    for(index_t i = i_begin; i <= i_end; ++i)
    {
        uint8_t neighbors_counter = 1;
        
        index_t index = i - m_current_state->UP_LEFT_INDEXING_OFFSET;
        
        CellChecker(neighbors_counter, data[index++]);
        CellChecker(neighbors_counter, data[index++]);
        CellChecker(neighbors_counter, data[index]);

        index = i + m_current_state->DOWN_LEFT_INDEXING_OFFSET;
        CellChecker(neighbors_counter, data[index++]);
        CellChecker(neighbors_counter, data[index++]);
        CellChecker(neighbors_counter, data[index]);

        CellChecker(neighbors_counter, data[i-1]);
        CellChecker(neighbors_counter, data[i+1]);

        m_next_state->m_data[i] = 
                            (neighbors_counter & static_cast<uint8_t>(data[i])) ? cell_t::ALIVE : cell_t::DEAD;  
        //counter++;
    }    
}

void SequentialVectorTorusGOFAlgorithm::FillCorners(std::shared_ptr<VectorBorderCheckBoard>& m_current_state)
{
    const auto offset = m_current_state->m_big_checkboard_length;
    auto big_top_left_corner = m_current_state->m_data;
    auto big_top_right_corner = big_top_left_corner + offset - 1;

    auto big_bottom_right_corner = big_top_left_corner + m_current_state->m_big_checkboard_height * offset - 1;
    auto big_bottom_left_corner = big_bottom_right_corner - offset + 1;

    auto small_top_left_corner = big_top_left_corner + m_current_state->SMALL_TO_BIG_INDEXING_OFFSET;
    auto small_top_right_corner = small_top_left_corner + m_current_state->m_checkboard_length - 1;

    auto small_bottom_right_corner = big_bottom_left_corner - 2;
    auto small_bottom_left_corner = small_bottom_right_corner - m_current_state->m_checkboard_length + 1;
    

    *big_top_left_corner = *small_bottom_right_corner;
    *big_top_right_corner = *small_bottom_left_corner;
    *big_bottom_left_corner = *small_top_right_corner;
    *big_bottom_right_corner = *small_top_left_corner;
}

void SequentialVectorTorusGOFAlgorithm::FillRowsChunk(index_t begin, index_t end, std::shared_ptr<VectorBorderCheckBoard>& m_current_state)
{
    const auto offset = m_current_state->m_big_checkboard_length;
    auto first_big_row_chunk_begin = m_current_state->m_data + 1 + begin;
    auto first_small_row_chunk_begin = first_big_row_chunk_begin + offset;
    auto last_big_row_chunk_begin = first_big_row_chunk_begin + offset * (m_current_state->m_big_checkboard_height - 1);
    auto last_small_row_chunk_begin = last_big_row_chunk_begin - offset;
    auto size = (end - begin) * sizeof(cell_t);

    memcpy(first_big_row_chunk_begin, last_small_row_chunk_begin, size);
    memcpy(last_big_row_chunk_begin, first_small_row_chunk_begin, size);
}

void SequentialVectorTorusGOFAlgorithm::FillColumnsChunk(index_t begin, index_t end, std::shared_ptr<VectorBorderCheckBoard>& m_current_state)
{
    const auto offset = m_current_state->m_big_checkboard_length;
    auto left_big_colummn_chunk_begin = m_current_state->m_data + (begin + 1) * offset;
    auto left_small_column_chunk_begin = left_big_colummn_chunk_begin + 1;
    auto right_big_colummn_chunk_begin = left_big_colummn_chunk_begin + offset - 1;
    auto right_small_column_chunk_begin = right_big_colummn_chunk_begin - 1;
    auto size = end - begin;

    for (index_t i = 0; i < size; ++i)
    {
        *left_big_colummn_chunk_begin = *right_small_column_chunk_begin;
        *right_big_colummn_chunk_begin = *left_small_column_chunk_begin;

        left_big_colummn_chunk_begin += offset;
        left_small_column_chunk_begin += offset;
        right_big_colummn_chunk_begin += offset;
        right_small_column_chunk_begin += offset;
    }
}
}
