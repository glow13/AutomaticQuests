#include "AutomaticQuestManager.hpp"
#include "AutomaticQuestNotification.hpp"

bool AchievementBarAQ::init(char const * title, char const * desc, char const * icon, bool isQuest) {

	if (!AchievementBar::init(title, desc, icon, isQuest)) return false;
	if (AutomaticQuests::isFeatureDisabled("quest-preview")) return true;
	if (!isQuest) return true;

	auto fields = m_fields.self();

	// Get the quest that was just completed
	auto stats = GameStatsManagerAQ::sharedState();
	auto quest = stats->getCompletedQuest(desc);
	if (!quest) return true;

	// Initialize variables
	std::string iconString = "GJ_timeIcon_001.png";
	std::string newDesc = "New quest in ";

	// Find the new quest description and icon
	if (auto newQuest = stats->getQueuedChallenge(quest->m_position)) {
		newDesc = fmt::format("Collect {} ", (int)newQuest->m_goal);
		switch ((int)newQuest->m_challengeType) {
			case 1:
				newDesc += "Mana Orbs.";
				iconString = "currencyOrbIcon_001.png";
				break;
			case 2:
				newDesc += "Silver Coins.";
				iconString = "GJ_coinsIcon2_001.png";
				break;
			case 3:
				newDesc += "Stars/Moons.";
				iconString = "GJ_starsIcon_001.png";
				break;
		} // switch
	} else {
		auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto timeLeft = (currentTime < stats->m_challengeTime) ? stats->m_challengeTime - currentTime : 0;
		newDesc += (std::string)GameToolbox::getTimeString(timeLeft, true) + ".";
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
	fields->m_newTitleLabel = newTitleLabel;

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
	fields->m_newAchievementDescription = newAchievementDescription;

	// Create the new quest sprite
	auto newAchievementSprite = CCSprite::createWithSpriteFrameName(iconString.c_str());
	newAchievementSprite->setPosition(m_achievementSprite->getPosition());
	newAchievementSprite->setScale(m_achievementSprite->getScale());
	newAchievementSprite->setAnchorPoint(m_achievementSprite->getAnchorPoint());
	newAchievementSprite->setZOrder(m_achievementSprite->getZOrder());
	newAchievementSprite->setOpacity(0);
	mainLayer->addChild(newAchievementSprite);
	fields->m_newAchievementSprite = newAchievementSprite;

	// Calculate timings
	const float fadeTime = 0.4;
	const float delayTime = (AutomaticQuests::getQuestTime() - 0.8) / 2;

	// Setup fade out effects
	m_titleLabel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeOut::create(fadeTime)));
	m_achievementSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeOut::create(fadeTime)));
	m_achievementDescription->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeOut::create(fadeTime)));

	// Setup fade in effects
	newTitleLabel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeIn::create(fadeTime)));
	newAchievementSprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeIn::create(fadeTime)));
	newAchievementDescription->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delayTime), CCFadeIn::create(fadeTime)));

	return true;
} // init

void AchievementBarAQ::setOpacity(unsigned char alpha) {
	AchievementBar::setOpacity(alpha);

	auto fields = m_fields.self();
	if (!fields->m_newTitleLabel) return;

	fields->m_newTitleLabel->setOpacity(alpha);
	fields->m_newAchievementSprite->setOpacity(alpha);
	fields->m_newAchievementDescription->setOpacity(alpha);

	m_titleLabel->setOpacity(0);
	m_achievementSprite->setOpacity(0);
	m_achievementDescription->setOpacity(0);
} // setOpacity

// Extends the time the quest notification is displayed
// Values taken from the android binary, thanks ghidra :)
void AchievementBarAQ::show() {
	AchievementBar::show();
	if (!m_fields->m_newTitleLabel) return;

	float questTime = AutomaticQuests::getQuestTime();

	stopAllActions();
	runAction(CCSequence::createWithTwoActions(CCDelayTime::create(questTime - 0.8), CCEaseIn::create(CCFadeOut::create(0.8), 2.0)));

	m_layerColor->stopAllActions();
	m_layerColor->runAction(CCEaseInOut::create(CCScaleTo::create(0.4, 1.0), 2.0));

	auto notifier = AchievementNotifier::sharedState();
	auto action = CCCallFunc::create(notifier, callfunc_selector(AchievementNotifier::achievementDisplayFinished));
	m_layerColor->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(questTime), action));
} // show