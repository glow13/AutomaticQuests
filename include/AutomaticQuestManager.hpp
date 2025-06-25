#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(GameStatsManagerAQ, GameStatsManager) {
public:
	struct Fields {
        int m_totalRewards = 0;
    };

	void incrementChallenge(GJChallengeType type, int amount);
	gd::string getChallengeKey(GJChallengeItem* quest);
	int getQuestRewards();

	static GameStatsManagerAQ* sharedState() {
		auto stats = GameStatsManager::sharedState();
		return static_cast<GameStatsManagerAQ*>(stats);
	} // sharedState
};

class $modify(MenuLayerAQ, MenuLayer) {
public:
    bool init();
};