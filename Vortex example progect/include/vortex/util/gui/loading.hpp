#pragma once

namespace vortex {
namespace gui {

/**
 * @brief Show the calibration loading screen
 */
void show_loading_screen();

/**
 * @brief Update the loading bar percentage (0-100)
 */
void update_loading_bar(int percent);

/**
 * @brief Trigger error state (Red pulse + rumble)
 */
void show_loading_error();

/**
 * @brief Remove loading screen from view
 */
void hide_loading_screen();

} // namespace gui
} // namespace vortex
