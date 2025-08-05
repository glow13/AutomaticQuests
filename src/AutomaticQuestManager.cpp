#include "AutomaticQuestManager.hpp"
#include "AutomaticQuestUI.hpp"

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	GameStatsManager::incrementChallenge(type, amount);
	if (isModDisabled()) return;

	// Check if this completed any quests
	for (int i = 1; i <= 3; i++) {
		auto quest = getChallenge(i);
		if (quest && m_fields->m_completed[i]) {
			quest->m_count = 0;
			quest->m_canClaim = false;
		} else if (quest && quest->m_challengeType == type && quest->m_canClaim) {
			int reward = quest->m_reward.value();
			m_fields->m_totalRewards += reward;
			m_fields->m_completed[i] = true;
			incrementStat("13", reward);

			// Add challenge diamonds
			auto key = getChallengeKey(quest);
			auto object = CCString::createWithFormat("%i", reward);
			m_challengeDiamonds->setObject(object, key);

			log::info("Automatically claimed \"{}\" for {} diamonds", quest->m_name, quest->m_reward);
			processChallengeQueue(i);
		} // if
	} // for
} // incrementChallenge

// Gives the same result as GameStatsManager::getChallengeKey
gd::string GameStatsManagerAQ::getChallengeKey(GJChallengeItem* quest) {
	return fmt::format("c{}{}", quest->m_position, quest->m_timeLeft);
} // getChallengeKey

void GameStatsManagerAQ::resetQuestRewards() {
	m_fields->m_totalRewards = 0;

	m_fields->m_completed[1] = false;
	m_fields->m_completed[2] = false;
	m_fields->m_completed[3] = false;
} // resetQuestRewards

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewardsAndReset() {
	if (m_fields->m_totalRewards < 1) return 0;

	int rewardsNum = m_fields->m_totalRewards;
	resetQuestRewards();
	return rewardsNum;
} // getQuestRewards

void AchievementNotifierAQ::notifyAchievement(char const* title, char const* desc, char const* icon, bool isQuest) {
	if (!isQuest) {
		AchievementNotifier::notifyAchievement(title, desc, icon, isQuest);
		return;
	} // if

	auto quest = AchievementBarAQ::getCompletedQuest(desc);
	if (!quest) {
		AchievementNotifier::notifyAchievement(title, desc, icon, isQuest);
		return;
	} // if

	auto stats = GameStatsManagerAQ::sharedState();
	bool completed = stats->m_fields->m_completed[quest->m_position];
	if (!completed) AchievementNotifier::notifyAchievement(title, desc, icon, isQuest);
} // notifyAchievement

bool PlayLayerAQ::init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
	if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

	// Reset stats before starting a level to avoid any visual bugs
	auto stats = GameStatsManagerAQ::sharedState();
	stats->resetQuestRewards();

	return true;
} // init

// Kinda scuffed but it works so yeah
bool MenuLayerAQ::init() {
	if (!MenuLayer::init()) return false;
	if (isModDisabled()) return true;

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