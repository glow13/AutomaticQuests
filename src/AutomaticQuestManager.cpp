#include "AutomaticQuestManager.hpp"

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	GameStatsManager::incrementChallenge(type, amount);
	if (isFeatureDisabled("auto-claim")) return;

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

void GameStatsManagerAQ::resetQuestRewards() {
	m_fields->m_totalRewards = 0;

	m_fields->m_completed[1] = false;
	m_fields->m_completed[2] = false;
	m_fields->m_completed[3] = false;

	loadChallenges();
} // resetQuestRewards

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewardsAndReset() {
	if (m_fields->m_totalRewards < 1) return 0;

	int rewardsNum = m_fields->m_totalRewards;
	resetQuestRewards();
	return rewardsNum;
} // getQuestRewardsAndReset

void GameStatsManagerAQ::tryGetChallenges() {
	if (areChallengesLoaded() || m_fields->loadAttempts >= 10) {
		getScheduler()->unscheduleAllForTarget(this);
		pauseSchedulerAndActions();

		if (m_fields->loadAttempts >= 10) log::error("Failed to load quests!");
		else log::debug("Successfully loaded quests");

		m_fields->loadAttempts = 0;
		return;
	} // if

	m_fields->loadAttempts++;
	GameLevelManager::sharedState()->getGJChallenges();
} // tryGetChallenges

void GameStatsManagerAQ::loadChallenges() {
	if (m_fields->loadAttempts > 0) return;
	auto currentTime = std::chrono::system_clock::now();
	if (std::chrono::system_clock::to_time_t(currentTime) > m_challengeTime) resetChallengeTimer();

	schedule(schedule_selector(GameStatsManagerAQ::tryGetChallenges), 2);
	resumeSchedulerAndActions();
} // loadChallenges

void AchievementNotifierAQ::notifyAchievement(char const* title, char const* desc, char const* icon, bool isQuest) {
	if (isQuest) {
		auto stats = GameStatsManagerAQ::sharedState();
		auto quest = stats->getCompletedQuest(desc);

		// Make sure we haven't already claimed a quest at this position
		if (quest && stats->m_fields->m_completed[quest->m_position]) return;
	} // if

	AchievementNotifier::notifyAchievement(title, desc, icon, isQuest);
} // notifyAchievement

bool PlayLayerAQ::init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
	if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

	// Reset stats before starting a level to avoid any visual bugs
	auto stats = GameStatsManagerAQ::sharedState();
	stats->resetQuestRewards();

	return true;
} // init

bool MenuLayerAQ::init() {
	if (!MenuLayer::init()) return false;
	if (isModDisabled()) return true;

	// Check if quests are already loaded
	auto stats = GameStatsManagerAQ::sharedState();
	if (stats->areChallengesLoaded()) return true;
	else if (stats->m_fields->loadAttempts > 0) return true;

	// Load quests if not already loaded
	log::info("Loading active quests...");
	stats->loadChallenges();

	return true;
} // init

GJChallengeItem* GameStatsManagerAQ::getCompletedQuest(char const * desc) {
	int amount;
	GJChallengeType type;

	if (!parseQuestInfo(desc, &amount, &type)) {
		log::error("Failed to parse quest info!");
		return nullptr;
	} // if

	for (int i = 1; i <= 3; i++) {
		if (auto quest = getChallenge(i)) {
			if ((int)quest->m_goal == amount && quest->m_challengeType == type) return quest;
		} // if
	} // for

	log::error("Failed to find the completed quest!");
	return nullptr;
} // getCompletedQuest

bool GameStatsManagerAQ::parseQuestInfo(char const * desc, int * amount, GJChallengeType * type) {
	int amountInt;
	std::string typeString;

	std::stringstream ss(desc);
	ss >> typeString >> amountInt >> typeString;

	if (typeString == "Mana") *type = GJChallengeType::Orbs;
	else if (typeString == "Silver") *type = GJChallengeType::UserCoins;
	else if (typeString == "Stars/Moons.") *type = GJChallengeType::Stars;
	else *type = GJChallengeType::Unknown;

	*amount = amountInt;
	return *type != GJChallengeType::Unknown;
} // parseQuestInfo