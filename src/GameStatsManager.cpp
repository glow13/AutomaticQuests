#include "GameStatsManager.hpp"

int GameStatsManagerAQ::s_totalRewards = 0;

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	GameStatsManager::incrementChallenge(type, amount);

	for (int i = 1; i <= 3; i++) {
		auto quest = getChallenge(i);
		if (quest && quest->m_challengeType == type && quest->m_canClaim) {
			auto key = getChallengeKey(quest);
			auto object = CCString::create(std::to_string(quest->m_reward));
			m_challengeDiamonds->setObject(object, key);
			processChallengeQueue(i);

			s_totalRewards += quest->m_reward;
			incrementStat("13", quest->m_reward);
			log::info("Automatically claimed quest #{}!", i);
		} // if
	} // for
} // incrementChallenge

// Had to rewrite from scratch because its inlined on Windows :/
gd::string GameStatsManagerAQ::getChallengeKey(GJChallengeItem* quest) {
	return fmt::format("c{}{}", quest->m_position, quest->m_timeLeft);
} // getChallengeKey

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewards() {
	int rewardsNum = s_totalRewards;
	s_totalRewards = 0;
	log::info("resetting rewards from {}", rewardsNum);
	return rewardsNum;
} // getQuestRewards