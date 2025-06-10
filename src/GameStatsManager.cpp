#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>

using namespace geode::prelude;

class $modify(GSM, GameStatsManager) {

	inline static int s_totalRewards = 0;

	// BEFORE SHOW NEW BEST
	void incrementChallenge(GJChallengeType type, int amount) {
		for (int i = 1; i <= 3; i++) {
			if (!getChallenge(i)) processChallengeQueue(i);
		} // for

 		GameStatsManager::incrementChallenge(type, amount);

		s_totalRewards = 0;
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

	static int getQuestRewards() {
		int rewardsNum = s_totalRewards;
		s_totalRewards = 0;
		return rewardsNum;
	} // getQuestRewards
};