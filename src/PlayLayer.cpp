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
};