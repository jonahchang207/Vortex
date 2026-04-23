#pragma once
#include "pros/adi.hpp"

namespace vortex {

/**
 * @brief Simple wrapper for pneumatic pistons
 */
class Piston {
public:
    /**
     * @brief Construct a new Piston object
     * 
     * @param port ADI port (1-8 or 'A'-'H')
     * @param default_state Starting state (false = retracted, true = extended)
     */
    Piston(int8_t port, bool default_state = false)
        : piston(port), state(default_state) {
        piston.set_value(state);
    }

    /**
     * @brief Set the state of the piston
     * 
     * @param state true to extend, false to retract
     */
    void set(bool state) {
        this->state = state;
        piston.set_value(state);
    }

    /**
     * @brief Toggles the current state of the piston
     */
    void toggle() {
        set(!state);
    }

    /**
     * @brief Extend the piston
     */
    void extend() {
        set(true);
    }

    /**
     * @brief Retract the piston
     */
    void retract() {
        set(false);
    }

    /**
     * @brief Gets current state
     */
    bool get() const {
        return state;
    }

private:
    pros::adi::DigitalOut piston;
    bool state;
};

} // namespace vortex
