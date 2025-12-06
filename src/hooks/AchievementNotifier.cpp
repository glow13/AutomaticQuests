#include "AutomaticQuestManager.hpp"

#include <Geode/modify/AchievementNotifier.hpp>

class $modify(AchievementNotifier) {
	void notifyAchievement(char const* title, char const* desc, char const* icon, bool isQuest) {
        if (isQuest) {
            auto stats = GameStatsManagerAQ::sharedState();
            auto quest = stats->getCompletedQuest(desc);

            // Make sure we haven't already claimed a quest at this position
            if (quest && stats->m_fields->m_completed[quest->m_position]) return;
        } // if

        AchievementNotifier::notifyAchievement(title, desc, icon, isQuest);
    } // notifyAchievement
};