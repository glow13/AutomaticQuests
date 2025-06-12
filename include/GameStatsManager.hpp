#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {
private:
	inline static int s_totalRewards = 0;
public:
	void incrementChallenge(GJChallengeType type, int amount);

	// Resets the rewards to zero after use
	inline static int getQuestRewards() {
		int rewardsNum = s_totalRewards;
		s_totalRewards = 0;
		log::info("resetting rewards from {}", rewardsNum);
		return rewardsNum;
	} // getQuestRewards
};