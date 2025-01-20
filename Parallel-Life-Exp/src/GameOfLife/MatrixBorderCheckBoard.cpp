#include "MatrixBorderCheckBoard.hpp"


namespace GameOfLife
{

constexpr index_t INDEXING_OFFSET = 1;

MatrixBorderCheckBoard::MatrixBorderCheckBoard(index_t length, index_t height)
 : IBorderCheckBoard(length, height)
{
    m_data = new cell_t*[m_big_checkboard_height];

    for (index_t i = 0; i < m_big_checkboard_height; i++)
    {
        m_data[i] = new cell_t[m_big_checkboard_length]{cell_t::DEAD};
    }
}

MatrixBorderCheckBoard::~MatrixBorderCheckBoard()
{
    for (index_t i = 0; i < m_big_checkboard_height; i++)
    {
        delete m_data[i];
    }    

    delete[] m_data;
}

cell_t MatrixBorderCheckBoard::Get(index_t i, index_t j) const
{
    return m_data[INDEXING_OFFSET + j][INDEXING_OFFSET + i];
}

void MatrixBorderCheckBoard::Set(index_t i, index_t j, cell_t value)
{
    m_data[INDEXING_OFFSET + j][INDEXING_OFFSET + i] = value;
}

cell_t MatrixBorderCheckBoard::BigGet(index_t i, index_t j) const
{
    return m_data[j][i];
}

void MatrixBorderCheckBoard::BigSet(index_t i, index_t j, cell_t value)
{
    m_data[j][i] = value;
}
}
