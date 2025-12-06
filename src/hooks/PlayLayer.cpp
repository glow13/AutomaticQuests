#include "AutomaticQuestManager.hpp"

#include <Geode/modify/PlayLayer.hpp>

class $modify(PlayLayer) {

    // Reset stats before starting a level to avoid any visual bugs
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        auto stats = GameStatsManagerAQ::sharedState();
        stats->resetQuestRewards();

        return true;
    } // init

    // Excecutes after GameStatsManagerAQ::incrementChallenge, so the quest rewards are always up-to-date
	void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        if (AutomaticQuests::isFeatureDisabled("quest-diamonds") || AutomaticQuests::isFeatureDisabled("auto-claim")) {
            PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
            return;
        } // if

        auto stats = GameStatsManagerAQ::sharedState();
        int newDiamonds = diamonds + stats->getQuestRewardsAndReset();
        PlayLayer::showNewBest(newReward, orbs, newDiamonds, demonKey, noRetry, noTitle);
    } // showNewBest
};