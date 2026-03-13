#include "Vortex/chassis/trackingWheel.hpp"
#include "Vortex/util.hpp"
#include "Vortex/logger/logger.hpp"
#include <cmath>
#include <stdexcept>
#include <vector>
#include <numeric>

namespace vortex {

TrackingWheel::TrackingWheel(pros::adi::Encoder* enc, double wheelDiameter,
                             double distance, double gearRatio)
    : encoder(enc), diameter(wheelDiameter), distance(distance), gearRatio(gearRatio) {
    if (!enc) {
        infoSink()->fatal("ADI Encoder passed to TrackingWheel is null!");
        throw std::runtime_error("Vortex: null ADI Encoder");
    }
}

TrackingWheel::TrackingWheel(pros::Rotation* rot, double wheelDiameter,
                             double distance, double gearRatio)
    : rotation(rot), diameter(wheelDiameter), distance(distance), gearRatio(gearRatio) {
    if (!rot) {
        infoSink()->fatal("Rotation sensor passed to TrackingWheel is null!");
        throw std::runtime_error("Vortex: null Rotation sensor");
    }
}

TrackingWheel::TrackingWheel(pros::MotorGroup* motors, double wheelDiameter,
                             double distance, double rpm)
    : motors(motors), diameter(wheelDiameter), distance(distance), rpm(rpm) {
    if (!motors) {
        infoSink()->fatal("MotorGroup passed to TrackingWheel is null!");
        throw std::runtime_error("Vortex: null MotorGroup");
    }
    motors->set_encoder_units_all(pros::E_MOTOR_ENCODER_ROTATIONS);
}

void TrackingWheel::reset() {
    if (encoder)  encoder->reset();
    if (rotation) rotation->reset_position();
    if (motors)   motors->tare_position_all();
}

double TrackingWheel::getDistanceTraveled() {
    if (encoder) {
        // ADI quadrature: 360 ticks per revolution
        // distance = ticks / 360 * π * d / gearRatio
        return encoder->get_value() * M_PI * diameter / (360.0 * gearRatio);
    }

    if (rotation) {
        // V5 Rotation sensor: 36 000 centidegrees per revolution
        // distance = centideg / 36000 * π * d / gearRatio
        return rotation->get_position() * M_PI * diameter / (36000.0 * gearRatio);
    }

    if (motors) {
        // Motor encoder in rotations mode.
        // RPM ratio converts motor shaft rotations to wheel rotations.
        std::vector<pros::MotorGears> gearsets  = motors->get_gearing_all();
        std::vector<double>           positions  = motors->get_position_all();
        std::vector<double>           distList;

        for (size_t i = 0; i < positions.size(); i++) {
            double cartridgeRpm;
            pros::MotorGears g = (i < gearsets.size()) ? gearsets[i] : pros::MotorGears::green;
            switch (g) {
                case pros::MotorGears::red:   cartridgeRpm = 100;  break;
                case pros::MotorGears::green: cartridgeRpm = 200;  break;
                case pros::MotorGears::blue:  cartridgeRpm = 600;  break;
                default:                      cartridgeRpm = 200;  break;
            }
            distList.push_back(positions[i] * (rpm / cartridgeRpm) * M_PI * diameter);
        }
        if (distList.empty()) return 0;
        return std::accumulate(distList.begin(), distList.end(), 0.0) / distList.size();
    }

    return 0;
}

double TrackingWheel::getOffset() const { return distance; }
int    TrackingWheel::getType()   const { return (motors != nullptr) ? 1 : 0; }

} // namespace vortex
