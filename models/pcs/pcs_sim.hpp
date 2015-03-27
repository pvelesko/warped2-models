#ifndef PCS_HPP
#define PCS_HPP

#include <string>
#include <vector>
#include <memory>

#include "warped.hpp"

#include "MLCG.h"
#include "NegExp.h"


class Portable {
public:

    Portable(   unsigned int pid, 
                bool is_busy, 
                unsigned int call_arrival_ts, 
                unsigned int call_interval, 
                unsigned int call_duration      )
        :   pid_(pid), 
            is_busy_(is_busy), 
            call_arrival_ts_(call_arrival_ts), 
            call_interval_(call_interval), 
            call_duration_(call_duration) {}

    unsigned int    pid_;
    bool            is_busy_;
    unsigned int    call_arrival_ts_;
    unsigned int    call_interval_;
    unsigned int    call_duration_;
};

WARPED_DEFINE_OBJECT_STATE_STRUCT(PcsState) {

    PcsState() = default;
    PcsState(const PcsState& other) {
        idle_channel_cnt_ = other.idle_channel_cnt_;
        for (auto it = other.portables_.begin(); it != other.portables_.end(); it++) {
            auto portable = it->second;
            auto new_portable = std::make_shared<Portable>( portable->pid_,
                                                            portable->is_busy_,
                                                            portable->call_arrival_ts_,
                                                            portable->call_interval_,
                                                            portable->call_duration_    );
            portables_.insert(portables_.begin(), 
                std::pair <unsigned int, std::shared_ptr<Portable>> (portable->pid_, 
                                                                            new_portable));
        }
    };

    unsigned int idle_channel_cnt_;
    std::map <unsigned int, std::shared_ptr<Portable>> portables_;
};

enum event_type_t {

    CALL_ARRIVAL,
    CALL_COMPLETION,
    PORTABLE_MOVE_OUT,
    PORTABLE_MOVE_IN
};

enum direction_t {

    LEFT,
    RIGHT,
    DOWN,
    UP
};

class PcsEvent : public warped::Event {
public:

    PcsEvent() = default;

    PcsEvent(const std::string receiver_name, unsigned int timestamp, 
                        std::shared_ptr<Portable> portable, event_type_t event_type)
        : receiver_name_(receiver_name), event_timestamp_(timestamp), event_type_(event_type) {

        assert (portable != nullptr);
        pid_                = portable->pid_;
        is_busy_            = portable->is_busy_;
        call_arrival_ts_    = portable->call_arrival_ts_;
        call_interval_      = portable->call_interval_;
        call_duration_      = portable->call_duration_;
    }

    const std::string& receiverName() const { return receiver_name_; }
    unsigned int timestamp() const { return event_timestamp_; }

    std::string     receiver_name_;
    unsigned int    event_timestamp_;
    event_type_t    event_type_;
    unsigned int    pid_;
    bool            is_busy_;
    unsigned int    call_arrival_ts_;
    unsigned int    call_interval_;
    unsigned int    call_duration_;

    WARPED_REGISTER_SERIALIZABLE_MEMBERS(cereal::base_class<warped::Event>(this), 
                                            receiver_name_, event_timestamp_, event_type_, 
                                            pid_, is_busy_, call_arrival_ts_, call_interval_, 
                                            call_duration_)
};

class PcsCell : public warped::SimulationObject {
public:

    PcsCell(    const std::string& name, 
                unsigned int num_cells_x, 
                unsigned int num_cells_y, 
                unsigned int travel_duration, 
                unsigned int channel_cnt, 
                std::vector<std::shared_ptr<Portable>> portables, 
                unsigned int index      )

        :   SimulationObject(name), 
            state_(), 
            num_cells_x_(num_cells_x), 
            num_cells_y_(num_cells_y),
            travel_duration_(travel_duration),
            channel_cnt_(channel_cnt),
            index_(index),
            rng_(new MLCG) {

        // Update the state variables
        state_.idle_channel_cnt_ = channel_cnt_;
        for (auto& portable : portables) {
            state_.portables_.insert(state_.portables_.begin(), 
                std::pair <unsigned int, std::shared_ptr<Portable>> (portable->pid_, portable));
        }
    }

    virtual warped::ObjectState& getState() { return this->state_; }

    virtual std::vector<std::shared_ptr<warped::Event> > createInitialEvents();

    virtual std::vector<std::shared_ptr<warped::Event> > receiveEvent(const warped::Event&);

protected:

    PcsState                state_;
    unsigned int            num_cells_x_;
    unsigned int            num_cells_y_;
    unsigned int            travel_duration_;
    unsigned int            channel_cnt_;
    unsigned int            index_;
    std::shared_ptr<MLCG>   rng_;

    std::string compute_move(direction_t direction);
    std::string random_move();
};

#endif
