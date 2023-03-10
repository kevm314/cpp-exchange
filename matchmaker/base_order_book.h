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
        const matchmaker::InstrumentSymbol* instrument_symbol_;
    public:
        BaseOrderBook(
            matchmaker::InstrumentSymbol* instrument_symbol
        );
        const matchmaker::InstrumentSymbol* GetInstrumentSymbol();
        virtual OrderOutcomeType ConsumeOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) = 0;
        // execute new order
        virtual OrderOutcomeType ProcessNewOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) = 0;
        virtual OrderOutcomeType CancelOrder(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) = 0;
        // Decrease size by lots amount
        virtual OrderOutcomeType AlterOrderSize(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) = 0;
        virtual OrderOutcomeType AlterOrderPrice(matchmaker::TradeOrder& trade_order, std::shared_ptr<std::vector<matchmaker::TradeEvent>> trade_events) = 0;
        
};

}
