#pragma once

/**
 * @brief Supported types of order requests available in exchange system.
 * 
 */
enum OrderRequestType {
    PLACE_ORDER,
    CANCEL_ORDER,
    MOVE_ORDER,
    REDUCE_ORDER
};