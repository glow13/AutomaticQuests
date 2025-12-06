#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/AchievementNotifier.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {
	struct Fields {
		int m_totalRewards = 0;
		bool m_completed[4] = {false, false, false, false};
	};

	void incrementChallenge(GJChallengeType type, int amount);
	int getQuestRewardsAndReset();
	void resetQuestRewards();

	GJChallengeItem* getCompletedQuest(char const * desc);
	bool parseQuestInfo(char const * desc, int * amount, GJChallengeType * type);

	static inline GameStatsManagerAQ* sharedState() {
		auto stats = GameStatsManager::sharedState();
		return static_cast<GameStatsManagerAQ*>(stats);
	} // sharedState
};

class $modify(PlayLayerAQ, PlayLayer) {
	void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle);
	bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects);
};

class $modify(AchievementNotifierAQ, AchievementNotifier) {
	void notifyAchievement(char const* title, char const* desc, char const* icon, bool quest);
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

inline float getQuestTime() {
	return Mod::get()->getSettingValue<double>("quest-time");
} // getQuestTime