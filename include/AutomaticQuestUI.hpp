#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
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

	inline const static float s_showTime = 4.0; // Vanilla is 3.2 seconds

	bool init(char const * title, char const * desc, char const * icon, bool isQuest);
	void setOpacity(unsigned char alpha);
	void show();

	GJChallengeItem* getQuest(char const * desc);
	bool parseQuestInfo(char const * desc, int * amount, GJChallengeType * type);
};

class $modify(PlayLayerAQ, PlayLayer) {
	void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle);
};

class $modify(EndLevelLayerAQ, EndLevelLayer) {
	void playDiamondEffect(float time);
	void customSetup();
};