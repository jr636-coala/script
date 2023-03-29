#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

std::vector<std::string> getAudioDeviceNames();
std::vector<char> getAudioDeviceIndexes();
int getCurrentAudioDevice();

int main(int argc, char** argv) {
	std::vector<std::string> audioDeviceNames = getAudioDeviceNames();
	std::vector<char> audioDeviceIndexes = getAudioDeviceIndexes();
	int currentDevice = getCurrentAudioDevice();
	assert(currentDevice != -1);
	system(("~/.puswitch/switch.sh " + audioDeviceNames[currentDevice] + ' ' + audioDeviceIndexes[currentDevice]).c_str());
	return 0;
}

std::vector<std::string> getAudioDeviceNames() {
	FILE* file = popen("pacmd list-sinks | grep name:", "r");
	std::vector<std::string> out;
	std::string tmp;
	std::string t;
	while (!feof(file)) {
		tmp += fgetc(file);
	}
	pclose(file);
	int found;
	int prevFound = 0;
	while ((found = tmp.find('\n', prevFound)) != -1) {
		num.emplace_back(tmp.substr(prevFound + 8, found - prevFound - 9));
		prevFound = found + 1;
	}
	return out;
}

std::vector<char> getAudioDeviceIndexes() {
	FILE* file = popen("pacmd list-sinks | grep index", "r");
	std::string tmp = "";
	int found;
	int prevFound = 0;
	while (!feof(file)) {
		tmp += fgetc(file);
	}
	pclose(file);
	while ((found = tmp.find(':', prevFound)) != -1) {
		indexes.emplace_back(tmp.substr(found + 2, 1)[0]);
		prevFound = found + 1;
	}
}


int getCurrentAudioDevice(std::vector<std::string> audioDevices) {
	FILE* file = popen("pactl info | grep \"Default \"Sink", "r");
	std::string tmp = "";
	while (!feof(file)) {
		tmp += fgetc(file);
	}
	pclose(file);
	tmp = tmp.substr(14, tmp.length() - 16);
	int numIndex = -1;
	for (int i = 0; i < num.size(); i++) {
		if (num[i] == tmp) {
			numIndex = (i + 1) % num.size();
			break;
		}
	}
	return numIndex;
}
