#pragma once



namespace events_receiver {

template <class T1, class T2, class T3>
class BaseProducer {
    private:
        T1 events_store_;
        T2 configuration_;
    public:
        BaseProducer(T1 events_store, T2 configuration): events_store_(events_store), configuration_(configuration) {};
        virtual ~BaseProducer() = default;
        virtual T3 ReceiveEvent() = 0;
};

}