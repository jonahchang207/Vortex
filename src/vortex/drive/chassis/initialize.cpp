#include "vortex/drive/chassis.hpp"
#include "vortex/util/gui/loading.hpp"
#include "pros/rtos.hpp"

namespace vortex {

void Chassis::initialize() {
    gui::show_loading_screen();
    
    // Start IMU Calibration
    if (config.odom_config.imu) {
        config.odom_config.imu->reset();
    }

    // Monitoring Loop
    int timeout = 0;
    while (config.odom_config.imu && config.odom_config.imu->is_calibrating()) {
        gui::update_loading_bar((timeout / 3000.0) * 100); // Faked % based on typical 3s
        
        // Error Check
        if (static_cast<int>(config.odom_config.imu->get_status()) & 0x01) { // Error flag
             gui::show_loading_error();
             // Continuously vibrate as requested
             while(true) {
                 pros::Controller master(pros::E_CONTROLLER_MASTER);
                 master.rumble(".");
                 pros::delay(500);
             }
        }
        
        pros::delay(10);
        timeout += 10;
        if (timeout > 5000) break; // Safety timeout
    }

    gui::hide_loading_screen();
    odom.start();
    original_left = config.left_motors;
    original_right = config.right_motors;
    
    linear_pid.reset();
    angular_pid.reset();
}

} // namespace vortex
