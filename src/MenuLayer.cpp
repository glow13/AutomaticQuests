#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(MenuLayer) {

    bool init() {
        if (!MenuLayer::init()) return false;

        // Check if quests are already loaded
        auto stats = GameStatsManager::sharedState();
        if (stats->m_challengeTime > 0) return true;

        // Load quests if not already loaded
        log::info("Loading active quests...");
        auto questLayer = ChallengesPage::create();
        questLayer->onClose(questLayer);
        questLayer->release();

        return true;
    } // init
};