#include "DCQCN.hpp"

DCQCN::DCQCN() = default;

void DCQCN::ScheduleUpdateAlpha(){
        m_eventUpdateAlpha = scheduler.add([this]{UpdateAlpha();}, std::chrono::microseconds(m_alpha_timer_interval));
}

void DCQCN::UpdateAlpha(){
    if(m_alpha_cnp_arrived)     m_alpha = (1 - m_g) * m_alpha + m_g;
    else                        m_alpha = (1 - m_g) * m_alpha;
    m_alpha_cnp_arrived = false;
    ScheduleUpdateAlpha();
}
    
void DCQCN::cnp_received(){
    m_alpha_cnp_arrived = true; // set m_alpha_CNP for alpha update
    m_decrease_cnp_arrived = true; // set m_decrease_CNP for rate decrease
    if(m_first_cnp){
        m_alpha = 1;
        m_alpha_cnp_arrived = false;
        ScheduleUpdateAlpha();
        ScheduleRateDecrease();

        m_targetRate = m_rate;
        m_first_cnp = false;
    }
}

void DCQCN::CheckRateDecrease(){
    ScheduleRateDecrease();
    if(m_decrease_cnp_arrived){
        bool clamp = true;
        if(m_rpTimeStage == 0)  clamp = false;
        if(clamp)   m_targetRate = m_rate;
        m_rate = m_rate * (1 - m_alpha / 2);

        // reset rate increase staff
        m_rpTimeStage = 0;
        m_decrease_cnp_arrived = false;
        // Cancel the timer and arm a new one 
        scheduler.cancel(m_rpTimer);
        m_rpTimer = scheduler.add([this]{RateIncEventTimer();}, std::chrono::microseconds(m_rpgTimeReset));
    }
}
void DCQCN::ScheduleRateDecrease(){
    m_eventDecreaseRate = scheduler.add([this]{CheckRateDecrease();}, std::chrono::microseconds(m_decrease_timer_interval));
}

void DCQCN::RateIncEventTimer(){
    m_rpTimer = scheduler.add([this]{RateIncEventTimer();}, std::chrono::microseconds(m_rpgTimeReset));
    RateIncEvent();
    m_rpTimeStage++;
}

void DCQCN::RateIncEvent(){
    if(m_rpTimeStage < m_rpgThreshold)          FastRecovery(); 
    else if (m_rpTimeStage == m_rpgThreshold)   ActiveIncrease();
    else                                        HyperIncrease();
}

void DCQCN::FastRecovery(){
    m_rate = (m_rate >> 1) + (m_targetRate >> 1);
}

void DCQCN::ActiveIncrease(){
    m_targetRate = m_targetRate + m_rai;
    if(m_targetRate > m_maxRate)   m_targetRate = m_maxRate;
    m_rate = (m_rate >> 1) + (m_targetRate >> 1);
}

void DCQCN::HyperIncrease(){
    m_targetRate = m_targetRate + m_rhai;
    if(m_targetRate > m_maxRate)   m_targetRate = m_maxRate;
    m_rate = (m_rate >> 1) + (m_targetRate >> 1);
}

	
