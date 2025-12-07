#pragma once
#include <cstdint>
#include <chrono>
#include "Scheduler.hpp"

class DCQCN{
public:
    using DataRate = std::uint64_t;

    DCQCN();
    void cnp_received();
    void UpdateAlpha();
    void ScheduleUpdateAlpha();
    void CheckRateDecrease();
    void ScheduleRateDecrease();
    void RateIncEventTimer();
    void RateIncEvent();
    void FastRecovery();
    void ActiveIncrease();
    void HyperIncrease();

    static constexpr DataRate kbps = 1000ULL;
    static constexpr DataRate mbps = 1000ULL * kbps;
    static constexpr DataRate gbps = 1000ULL * mbps;
    
    DataRate m_rate = 40 * gbps; // Current rate
    DataRate m_targetRate;	// Target rate
	DataRate m_rai = 5 * mbps;		// Rate of additive increase 
	DataRate m_rhai = 50 * mbps;		// Rate of hyper-additive increase
    DataRate m_maxRate = 40 * gbps;     // 40Gb/s

    double m_alpha = 1;
    double m_g = 1.0 / 16; //feedback weight
    bool m_alpha_cnp_arrived = false; // indicate if CNP arrived in the last slot
    bool m_first_cnp = true; // indicate if the current CNP is the first CNP
    bool m_decrease_cnp_arrived = false; // indicate if CNP arrived in the last slot
    uint32_t m_rpTimeStage = 0;
    uint32_t m_rpgThreshold = 5; // 5 in default
    
    Scheduler::TaskId m_rpTimer;
    Scheduler::TaskId m_eventDecreaseRate;
    Scheduler::TaskId m_eventUpdateAlpha;

    uint32_t t_multiplier =  150;
	uint32_t m_alpha_timer_interval = 55 * t_multiplier; // 55 by default
    uint32_t m_decrease_timer_interval = 4 * t_multiplier; // 4 mircosends by default
    uint32_t m_rpgTimeReset = 1500 * t_multiplier;

    Scheduler scheduler;
};