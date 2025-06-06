#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {

	static inline int totalRewards = 0;

	// BEFORE SHOW NEW BEST
	void incrementChallenge(GJChallengeType type, int amount) {
		GameStatsManager::incrementChallenge(type, amount);

		totalRewards = 0;
		for (int i = 1; i <= 3; i++) {
			auto quest = getChallenge(i);
			if (quest && quest->m_challengeType == type && quest->m_canClaim) {
				processChallengeQueue(i);

				totalRewards += quest->m_reward;
				incrementStat("13", quest->m_reward);
				log::info("automatically claimed quest #{}!", i);
			} // if
		} // for
	} // incrementChallenge
};