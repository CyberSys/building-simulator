//
// Created by bartek on 25.03.19.
//

#include "configuration.h"
#include <regex>
#include <iostream>
#include <vector>
#include <string>
#include "yaml-cpp/yaml.h"
#include "utills.h"

namespace configuration {

    template<typename Config>
    Config loadConfiguration(const std::string &path) {
        const YAML::Node yamlNode = YAML::LoadFile(path);
        return yamlNode.as<Config>();
    }

    template Configuration loadConfiguration<Configuration>(const std::string &path);

    Time::Time() {}

    Time::Time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
        setHours(hours);
        setMinutes(minutes);
        setSeconds(seconds);
    }

    uint8_t Time::getHours() const {
        return hours;
    }

    void Time::setHours(uint8_t hours) {
        if (hours > 23)
            Time::hours = 23;
        else
            Time::hours = hours;
    }

    uint8_t Time::getMinutes() const {
        return minutes;
    }

    void Time::setMinutes(uint8_t minutes) {
        if (minutes > 59)
            Time::minutes = 59;
        else
            Time::minutes = minutes;
    }

    uint8_t Time::getSeconds() const {
        return seconds;
    }

    void Time::setSeconds(uint8_t seconds) {
        if (seconds > 59)
            Time::seconds = 59;
        else
            Time::seconds = seconds;
    }

    uint32_t Time::getTimestamp() const {
        return (((hours * 60u) + minutes) * 60u + seconds) * 1000u;
    }

    std::string Time::toString() const {
        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%.2hhu:%.2hhu:%.2hhu", hours, minutes, seconds);
        return std::string(buffer);
    }

    Time &Time::operator=(const Time &time) {
        Time::hours = time.hours;
        Time::minutes = time.minutes;
        Time::seconds = time.seconds;
        return *this;
    }

    using ::operator<<;

    std::ostream &operator<<(std::ostream &ostream, const configuration::Time &time) {
        return ostream << time.toString();
    }

    std::ostream &operator<<(std::ostream &ostream, const configuration::TemperaturePoint &temperaturePoint) {
        return ostream << "TemperaturePoint(time=" << temperaturePoint.time << ",value=" << temperaturePoint.value
                       << ')';
    }

    std::ostream &operator<<(std::ostream &ostream, const configuration::Configuration &configuration) {
        return ostream << "Configuration(temperatures=" << configuration.temperatures << ",\ntimeStep="
                       << configuration.timeStep << ",\ntimeDelay=" << configuration.timeDelay << ')';
    }
}

namespace YAML {

    bool convert<configuration::Configuration>::decode(const Node &node, configuration::Configuration &rhs) {
        rhs.temperatures = node["temperatures"].as < std::vector < configuration::TemperaturePoint >> ();
        rhs.timeStep = node["timeStep"].as<std::string>();
        rhs.timeDelay = node["timeDelay"].as<std::string>();
        return true;
    }


    bool convert<configuration::TemperaturePoint>::decode(const Node &node, configuration::TemperaturePoint &rhs) {
        if (!node.IsMap()) {
            return false;
        }
        rhs.time = node["time"].as<configuration::Time>();
        rhs.value = node["value"].as<float>();
        return true;
    }


    bool convert<configuration::Time>::decode(const Node &node, configuration::Time &rhs) {
        if (!node.IsScalar()) {
            return false;
        }
        const std::string &scalar = node.Scalar();
        const std::regex regex(R"((\d+):(\d+):(\d+))");
        std::smatch match;
        if (regex_match(scalar, match, regex)) {
            rhs.setHours(static_cast<uint8_t>(std::stoi(match[1].str())));
            rhs.setMinutes(static_cast<uint8_t>(std::stoi(match[2].str())));
            rhs.setSeconds(static_cast<uint8_t>(std::stoi(match[3].str())));
            return true;
        } else {
            return false;
        }
    }

}