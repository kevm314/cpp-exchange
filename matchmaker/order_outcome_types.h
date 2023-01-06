#pragma once

/**
 * @brief Supported types order outcomes that can arise when processing an order in the exchanges system.
 * 
 */
enum OrderOutcomeType {
    NOT_PROCESSED,
    SUCCESS,
    FAIL,
    INVALID_SYMBOL_OR_SIZE,
    INSUFFICIENT_LIQUIDITY
};