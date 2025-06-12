#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "GameStatsManager.hpp"

using namespace geode::prelude;

class $modify(PlayLayer) {

	// AFTER INCREMENT CHALLENGE
	void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
		int newDiamonds = diamonds + GameStatsManagerAQ::getQuestRewards();
		PlayLayer::showNewBest(newReward, orbs, newDiamonds, demonKey, noRetry, noTitle);
	} // showNewBest

	void levelComplete() {
		PlayLayer::levelComplete();

		auto screenSize = CCDirector::sharedDirector()->getWinSize();
		auto rewardLayer = CurrencyRewardLayer::create(
			0, 0, 0,
			GameStatsManagerAQ::getQuestRewards(), 
			(CurrencySpriteType)0, 0,
			(CurrencySpriteType)0, 0,
			screenSize,
			(CurrencyRewardType)0, 0, 1
		);
		addChild(rewardLayer);
	} // levelComplete
};