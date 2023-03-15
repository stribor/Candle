#include "QtCore/qbytearray.h"
#include "parser/gcodeparser.h"
#include "parser/gcodepreprocessorutils.h"
#include "tables/gcodetablemodel.h"
#include <QBuffer>
#include <QFile>
#include <QIODevice>
#include <array>
#include <benchmark/benchmark.h>
#include <fmt/core.h>
#include <string>
#include <string_view>

double AtoF(std::string_view str);
double AtoF_char(char const *num);

std::string getnumber()
{
    double num = rand();
    return fmt::format("{: f};", num);
}

static void BM_StringCreation(benchmark::State &state)
{
    for (auto _ : state)
        std::string empty_string;
}
// Register the function as a benchmark
//BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State &state)
{
    std::string x = "hello";
    for (auto _ : state)
        std::string copy(x);
}
//BENCHMARK(BM_StringCopy);


static void BM_AtoF(benchmark::State &state)
{
//    std::string x = getnumber();

    for (auto _ : state) {
        state.PauseTiming();
        std::string x = getnumber();
        state.ResumeTiming();
        GcodePreprocessorUtils::AtoF(x);
    }
}
//BENCHMARK(BM_AtoF);

std::size_t loadFile(QIODevice &data, qint64 bytesAvailable)
{
    // Prepare parser
    GcodeParser gp;
    gp.reset(QVector3D(qQNaN(), qQNaN(), 0));
    GCodeTableModel programModel;
    auto &model_data = programModel.data();

    char lineBuf[1024];
    while (!data.atEnd()) {
        // auto command = data.readLine(100);
        // Trim command
#if 0
        auto const trimmed = data.readLine(1024).trimmed();
#else
        auto bytes_read = data.readLine(lineBuf, 1024);
        auto trimmed = Command(lineBuf, bytes_read);
#endif
        if (trimmed.size() > 0) {
            auto &item = model_data.emplace_back();

            // Split command
            auto &args = item.args;
            args = GcodePreprocessorUtils::splitCommand(trimmed);
            gp.addCommand(args);

            item.state = GCodeItem::InQueue;
            item.line = gp.getCommandNumber();
            item.command = trimmed;
        }
    }
    auto &psl = gp.getPointSegmentList();
    return psl.size();
}

void benchmark_parser(benchmark::State &state)
{
    QByteArray data;
    if (state.thread_index() == 0) {
         QFile file("/Volumes/Data/proj/GCodePainter/build/Cat2.txt");
        //QFile file("/Volumes/Data/proj/GCodePainter/build/GodzilaCat.nc");
        file.open(QIODevice::ReadOnly);
        data = file.readAll();
    }

    for (auto _ : state) {
        QBuffer buffer(&data);
        buffer.open(QIODevice::ReadOnly);
        std::size_t total = loadFile(buffer, buffer.size());
        benchmark::DoNotOptimize(total);
    }
}

//BENCHMARK(benchmark_parser);//->Threads(4);

Command removeWS(Command const &command)
{
    Command result;
    result.reserve(command.size());
    for (auto c : command) {
        if (!std::isspace(c))
            result.push_back(c);
    }
    return result;
}

Command removeWS2(Command command)
{
    command.resize(std::distance(command.begin(), std::remove_if(command.begin(), command.end(), [](char c) { return std::isspace(c); })));
    return command;
}
Command removeAllWhitespace(CommandView command)
{
    Command result;
    std::remove_copy_if(command.begin(), command.end(), std::back_inserter(result), std::isspace);
    return result;
}

Command removeAllWhitespace2(CommandView command)
{
    Command result;
    result.reserve(command.size());
    std::remove_copy_if(command.begin(), command.end(), std::back_inserter(result),  std::isspace);
    return result;
}
