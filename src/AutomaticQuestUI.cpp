#include "AutomaticQuestUI.hpp"

// Excecutes after GameStatsManagerAQ::incrementChallenge
void PlayLayerAQ::showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
	if (isFeatureDisabled("quest-diamonds")) {
		PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
		return;
	} // if

	auto stats = GameStatsManagerAQ::sharedState();
	int newDiamonds = diamonds + stats->getQuestRewardsAndReset();
	PlayLayer::showNewBest(newReward, orbs, newDiamonds, demonKey, noRetry, noTitle);
} // showNewBest

// Excecutes after GameStatsManagerAQ::incrementChallenge
void EndLevelLayerAQ::customSetup() {
	if (isFeatureDisabled("quest-diamonds")) {
		EndLevelLayer::customSetup();
		return;
	} // if

	// See if any diamonds were earned from quests
	auto stats = GameStatsManagerAQ::sharedState();
	int questDiamonds = stats->getQuestRewardsAndReset();

	// Trick the EndLevelLayer to display the quest diamonds
	int diamonds = m_playLayer->m_diamonds;
	m_playLayer->m_diamonds += questDiamonds;
	EndLevelLayer::customSetup();
	m_playLayer->m_diamonds = diamonds;

	// Make sure the quest diamonds still show correctly even if no stars were earned
	if (m_starsPosition.isZero()) m_starsPosition = m_diamondsPosition;
} // customSetup

// Excecutes after EndLevelLayerAQ::customSetup
void EndLevelLayerAQ::playDiamondEffect(float time) {
	int diamonds = m_playLayer->m_diamonds;
	m_playLayer->m_diamonds = m_diamonds;
	EndLevelLayer::playDiamondEffect(time);
	m_playLayer->m_diamonds = diamonds;
} // playDiamondEffect

bool AchievementBarAQ::init(char const * title, char const * desc, char const * icon, bool isQuest) {

	if (!AchievementBar::init(title, desc, icon, isQuest)) return false;
	if (isFeatureDisabled("quest-preview")) return true;
	if (!isQuest) return true;

	if (auto quest = getQuest(desc)) {
		auto stats = GameStatsManager::sharedState();

		// Find the new quest description and icon
		std::string iconString = "GJ_timeIcon_001.png";
		std::string newDesc = "New quest in ";
		if (auto newQuest = stats->getQueuedChallenge(quest->m_position)) {
			newDesc = fmt::format("Collect {} ", (int)newQuest->m_goal);
			if((int)newQuest->m_challengeType == 1) {
				newDesc += "Mana Orbs.";
				iconString = "currencyOrbIcon_001.png";
			} else if ((int)newQuest->m_challengeType == 2) {
				newDesc += "Silver Coins.";
				iconString = "GJ_coinsIcon2_001.png";
			} else if ((int)newQuest->m_challengeType == 3) {
				newDesc += "Stars/Moons.";
				iconString = "GJ_starsIcon_001.png";
			} // if
		} else {
			auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			int hours = (stats->m_challengeTime - currentTime) / 3600;
			int minutes = (stats->m_challengeTime - currentTime) / 60 - hours * 60;
			if (hours > -1) newDesc += fmt::format("{}h {}min.", hours, minutes);
			else newDesc += "?h ??min.";
		} // if

		// Save references
		auto mainLayer = getChildByType<CCLayerColor>(0);
		auto textNode = mainLayer->getChildByType<CCNode>(4);

		// Create the new quest title
		auto newTitleLabel = CCLabelBMFont::create("New Quest!", m_titleLabel->getFntFile());
		newTitleLabel->setPosition(m_titleLabel->getPosition());
		newTitleLabel->setScale(m_titleLabel->getScale());
		newTitleLabel->setContentSize(m_titleLabel->getContentSize());
		newTitleLabel->setAnchorPoint(m_titleLabel->getAnchorPoint());
		newTitleLabel->setOpacity(0);
		textNode->addChild(newTitleLabel);
		m_fields->m_newTitleLabel = newTitleLabel;

		// Create the new quest description
		auto newAchievementDescription = TextArea::create(
			newDesc,
			m_achievementDescription->m_fontFile.c_str(),
			m_achievementDescription->m_scale,
			m_achievementDescription->m_width,
			m_achievementDescription->m_anchorPoint,
			m_achievementDescription->m_height,
			m_achievementDescription->m_disableColor
		);
		newAchievementDescription->setPosition(m_achievementDescription->getPosition());
		newAchievementDescription->setScale(m_achievementDescription->getScale());
		newAchievementDescription->setContentSize(m_achievementDescription->getContentSize());
		newAchievementDescription->setAnchorPoint(m_achievementDescription->getAnchorPoint());
		newAchievementDescription->setOpacity(0);
		textNode->addChild(newAchievementDescription);
		m_fields->m_newAchievementDescription = newAchievementDescription;

		// Create the new quest sprite
		auto newAchievementSprite = CCSprite::createWithSpriteFrameName(iconString.c_str());
		newAchievementSprite->setPosition(m_achievementSprite->getPosition());
		newAchievementSprite->setScale(m_achievementSprite->getScale());
		newAchievementSprite->setAnchorPoint(m_achievementSprite->getAnchorPoint());
		newAchievementSprite->setZOrder(m_achievementSprite->getZOrder());
		newAchievementSprite->setOpacity(0);
		mainLayer->addChild(newAchievementSprite);
		m_fields->m_newAchievementSprite = newAchievementSprite;

		// Calculate timings
		const float fadeTime = 0.4;
		const float delayTime = (getQuestTime() - 0.8) / 2;

		// Setup fade out effects
		m_titleLabel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeOut::create(fadeTime)));
		m_achievementSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeOut::create(fadeTime)));
		m_achievementDescription->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeOut::create(fadeTime)));

		// Setup fade in effects
		newTitleLabel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeIn::create(fadeTime)));
		newAchievementSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeIn::create(fadeTime)));
		newAchievementDescription->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeIn::create(fadeTime)));
	} // if
	return true;
} // init

void AchievementBarAQ::setOpacity(unsigned char alpha) {
	AchievementBar::setOpacity(alpha);
	if (!m_fields->m_newTitleLabel) return;

	m_fields->m_newTitleLabel->setOpacity(alpha);
	m_fields->m_newAchievementSprite->setOpacity(alpha);
	m_fields->m_newAchievementDescription->setOpacity(alpha);

	m_titleLabel->setOpacity(0);
	m_achievementSprite->setOpacity(0);
	m_achievementDescription->setOpacity(0);
} // setOpacity

// Extends the time the quest notification is displayed
// Values taken from the andriod binary, thanks ghidra :)
void AchievementBarAQ::show() {
	AchievementBar::show();
	if (!m_fields->m_newTitleLabel) return;

	float questTime = getQuestTime();

	stopAllActions();
	runAction(CCSequence::createWithTwoActions(CCDelayTime::create(questTime - 0.8), CCEaseIn::create(CCFadeOut::create(0.8), 2.0)));

	m_layerColor->stopAllActions();
	m_layerColor->runAction(CCEaseInOut::create(CCScaleTo::create(0.4, 1.0), 2.0));

	auto notifier = AchievementNotifier::sharedState();
	auto action = CCCallFunc::create(notifier, callfunc_selector(AchievementNotifier::achievementDisplayFinished));
	m_layerColor->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(questTime), action));
} // show

GJChallengeItem* AchievementBarAQ::getQuest(char const * desc) {
	int amount;
	GJChallengeType type;

	if (!parseQuestInfo(desc, &amount, &type)) {
		log::error("Failed to parse quest info!");
		return nullptr;
	} // if

	auto stats = GameStatsManager::sharedState();
	for (int i = 1; i <= 3; i++) {
		if (auto quest = stats->getChallenge(i)) {
			if ((int)quest->m_goal == amount && quest->m_challengeType == type) return quest;
		} // if
	} // for

	log::error("Failed to find the specified quest!");
	return nullptr;
} // getQuest

bool AchievementBarAQ::parseQuestInfo(char const * desc, int * amount, GJChallengeType * type) {
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