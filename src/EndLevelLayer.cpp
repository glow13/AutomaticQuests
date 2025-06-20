#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include "GameStatsManager.hpp"

using namespace geode::prelude;

class $modify(EndLevelLayer) {

	void playEndEffect() {
		m_diamonds += GameStatsManagerAQ::getQuestRewards();
		EndLevelLayer::playEndEffect();
	} // playEndEffect
};