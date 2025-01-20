#include "IGOFAlgorithm.hpp"

namespace GameOfLife
{

void IBorderGOFAlgorithm::FillBorder(std::shared_ptr<IBorderCheckBoard> m_current_state)
{
    for(index_t i = 1; i < m_current_state->Length() + 1; ++i)
    {
        m_current_state->BigSet(i,0, m_current_state->BigGet(i, m_current_state->Height()));
        m_current_state->BigSet(i,m_current_state->BigHeight() - 1, m_current_state->BigGet(i,1));
    }
    for(index_t i = 1; i < m_current_state->Height() + 1; ++i)
    {
        m_current_state->BigSet(0, i, m_current_state->BigGet(m_current_state->Length(), i));
        m_current_state->BigSet(m_current_state->BigLength() - 1, i, m_current_state->BigGet(1, i));
    }

    m_current_state->BigSet(0,0,
        m_current_state->Get(m_current_state->Length() - 1, m_current_state->Height() - 1));
    
    m_current_state->BigSet(m_current_state->BigLength() - 1, m_current_state->BigHeight() - 1,
        m_current_state->Get(0, 0));

    m_current_state->BigSet(0,m_current_state->BigHeight() - 1,
        m_current_state->Get(m_current_state->Length() - 1, 0));
    
    m_current_state->BigSet(m_current_state->BigLength() - 1,0,
        m_current_state->Get(0, m_current_state->Height() - 1));
    
}

}