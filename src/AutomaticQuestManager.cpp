#include "AutomaticQuestManager.hpp"

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	GameStatsManager::incrementChallenge(type, amount);
	if ((int)type == 1) m_fields->m_totalRewards = 0;

	// Check if this completed any quests
	for (int i = 1; i <= 3; i++) {
		auto quest = getChallenge(i);
		if (quest && quest->m_challengeType == type && quest->m_canClaim) {
			int reward = quest->m_reward.value();
			m_fields->m_totalRewards += reward;
			incrementStat("13", reward);

			// Add challenge diamonds
			auto key = getChallengeKey(quest);
			auto object = CCString::createWithFormat("%i", reward);
			m_challengeDiamonds->setObject(object, key);

			processChallengeQueue(i);
			log::info("Automatically claimed quest #{}!", i);
		} // if
	} // for
} // incrementChallenge

// Same implementation as GameStatsManager::getChallengeKey
gd::string GameStatsManagerAQ::getChallengeKey(GJChallengeItem* quest) {
	return fmt::format("c{}{}", quest->m_position, quest->m_timeLeft);
} // getChallengeKey

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewards() {
	if (m_fields->m_totalRewards < 1) return 0;

	int rewardsNum = m_fields->m_totalRewards;
	m_fields->m_totalRewards = 0;
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
	questLayer->onClose(nullptr);
	questLayer->release();

	return true;
} // init