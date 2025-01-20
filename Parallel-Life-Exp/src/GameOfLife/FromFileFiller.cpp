#include "FromFileFiller.hpp"
#include <fstream>
#include <iostream>

namespace GameOfLife
{

FromFileFiller::FromFileFiller(std::string file_path) 
 : m_file_path(file_path)
{
}


struct comma_is_space : std::ctype<char> 
{
  comma_is_space() 
  : std::ctype<char>(get_table()) 
  {}
  static mask const* get_table()
  {
    static mask rc[table_size];
    rc[','] = std::ctype_base::space;
    rc['\n'] = std::ctype_base::space;
    rc[' '] = std::ctype_base::space;
    return &rc[0];
  }
};

void FromFileFiller::Fill(std::shared_ptr<ICheckBoard> checkboard)
{
    index_t input_length;
    index_t input_height;

    std::ifstream file(m_file_path);
    file.imbue(std::locale(file.getloc(), new comma_is_space()));

    if(!file)
    {
        std::cerr << "Cannot open file at: " << m_file_path << std::endl;
        return;
    }

    file >> input_length >> input_height;

    if(input_height > checkboard->Height() || input_length > checkboard->Length())
    {
        std::cerr << "Dimensions missmatch" << std::endl;
        return;        
    }

    bool input_is_alive;
    for (index_t j = 0; j < input_height; j++)
    {
        for (index_t i = 0; i < input_length; i++)
        {
            file >> input_is_alive;
            checkboard->Set(i,j, (input_is_alive) ? cell_t::ALIVE : cell_t::DEAD );
        }
    }
    
}
}