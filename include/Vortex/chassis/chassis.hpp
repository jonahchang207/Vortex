#pragma once
#include "Vortex/chassis/odom.hpp"
#include "Vortex/pid.hpp"
#include "Vortex/pose.hpp"
#include "Vortex/exitcondition.hpp"
#include "Vortex/driveCurve.hpp"
#include "Vortex/asset.hpp"
#include "Vortex/util.hpp"
#include "pros/motors.hpp"
#include "pros/motor_group.hpp"
#include "pros/imu.hpp"
#include "pros/rtos.hpp"
#include <memory>
#include <string>

namespace vortex {

// ── Configuration structs ─────────────────────────────────────────────────────

/**
 * @brief Physical drivetrain dimensions.
 *
 * @param trackWidth      Left-to-right distance between wheel centers (inches).
 * @param wheelDiameter   Drive wheel diameter (inches).
 * @param rpm             Output shaft RPM (e.g., 360 for 36:60 on blue cartridge).
 * @param horizontalDrift Lateral friction coefficient (start with 2.0, tune up/down).
 * @param scrubFactor     Heading scale factor — increase if robot under-turns, decrease if over-turns.
 */
class Drivetrain {
public:
    Drivetrain(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors,
               float trackWidth, float wheelDiameter, float rpm,
               float horizontalDrift, float scrubFactor = 1.0);

    pros::MotorGroup* leftMotors;
    pros::MotorGroup* rightMotors;
    float trackWidth;
    float wheelDiameter;
    float rpm;
    float horizontalDrift;
    float scrubFactor;
};

/**
 * @brief PID tuning constants + exit conditions for one axis of motion.
 *
 * @param kP, kI, kD       PID gains.
 * @param windupRange      Integral accumulates only when |error| < this.
 * @param smallError       "Settled" threshold (inches or degrees).
 * @param smallErrorTimeout  ms to stay within smallError before exiting.
 * @param largeError       "Close enough" threshold.
 * @param largeErrorTimeout  Global motion timeout from first update (ms).
 * @param slew             Max motor-power change per 10 ms loop.
 * @param maxVelocity      Max motor power (0–127).
 * @param maxAccel         Max acceleration for motion profiles.
 */
class ControllerSettings {
public:
    ControllerSettings(float kP, float kI, float kD, float windupRange,
                       float smallError, float smallErrorTimeout,
                       float largeError, float largeErrorTimeout,
                       float slew, float maxVelocity = 127, float maxAccel = 127);
    float kP, kI, kD;
    float windupRange;
    float smallError, smallErrorTimeout;
    float largeError, largeErrorTimeout;
    float slew;
    float maxVelocity;
    float maxAccel;
};

/**
 * @brief Sensor configuration passed to the Chassis.
 *
 * Pass nullptr for any sensor that isn't used.
 * At minimum you need either two vertical tracking wheels OR one + an IMU.
 */
class OdomSensors {
public:
    OdomSensors(TrackingWheel* vertical1, TrackingWheel* vertical2,
                TrackingWheel* horizontal1, TrackingWheel* horizontal2,
                pros::Imu* imu);
    TrackingWheel* vertical1;
    TrackingWheel* vertical2;
    TrackingWheel* horizontal1;
    TrackingWheel* horizontal2;
    pros::Imu*     imu;
};

// ── Motion parameter structs ──────────────────────────────────────────────────

struct TurnToHeadingParams {
    AngularDirection direction  = AngularDirection::AUTO;
    int  maxSpeed               = 127;
    int  minSpeed               = 0;
    float earlyExitRange        = 0;
};

struct TurnToPointParams {
    bool forwards               = true;
    AngularDirection direction  = AngularDirection::AUTO;
    int  maxSpeed               = 127;
    int  minSpeed               = 0;
    float earlyExitRange        = 0;
};

struct SwingToHeadingParams {
    AngularDirection direction  = AngularDirection::AUTO;
    int  maxSpeed               = 127;
    int  minSpeed               = 0;
    float earlyExitRange        = 0;
};

struct MoveToPoseParams {
    bool  forwards              = true;
    float horizontalDrift       = 0;  ///< 0 = use drivetrain default
    float lead                  = 0.6;
    float maxSpeed              = 127;
    float minSpeed              = 0;
    float earlyExitRange        = 0;
};

struct MoveToPointParams {
    bool  forwards              = true;
    float maxSpeed              = 127;
    float minSpeed              = 0;
    float earlyExitRange        = 0;
};

// ── Chassis ───────────────────────────────────────────────────────────────────

extern ExpoDriveCurve defaultDriveCurve;

/**
 * @brief Main robot controller — wraps odometry, PID, and all motion primitives.
 */
class Chassis {
public:
    Chassis(Drivetrain drivetrain, ControllerSettings linearSettings,
            ControllerSettings angularSettings, OdomSensors sensors,
            DriveCurve* throttleCurve = &defaultDriveCurve,
            DriveCurve* steerCurve   = &defaultDriveCurve);

    // ── Setup ────────────────────────────────────────────────────────────────
    void calibrate(bool calibrateImu = true);
    void calibratePhysical();

    // ── Odometry ─────────────────────────────────────────────────────────────
    void setPose(float x, float y, float theta, bool radians = false);
    void setPose(Pose pose, bool radians = false);
    Pose getPose(bool radians = false, bool standardPos = false);

    // ── Movement ─────────────────────────────────────────────────────────────
    void moveToPose(float x, float y, float theta, int timeout,
                    MoveToPoseParams params = {}, bool async = false);

    void moveToPoint(float x, float y, int timeout,
                     MoveToPointParams params = {}, bool async = false);

    void turnThenMove(float x, float y, int timeout,
                      MoveToPointParams params = {}, bool async = false);

    void turnToHeading(float heading, int timeout,
                       TurnToHeadingParams params = {}, bool async = false);

    void turnToPoint(float x, float y, int timeout,
                     TurnToPointParams params = {}, bool async = false);

    void swingToHeading(float heading, DriveSide side, int timeout,
                        SwingToHeadingParams params = {}, bool async = false);

    void follow(const asset& path, float lookahead, int timeout,
                bool forwards = true, bool async = false);

    // ── Tuning ───────────────────────────────────────────────────────────────
    void tuneLateral(float dist, int timeout = 2000);
    void tuneAngular(float heading, int timeout = 2000);

    // ── Driver control ───────────────────────────────────────────────────────
    void tank(int left, int right, bool disableDriveCurve = false);
    void arcade(int throttle, int turn, bool disableDriveCurve = false,
                float desaturateBias = 0.5);
    void curvature(int throttle, int turn, bool disableDriveCurve = false);

    // ── Configuration ────────────────────────────────────────────────────────
    void setBrakeMode(pros::motor_brake_mode_e_t mode);
    void setStallDetection(double velocityThreshold, int timeMs);
    void setLogging(bool enabled, std::string filename = "/usd/vortex_log.csv");
    void setFieldRelative(bool enabled);
    void setImuDriftCompensation(bool enabled);

    // ── Waiting ──────────────────────────────────────────────────────────────
    void waitUntil(float dist);
    void waitUntilPose(float x, float y, float range);
    void waitUntilDone();
    void waitUntilDone(int timeout);

    // ── State ────────────────────────────────────────────────────────────────
    void cancelMotion();
    void cancelAllMotions();
    bool isInMotion()  const;
    bool isStalled()   const;

    PID lateralPID;
    PID angularPID;

protected:
    void requestMotionStart();
    void endMotion();

    Drivetrain         drivetrain;
    ControllerSettings linearSettings;
    ControllerSettings angularSettings;
    OdomSensors        sensors;
    DriveCurve*        throttleCurve;
    DriveCurve*        steerCurve;
    std::unique_ptr<Odom> odom;

    ExitCondition lateralLargeExit;
    ExitCondition lateralSmallExit;
    ExitCondition angularLargeExit;
    ExitCondition angularSmallExit;

    pros::Task*  odomTask     = nullptr;
    pros::Task*  screenTask   = nullptr;
    pros::Task*  stallTask    = nullptr;
    pros::Task*  loggingTask  = nullptr;
    pros::Mutex  mutex;

    float  distTraveled      = -1;
    bool   motionRunning     = false;
    bool   motionQueued      = false;
    bool   stalled           = false;
    double stallThreshold    = 0;
    int    stallTime         = 250;
    bool   loggingEnabled    = false;
    std::string logFilename  = "/usd/vortex_log.csv";
    bool   fieldRelative     = false;
    bool   imuDriftComp      = false;
};

} // namespace vortex
