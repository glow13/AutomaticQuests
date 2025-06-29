#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {
	struct Fields {
		int m_totalRewards = 0;
		bool m_completed[4] = {false, false, false, false};
	};

	void incrementChallenge(GJChallengeType type, int amount);
	gd::string getChallengeKey(GJChallengeItem* quest);
	int getQuestRewardsAndReset();
	void resetQuestRewards();

	static inline GameStatsManagerAQ* sharedState() {
		auto stats = GameStatsManager::sharedState();
		return static_cast<GameStatsManagerAQ*>(stats);
	} // sharedState
};

class $modify(MenuLayerAQ, MenuLayer) {
    bool init();
};

inline bool isModDisabled() {
	return Mod::get()->getSettingValue<bool>("disable-mod");
} // isModEnabled

inline bool isFeatureDisabled(gd::string setting) {
	return isModDisabled() || Mod::get()->getSettingValue<bool>(setting);
} // isSettingEnabled