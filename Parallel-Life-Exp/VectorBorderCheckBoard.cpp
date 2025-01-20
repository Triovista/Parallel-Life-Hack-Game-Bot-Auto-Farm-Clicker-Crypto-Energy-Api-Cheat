#include "VectorBorderCheckBoard.hpp"
#include <algorithm>

//#include <Windows.h>

namespace GameOfLife
{
VectorBorderCheckBoard::VectorBorderCheckBoard(index_t length, index_t height)
: IBorderCheckBoard(length, height),
  SMALL_TO_BIG_INDEXING_OFFSET(m_big_checkboard_length + 1),
  UP_LEFT_INDEXING_OFFSET(m_big_checkboard_length + 1),
  DOWN_LEFT_INDEXING_OFFSET(m_big_checkboard_length - 1)
{
    const auto size = m_big_checkboard_height * m_big_checkboard_length;
    //m_data = reinterpret_cast<cell_t*>(VirtualAlloc(NULL, size * sizeof(cell_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    //m_data = reinterpret_cast<cell_t*>(malloc(size * sizeof(cell_t)));
    m_data = new cell_t[size];
    std::fill_n(m_data, size, cell_t::DEAD);
}

VectorBorderCheckBoard::~VectorBorderCheckBoard()
{
    //VirtualFree(m_data,  0, MEM_RELEASE);
    
    //free(m_data);
    delete[] m_data;
}

cell_t VectorBorderCheckBoard::Get(index_t i, index_t j) const
{
    return m_data[SMALL_TO_BIG_INDEXING_OFFSET + i + j * m_big_checkboard_length];
}

void VectorBorderCheckBoard::Set(index_t i, index_t j, cell_t value)
{
    m_data[SMALL_TO_BIG_INDEXING_OFFSET + i + j * m_big_checkboard_length] = value;
}

cell_t VectorBorderCheckBoard::BigGet(index_t i, index_t j) const
{
    return m_data[i + j * m_big_checkboard_length];
}

void VectorBorderCheckBoard::BigSet(index_t i, index_t j, cell_t value)
{
    m_data[i + j * m_big_checkboard_length] = value;
}

}
