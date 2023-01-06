#pragma once

#include <memory>
#include <vector>

#include "instrument_symbol.h"
#include "order_outcome_types.h"
#include "trade_event.h"
#include "trade_order.h"

namespace matchmaker {

/**
 * @brief Base class for order books. Derived classes implement appropriate order matching algorithms. 
 * Base class implementations will return a `FAIL` outcome by default if not implemented.
 * 
 */
class BaseOrderBook {
    private:
        const matchmaker::InstrumentSymbol instrument_symbol_;
    public:
        BaseOrderBook(
            matchmaker::InstrumentSymbol instrument_symbol
        );
        matchmaker::InstrumentSymbol GetInstrumentSymbol();
        virtual OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order);
        // execute new order
        virtual OrderOutcomeType ProcessNewOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events);
        virtual OrderOutcomeType CancelOrder(matchmaker::TradeOrder& trade_order);
        // Decrease size by lots amount
        virtual OrderOutcomeType ReduceOrderSize(matchmaker::TradeOrder& trade_order);
        virtual OrderOutcomeType AlterOrderPrice(matchmaker::TradeOrder& trade_order);
        virtual std::vector<matchmaker::TradeOrder> GetUserOrders(const std::array<uint8_t, 32>& user_id);
};

}
