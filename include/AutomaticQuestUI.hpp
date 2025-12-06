#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/AchievementBar.hpp>

using namespace geode::prelude;

class $modify(AchievementBarAQ, AchievementBar) {

	struct Fields {
        CCLabelBMFont* m_newTitleLabel;
		TextArea* m_newAchievementDescription;
		CCSprite* m_newAchievementSprite;
    };

	bool init(char const * title, char const * desc, char const * icon, bool isQuest);
	void setOpacity(unsigned char alpha);
	void show();
};