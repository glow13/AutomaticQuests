#include "GameStatsManager.hpp"

// BEFORE SHOW NEW BEST
void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	for (int i = 1; i <= 3; i++) {
		if (!getChallenge(i)) processChallengeQueue(i);
	} // for

	GameStatsManager::incrementChallenge(type, amount);

	for (int i = 1; i <= 3; i++) {
		auto quest = getChallenge(i);
		if (quest && quest->m_challengeType == type && quest->m_canClaim) {
			processChallengeQueue(i);

			s_totalRewards += quest->m_reward;
			incrementStat("13", quest->m_reward);
			log::info("automatically claimed quest #{}!", i);
		} // if
	} // for
} // incrementChallenge