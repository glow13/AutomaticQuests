#include <Geode/Geode.hpp>
#include <Geode/modify/ChallengesPage.hpp>

using namespace geode::prelude;

class $modify(ChallengesPageAQ, ChallengesPage) {

	bool init() {
		if (!ChallengesPage::init()) return false;

		auto menu = getChildByID("main-layer")->getChildByID("top-right-menu");

		auto sprite = CCSprite::createWithSpriteFrameName("GJ_redoBtn_001.png");
		auto button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(ChallengesPageAQ::onButton1));

		auto sprite2 = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
		auto button2 = CCMenuItemSpriteExtra::create(sprite2, this, menu_selector(ChallengesPageAQ::incrementQuests));

		auto sprite3 = CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
		auto button3 = CCMenuItemSpriteExtra::create(sprite3, this, menu_selector(ChallengesPageAQ::onButton3));

		menu->addChild(button);
		menu->addChild(button2);
		menu->addChild(button3);
		menu->updateLayout();

		return true;
	} // init

	void onButton1(CCObject* sender) {
		auto stats = GameStatsManager::sharedState();

		log::info("adding quests");
		auto item1 = GJChallengeItem::create(GJChallengeType::Orbs, 1, 10, 1, "Amazing Quest");
		stats->storeQueuedChallenge(1, item1);

		auto item2 = GJChallengeItem::create(GJChallengeType::Stars, 1, 15, 1, "Fantastic Quest");
		stats->storeQueuedChallenge(2, item2);

		auto item3 = GJChallengeItem::create(GJChallengeType::UserCoins, 1, 20, 1, "Impossible Quest");
		stats->storeQueuedChallenge(3, item3);

		onClose(sender);
	} // onButton

	void onButton2(CCObject* sender) {
		auto stats = GameStatsManager::sharedState();

		log::info("completing quests");
		if (auto quest = stats->getChallenge(1)) quest->m_canClaim = true;
		if (auto quest = stats->getChallenge(2)) quest->m_canClaim = true;
		if (auto quest = stats->getChallenge(3)) quest->m_canClaim = true;

		onClose(sender);
	} // onButton2

	void incrementQuests(CCObject* sender) {
		auto stats = GameStatsManager::sharedState();

		log::info("incrementing quests");
		stats->incrementChallenge(GJChallengeType::Orbs, 100);
		stats->incrementChallenge(GJChallengeType::Stars, 5);
		stats->incrementChallenge(GJChallengeType::UserCoins, 2);

		onClose(sender);
	} // incrementQuests

	void onButton3(CCObject* sender) {
		auto notifier = AchievementNotifier::sharedState();
		notifier->notifyAchievement("Quest Complete!", "Collect 1 Mana Orbs.", "currencyOrbIcon_001.png", true);

		auto stats = GameStatsManager::sharedState();
		auto challengeDiamonds = stats->m_challengeDiamonds;
		auto keys = challengeDiamonds->allKeys();
		for (int i = 0; i < keys->count(); i++) {
			auto key = static_cast<CCString*>(keys->objectAtIndex(i))->getCString();
			auto object = static_cast<CCString*>(challengeDiamonds->objectForKey(key))->intValue();
			if (key[0] == 'c') log::info("{} {}", key, object);
		} // for

		for (int i = 1; i <= 3; i++) if (auto quest = stats->getChallenge(i)) {
			auto key = CCString::createWithFormat("c%i%i", (int)quest->m_position, (int)quest->m_timeLeft);
			log::info("key {}: {}", i, key->getCString());
		} // for
		log::info("length: {}", keys->count());
	} // onButton3

	// static void GameStatsManager_storeChallenge(GameStatsManager * self, int p0, GJChallengeItem * p1) {
	// 	static tulip::hook::WrapperMetadata metadata;
	// 	metadata.m_convention = geode::hook::createConvention(tulip::hook::TulipConvention::Thiscall);
	// 	metadata.m_abstract = tulip::hook::AbstractFunction::from(&ChallengesPageAQ::GameStatsManager_storeChallenge);
	// 	static auto original = geode::hook::createWrapper(reinterpret_cast<void*>(geode::base::get() + 0x1ded90), metadata);
	// 	auto func = reinterpret_cast<void(*)(GameStatsManager*, int, GJChallengeItem*)>(original.unwrap());
	// 	func(self, p0, p1);
	// } // GameStatsManager_storeChallenge
};