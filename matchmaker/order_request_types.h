#pragma once

/**
 * @brief Supported types of order requests available in exchange system.
 * 
 */
enum OrderRequestType {
    PLACE_ORDER,
    CANCEL_ORDER,
    ALTER_ORDER_PRICE,
    ALTER_ORDER_SIZE
};