#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/AchievementBar.hpp>

#include "AutomaticQuestManager.hpp"

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

	GJChallengeItem* getCompletedQuest(char const * desc);
	bool parseQuestInfo(char const * desc, int * amount, GJChallengeType * type);
};

class $modify(EndLevelLayerAQ, EndLevelLayer) {
	void playDiamondEffect(float time);
	void customSetup();
};