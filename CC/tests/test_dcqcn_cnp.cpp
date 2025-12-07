// tests/test_dcqcn_cnp.cpp
#define private public
#define protected public
#include "DCQCN.hpp"
#undef private
#undef protected

#include <gtest/gtest.h>

TEST(DCQCN, SIMPLE_CHECK) {
    DCQCN d;
    
    // Test 1, check first CNP arrived
    EXPECT_TRUE(d.m_first_cnp);
    EXPECT_FALSE(d.m_alpha_cnp_arrived);
    EXPECT_FALSE(d.m_decrease_cnp_arrived);

    d.cnp_received();
    EXPECT_FALSE(d.m_first_cnp);
    EXPECT_EQ(d.m_targetRate, 40 * d.gbps);
    EXPECT_TRUE(d.m_decrease_cnp_arrived);

    d.CheckRateDecrease(); 
    EXPECT_FALSE(d.m_decrease_cnp_arrived);
    EXPECT_EQ(d.m_rate, d.m_targetRate >> 1);
    EXPECT_EQ(d.m_rpTimeStage, 0u);
    
    d.UpdateAlpha();
    EXPECT_DOUBLE_EQ(d.m_alpha, 0.9375);
}

TEST(DCQCN, SIMPLE_CHECK2) {
    DCQCN d;
    
    EXPECT_TRUE(d.m_first_cnp);
    EXPECT_FALSE(d.m_alpha_cnp_arrived);
    EXPECT_FALSE(d.m_decrease_cnp_arrived);

    d.cnp_received();
    EXPECT_FALSE(d.m_first_cnp);
    EXPECT_EQ(d.m_targetRate, 40 * d.gbps);
    EXPECT_TRUE(d.m_decrease_cnp_arrived);

    d.CheckRateDecrease(); 
    EXPECT_FALSE(d.m_decrease_cnp_arrived);
    EXPECT_EQ(d.m_rate, d.m_targetRate >> 1);
    EXPECT_EQ(d.m_rpTimeStage, 0u);

    d.cnp_received();  
    EXPECT_TRUE(d.m_decrease_cnp_arrived);
    EXPECT_TRUE(d.m_alpha_cnp_arrived);
    
    d.UpdateAlpha();
    EXPECT_DOUBLE_EQ(d.m_alpha, 1.0);
}

TEST(DCQCN, INC_CHECK) {
    DCQCN d;
    d.m_rate = 200;
    d.m_targetRate = 400;
    d.m_rai = 60;
    d.m_rhai = 600;
    d.m_rpgThreshold = 1;
    d.m_first_cnp = false;

    d.RateIncEventTimer();
    EXPECT_EQ(d.m_rpTimeStage, 1);
    EXPECT_EQ(d.m_rate, 300ull);

    d.RateIncEventTimer();
    EXPECT_EQ(d.m_rpTimeStage, 2);
    EXPECT_EQ(d.m_targetRate, 460ull);
    EXPECT_EQ(d.m_rate, 380ull);

    d.RateIncEventTimer();
    EXPECT_EQ(d.m_rpTimeStage, 3);
    EXPECT_EQ(d.m_targetRate, 1060ull);
    EXPECT_EQ(d.m_rate, 720ull);

    d.cnp_received();
    d.CheckRateDecrease(); 
    EXPECT_FALSE(d.m_decrease_cnp_arrived);
    EXPECT_EQ(d.m_rpTimeStage, 0u);
    EXPECT_EQ(d.m_targetRate, 720ull);
    EXPECT_EQ(d.m_rate, 360ull);

    d.RateIncEventTimer();
    EXPECT_EQ(d.m_rpTimeStage, 1);
    EXPECT_EQ(d.m_rate, 540ull);

}