#include "AutomaticQuestManager.hpp"

#include <Geode/modify/EndLevelLayer.hpp>

class $modify(EndLevelLayer) {

    // Somewhat hacky hook to show rewarded quest diamonds on the level complete screen
	void customSetup() {
        if (AutomaticQuests::isFeatureDisabled("quest-diamonds") || AutomaticQuests::isFeatureDisabled("auto-claim")) {
            EndLevelLayer::customSetup();
            return;
        } // if

        // See if any diamonds were earned from quests
        auto stats = GameStatsManagerAQ::sharedState();
        int questDiamonds = stats->getQuestRewardsAndReset();

        // Trick the EndLevelLayer to display the quest diamonds
        int diamonds = m_playLayer->m_diamonds;
        m_playLayer->m_diamonds += questDiamonds;
        EndLevelLayer::customSetup();
        m_playLayer->m_diamonds = diamonds;

        // Make sure the quest diamonds still show correctly even if no stars were earned
        if (m_starsPosition.isZero()) m_starsPosition = m_diamondsPosition;
    } // customSetup

    // This is needed for the above function to work
    void playDiamondEffect(float dt) {
        int diamonds = m_playLayer->m_diamonds;
        m_playLayer->m_diamonds = m_diamonds;
        EndLevelLayer::playDiamondEffect(dt);
        m_playLayer->m_diamonds = diamonds;
    } // playDiamondEffect
};