#include "Vortex/chassis/chassis.hpp"
#include "Vortex/util.hpp"
#include "Vortex/logger/logger.hpp"
#include "pros/rtos.hpp"
#include "pros/screen.hpp"
#include "pros/misc.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <string>

namespace vortex {

// Default drive curve: linear, no deadband, no minimum output
ExpoDriveCurve defaultDriveCurve(0, 0, 1.0);

// ── ControllerSettings ────────────────────────────────────────────────────────

ControllerSettings::ControllerSettings(float kP, float kI, float kD, float windupRange,
                                       float smallError, float smallErrorTimeout,
                                       float largeError, float largeErrorTimeout,
                                       float slew, float maxVelocity, float maxAccel)
    : kP(kP), kI(kI), kD(kD), windupRange(windupRange),
      smallError(smallError), smallErrorTimeout(smallErrorTimeout),
      largeError(largeError), largeErrorTimeout(largeErrorTimeout),
      slew(slew), maxVelocity(maxVelocity), maxAccel(maxAccel) {}

// ── Drivetrain ────────────────────────────────────────────────────────────────

Drivetrain::Drivetrain(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors,
                       float trackWidth, float wheelDiameter, float rpm,
                       float horizontalDrift, float scrubFactor)
    : leftMotors(leftMotors), rightMotors(rightMotors),
      trackWidth(trackWidth), wheelDiameter(wheelDiameter), rpm(rpm),
      horizontalDrift(horizontalDrift), scrubFactor(scrubFactor) {}

// ── OdomSensors ───────────────────────────────────────────────────────────────

OdomSensors::OdomSensors(TrackingWheel* v1, TrackingWheel* v2,
                         TrackingWheel* h1, TrackingWheel* h2, pros::Imu* imu)
    : vertical1(v1), vertical2(v2), horizontal1(h1), horizontal2(h2), imu(imu) {}

// ── Chassis constructor ───────────────────────────────────────────────────────

Chassis::Chassis(Drivetrain dt, ControllerSettings lin, ControllerSettings ang,
                 OdomSensors s, DriveCurve* tCurve, DriveCurve* sCurve)
    : drivetrain(dt), linearSettings(lin), angularSettings(ang), sensors(s),
      throttleCurve(tCurve), steerCurve(sCurve),
      lateralPID(lin.kP, lin.kI, lin.kD, lin.windupRange, true),
      angularPID(ang.kP, ang.kI, ang.kD, ang.windupRange, true),
      lateralLargeExit(lin.largeError, 100, (int)lin.largeErrorTimeout),
      lateralSmallExit(lin.smallError, 300, (int)lin.smallErrorTimeout),
      angularLargeExit(ang.largeError, 100, (int)ang.largeErrorTimeout),
      angularSmallExit(ang.smallError, 300, (int)ang.smallErrorTimeout)
{
    if (!dt.leftMotors)  { infoSink()->fatal("Left motors are null!");  throw std::runtime_error("Vortex: left motors null"); }
    if (!dt.rightMotors) { infoSink()->fatal("Right motors are null!"); throw std::runtime_error("Vortex: right motors null"); }
    if (!s.vertical1 && !s.vertical2)
        infoSink()->warn("No vertical tracking wheels — odometry may be inaccurate.");

    odom = std::make_unique<Odom>(s.vertical1, s.vertical2, s.horizontal1, s.horizontal2,
                                  s.imu, dt.scrubFactor);
}

// ── calibrate ────────────────────────────────────────────────────────────────

void Chassis::calibrate(bool calibrateImu) {
    // IMU calibration with up to 5 retry attempts
    if (sensors.imu && calibrateImu) {
        for (int attempt = 1; attempt <= 5; attempt++) {
            sensors.imu->reset();
            pros::delay(10);
            while (sensors.imu->is_calibrating()) pros::delay(10);

            double h = sensors.imu->get_heading();
            if (!std::isnan(h) && !std::isinf(h)) break;

            infoSink()->warn("IMU calibration failed, attempt " + std::to_string(attempt));
            if (attempt == 5) {
                infoSink()->fatal("IMU failed all 5 calibration attempts!");
                throw std::runtime_error("Vortex: IMU calibration failed");
            }
        }
        infoSink()->info("IMU calibrated.");
    }

    // Odometry background task (10 ms update rate)
    if (!odomTask) {
        odomTask = new pros::Task([this]() {
            while (true) { odom->update(); pros::delay(10); }
        });
    }

    // Brain screen telemetry task (20 ms, 50 Hz — fast enough for visuals)
    if (!screenTask) {
        screenTask = new pros::Task([this]() {
            while (true) {
                Pose p = getPose(true, true); // standard position, radians

                // ── Field map (240×240 px, top-left corner) ──
                // VEX field ≈ 144 in square → scale = 240/144 ≈ 1.667 px/in
                const float scale = 240.0f / 144.0f;
                const int   cx    = 120; // screen center X
                const int   cy    = 120; // screen center Y

                pros::screen::set_pen(0x111111);
                pros::screen::fill_rect(0, 0, 240, 240);

                // Grid lines (6×6 tiles)
                pros::screen::set_pen(0x2A2A2A);
                for (int i = 0; i <= 6; i++) {
                    int px = i * 40;
                    pros::screen::draw_line(px, 0, px, 240);
                    pros::screen::draw_line(0, px, 240, px);
                }

                // Robot dot
                int rx = (int)(cx + p.x * scale);
                int ry = (int)(cy - p.y * scale);
                rx = std::clamp(rx, 2, 237);
                ry = std::clamp(ry, 2, 237);
                pros::screen::set_pen(0xFF4040);
                pros::screen::fill_circle(rx, ry, 5);

                // Heading arrow (15 px long)
                int ax = (int)(rx + std::sin(p.theta) * 15);
                int ay = (int)(ry - std::cos(p.theta) * 15);
                pros::screen::set_pen(0xFFFFFF);
                pros::screen::draw_line(rx, ry, ax, ay);

                // ── Telemetry text (right panel) ──
                pros::screen::set_pen(0xFFFFFF);
                pros::screen::print(pros::E_TEXT_MEDIUM, 248, 10,  "X:    %6.2f", p.x);
                pros::screen::print(pros::E_TEXT_MEDIUM, 248, 30,  "Y:    %6.2f", p.y);
                pros::screen::print(pros::E_TEXT_MEDIUM, 248, 50,  "HDG:  %6.1f", radToDeg(p.theta));

                // Motor temps
                std::vector<double> lTemps = drivetrain.leftMotors->get_temperature_all();
                std::vector<double> rTemps = drivetrain.rightMotors->get_temperature_all();
                double maxTemp = 0;
                for (auto t : lTemps) maxTemp = std::max(maxTemp, t);
                for (auto t : rTemps) maxTemp = std::max(maxTemp, t);

                // Red if hot, green if cool
                pros::screen::set_pen(maxTemp > 55 ? 0xFF4040 : 0x40FF40);
                pros::screen::print(pros::E_TEXT_MEDIUM, 248, 70, "TEMP: %5.1fC", maxTemp);

                // Battery
                int  batt  = pros::battery::get_capacity();
                pros::screen::set_pen(batt < 20 ? 0xFF4040 : 0xFFFFFF);
                pros::screen::print(pros::E_TEXT_MEDIUM, 248, 90, "BATT:  %4d%%", batt);

                pros::delay(20);
            }
        });
    }

    // Stall detection task
    if (!stallTask) {
        stallTask = new pros::Task([this]() {
            uint32_t stalledSince = 0;
            while (true) {
                if (motionRunning && stallThreshold > 0) {
                    double lVel  = avg(drivetrain.leftMotors->get_actual_velocity_all());
                    double rVel  = avg(drivetrain.rightMotors->get_actual_velocity_all());
                    double lVolt = std::abs(avg(drivetrain.leftMotors->get_voltage_all()));
                    double rVolt = std::abs(avg(drivetrain.rightMotors->get_voltage_all()));

                    // Robot is stalled if velocity is below threshold while receiving
                    // significant voltage (>33% = 4000 mV of 12000 mV max)
                    bool lowVel  = std::abs(lVel) < stallThreshold && std::abs(rVel) < stallThreshold;
                    bool highVolt = lVolt > 4000 || rVolt > 4000;

                    if (lowVel && highVolt) {
                        if (!stalledSince) stalledSince = pros::millis();
                        if ((int)(pros::millis() - stalledSince) > stallTime) {
                            stalled = true;
                            cancelMotion();
                            infoSink()->warn("Stall detected — motion cancelled.");
                        }
                    } else {
                        stalledSince = 0;
                        stalled      = false;
                    }
                } else {
                    stalledSince = 0;
                    stalled      = false;
                }
                pros::delay(10);
            }
        });
    }

    // SD card logging task (10 Hz)
    if (!loggingTask) {
        loggingTask = new pros::Task([this]() {
            bool headerDone = false;
            while (true) {
                if (loggingEnabled) {
                    FILE* f = fopen(logFilename.c_str(), "a");
                    if (f) {
                        if (!headerDone) {
                            fputs("time_ms,x,y,theta_deg,l_volt,r_volt,batt_v\n", f);
                            headerDone = true;
                        }
                        Pose p     = getPose(false, true);
                        double lV  = avg(drivetrain.leftMotors->get_voltage_all());
                        double rV  = avg(drivetrain.rightMotors->get_voltage_all());
                        double bat = pros::battery::get_voltage() / 1000.0;
                        fprintf(f, "%u,%.3f,%.3f,%.3f,%.0f,%.0f,%.3f\n",
                                pros::millis(), p.x, p.y, p.theta, lV, rV, bat);
                        fclose(f);
                    } else {
                        infoSink()->error("Cannot open log file: " + logFilename);
                        loggingEnabled = false;
                    }
                }
                pros::delay(100);
            }
        });
    }
}

// ── Pose ─────────────────────────────────────────────────────────────────────

void Chassis::setPose(float x, float y, float theta, bool radians) {
    odom->setPose(Pose(x, y, radians ? theta : degToRad(theta)));
}

void Chassis::setPose(Pose p, bool radians) {
    if (!radians) p.theta = degToRad(p.theta);
    odom->setPose(p);
}

Pose Chassis::getPose(bool radians, bool standardPos) {
    // odom stores theta as CW-from-north. standardPos converts to math-standard
    // CCW-from-east convention (used by some path planners).
    Pose p = odom->getPose(true);
    if (standardPos) p.theta = M_PI_2 - p.theta;
    if (!radians)    p.theta = radToDeg(p.theta);
    return p;
}

// ── Driver control ────────────────────────────────────────────────────────────

void Chassis::tank(int left, int right, bool disableDriveCurve) {
    if (disableDriveCurve) {
        drivetrain.leftMotors->move(left);
        drivetrain.rightMotors->move(right);
    } else {
        drivetrain.leftMotors->move(throttleCurve->curve(left));
        drivetrain.rightMotors->move(throttleCurve->curve(right));
    }
}

void Chassis::arcade(int throttle, int turn, bool disableDriveCurve, float desaturateBias) {
    if (!disableDriveCurve) {
        throttle = (int)std::round(throttleCurve->curve(throttle));
        turn     = (int)std::round(steerCurve->curve(turn));
    }

    // Field-relative: rotate joystick inputs by the robot's heading so
    // pushing the stick north always moves north regardless of robot facing.
    if (fieldRelative) {
        double theta  = getPose(true).theta;
        double sinT   = std::sin(theta);
        double cosT   = std::cos(theta);
        int newThrottle = (int)std::round(throttle * cosT + turn * sinT);
        int newTurn     = (int)std::round(-throttle * sinT + turn * cosT);
        throttle = newThrottle;
        turn     = newTurn;
    }

    // Desaturate: prevent one side from clipping by scaling both proportionally
    if (std::abs(throttle) + std::abs(turn) > 127) {
        double bias = desaturateBias;
        throttle = (int)(throttle * (1.0 - bias * std::abs(turn     / 127.0)));
        turn     = (int)(turn     * (1.0 - (1.0 - bias) * std::abs(throttle / 127.0)));
    }

    drivetrain.leftMotors->move(throttle + turn);
    drivetrain.rightMotors->move(throttle - turn);
}

void Chassis::curvature(int throttle, int turn, bool disableDriveCurve) {
    // At zero throttle, fall back to pure point turn (arcade)
    if (throttle == 0) { arcade(0, turn, disableDriveCurve); return; }

    if (!disableDriveCurve) {
        throttle = (int)throttleCurve->curve(throttle);
        turn     = (int)steerCurve->curve(turn);
    }

    // Curvature drive: differential based on current speed
    double leftPow  = throttle + (std::abs(throttle) * turn / 127.0);
    double rightPow = throttle - (std::abs(throttle) * turn / 127.0);

    // Normalize if either side exceeds 127
    double peak = std::max(std::abs(leftPow), std::abs(rightPow));
    if (peak > 127.0) { leftPow = leftPow / peak * 127.0; rightPow = rightPow / peak * 127.0; }

    drivetrain.leftMotors->move(leftPow);
    drivetrain.rightMotors->move(rightPow);
}

// ── Configuration setters ────────────────────────────────────────────────────

void Chassis::setBrakeMode(pros::motor_brake_mode_e_t mode) {
    drivetrain.leftMotors->set_brake_mode_all(mode);
    drivetrain.rightMotors->set_brake_mode_all(mode);
}

void Chassis::setStallDetection(double vel, int timeMs) {
    stallThreshold = vel;
    stallTime      = timeMs;
}

void Chassis::setLogging(bool enabled, std::string filename) {
    loggingEnabled = enabled;
    logFilename    = filename;
}

void Chassis::setFieldRelative(bool enabled)        { fieldRelative = enabled; }
void Chassis::setImuDriftCompensation(bool enabled) {
    imuDriftComp = enabled;
    if (odom) odom->setDriftCompensation(enabled);
}

// ── Motion lifecycle ─────────────────────────────────────────────────────────

void Chassis::requestMotionStart() {
    // If a motion is already running, queue this one and block until the mutex
    // is released by endMotion(). Only one motion can be queued at a time.
    if (motionRunning) motionQueued = true;
    else               motionRunning = true;
    mutex.take(TIMEOUT_MAX);
}

void Chassis::endMotion() {
    motionRunning = motionQueued; // if queued, start immediately
    motionQueued  = false;
    mutex.give();
}

// ── Waiting helpers ───────────────────────────────────────────────────────────

void Chassis::waitUntil(float dist) {
    pros::delay(10);
    while (distTraveled >= 0 && distTraveled <= dist) pros::delay(10);
}

void Chassis::waitUntilPose(float x, float y, float range) {
    pros::delay(10);
    while (distTraveled >= 0 && getPose().distance(Pose(x, y)) > range) pros::delay(10);
}

void Chassis::waitUntilDone() {
    pros::delay(10);
    while (distTraveled >= 0) pros::delay(10);
}

void Chassis::waitUntilDone(int timeout) {
    Timer t(timeout);
    while (distTraveled >= 0 && !t.isDone()) pros::delay(10);
}

// ── Motion control ────────────────────────────────────────────────────────────

void Chassis::cancelMotion()    { motionRunning = false; }
void Chassis::cancelAllMotions(){ motionRunning = false; motionQueued = false; }
bool Chassis::isInMotion() const { return motionRunning; }
bool Chassis::isStalled()  const { return stalled; }

} // namespace vortex
