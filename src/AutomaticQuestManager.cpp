#include "AutomaticQuestManager.hpp"

void GameStatsManagerAQ::incrementChallenge(GJChallengeType type, int amount) {
	GameStatsManager::incrementChallenge(type, amount);
	if (AutomaticQuests::isFeatureDisabled("auto-claim")) return;

	auto fields = m_fields.self();

	// Check if this completed any quests
	for (int i = 1; i <= 3; i++) {
		auto quest = getChallenge(i);
		if (quest && fields->m_completed[i]) {
			quest->m_count = 0;
			quest->m_canClaim = false;
		} else if (quest && quest->m_challengeType == type && quest->m_canClaim) {
			int reward = quest->m_reward.value();
			fields->m_totalRewards += reward;
			fields->m_completed[i] = true;
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
	auto fields = m_fields.self();
	fields->m_totalRewards = 0;

	fields->m_completed[1] = false;
	fields->m_completed[2] = false;
	fields->m_completed[3] = false;

	tryGetChallenges();
} // resetQuestRewards

// Resets the rewards to zero after use
int GameStatsManagerAQ::getQuestRewardsAndReset() {
	if (int rewards = m_fields->m_totalRewards; rewards > 0) {
		resetQuestRewards();
		return rewards;
	} else return 0;
} // getQuestRewards

GJChallengeItem* GameStatsManagerAQ::getCompletedQuest(char const * desc) {
	int amount;
	GJChallengeType type;

	if (!parseQuestInfo(desc, &amount, &type)) {
		log::error("Failed to parse quest info!");
		return nullptr;
	} // if

	for (auto [key, quest] : CCDictionaryExt<std::string, GJChallengeItem*>(m_activeChallenges)) {
		if ((int)quest->m_goal == amount && quest->m_challengeType == type) return quest;
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
	fields->m_getQuestTries++;
	fields->stats = this;

	auto manager = GameLevelManager::sharedState();
	manager->m_GJChallengeDelegate = fields;
	manager->getGJChallenges();
} // tryGetChallenges

void GameStatsManagerAQ::Fields::challengeStatusFinished() {
	log::info("Load quests finished!");
	m_getQuestTries = 0;

	// Moved queued quests to the active state if possible
	for (int i = 1; i <= 3; i++) {
		if (!stats->getChallenge(i)) stats->processChallengeQueue(i);
	} // for
} // challengeStatusFinished

void GameStatsManagerAQ::Fields::challengeStatusFailed() {
	if (stats->areChallengesLoaded() || stats->getActionByTag(5)) return;
	log::debug("Load quests failed, retrying request...");

	// Stop trying to get challenges after 30 seconds
	if (m_getQuestTries > 6) {
		log::warn("Load quests failed!");
		m_getQuestTries = 0;
		return;
	} // if

	auto func = callfunc_selector(GameStatsManagerAQ::tryGetChallenges);
	auto action = CCSequence::create(CCDelayTime::create(5), CCCallFunc::create(stats, func), 0);
	action->setTag(5);

	auto actions = stats->getActionManager();
	actions->addAction(action, stats, false);
} // challengeStatusFailed