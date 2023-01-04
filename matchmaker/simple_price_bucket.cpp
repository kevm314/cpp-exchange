#include <stdint.h>

#include <array>
#include <vector>

#include "base_order_book.h"
#include "instrument_symbol.h"
#include "simple_order_book.h"
#include "simple_price_bucket.h"

namespace matchmaker {

SimplePriceBucket::SimplePriceBucket(
    uint64_t price
) {
    price_ = price;
    total_volume_ = 0;
    order_list_head_ = nullptr;
}
uint64_t SimplePriceBucket::GetPrice() {
    return price_;
}
uint64_t SimplePriceBucket::GetTotalVolume() {
    return total_volume_;
}
matchmaker::TradeOrder* SimplePriceBucket::GetFirstOrder() {
    return order_list_head_;
}
// bool SimplePriceBucket::InsertOrder(matchmaker::TradeOrder& trade_order) {
//     return true;
// }

}