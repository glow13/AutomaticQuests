#include "AutomaticQuestManager.hpp"

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	GameStatsManager::incrementChallenge(type, amount);
	if (AutomaticQuests::isFeatureDisabled("auto-claim")) return;

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

	tryGetChallenges();
} // resetQuestRewards

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewardsAndReset() {
	if (m_fields->m_totalRewards < 1) return 0;

	int rewardsNum = m_fields->m_totalRewards;
	resetQuestRewards();
	return rewardsNum;
} // getQuestRewards

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

void GameStatsManagerAQ::tryGetChallenges() {
	auto currentTime = std::chrono::system_clock::now();
	if (std::chrono::system_clock::to_time_t(currentTime) > m_challengeTime) resetChallengeTimer();
	if (areChallengesLoaded()) return;

	auto fields = m_fields.self();
	fields->stats = this;

	auto manager = GameLevelManager::sharedState();
	manager->m_GJChallengeDelegate = fields;
	manager->getGJChallenges();
} // tryGetChallenges

void GameStatsManagerAQ::Fields::challengeStatusFinished() {
	log::info("challenge status finished");

	for (auto [key, val] : CCDictionaryExt<std::string, GJChallengeItem*>(stats->m_activeChallenges)) log::info("before {}: {}", key, val->m_name);
	for (auto [key, val] : CCDictionaryExt<std::string, GJChallengeItem*>(stats->m_upcomingChallenges)) log::info("before queued {}: {}", key, val->m_name);

	for (int i = 1; i <= 3; i++) {
		if (!stats->getChallenge(i)) stats->processChallengeQueue(i);
	} // for

	for (auto [key, val] : CCDictionaryExt<std::string, GJChallengeItem*>(stats->m_activeChallenges)) log::info("after {}: {}", key, val->m_name);
	for (auto [key, val] : CCDictionaryExt<std::string, GJChallengeItem*>(stats->m_upcomingChallenges)) log::info("after queued {}: {}", key, val->m_name);
} // challengeStatusFinished

void GameStatsManagerAQ::Fields::challengeStatusFailed() {
	if (stats->areChallengesLoaded() || stats->getActionByTag(5)) return;
	log::error("challenge status failed");

	for (auto [key, val] : CCDictionaryExt<std::string, GJChallengeItem*>(stats->m_activeChallenges)) log::error("{}: {}", key, val->m_name);
	for (auto [key, val] : CCDictionaryExt<std::string, GJChallengeItem*>(stats->m_upcomingChallenges)) log::error("queued {}: {}", key, val->m_name);

	auto func = callfunc_selector(GameStatsManagerAQ::tryGetChallenges);
	auto action = CCSequence::create(CCDelayTime::create(5), CCCallFunc::create(stats, func), 0);
	action->setTag(5);

	auto actions = stats->getActionManager();
	actions->addAction(action, stats, false);
} // challengeStatusFailed