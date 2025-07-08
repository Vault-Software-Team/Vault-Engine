#include "spdlog/common.h"
#include <Renderer/Logger.hpp>
#include <iostream>
#include <iterator>
#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
using namespace VaultRenderer;

Logger *Logger::instance;

Logger::Logger() {
    instance = this;
    std::cout << "setting up logger\n";
    // spdlog::set_pattern("%^[%H:%M:%S] %n: %v%$");

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/vault_engine.log", true));

    sinks[0]->set_pattern("%^[%T] [%l] %n:%$ %v");
    sinks[1]->set_pattern("[%T] [%l] %n: %v");

    engine_logger = std::make_shared<spdlog::logger>("VAULT", std::begin(sinks), std::end(sinks));
    app_logger = std::make_shared<spdlog::logger>("APP", std::begin(sinks), std::end(sinks));

    spdlog::register_logger(engine_logger);
    spdlog::register_logger(app_logger);

    engine_logger->set_level(spdlog::level::trace);
    app_logger->set_level(spdlog::level::trace);

    engine_logger->flush_on(spdlog::level::trace);
    app_logger->flush_on(spdlog::level::trace);
}
