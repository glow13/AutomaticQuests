#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {
	struct Fields : public GJChallengeDelegate {
		int m_totalRewards = 0;
		bool m_completed[4] = {false, false, false, false};

		void challengeStatusFinished() override;
		void challengeStatusFailed() override;
	};

	void incrementChallenge(GJChallengeType type, int amount);
	void tryGetChallenges();
	int getQuestRewardsAndReset();
	void resetQuestRewards();

	GJChallengeItem* getCompletedQuest(char const * desc);
	bool parseQuestInfo(char const * desc, int * amount, GJChallengeType * type);

	static inline GameStatsManagerAQ* sharedState() {
		auto stats = GameStatsManager::sharedState();
		return static_cast<GameStatsManagerAQ*>(stats);
	} // sharedState
};

namespace AutomaticQuests {
	inline bool isModDisabled() {
		return Mod::get()->getSettingValue<bool>("disable-mod");
	} // isModEnabled

	inline bool isFeatureDisabled(gd::string setting) {
		return isModDisabled() || Mod::get()->getSettingValue<bool>(setting);
	} // isSettingEnabled

	inline float getQuestTime() {
		return Mod::get()->getSettingValue<double>("quest-time");
	} // getQuestTime
};