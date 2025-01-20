#include "ParallelTorusAlgorithm.hpp"
#include "SequentialVectorTorusGOFAlgorithm.hpp"

namespace GameOfLife
{

ParallelTaskExecutionPool::ParallelTaskExecutionPool(int threads_num)
: m_threads_num(threads_num),
  m_thread_pool(threads_num),
  m_is_executing(false),
    m_finished_threads(0),
    m_working_threads(0)
{
    for (int i = 0; i < m_threads_num; ++i)
    {
        m_thread_pool[i].executor_thread = std::thread(&ParallelTaskExecutionPool::Worker, this);
    }
    m_is_executing = true;
}

ParallelTaskExecutionPool::~ParallelTaskExecutionPool()
{
    m_is_executing = false;
    m_working_threads.release(m_threads_num);
    for (int i = 0; i < m_threads_num; ++i)
    {
        m_thread_pool[i].executor_thread.join();
    }
}

void ParallelTaskExecutionPool::Start(std::unique_ptr<ITask> task)
{
    m_task = std::move(task);

    m_working_threads.release(m_threads_num);
}

void ParallelTaskExecutionPool::Join()
{
    for (size_t i = 0; i < m_threads_num; i++)
    {
        m_finished_threads.acquire();
    }
}

void ParallelTaskExecutionPool::Worker()
{
    while (true)
    {
        m_working_threads.acquire();

        if(!m_is_executing)
        {
            break;
        }

        m_task->Execute();

        m_finished_threads.release();
    }
}


ParralelComputeTask::ParralelComputeTask(std::shared_ptr<VectorBorderCheckBoard> current_state,
    std::shared_ptr<VectorBorderCheckBoard> next_state, int tasks_num, index_t task_block)
    : m_current_state(std::move(current_state)),
    m_next_state(std::move(next_state)),
    m_tasks_num(tasks_num),
    m_task_block(task_block),
    m_thread_task_index(0)
{

}

void ParralelComputeTask::Execute() 
{
    index_t current_index;
    while ((current_index = ++m_thread_task_index) <= m_tasks_num)
    {
        const auto end = current_index * m_task_block;
        const auto begin = end - m_task_block;

        SequentialVectorTorusGOFAlgorithm::ComputeVectorChunk(begin, end, m_current_state, m_next_state);
    }
}

ParralelFillRowTask::ParralelFillRowTask(std::shared_ptr<VectorBorderCheckBoard> current_state, int tasks_num, index_t task_block)
    : m_current_state(std::move(current_state)),
    m_tasks_num(tasks_num),
    m_task_block(task_block),
    m_task_index(0)
{
    
}

void ParralelFillRowTask::Execute()
{
    index_t current_index;
    while ((current_index = ++m_task_index) <= m_tasks_num)
    {
        const auto end = current_index * m_task_block;
        const auto begin = end - m_task_block;

        SequentialVectorTorusGOFAlgorithm::FillRowsChunk(begin, end, m_current_state);
    }    
}

ParralelFillColumnTask::ParralelFillColumnTask(std::shared_ptr<VectorBorderCheckBoard> current_state, int tasks_num, index_t task_block)
    : m_current_state(std::move(current_state)),
    m_tasks_num(tasks_num),
    m_task_block(task_block),
    m_task_index(0)
{

}

void ParralelFillColumnTask::Execute()
{
    index_t current_index;
    while ((current_index = ++m_task_index) <= m_tasks_num)
    {
        const auto end = current_index * m_task_block;
        const auto begin = end - m_task_block;

        SequentialVectorTorusGOFAlgorithm::FillColumnsChunk(begin, end, m_current_state);
    }   
}

ParallelTorusAlgorithm::ParallelTorusAlgorithm()
: ParallelTorusAlgorithm(std::max(1u, std::thread::hardware_concurrency() - 2))
{
    
}

ParallelTorusAlgorithm::ParallelTorusAlgorithm(int threads_num)
    : m_pool(threads_num - 1), 
    m_threads_num(threads_num - 1)
{

}

struct task_sizing_t
{
    index_t block_size;
    index_t last_task_begin;
    index_t last_task_end;
    int tasks_num;
};
task_sizing_t GetSizing(index_t task_size, int num_threads)
{
    task_sizing_t result{ 0 };
    const int number_of_whole_parts = task_size / CHUNK_SIZE;

    const int number_of_whole_parts_per_thread = std::max(number_of_whole_parts / (num_threads + 1), 1);

    result.block_size = number_of_whole_parts_per_thread * CHUNK_SIZE;

    result.tasks_num = std::max<int64_t>(task_size / result.block_size - 1, 0);
    result.last_task_begin = result.tasks_num * result.block_size;
    result.last_task_end = task_size;

    return result;
}

void ParallelTorusAlgorithm::Compute(std::shared_ptr<VectorBorderCheckBoard>& m_current_state,
                                     std::shared_ptr<VectorBorderCheckBoard>& m_next_state)
{
    auto row_sizing = GetSizing(m_current_state->Length(), m_threads_num);

    m_pool.Start(std::make_unique<ParralelFillRowTask>(m_current_state, row_sizing.tasks_num, row_sizing.block_size));
    SequentialVectorTorusGOFAlgorithm::FillRowsChunk(row_sizing.last_task_begin, row_sizing.last_task_end, m_current_state);

    auto column_sizing = GetSizing(m_current_state->Height(), m_threads_num);
    m_pool.Join();

    m_pool.Start(std::make_unique<ParralelFillColumnTask>(m_current_state, column_sizing.tasks_num, column_sizing.block_size));
    SequentialVectorTorusGOFAlgorithm::FillColumnsChunk(column_sizing.last_task_begin, column_sizing.last_task_end, m_current_state);

    SequentialVectorTorusGOFAlgorithm::FillCorners(m_current_state);
    auto compute_sizing = GetSizing(m_current_state->Length() * m_current_state->Height(), m_threads_num);
    m_pool.Join();

    m_pool.Start(std::make_unique<ParralelComputeTask>(m_current_state, m_next_state, compute_sizing.tasks_num, compute_sizing.block_size));

    SequentialVectorTorusGOFAlgorithm::ComputeVectorChunk(compute_sizing.last_task_begin, compute_sizing.last_task_end, m_current_state, m_next_state);
    
    m_pool.Join();
}

}
