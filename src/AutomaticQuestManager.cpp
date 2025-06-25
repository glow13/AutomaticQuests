#include "AutomaticQuestManager.hpp"

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	log::info("GameStatsManager::incrementChallenge");
	GameStatsManager::incrementChallenge(type, amount);

	for (int i = 1; i <= 3; i++) {
		auto quest = getChallenge(i);
		if (quest && quest->m_challengeType == type && quest->m_canClaim) {
			auto key = getChallengeKey(quest);
			auto object = CCString::create(std::to_string(quest->m_reward));
			m_challengeDiamonds->setObject(object, key);
			processChallengeQueue(i);

			m_fields->m_totalRewards += quest->m_reward;
			incrementStat("13", quest->m_reward);
			log::info("Automatically claimed quest #{}!", i);
			log::info("rewards = {} {}", m_fields->m_totalRewards, quest->m_reward);
		} // if
	} // for
} // incrementChallenge

// Had to rewrite from scratch because its inlined on Windows :/
gd::string GameStatsManagerAQ::getChallengeKey(GJChallengeItem* quest) {
	return fmt::format("c{}{}", quest->m_position, quest->m_timeLeft);
} // getChallengeKey

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewards() {
	//if (m_fields->m_totalRewards < 1) return 0;

	int rewardsNum = m_fields->m_totalRewards;
	m_fields->m_totalRewards = 0;
	log::info("resetting rewards from {}", rewardsNum);
	return rewardsNum;
} // getQuestRewards

bool MenuLayerAQ::init() {
	if (!MenuLayer::init()) return false;

	// Check if quests are already loaded
	auto stats = GameStatsManager::sharedState();
	if (stats->m_challengeTime > 0) return true;

	// Load quests if not already loaded
	log::info("Loading active quests...");
	auto questLayer = ChallengesPage::create();
	questLayer->onClose(questLayer);
	questLayer->release();

	return true;
} // init