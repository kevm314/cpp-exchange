#pragma once

#include <array>
#include <list>
#include <string>
#include <vector>

#include "base_producer.h"
#include "trade_order.h"

namespace events_receiver {

class PlaybackProducer : public BaseProducer<int, std::string, matchmaker::TradeOrder> {
    private:
        bool is_successfully_instantiated_ = false;
        std::list<matchmaker::TradeOrder> events_;
        std::list<matchmaker::TradeOrder>::iterator current_event_ = events_.begin();
    public:
        PlaybackProducer(int events_store, std::string configuration);
        bool ProcessPlaybackentry(std::string playback_entry);
        matchmaker::TradeOrder ReceiveEvent() override;
        bool IsInstantiated();
};

}