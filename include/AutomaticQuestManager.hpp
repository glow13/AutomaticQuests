#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {
private:
	static int s_totalRewards;
public:
	void incrementChallenge(GJChallengeType type, int amount);
	gd::string getChallengeKey(GJChallengeItem* quest);
	static int getQuestRewards();
};

class $modify(MenuLayerAQ, MenuLayer) {
public:
    bool init();
};