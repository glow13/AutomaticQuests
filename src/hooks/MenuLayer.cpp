#include "AutomaticQuestManager.hpp"

#include <Geode/modify/MenuLayer.hpp>

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        if (AutomaticQuests::isModDisabled()) return true;

        // Check if quests are already loaded
        auto stats = GameStatsManagerAQ::sharedState();
        if (stats->areChallengesLoaded()) return true;

        // Load quests if not already loaded
        log::info("Loading active quests...");
        stats->tryGetChallenges();

        return true;
    } // init
};