/* statistics.h
 * Keeps statistics of scene usage time
 *
 * Author: Artem Hiblov
 */

#pragma once

#include <map>
#include <string>

class SceneStatistics
{
public:
	SceneStatistics(const std::string &fileName);
	~SceneStatistics();

	void addTime(const std::string &sceneName, int seconds);
	int getTotalTime(const std::string &sceneName);

	void save();

private:
	void load();

	std::map<std::string, int> sceneUsage;
	const std::string path;
};
