#include "AutomaticQuestManager.hpp"

#include <Geode/modify/AchievementNotifier.hpp>

class $modify(AchievementNotifier) {

    // Prevents a scenario where a quest notification would be shown for a quest that wasn't actually completed.
    // This happens in rare cases where you complete two quests in the same position at the same time.
    // The mod automatically corrects this mistake, but the notification still shows, so this hook prevents it from doing so.
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