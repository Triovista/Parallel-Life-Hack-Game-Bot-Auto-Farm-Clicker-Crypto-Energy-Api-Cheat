#include <iostream>
#include <chrono>
#include <array>
#include <cmath>
#include <iomanip>
#include <fstream>

#include "ParallelTorusAlgorithm.hpp"
#include "SequentialVectorTorusGOFAlgorithm.hpp"
#include "RandomFiller.hpp"

using namespace GameOfLife;

inline constexpr size_t PROCESSOR_OVERALL_WARMUP_ITERATIONS = 500'000'000;
inline constexpr size_t MEASURES = 20;
inline constexpr size_t MINIMUM_MATRIX_SIZE = 1000ull * 1000ull * 1ull;
inline constexpr size_t MAXIMUM_MATRIX_SIZE = 1024ull * 1024ull * 1024ull * 4ull;
inline constexpr int SEED = 131313;

inline constexpr size_t FIRST_COLUMN_WIDHT = 12;
inline constexpr size_t ALG_NAME_WIDTH = 18;
inline constexpr size_t ALG_MEAN_PRECISION = 5;
inline constexpr size_t ALG_MEAN_WIDTH = ALG_MEAN_PRECISION + 2;
inline constexpr size_t ALG_DEVIATION_PRECISION = 4;
inline constexpr size_t ALG_DEVIATION_WIDTH = ALG_DEVIATION_PRECISION + 2;
inline constexpr size_t CELL_WIDTH = ALG_NAME_WIDTH + ALG_MEAN_WIDTH + ALG_DEVIATION_WIDTH + 2;


inline constexpr char FILL = ' ';
inline constexpr char FIRST_COLUMN_SEPARATOR = '|';
inline constexpr char FIRST_ROW_SEPARATOR = '-';
inline constexpr char CELL_SEPARATOR = '|';
inline constexpr char INTERSECT = '+';
inline constexpr auto REPORT_FILENAME = "report.csv";

struct measure_t
{
    double mean;
    double deviation;
};

class LocalBenchmark
{
public:
    static void WarmUpAllLogicalCores()
    {
        std::atomic_uint64_t chunk_index = 0;

        std::vector<std::thread> threads(std::thread::hardware_concurrency());

        for (auto &thread : threads)
        {
            thread = std::thread([&chunk_index]()
            {
                volatile size_t counter = 0;
                for (; ++chunk_index < PROCESSOR_OVERALL_WARMUP_ITERATIONS / std::thread::hardware_concurrency();)
                {
                    counter++;
                } 
            });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    static measure_t Measure(const std::unique_ptr<IVectorBorderGOFAlgorithm> &algorithm,
                             index_t lenght,
                             index_t height)
    {
        auto current_state = std::make_shared<VectorBorderCheckBoard>(lenght, height);
        auto filler = std::make_unique<RandomFiller>(0.3, SEED);
        filler->Fill(current_state);
        auto next_state = std::make_shared<VectorBorderCheckBoard>(lenght, height);

        measure_t result{};
        std::array<long long, MEASURES> measures{};

        double sum = 0;

        WarmUpAllLogicalCores();

        for (auto &measure : measures)
        {
            auto begin = std::chrono::system_clock::now();
            algorithm->Compute(current_state, next_state);
            auto end = std::chrono::system_clock::now();
            measure = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
            sum += measure;
        }
        result.mean = sum / MEASURES;

        double differences_sum = 0;
        for (auto &measure : measures)
        {
            differences_sum += pow(result.mean - measure, 2);
        }
        result.deviation = sqrt(differences_sum / MEASURES);

        return result;
    }
};

struct algorithm_info_t
{
    std::unique_ptr<IVectorBorderGOFAlgorithm> implementation;
    std::string name;

    algorithm_info_t(std::unique_ptr<IVectorBorderGOFAlgorithm> &&implementation, std::string &&name)
    {
        this->implementation = std::move(implementation);
        this->name = std::move(name);
    }

    algorithm_info_t(algorithm_info_t &&other) = default;

    algorithm_info_t &operator=(algorithm_info_t &&other) = default;
};

class GlobalBenchmark
{
public:
    static void Measure(std::initializer_list<algorithm_info_t> algorithms, std::vector<index_t> sizes)
    {
        std::ofstream report_file(REPORT_FILENAME);
        std::stringstream buffer;

        PrintSizes(buffer, sizes);
        FlushBufferToStdoutAndFile(buffer, report_file);

        PrintFirstRow(buffer, sizes);
        
        for (const auto height : sizes)
        {
            PrintFirstLocalRow(buffer, height, sizes);
            for (auto &algorithm : algorithms)
            {
                buffer << std::setw(FIRST_COLUMN_WIDHT) << "" << FIRST_COLUMN_SEPARATOR;
                for (const auto lenght : sizes)
                {
                    if(lenght * height < MINIMUM_MATRIX_SIZE)
                    {
                        PrintSizeTooSmall(buffer);
                        continue;
                    }
                    else if(lenght * height > MAXIMUM_MATRIX_SIZE)
                    {
                        PrintSizeTooBig(buffer);
                        continue;
                    }

                    buffer << std::setw(ALG_NAME_WIDTH) << algorithm.name << CELL_SEPARATOR;
                    FlushBufferToStdoutAndFile(buffer, report_file);
                    measure_t measure = LocalBenchmark::Measure(algorithm.implementation, lenght, height);
                    buffer << std::setw(ALG_MEAN_WIDTH) << std::setprecision(ALG_MEAN_PRECISION) << measure.mean << CELL_SEPARATOR;
                    buffer << std::setw(ALG_DEVIATION_WIDTH) << std::setprecision(ALG_DEVIATION_PRECISION) << measure.deviation << FIRST_COLUMN_SEPARATOR;

                    FlushBufferToStdoutAndFile(buffer, report_file);
                }
                buffer << '\n';
            }
            PrintHorizontalSeparator(buffer, sizes);
        }
        buffer << '\n';
        FlushBufferToStdoutAndFile(buffer, report_file);
    }

private:
    static void PrintSizes(std::stringstream &buffer, std::vector<index_t> &sizes)
    {
        const int SIZES_CELL_WIDTH = log10(MAXIMUM_MATRIX_SIZE) + 2;

        buffer << std::setfill(FILL);
        buffer << std::setw(SIZES_CELL_WIDTH) << "" << FIRST_COLUMN_SEPARATOR;
        for (const auto lenght : sizes)
        {
            buffer << std::setw(SIZES_CELL_WIDTH) << lenght << FIRST_COLUMN_SEPARATOR;
        }
        buffer << '\n';

        buffer << std::setfill(FIRST_ROW_SEPARATOR) << std::setw(SIZES_CELL_WIDTH) << "" << INTERSECT;
        for (const auto lenght : sizes)
        {
            buffer << std::setw(SIZES_CELL_WIDTH) << "" << FIRST_ROW_SEPARATOR;
        }
        buffer << '\n';
        buffer << std::setfill(FILL);


        for(const auto height : sizes)
        {
            buffer << std::setw(SIZES_CELL_WIDTH) << height << FIRST_COLUMN_SEPARATOR;
            for(const auto lenght : sizes)
            {
                auto compute_size = (height) * (lenght + 2) - 2;
                auto interface_size = height * lenght;
                if(interface_size >= MINIMUM_MATRIX_SIZE && interface_size <= MAXIMUM_MATRIX_SIZE)
                {
                    buffer << std::setw(SIZES_CELL_WIDTH) << compute_size << FIRST_COLUMN_SEPARATOR;
                }
                else
                {
                    buffer << std::setw(SIZES_CELL_WIDTH) << '-' << FIRST_COLUMN_SEPARATOR;
                }
            }
            buffer << '\n';
        }
        buffer << std::setfill(FIRST_ROW_SEPARATOR) << std::setw(SIZES_CELL_WIDTH) << "" << INTERSECT;
        for (const auto lenght : sizes)
        {
            buffer << std::setw(SIZES_CELL_WIDTH) << "" << FIRST_ROW_SEPARATOR;
        }
        buffer << '\n';
        buffer << '\n';
        buffer << std::setfill(FILL);
    }

    static void PrintHorizontalSeparator(std::stringstream &buffer, std::vector<index_t> &sizes)
    {
        buffer << std::setfill(FIRST_ROW_SEPARATOR) << std::setw(FIRST_COLUMN_WIDHT) << "" << INTERSECT;
        for (const auto lenght : sizes)
        {
            buffer << std::setw(CELL_WIDTH) << "" << FIRST_ROW_SEPARATOR;
        }
        buffer << '\n';
        buffer << std::setfill(FILL);
    }

    static void PrintFirstRow(std::stringstream &buffer, std::vector<index_t> &sizes)
    {
        buffer << std::setfill(FILL);
        buffer << std::setw(FIRST_COLUMN_WIDHT) << "" << FIRST_COLUMN_SEPARATOR;
        for (const auto lenght : sizes)
        {
            buffer << std::setw(CELL_WIDTH) << lenght << FIRST_COLUMN_SEPARATOR;
        }
        buffer << '\n';

        PrintHorizontalSeparator(buffer, sizes);
    }

    static void PrintFirstLocalRow(std::stringstream &buffer, index_t height, std::vector<index_t> &sizes)
    {
        buffer << std::setw(FIRST_COLUMN_WIDHT) << height << FIRST_COLUMN_SEPARATOR;
        for (const auto lenght : sizes)
        {
            buffer << std::setw(ALG_NAME_WIDTH) << "Algorithm | Trds" << CELL_SEPARATOR;
            buffer << std::setw(ALG_MEAN_WIDTH) << "Mean" << CELL_SEPARATOR;
            buffer << std::setw(ALG_DEVIATION_WIDTH) << "Dev" << FIRST_COLUMN_SEPARATOR;
        }
        buffer << '\n';
    }

    static void FlushBufferToStdoutAndFile(std::stringstream &buffer, std::ofstream& file)
    {
        std::cout << buffer.str();
        file << buffer.str();
        file.flush();
        buffer.str("");
    }

    static void PrintSizeTooSmall(std::stringstream &buffer)
    {
        buffer << std::setw(CELL_WIDTH) << "Matrix size is too small" << FIRST_COLUMN_SEPARATOR;
    }

    static void PrintSizeTooBig(std::stringstream &buffer)
    {
        buffer << std::setw(CELL_WIDTH) << "Matrix size is too big" << FIRST_COLUMN_SEPARATOR;
    }
};

int main()
{
    GlobalBenchmark::Measure(
        {
            {
                std::make_unique<SequentialVectorTorusGOFAlgorithm>(),
                std::string("Sequential |    1")
            },
            {
                std::make_unique<ParallelTorusAlgorithm>(2),
                std::string("Parralel  |    2")
            },
            {
                std::make_unique<ParallelTorusAlgorithm>(4),
                std::string("Parralel  |    4")
            },
            {
                std::make_unique<ParallelTorusAlgorithm>(8),
                std::string("Parralel  |    8")
            },
                        {
                std::make_unique<ParallelTorusAlgorithm>(10),
                std::string("Parralel  |   10")
            },
            {
                std::make_unique<ParallelTorusAlgorithm>(12),
                std::string("Parralel  |   12")
            },
            {
                std::make_unique<ParallelTorusAlgorithm>(16),
                std::string("Parralel  |   16")
            }
        },
        {
            5'000,
            10'000,
            16'384,
            26'013,
            50'000,
            65'536,
            100'000,
        });

    return 0;
}
